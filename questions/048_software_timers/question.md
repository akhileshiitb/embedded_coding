# 048. N Software Timers on One Hardware Timer

## Difficulty: Hard

## Problem Statement

Your embedded system has **only one hardware timer**. It can be armed to fire **once** at a
single future time. But the firmware needs **many** independent timeouts running concurrently
(debounce, sensor sampling, blink, comms retransmit, …).

Design a **software timer module** that multiplexes **N one-shot software timers** onto the
single hardware timer. Each software timer has an absolute expiry time and a **callback**
(function pointer + argument) that must be invoked when it expires.

This is a **design** question and a pure simulation — there is no real hardware. `solution.c`
contains **plain function stubs only**; you design the internal data structure yourself (how you
store pending timers, how you find the soonest, how you dispatch). The full data-structure
trade-off discussion is in `hints.md`.

> **Read `timer_design.md` first.** It explains multiplexing, the data-structure trade-offs,
> delta queues, timer wheels, and how FreeRTOS / Linux do this.

---

## The Single Hardware Timer (provided by the test harness)

You do **not** implement these — the test harness provides them and drives time:

```c
/* Current absolute time (ticks). The test harness advances this. */
unsigned int hw_get_time(void);

/* Arm the ONE hardware timer to fire at absolute time `expiry`.
 * Calling it again overrides any previously armed time.
 * When the harness's time reaches `expiry`, it calls sw_timer_on_expire(). */
void hw_timer_set(unsigned int expiry);
```

There is exactly **one** hardware timer. You may only ever have **one** expiry armed at a time —
that is the whole constraint the software layer works around.

---

## What You Must Implement

```c
/* Reset the module to empty (no pending software timers). */
void sw_timer_init(void);

/* Register a one-shot software timer that fires at absolute time `expiry`,
 * invoking cb(arg) when it expires. May be called many times to create many
 * concurrent timers. */
void sw_timer_start(unsigned int expiry, void (*cb)(void *arg), void *arg);

/* Called by the hardware layer when the armed hardware timer fires.
 * Must invoke the callbacks of ALL software timers that are now due, then
 * re-arm the hardware timer for the next-soonest remaining timer (if any). */
void sw_timer_on_expire(void);
```

---

## Behavior Specification

### `sw_timer_init(void)`
Reset to empty: no pending timers. (The hardware timer need not be armed until a timer is started.)

### `sw_timer_start(unsigned int expiry, void (*cb)(void *arg), void *arg)`
1. Record a new software timer: `{ expiry, cb, arg }`.
2. Arm the hardware timer for the **soonest** pending expiry. In particular, if this new timer is
   now the soonest, call `hw_timer_set(expiry)` to (re-)arm the hardware for it.
3. A timer with `expiry <= hw_get_time()` (past-due) must still fire — arm the hardware for it so
   it dispatches on the next `sw_timer_on_expire()`.
4. Assume capacity for up to at least 16 pending timers.

### `sw_timer_on_expire(void)`
Called when the hardware timer fires. Let `now = hw_get_time()`.
1. Invoke `cb(arg)` for **every** pending timer whose `expiry <= now` (there may be several — same
   expiry, or multiple overdue). Each such timer fires **once** and is then removed.
2. After firing all due timers, **re-arm** the hardware timer for the **next-soonest** remaining
   pending timer via `hw_timer_set(...)`. If no timers remain, do not arm it (leave idle).

---

## Examples

### Ordered expiry
```
now = 0
sw_timer_start(30, cbA, ...);   // hw armed for 30
sw_timer_start(10, cbB, ...);   // 10 is sooner -> hw re-armed for 10
sw_timer_start(20, cbC, ...);   // hw armed for 10 (unchanged)

// time advances to 10 -> harness calls sw_timer_on_expire()
//   -> cbB fires; re-arm hw for 20
// time advances to 20 -> sw_timer_on_expire()
//   -> cbC fires; re-arm hw for 30
// time advances to 30 -> sw_timer_on_expire()
//   -> cbA fires; nothing left, hw idle
```

### Multiple due at the same time
```
now = 0
sw_timer_start(50, cbX, ...);
sw_timer_start(50, cbY, ...);   // same expiry

// time advances to 50 -> sw_timer_on_expire()
//   -> BOTH cbX and cbY fire (one pass), then hw idle
```

### Past-due
```
now = 100
sw_timer_start(80, cbZ, ...);   // expiry < now -> fire on next dispatch
// harness dispatches -> cbZ fires
```

## Constraints
- Only one hardware expiry may be armed at a time (`hw_timer_set` overrides the previous).
- One-shot timers only (each fires at most once).
- Support at least 16 concurrent pending software timers.
- All due timers (`expiry <= now`) must fire when `sw_timer_on_expire` runs; then re-arm for the
  next soonest.
- Callbacks are `void (*)(void *arg)`; invoke as `cb(arg)`.
- Pure C, no dynamic allocation required (fixed static storage is fine).
- **You choose the internal data structure** — `solution.c` has no struct/DS handed to you.

## Notes

### Interview Discussion Points (see `timer_design.md` and the DS table in `hints.md`):
1. **Which data structure?** Unsorted list (simple, O(N) find-min), sorted list / delta queue
   (O(1) next, O(N) insert), min-heap (O(log N)), timer wheel (O(1), for thousands). Justify your
   choice by expected timer count.
2. **Why absolute expiry, not relative?** Comparing absolute times avoids drift and makes
   "is it due?" a simple `expiry <= now` check.
3. **Why fire ALL due timers per dispatch?** Several may share an expiry, or a late dispatch may
   leave several overdue. Fire them all, then re-arm once.
4. **The #1 bug:** forgetting to re-arm the hardware for the next timer after dispatching.
5. **Scaling / cancellation / periodic / ISR-safety:** see the follow-up umbrella in
   `timer_design.md`.
6. **Real-world parallel:** FreeRTOS timer service task, Linux `hrtimer`, Zephyr `k_timer` — all
   multiplex many timers onto one time source.
