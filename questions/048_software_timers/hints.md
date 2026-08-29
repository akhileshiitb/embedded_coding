# Hints

## Hint 1 — Choose your data structure (the key decision)

You must store pending software timers `{expiry, cb, arg}` and repeatedly answer: *"which is the
soonest?"* (to arm the hardware) and *"which are now due?"* (to dispatch). The trade-off is
between **insert cost** and **find-soonest / expiry cost**:

| Data structure | `start` (insert) | find soonest | dispatch due | Space | When to use |
|----------------|------------------|--------------|--------------|-------|-------------|
| **Unsorted array/list** | O(1) | O(N) scan | O(N) scan | O(N) | Simplest. A handful of timers (typical MCU). **Recommended for this 30-min question.** |
| **Sorted array** | O(N) shift | O(1) (front) | O(1)+shift | O(N) | Fast peek; costly insert due to shifting. |
| **Sorted linked list / delta queue** | O(N) find spot | O(1) (head) | O(1) pop head | O(N) | Classic embedded structure; cheap dispatch, O(N) insert. |
| **Min-heap (priority queue)** | O(log N) | O(1) peek | O(log N) pop | O(N) | Many timers; best general-purpose balance. |
| **Timer wheel** | O(1) amortized | O(1) | O(1) | O(wheel) | Thousands–millions of timers (OS kernels, network stacks). Overkill here. |

For **N up to ~16** (this question), an **unsorted fixed array** is the pragmatic choice: trivial
insert, and an O(N) scan over 16 entries is nothing. In an interview, state this reasoning
explicitly, then mention the heap/wheel as the scaling answer.

## Hint 2 — The three operations

Whatever structure you pick, the logic is the same:

- **`sw_timer_init`**: mark all slots free (empty set).
- **`sw_timer_start(expiry, cb, arg)`**:
  1. store the timer in a free slot,
  2. find the **minimum expiry** across all active timers,
  3. `hw_timer_set(min_expiry)` to arm the hardware for the soonest.
- **`sw_timer_on_expire`**: let `now = hw_get_time()`.
  1. loop over all active timers; for each with `expiry <= now`, mark it free and call `cb(arg)`,
  2. after firing all due ones, find the new minimum expiry among the survivors and
     `hw_timer_set(min)`; if none remain, don't arm.

Edge cases to get right: **fire all due timers** (not just one) per dispatch; **re-arm** for the
next soonest after dispatching (the #1 bug is forgetting this); **past-due** (`expiry <= now`)
must still arm so it dispatches.

## Hint 3 — Full solution (unsorted fixed array)

```c
extern unsigned int hw_get_time(void);
extern void         hw_timer_set(unsigned int expiry);

#define MAX_TIMERS 16

struct sw_timer {
    unsigned int expiry;
    void (*cb)(void *arg);
    void *arg;
    int  active;
};

static struct sw_timer s_timers[MAX_TIMERS];

/* Arm the hardware for the soonest active timer (if any). */
static void arm_next(void) {
    int found = 0;
    unsigned int soonest = 0;
    for (int i = 0; i < MAX_TIMERS; i++) {
        if (s_timers[i].active) {
            if (!found || s_timers[i].expiry < soonest) {
                soonest = s_timers[i].expiry;
                found = 1;
            }
        }
    }
    if (found) hw_timer_set(soonest);
    /* if none active, leave the hardware idle */
}

void sw_timer_init(void) {
    for (int i = 0; i < MAX_TIMERS; i++) s_timers[i].active = 0;
}

void sw_timer_start(unsigned int expiry, void (*cb)(void *arg), void *arg) {
    for (int i = 0; i < MAX_TIMERS; i++) {
        if (!s_timers[i].active) {
            s_timers[i].expiry = expiry;
            s_timers[i].cb = cb;
            s_timers[i].arg = arg;
            s_timers[i].active = 1;
            break;
        }
    }
    arm_next();                    /* re-arm for the soonest (maybe this one) */
}

void sw_timer_on_expire(void) {
    unsigned int now = hw_get_time();
    /* Fire ALL due timers. Snapshot cb/arg before firing in case a callback
     * starts new timers or reuses slots. */
    for (int i = 0; i < MAX_TIMERS; i++) {
        if (s_timers[i].active && s_timers[i].expiry <= now) {
            void (*cb)(void *) = s_timers[i].cb;
            void *arg = s_timers[i].arg;
            s_timers[i].active = 0;    /* remove before calling (one-shot) */
            cb(arg);
        }
    }
    arm_next();                    /* re-arm for the next soonest survivor */
}
```

**Key reasoning:**
- The **unsorted array** makes `start` O(1) and both find-soonest and dispatch O(N) — perfect for
  a small N, and trivially correct.
- `arm_next()` centralizes the "arm the hardware for the minimum expiry" rule, called after both
  `start` and `on_expire` — this is what keeps the single hardware timer always pointed at the
  soonest pending event.
- Marking a timer inactive **before** invoking its callback keeps one-shot semantics correct even
  if the callback registers a new timer.
- To scale to thousands of timers, swap the array for a **min-heap** (O(log N) insert, O(1) peek)
  or a **timer wheel** (O(1)) — the three-function shape stays identical.
