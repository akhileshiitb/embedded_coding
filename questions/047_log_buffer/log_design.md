# Embedded Logging Framework — Design Concepts

A reference for the fundamentals behind a lightweight logging framework for embedded systems,
influenced by the Linux kernel `printk` / `dmesg` design. Read this before attempting the
coding question.

---

## 1. Why Logging Is Hard on Embedded

On a device in the field — no debugger, no GDB, no logic analyzer — logs are your **only**
window into what happened before a crash or misbehavior. But embedded logging is constrained:

- **Limited RAM** — no room for large string buffers; you get a small fixed budget.
- **Limited flash** — can't store endless verbose strings.
- **Real-time constraints** — logging must not block or delay an ISR or a control loop.
- **Often no filesystem** — nowhere to "write a file"; logs live in RAM.

A good embedded logger is defined as much by what it **refuses** to do as by what it does.

---

## 2. The Core Principles (What a Good Logger Must Guarantee)

1. **Never block** — safe to call from an ISR or real-time context. No waiting on UART, no
   waiting on a consumer. → use a **ring buffer**, not a direct `printf` to a slow peripheral.
2. **Never allocate** — no `malloc` (many systems have no heap, and heap in an ISR is unsafe).
   → a **fixed, statically-sized** buffer.
3. **Bounded memory, never lose the newest** — when the buffer fills, **overwrite the oldest**
   entry rather than dropping the new one or growing. The most recent events (usually the ones
   near a crash) are the most valuable.
4. **Severity filtering** — attach a level to every message and drop messages below a threshold,
   so you can turn verbosity up/down without recompiling (runtime) or compile debug logs out
   entirely (compile-time).

---

## 3. Severity Levels

Like the kernel, we rank messages by severity. **Lower number = more severe** (kernel spirit):

| Value | Level | Meaning |
|-------|-------|---------|
| 0 | `ERR`   | Something failed; needs attention. |
| 1 | `WARN`  | Unexpected but recoverable. |
| 2 | `INFO`  | Normal operational milestones. |
| 3 | `DEBUG` | Verbose developer detail. |

A **threshold** means "keep this severity and everything *more* severe." E.g. threshold = `INFO`
keeps ERR, WARN, INFO and **drops** DEBUG. This is exactly `dmesg -n <level>` / the kernel's
console loglevel.

---

## 4. The Ring Buffer (Heart of the Design)

A ring (circular) buffer is a fixed-size array with two indices:
- a **write position** (where the next record goes),
- a **read position** (where the next unread record is),
- and a **count** of unread records (or use head/tail comparison).

Behavior:
- **Write**: place the record at the write index, advance it (wrapping around at the end).
- **When full**: advance the **read** index too — this *discards the oldest* record so the new
  one fits. This is the "overwrite oldest" policy.
- **Read**: return the record at the read index, advance it, decrement count.

Because the size is fixed and indices wrap with modulo, memory use is bounded and constant — no
allocation, no blocking.

---

## 5. Influence from Linux: `printk` and `dmesg`

The Linux kernel's log buffer (`log_buf`) is the canonical example of this design:

- **`printk(level, "...")`** is the *producer*. It writes a message tagged with a log level
  (`KERN_ERR`, `KERN_WARNING`, `KERN_INFO`, `KERN_DEBUG`, …) into a **fixed-size ring buffer**
  in kernel memory. It never blocks and never allocates.
- **When the ring fills, the newest overwrites the oldest** — exactly our overwrite policy. This
  is why very early boot messages can scroll out of `dmesg` on a busy system.
- **`dmesg`** is the *consumer*. It reads the buffer out for display. The buffer preserves
  messages from early boot until userspace (syslog) can drain them to persistent storage.
- The buffer size is a compile-time kernel config (`CONFIG_LOG_BUF_SHIFT`).

Our coding question is a **minimal bare-metal version of this exact pattern**:

| This coding question | Linux kernel |
|----------------------|--------------|
| `log_write(level, ts, msg)` | `printk(KERN_xxx, "...")` — the producer |
| fixed ring buffer, overwrite oldest when full | `log_buf` ring buffer, newest overwrites oldest |
| `log_set_level(threshold)` | console loglevel / `dmesg -n` |
| `dmesg_read(...)` draining the ring | `dmesg` reading `log_buf` |
| severity levels (ERR/WARN/INFO/DEBUG) | `KERN_ERR` … `KERN_DEBUG` |

Understanding this mapping lets you say in an interview: *"This is the printk/dmesg model — a
lock-free-friendly fixed ring buffer with severity levels, where the producer never blocks and
the newest data wins when space runs out."*

---

## 6. Follow-Up Topics (Umbrella — Out of Scope for the Coding Question)

These are important in a real logger but deliberately excluded to keep the exercise ~30 minutes:

- **Compile-time level filtering**: wrap `log_write` in a macro guarded by `#if` on a build-time
  `LOG_COMPILE_LEVEL`, so debug logs generate **zero code** in a release build (no runtime check,
  no string stored in flash). This is how you get "free" debug logging you can strip for release.
- **ISR-safety / concurrency**: if an ISR and the main loop both write, the indices are shared
  state and can corrupt. Real loggers protect the write with a brief interrupt-disable, atomics,
  or a lock-free single-producer/single-consumer design (the kernel uses a lock-free ring). This
  is the same critical-section concern as the watchdog task check-in.
- **Binary / tokenized logging**: instead of formatted strings, store an *event ID* + timestamp +
  raw args, and reconstruct the text off-device using a token dictionary. This is **10–100× more
  efficient** in RAM/flash/bandwidth than storing full strings — critical for high-rate logging.
- **Post-mortem / persistent logs**: place the ring in a **no-init RAM** section that is *not*
  cleared on reboot, so the logs leading up to a crash survive the reset and can be read out on
  the next boot (pairs naturally with a watchdog reset — see the WDT design doc).
- **Formatted (`printf`-style) logging**: real loggers accept a format string + varargs. We use a
  plain pre-formatted string to keep the exercise focused on the ring-buffer/level design.
- **Timestamps source**: here the caller passes a tick; a real system reads a monotonic timer.
  Monotonic (not wall-clock) is important so log ordering is stable across clock adjustments.
