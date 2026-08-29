# Watchdog Timer (WDT) — Design Concepts

A reference for the fundamental concepts behind watchdog timers in embedded systems.
Read this before attempting the coding question. Based on the
[Memfault "Firmware Watchdog Best Practices"](https://interrupt.memfault.com/blog/firmware-watchdog-best-practices)
article and practical embedded design experience.

---

## 1. What is a Hardware Watchdog?

A hardware watchdog is a **dedicated timer block** (isolated RTL) inside the SoC/MCU:

- It contains a **down-counter** that the hardware decrements every clock cycle.
- It is typically **disabled by default** — software must configure and enable it.
- Once enabled, software must periodically **reset the counter** ("feed", "kick", or "pet" the dog).
- If the counter reaches **zero** (software failed to pet it in time) → the hardware forces a **SoC reset**.

The rationale: if software can no longer pet the watchdog, the system is not running correctly and must be reset to a known-good state. The watchdog is the **last line of defense**.

> For mission-critical systems, dedicated external watchdog ICs exist. These are typically fed by toggling a GPIO line from the monitored MCU.

---

## 2. Why Use a Watchdog? (Failure Scenarios It Recovers From)

1. **Memory corruption** → code winds up stuck in an infinite `while(1)` loop.
2. **Wedged hardware** → software polls an MMIO status bit forever (e.g., a NOR flash, accelerometer, or HRM that stopped responding) with no timeout.
3. **Deadlock** → two or more tasks block forever because mutexes/spinlocks were grabbed in the wrong order or never released.
4. **Starvation** → a high-priority task hogs the CPU so a low-priority task (e.g., WiFi, sensor publish) never runs and makes no forward progress.

---

## 3. Feeding the Watchdog — Why a "Magic Number"?

The pet/feed operation usually writes a specific **magic value** to a reload register
(e.g., `0x6E524635`), not just any value.

**Why?** If the program counter goes wild (memory corruption, stack smash) and the CPU
executes garbage, a random write to the reload register could *accidentally* feed the dog
— masking the very hang the watchdog exists to catch. Requiring a specific, unusual magic
value makes accidental feeding statistically near-impossible. It makes the "I am
deliberately alive" intent explicit.

---

## 4. Watchdog Configuration Nuances (Read the Datasheet!)

Watchdog peripherals are **vendor-specific**. Always check the datasheet for:

### 4a. When does the configuration reset?
Two common models:
- **Re-configurable**: can be enabled/disabled multiple times; must be configured on each boot.
- **Write-once / config-lock**: once enabled, config registers (timeout, reload settings) are
  **locked until a reset** occurs. You get one shot to configure it.

### 4b. A soft reset does NOT always disable the watchdog
A classic bug: the main application enables the watchdog, then a soft reset jumps to a
bootloader that is *unaware* the watchdog is running. If the bootloader does a long operation
(e.g., a firmware update), the still-running watchdog resets the system **mid-update**. Only
certain resets (like a Power-On-Reset) clear the watchdog on some chips.

### 4c. Choosing the timeout value
Since the watchdog is a *last* line of defense, choose a timeout **much larger** than the
longest legitimate event the system handles.
- Rule of thumb: **5–30 seconds**.
- Too short → false resets during normal long operations.
- Too long → slow recovery from a real hang.

---

## 5. Watchdog Behavior With a Debugger (JTAG)

Problem: you attach JTAG, halt the CPU at a breakpoint — but the watchdog keeps counting down
→ it bites → SoC resets while you're debugging.

Solutions:
- **Hardware clock gate/pause**: many MCUs (e.g., STM32 `DBGMCU`, NRF52) can be configured to
  **automatically pause the watchdog clock** while the core is halted by the debugger.
- **Debugger register writes**: if no HW pause exists, configure the debugger (e.g., GDB
  Python hooks) to issue register writes that pause or feed the peripheral on halt/continue.

> Anti-pattern: disabling the watchdog entirely in debug builds. This lets hangs go unnoticed
> until a release build ships. Prefer the pause mechanisms above.

---

## 6. What Happens When the Watchdog Expires?

Common hardware designs:

- **Design 1 — Immediate reset**: the watchdog timeout routes directly to the SoC/PMIC reset
  pin. Simple, but you get *no* diagnostics about *why* it hung.
- **Design 2 — "Bark then Bite" (pretimeout interrupt)**: when the counter gets *near* zero,
  the HW raises an interrupt (the "bark"). An ISR can collect diagnostics (stack frame,
  coredump, which task was stuck) before the eventual forced reset (the "bite").

---

## 7. How Do I Know a Watchdog Reset Happened? (Reset Reason Register)

Almost every SoC exposes a **reset-reason register** (names vary: `RESETREAS` on NRF52,
`RCC_CSR` on STM32F, `PWRSEQ_FLAGS` on MAX32, etc.). One bit indicates a watchdog reset.

**Critical detail — the register is "sticky":**
- After reading it on boot, you must **clear it** (usually **write-1-to-clear**).
- If you don't clear it, the bit persists across every reboot and you'll misattribute future
  resets to the watchdog forever (until a full Power-On-Reset).

This lets firmware distinguish: power-on reset vs. watchdog reset vs. software-requested reset.

---

## 8. The Naive Watchdog and Its Limitation

Simplest scheme: one low-priority task loops and unconditionally pets the watchdog.

```
while (1) {
    delay(1s);
    hardware_watchdog_feed();
}
```

- ✅ Catches infinite loops in higher-priority code (the low-priority task never runs → no feed → reset).
- ✅ Catches starvation (low-priority task starved → no feed → reset).
- ❌ **CANNOT catch a deadlock.** If a high-priority task blocks forever waiting on a mutex,
  the RTOS simply schedules the low-priority watchdog task, which keeps happily feeding the
  dog — the hang goes **undetected**.

---

## 9. The Task Check-In Watchdog (Solves the Deadlock Gap)

Instead of unconditionally feeding, monitor that **every task is making forward progress**:

- Maintain two bitmasks: `registered_tasks` and `checked_in_tasks` (one bit per task).
- Each task **registers** itself (sets its bit in `registered_tasks`).
- Each task periodically **checks in** (sets its bit in `checked_in_tasks`) to signal "I'm making progress".
- A dedicated **watchdog task** runs periodically and calls a check function:
  - If `(checked_in_tasks & registered_tasks) == registered_tasks` → **all registered tasks
    checked in** → feed the HW watchdog, then clear `checked_in_tasks` for the next round.
  - Otherwise → do **not** feed → the HW watchdog eventually bites → reset.

This catches deadlocks: the stuck task never sets its check-in bit, so the check fails and the
system resets even though other tasks (including the watchdog task) are running fine.

### Task lifecycle (event-loop pattern)
```
while (1) {
    unregister_task(id);      // 1. not doing work — don't monitor me
    wait_for_work();          // 2. block until there's work
    register_task(id);        // 3. work arrived — start monitoring me
    // 4. do the work — may take time, or may HANG (that's what we detect)
    checkin_task(id);         // 5. signal progress
}                             //    loop back to 1
```

> Concurrency note: the bitmasks are shared between tasks and possibly interrupts, so updates
> must be atomic (disable interrupts briefly, or use atomic builtins). See follow-up on
> critical sections below.

---

## 10. The Three-Layer Model (Context)

A mature watchdog subsystem has three layers:

1. **Hardware watchdog** — the silicon down-counter that forces a reset. Dumb but reliable.
2. **Task watchdog** — the software bitmask check-in layer (Section 9) that decides *whether*
   to feed the hardware watchdog based on forward progress.
3. **Software watchdog** — a *separate* peripheral timer set slightly shorter than the HW
   timeout, whose ISR runs *before* the HW bite to collect diagnostics (coredump, stack trace).

> This document and the coding question focus on layers **1 and 2**. The software watchdog
> (layer 3) is out of scope for the exercise but good to know exists.

---

## 11. Watchdogs in the Linux Kernel (Conceptual)

Embedded Linux systems use the same hardware watchdog, but the plumbing is different.

### The `/dev/watchdog` character device
- The kernel's **watchdog framework** (`drivers/watchdog/`) exposes each HW watchdog as a
  character device: `/dev/watchdog` (or `/dev/watchdog0`, `1`, …).
- **Opening** `/dev/watchdog` **arms** the watchdog (starts the countdown).
- **Writing any data** to the device **pings/feeds** it (resets the timeout).
- **Closing** it normally would stop the watchdog — but to prevent a crashed process from
  silently disarming protection, the kernel uses a **"magic close"**: the watchdog is only
  cleanly disarmed if the magic character **`'V'`** was written before closing. Otherwise, on
  close the watchdog keeps running (fail-safe). This is the kernel's equivalent of the
  magic-number idea from Section 3.
- `ioctl()` calls configure timeout, query capabilities, and read the "boot reason" (whether
  the last boot was caused by a watchdog reset).

### The kernel driver interface: `struct watchdog_ops`
A hardware watchdog driver registers a `watchdog_device` with an ops table:
```
struct watchdog_ops {
    int (*start)(struct watchdog_device *);
    int (*stop)(struct watchdog_device *);
    int (*ping)(struct watchdog_device *);       // feed
    int (*set_timeout)(struct watchdog_device *, unsigned int);
    unsigned int (*get_timeleft)(struct watchdog_device *);
    ...
};
```
This is the same `start` / `stop` / `ping` (feed) abstraction we use in the coding question —
the kernel just standardizes it across all vendor watchdog chips behind one API. (Note the
parallel to the generic `driver_ops` pattern from the UART/SPI/I2C driver questions.)

### Who pets it in userspace? — systemd
- **`RuntimeWatchdogSec=`** (in `system.conf`): systemd itself opens `/dev/watchdog` and pings
  the **hardware** watchdog at half this interval. If systemd (PID 1) hangs or the kernel
  locks up, the HW watchdog reboots the box.
- **Per-service `WatchdogSec=`** + `sd_notify(WATCHDOG=1)`: an individual service sends a
  **heartbeat** to systemd. If the service is alive as a process but **stuck/unresponsive**
  (no heartbeat), systemd acts per policy — commonly `Restart=on-watchdog` to restart just
  that service, or escalate to `FailureAction=reboot`.
- The classic userspace **`watchdog(8)` daemon** is an older alternative: it opens
  `/dev/watchdog` and keeps writing to keep the kernel from resetting, and can run repair
  scripts on failure.

### The layered picture in Linux
```
Service heartbeat  ── sd_notify(WATCHDOG=1) ──▶ systemd (per-service WatchdogSec)
                                                     │
systemd ── writes /dev/watchdog (RuntimeWatchdogSec) ▶ kernel watchdog framework
                                                     │
                                          watchdog_ops.ping() ──▶ HW watchdog counter
                                                     │
                                       (no ping) ────▶ SoC reset
```
The same "forward progress → feed, no progress → reset" principle applies at every layer —
just with more software indirection than a bare-metal MCU.

---

## 12. Two Must-Know Follow-Ups: Windowed Watchdog & Independent Clock

These two concepts come up in almost every watchdog interview beyond the basics. They are
*not* part of the coding question, but you should understand them well.

### 12a. Windowed Watchdog (WWDG) — add a lower bound

A basic watchdog is **single-sided**: it only has an *upper* bound — pet before the timeout or
you reset. It only detects "too slow / stopped."

A **windowed watchdog** adds a *lower* bound, splitting the feed period into two windows:
- **CLOSED window** (too early): petting here is illegal → **reset**.
- **OPEN window** (just right): petting here is the *only* valid feed.
- **Past the open window** (too late): timeout → **reset**.

```
|<---- CLOSED (too early) ---->|<--- OPEN (valid feed) --->| timeout
0                            Wlow                        Whigh -> reset
        pet here => RESET            pet here => OK           no pet => RESET
```

**Why it's stronger than a single-sided watchdog:**
- Consider code stuck in a tight loop that *happens to include the pet call*. A single-sided
  watchdog is fed on every iteration and **never fires** — the bug is completely masked.
- A windowed watchdog catches this: the runaway loop pets **too fast**, lands in the CLOSED
  window, and triggers a reset. It detects a **timing/frequency anomaly**, not just a total stop.
- In short: single-sided catches "the code stopped"; windowed also catches "the code is running
  at the wrong rate."

STM32 exposes both flavors as separate peripherals: `IWDG` (simple, single-sided) and `WWDG`
(windowed). On the STM32 WWDG, a reset occurs if the counter is reloaded outside the time
window (or if it counts down past its floor).

**Trade-off:** windowed watchdogs need more careful timing analysis — your feed cadence must be
stable enough to always land in the open window, which is harder in an event-driven system with
variable loop times.

### 12b. Independent Clock Source — survive a main-clock failure

A robust watchdog is clocked from a **separate, independent low-speed oscillator** (e.g., the
STM32 IWDG runs on the ~40 kHz LSI RC), *not* from the main system/CPU clock.

**Why this matters:**
- If the watchdog were clocked from the **same** PLL/main clock as the CPU, then a main-clock
  failure or glitch would freeze the watchdog counter too — it would **never fire**, exactly
  when you need it most. That's a single point of failure.
- An independent clock guarantees the watchdog keeps counting and can still reset the SoC even
  during a **total main-clock failure** or when the core is in a deep low-power mode (STOP/STANDBY).
- The independent oscillator is also very low power, so the watchdog barely affects the power budget.

**Interview phrasing:** *"Why not just clock the watchdog from the CPU clock?"* →
Because then a clock failure disables both the CPU and its safety net simultaneously. The
watchdog must be able to observe and recover from failures of the very subsystem it monitors,
so it needs its own clock domain.

> The naming reflects this: STM32's **I**WDG = **Independent** Watchdog (own clock, single-sided);
> **W**WDG = **Window** Watchdog (APB-clocked, windowed). Robust designs often use *both* together.

---

## 13. Follow-Up Questions (Umbrella — Advanced / Conceptual)

These are good interview follow-ups and deeper design topics beyond the core coding question:

- **Bark/bite (pretimeout)**: use a pretimeout interrupt to collect diagnostics before the
  reset. Beware: some HW gives only a *tiny*, fixed number of cycles after the bark before the
  forced bite — if your cleanup is too slow, the reset fires mid-cleanup. Keep the bark handler
  minimal.
- **Hangs inside an ISR**: make the watchdog's bark interrupt the **highest priority** so it
  can preempt a hung lower-priority ISR and still collect diagnostics.
- **Critical-section subtlety (Cortex-M)**: if the task-watchdog protects its bitmasks with
  `__disable_irq()`, a hang *inside* that critical section can't be caught (the watchdog
  interrupt is masked too). On Cortex-M3+, use `BASEPRI` (`__set_BASEPRI`) for critical
  sections instead, and give the watchdog interrupt a priority above `BASEPRI` — then it can
  fire even inside a critical section.
- **Multiple hardware reload registers**: some watchdogs (e.g., NRF52) have several reload
  request registers; *all* enabled ones must be fed. This is a hardware form of the task
  check-in mechanism.
- **Reset-reason handling on boot**: read and clear the sticky reset-reason register early in
  boot; log/report if the last reset was a watchdog.
