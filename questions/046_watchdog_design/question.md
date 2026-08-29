# 046. Watchdog Timer Driver with Task Check-In

## Difficulty: Hard

## Problem Statement

Implement a **bare-metal watchdog timer (WDT) driver** against the simulated hardware register
spec below, plus a **task check-in layer** that only feeds the hardware watchdog when all
registered tasks have made forward progress.

This is a pure simulation — there is no real hardware. The test harness models the watchdog
counter and "ticks" it forward, triggering a simulated SoC reset when the counter reaches zero.

> **Read `wdt_design.md` first.** It explains the concepts (why watchdogs exist, the magic-feed,
> the naive-watchdog limitation, and the task check-in mechanism that catches deadlocks).

---

## Part A — Hardware Watchdog Register Interface

### Simulated WDT Register Map

Access registers via a `volatile struct wdt_regs *` (provided to your functions):

| Register | Access | Reset | Description |
|----------|--------|-------|-------------|
| `CTRL`   | R/W    | 0     | Control. Bit [0] = `EN` (1 = watchdog enabled/running). |
| `LOAD`   | R/W    | 0     | Reload value: number of ticks before timeout. |
| `COUNT`  | R/W    | 0     | Current down-counter. Hardware decrements every tick. |
| `KICK`   | W      | 0     | Feed register. Writing the magic value reloads `COUNT` ← `LOAD`. |
| `RESET`  | R/W    | 0     | Sticky reset flag. Bit [0] set by hardware when a watchdog reset fires. |

### Bit / value definitions
```c
#define WDT_CTRL_EN     (1U << 0)      /* CTRL enable bit */
#define WDT_KICK_MAGIC  0x5A5A5A5AU    /* magic value to feed the watchdog */
#define WDT_RESET_FLAG  (1U << 0)      /* RESET sticky flag bit */
```

### How the simulated hardware behaves (done by the test harness — for your understanding)
- Each **tick**, if `CTRL.EN == 1`, hardware decrements `COUNT` by 1.
- When `COUNT` reaches `0`, hardware sets `RESET.WDT_RESET_FLAG` (a simulated SoC reset) and
  stops counting.
- Writing `WDT_KICK_MAGIC` to `KICK` reloads `COUNT = LOAD`. Writing any **other** value to
  `KICK` does nothing (models the magic-number safety — a wild write must not accidentally feed).

---

## Part B — Task Check-In Layer

A naive watchdog that is fed unconditionally cannot detect a **deadlock** (a stuck task while
other tasks keep running and feeding). To fix this, track per-task forward progress with bitmasks:

- `registered_tasks` — bit set = task is currently being monitored.
- `checked_in_tasks` — bit set = task has reported progress this round.

A dedicated **watchdog task** periodically calls a check function that feeds the hardware
**only if every registered task has checked in** — otherwise it withholds the feed and the
hardware watchdog eventually resets the system.

Task IDs are `0..31` (one bit each in a `uint32_t`).

---

## What You Must Implement

```c
/* ---- Part A: hardware watchdog ---- */
void wdt_enable(volatile struct wdt_regs *regs, uint32_t timeout_ticks);
void wdt_pet(volatile struct wdt_regs *regs);
void wdt_disable(volatile struct wdt_regs *regs);

/* ---- Part B: task check-in layer ---- */
void wdt_task_init(void);                 /* reset both bitmasks to 0 */
void wdt_register_task(int task_id);
void wdt_unregister_task(int task_id);
void wdt_task_checkin(int task_id);

/* Called by the watchdog task. Feeds the HW watchdog (wdt_pet) ONLY if all
 * registered tasks have checked in. If it feeds, it clears the check-in mask
 * for the next round. Returns 1 if it fed the watchdog, 0 if it withheld. */
int wdt_task_check(volatile struct wdt_regs *regs);
```

---

## Behavior Specification

### `wdt_enable(regs, timeout_ticks)`
1. Set `LOAD = timeout_ticks`.
2. Set `COUNT = timeout_ticks` (start fully loaded).
3. Set `CTRL.EN = 1`.

### `wdt_pet(regs)`
1. Write `WDT_KICK_MAGIC` to `KICK` (this reloads `COUNT = LOAD` in hardware).

### `wdt_disable(regs)`
1. Clear `CTRL.EN` (bit 0). Other bits unchanged.

### `wdt_task_init(void)`
1. Reset `registered_tasks = 0` and `checked_in_tasks = 0`.

### `wdt_register_task(task_id)`
1. Set bit `task_id` in `registered_tasks`.

### `wdt_unregister_task(task_id)`
1. Clear bit `task_id` in both `registered_tasks` and `checked_in_tasks`.

### `wdt_task_checkin(task_id)`
1. Set bit `task_id` in `checked_in_tasks`.

### `wdt_task_check(regs)`
1. If `(checked_in_tasks & registered_tasks) == registered_tasks` (all registered tasks
   checked in — note this is trivially true when nothing is registered):
   - Call `wdt_pet(regs)`.
   - Clear `checked_in_tasks = 0` for the next round.
   - Return `1`.
2. Otherwise, do not feed. Return `0`.

---

## Examples

### Happy path
```
wdt_enable(regs, 10);          // COUNT=10, running
wdt_register_task(0);
wdt_register_task(1);
// each round both tasks check in, then watchdog task runs:
wdt_task_checkin(0);
wdt_task_checkin(1);
wdt_task_check(regs) -> 1      // all in -> pets HW, COUNT reloads to 10
```

### Deadlock path (the key case)
```
wdt_enable(regs, 5);
wdt_register_task(0);
wdt_register_task(1);
// task 1 is deadlocked and never checks in:
wdt_task_checkin(0);
wdt_task_check(regs) -> 0      // task 1 missing -> HW NOT fed
// ... ticks elapse, COUNT hits 0 -> RESET flag set -> SoC reset
```

## Constraints
- Access hardware registers only through the provided `volatile struct wdt_regs *`.
- Task IDs are in range `0..31`.
- Bitmask state (`registered_tasks`, `checked_in_tasks`) is file-scope `static` in your solution.
- Pure C, no libraries beyond `<stdint.h>`.
- Feeding must use the magic value — a non-magic write must not feed.

## Notes

### Interview Discussion Points (see `wdt_design.md` §13 for the follow-up umbrella):
1. **Why the magic number for feeding?** Prevents a wild/corrupted write from accidentally
   petting the dog and masking a real hang.
2. **Why can't a naive watchdog catch deadlocks?** A low-priority feeder task keeps running and
   feeding while a higher-priority task is blocked forever. The check-in bitmask fixes this.
3. **Why clear `checked_in_tasks` after feeding?** Each monitoring round must require *fresh*
   check-ins; otherwise a task that checked in once would look alive forever.
4. **Concurrency**: in real firmware the bitmasks are shared across tasks/ISRs and need atomic
   updates (disable IRQ briefly or atomic builtins). Out of scope for this single-threaded sim,
   but expect the follow-up.
5. **Linux parallel**: `wdt_enable`/`wdt_pet`/`wdt_disable` mirror the kernel's
   `watchdog_ops` `start`/`ping`/`stop`. See `wdt_design.md` §11.
