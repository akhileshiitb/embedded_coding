# Software Timers on One Hardware Timer — Design Concepts

A reference for the classic problem: **support N software timers using a single hardware
timer.** Read this before attempting the coding question.

---

## 1. The Problem and Why It Exists

Most MCUs give you only a **handful of hardware timer peripherals** — sometimes just one free
for general use. A hardware timer can be armed to fire an interrupt at **one** future moment.
But real firmware needs **many** independent timeouts running at once:

- a 50 ms debounce timer,
- a 1 s sensor-sample timer,
- a 30 s watchdog-adjacent housekeeping timer,
- a 200 ms LED blink,
- a comms retransmit timeout…

You cannot dedicate a hardware timer to each. The solution is a **software timer module** that
**multiplexes** many logical timers onto the one physical timer. This is exactly what an RTOS
timer service (FreeRTOS software timers, Zephyr `k_timer`, Linux `hrtimer`) does under the hood.

---

## 2. The Core Idea — Multiplexing

The physical timer can only be armed for **one** expiry at a time. So:

1. Keep a collection of pending software timers, each with an **absolute expiry time** and a
   **callback** (function pointer + argument).
2. Always arm the single hardware timer for the **soonest** (minimum) expiry among all pending
   timers.
3. When the hardware timer fires:
   - fire the callback of **every** timer whose expiry has now passed (there may be more than
     one — several could be due at the same tick, or a late dispatch can make several overdue),
   - then **re-arm** the hardware timer for the next-soonest remaining timer (or leave it idle if
     none remain).

That's the whole design. Everything else is *how efficiently* you can (a) find the minimum
expiry and (b) remove expired entries.

> Absolute vs relative time: store **absolute** expiry (`now + delay` computed at registration).
> Comparing absolute times avoids the drift you'd get from repeatedly subtracting deltas, and
> makes "is this due?" a simple `expiry <= now` check.

---

## 3. The Central Engineering Question — Which Data Structure?

The module lives or dies by how it stores pending timers. The trade-off is between **insertion
cost** and **find-next / expiry cost**. (The full comparison table with complexities is in
`hints.md`.) The main options:

- **Unsorted array/list** — trivial insert, but you scan everything to find the minimum.
- **Sorted list / delta queue** — the classic embedded structure; keeps the soonest at the head.
- **Min-heap (priority queue)** — great when you have many timers; O(log N) insert, O(1) peek.
- **Timer wheel** — O(1) amortized for huge numbers of timers; used by OS kernels.

The "right" answer depends on **how many timers** you expect and **how often** you insert vs.
expire. For a handful of timers (typical MCU), an unsorted list or sorted list is perfectly fine
and simplest to get correct.

---

## 4. The Delta Queue (Classic Embedded Structure)

A **delta queue** is a sorted linked list where each node stores the **difference (delta)** from
the previous node's expiry rather than an absolute time:

```
timers due at absolute 100, 100, 250, 400   (now = 0)
delta queue:   [100] -> [0] -> [150] -> [150]
                head
```

Why deltas? If the timer tick decrements only the **head** delta each tick, then a single
subtraction per tick advances *all* timers (a node expires when its delta hits 0, along with any
following nodes whose delta is also 0). This was popular when the tick handler had to be extremely
cheap. With a modern "arm for absolute expiry" model (like this question), a plain sorted list of
absolute times is equivalent and easier to reason about.

---

## 5. Timer Wheels (How Kernels Scale to Thousands)

When you have **thousands to millions** of timers (a network server tracking per-connection
timeouts, an OS kernel), even O(log N) per operation is too slow. A **timer wheel** gives
**O(1)** insert and expiry:

- A fixed-size circular array of "buckets," each holding a list of timers.
- A timer due in `d` ticks goes into bucket `(current + d) mod wheel_size`.
- Each tick, advance the "current" pointer by one bucket and fire everything in that bucket.
- **Hierarchical timer wheels** layer multiple wheels at different resolutions (like clock hands:
  seconds wheel, minutes wheel, hours wheel) so far-future timers don't need a giant array.

This is what the Linux kernel and high-performance network stacks use. It's the standard
"how would you scale this to 10,000+ timers?" follow-up — **overkill for a handful of MCU timers**,
but important to name.

---

## 6. How Real Systems Do This (Parallels)

- **FreeRTOS software timers**: a dedicated **timer service task** ("daemon task") owns all
  software timers and processes a command queue; callbacks run in that task's context (not an
  ISR). One tick source drives all of them.
- **Linux `hrtimer` / timer wheel**: the kernel multiplexes all kernel timers onto the hardware
  clock event device; `hrtimers` use a red-black tree (ordered by expiry) for high-resolution
  timers, and the classic timer wheel for coarse timers.
- **Zephyr `k_timer`**: software timers driven by the system tick / a single timer driver.

The common shape everywhere: **one physical time source → a data structure ordered by expiry →
dispatch due callbacks → re-arm.** Our coding question is the minimal bare-metal version of this.

---

## 7. Important Edge Cases

1. **Past-due timers**: a timer registered with `expiry <= now` should fire on the next dispatch,
   not be lost or scheduled far in the future.
2. **Duplicate expiry times**: several timers due at the same tick must **all** fire.
3. **Multiple due at once**: when the HW fires, more than one timer may be due (same expiry, or a
   late dispatch). Fire *all* of them in one pass, then re-arm once.
4. **Re-arming correctly**: after firing due timers, arm the HW for the **new** soonest remaining
   timer. If none remain, leave the HW idle. Forgetting to re-arm is the most common bug.
5. **Empty set**: starting a timer when none were pending must arm the HW; firing the last timer
   must leave the system idle.
6. **Callback that registers a new timer**: a callback may call `sw_timer_start` again (e.g., to
   reschedule). The module must remain consistent if this happens during dispatch.

---

## 8. Follow-Up Topics (Umbrella — Out of Scope for the Coding Question)

Deliberately excluded to keep the exercise ~30 minutes:

- **Cancellation**: `sw_timer_cancel(id)` to stop a pending timer. Requires stable handles/ids and
  removal from the structure; re-arm if the cancelled timer was the soonest. A very common
  extension and interview follow-up.
- **Periodic / auto-reload timers**: on expiry, re-insert with `expiry += period`. Watch for drift
  (reload from the scheduled expiry, not from "now") and for a period shorter than the callback
  runtime.
- **Concurrency / ISR-safety**: `sw_timer_start` may be called from thread context while
  `sw_timer_on_expire` runs in ISR context — the shared structure needs a brief critical section,
  or a lock-free/command-queue design (FreeRTOS uses a command queue for exactly this).
- **Timer wheel / hierarchical wheel**: the O(1) scaling structure for thousands of timers (§5).
- **Tick vs tickless**: a periodic tick that decrements deltas vs. a "one-shot, arm-for-next-expiry"
  (tickless) design. Tickless saves power (no wakeups when idle) — this question uses the tickless
  model (arm HW for the exact next expiry).
- **Time-base width / rollover**: a 32-bit tick counter wraps; robust comparisons use *signed
  difference* (`(int32_t)(a - b) < 0`) rather than `a < b` to handle wraparound.
- **Callback context & runtime**: long-running callbacks delay every other timer. Real systems run
  callbacks in a task, keep them short, or defer heavy work.
