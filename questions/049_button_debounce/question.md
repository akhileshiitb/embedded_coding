# 049. Button Debounce

## Difficulty: Medium

## Problem Statement

Implement a **software button debouncer**. A mechanical button's raw signal bounces (rapid
on/off transitions) for a few milliseconds when pressed or released. Your job: turn the noisy
raw samples into a clean, stable **debounced state**, and report a **press event** exactly once
per real press.

This is a pure simulation. The test harness feeds you a sequence of raw samples (0 = released,
1 = pressed), one per "tick," by calling your update function repeatedly.

> **Read `debounce_design.md` first.** It explains bounce, the stability principle, the
> counter and shift-register methods, and how to choose the threshold.

You **design the internal state yourself** — `solution.c` has plain function stubs only, no
struct or macros handed to you.

---

## The Debounce Rule

The debounced state changes to a new level **only after the raw input holds that new level for
`DEBOUNCE_N` consecutive samples**. Use:

```
DEBOUNCE_N = 4      /* need 4 consecutive identical samples to accept a change */
```

- Raw sample: `0` = released, `1` = pressed.
- Initial debounced state after init: `0` (released).

---

## What You Must Implement

```c
/* Reset the debouncer: debounced state = released (0), counters cleared. */
void debounce_init(void);

/* Feed one raw sample (0 or 1). Returns the current DEBOUNCED state (0 or 1)
 * after processing this sample. */
int debounce_update(int raw_sample);

/* Returns 1 exactly once for each debounced RELEASED->PRESSED transition
 * (a "press event"), otherwise 0. Call after debounce_update each tick. */
int debounce_pressed_event(void);
```

---

## Behavior Specification

### `debounce_init(void)`
Reset: debounced state = `0` (released); clear any sample counter and edge-tracking state.

### `debounce_update(int raw_sample)`
1. If `raw_sample` differs from the current **debounced** state, count consecutive occurrences of
   this new level. If the same new level has now occurred `DEBOUNCE_N` times in a row, **commit**
   the change (debounced state becomes the new level).
2. If `raw_sample` equals the current debounced state, reset the "pending change" counter (the
   input agrees with the committed state — any bounce toward the other level is discarded).
3. Return the (possibly updated) debounced state.

### `debounce_pressed_event(void)`
Return `1` if the most recent `debounce_update` **committed** a transition from released (0) to
pressed (1); otherwise return `0`. Each real press yields exactly one `1`. (A press event is
consumed by the call — a subsequent call returns 0 until the next press.)

---

## Examples

`DEBOUNCE_N = 4`. Raw sample stream and resulting debounced output:

### Bounce is rejected
```
raw:        0 0 1 0 1 0 1 1 1 1 0 0 0 0
                  ^ bouncing ^ stable pressed        ^ stable released
debounced:  0 0 0 0 0 0 0 0 0 1 1 1 1 0
                              ^ 4th consecutive 1 -> commit PRESSED
                                      ^ 4th consecutive 0 -> commit RELEASED
press_event fires once, at the tick debounced goes 0->1.
```

### Clean press (no bounce)
```
raw:        0 1 1 1 1 0 0 0 0
debounced:  0 0 0 0 1 1 1 1 0
                    ^ commit PRESSED after 4 consecutive 1s
                            ^ commit RELEASED after 4 consecutive 0s
```

## Constraints
- `DEBOUNCE_N = 4` consecutive identical samples required to accept a state change.
- Raw samples are `0` or `1`; debounced state is `0` or `1`.
- Initial debounced state is `0` (released).
- A press event = a committed `0 -> 1` transition, reported exactly once per press.
- Pure C, fixed static storage (no malloc). You design the internal state.

## Notes

### Interview Discussion Points (see `debounce_design.md`):
1. **Why require N consecutive samples?** The stable window `N × sample_period` must exceed the
   worst-case bounce time, so transient bounce never reaches the committed state.
2. **Counter vs. shift-register method?** Counter: increment/reset an integer. Shift-register:
   `history = (history << 1) | raw`, test for all-1s/all-0s. Both need N stable samples.
3. **Level vs. event?** Debounce yields a clean *level*; the press *event* is an edge
   (0->1 transition) on that clean level — detect it separately.
4. **Latency trade-off?** Larger N = more robust but adds `N × sample_period` input latency.
5. **Hardware alternative?** RC filter + Schmitt trigger debounces before the signal reaches the
   MCU (costs parts, frees CPU).
