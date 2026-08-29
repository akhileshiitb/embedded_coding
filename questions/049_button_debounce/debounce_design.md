# Button Debounce — Design Concepts

A reference for debouncing mechanical switch inputs in embedded systems. Read this before
attempting the coding question.

---

## 1. What Is Switch Bounce?

When you press or release a mechanical button, the metal contacts do **not** make or break cleanly.
They physically **bounce** — rapidly making and breaking contact for typically **1–50 ms** before
settling. To an MCU sampling GPIO at MHz speeds, one physical press looks like **dozens of rapid
presses**:

```
Ideal press:     ________|‾‾‾‾‾‾‾‾‾‾‾‾‾‾
Real (bouncy):   ________|‾|_|‾‾|_|‾‾‾‾‾‾   <- multiple edges from one press
                         ^ contact bounce (a few ms)
```

Without debouncing, a single button press registers as many presses → menus skip, counters
jump, state machines misfire.

---

## 2. Why It Matters (and Why It's a Classic Question)

Debounce sits exactly at the **hardware/software boundary** — it forces you to reason about:
- physical signal behavior (bounce duration),
- sampling rate vs. bounce time,
- state stability over time,
- `volatile` GPIO reads and ISR-vs-polling.

That's why it's a staple embedded topic. The core skill it tests: **turning a noisy real-world
signal into a clean logical event.**

---

## 3. The Core Idea — Require Stability Over Time

Every debounce method is a variation on one principle:

> **Only accept a new state after the raw input has held that state for long enough.**

"Long enough" must exceed the worst-case bounce time. If a switch bounces for up to `T` ms and you
sample every `X` ms, you must see the new level stable across enough samples that
`(samples × X) > T`.

Two common ways to measure "stable":

### 3a. Counter / integrator method
Keep a counter. Each tick:
- if the raw sample equals the *candidate* new state, increment the counter,
- else reset it.
When the counter reaches a threshold `N` (N consecutive agreeing samples), commit the new
debounced state.

### 3b. Shift-register method
Shift each raw sample into an integer: `history = (history << 1) | raw`. When the low bits are all
1s (e.g. `history & 0xFF == 0xFF`), the input has been high for 8 straight samples → stable high;
all 0s → stable low. Elegant and branch-light, popular in firmware.

Both require the same thing: **N consecutive identical samples** before changing the output.

---

## 4. Choosing Sample Rate and Threshold

- **Sample period `X`**: usually driven by a periodic timer/tick (e.g., every 1–5 ms). Fast enough
  to feel responsive, slow enough not to waste CPU.
- **Threshold `N`**: pick so `N × X` comfortably exceeds max bounce time. Example: 5 ms bounce,
  1 ms sampling → `N = 8` samples (8 ms) is a safe, common choice.
- **Latency trade-off**: larger `N` = more robust but adds `N × X` ms of input latency. Smaller
  `N` = snappier but risks accepting bounce. This tuning is the design judgment interviewers probe.
- Some designs use **asymmetric thresholds** (e.g., debounce press faster than release, or vice
  versa) depending on the application.

---

## 5. Hardware vs. Software Debounce

- **Hardware**: an RC filter + Schmitt trigger, or an SR latch on a SPDT switch, smooths bounce
  before it reaches the MCU. Costs parts/board space; frees CPU.
- **Software** (this question): sample the raw pin periodically and apply one of the algorithms
  above. No extra hardware; costs a little CPU and adds latency. Most common in cost-sensitive
  designs.

A good answer knows both exist and picks based on cost, pin count, and CPU budget.

---

## 6. Edge Detection — Level vs. Event

Debouncing produces a clean **level** (stable pressed/released). Often you also want an **event**:
"a press just happened." That's a **rising/falling edge** on the debounced level:

- `pressed_event = (debounced == PRESSED) && (prev_debounced == RELEASED)`

Separating "stable level" from "edge event" keeps the logic clean — debounce first, then detect
edges on the clean signal.

---

## 7. Follow-Up Topics (Umbrella)

- **Interrupt + timer hybrid**: use a GPIO edge interrupt to *start* a debounce timer, then sample;
  avoids polling when idle (power saving), but must handle bounce re-triggering the interrupt.
- **Multiple buttons**: debounce a whole port at once with the shift-register method operating on
  bytes/words, one bit per button.
- **`volatile` and ISR safety**: if the raw read happens in an ISR and the debounced state is read
  in the main loop, the shared state needs `volatile` and/or a brief critical section.
- **Matrix keypads**: scanning rows/columns adds ghosting/masking concerns on top of debounce.
- **Long-press / double-click**: built as a small state machine *on top of* the clean debounced
  edge events.
