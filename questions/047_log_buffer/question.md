# 047. printk-style Log Ring Buffer

## Difficulty: Medium

## Problem Statement

Design and implement a minimal **logging framework** for an embedded system: a fixed-size,
**never-blocking, never-allocating ring buffer** with **severity-level filtering** — the same
model as the Linux kernel's `printk` / `dmesg`.

This is a **design** question. You are given only the four function prototypes and the behavior
spec below. **You design everything internal**: how you store a log record, the ring buffer,
the indices, the message copy — all of it. `solution.c` contains plain functions only; there is
no struct or macro handed to you.

> **Read `log_design.md` first.** It explains the ring-buffer model, severity levels, and how
> this maps to the kernel's `printk`/`dmesg`.

---

## The Interface Contract

You must implement exactly these four functions (signatures fixed; internals are yours):

```c
void log_init(void);
void log_set_level(int threshold);
void log_write(int level, unsigned int timestamp, const char *msg);
int  dmesg_read(int *out_level, unsigned int *out_timestamp,
                char *out_msg, int out_msg_size);
```

### Severity levels (numeric contract — lower = more severe)
```
0 = ERR
1 = WARN
2 = INFO
3 = DEBUG
```

### Design constants (you choose these internally, must satisfy the tests)
- **Ring capacity: holds up to 7 unread records.** Use a **power-of-2 array of size 8** with one
  reserved "sentinel" slot (the classic ring-buffer scheme — see the SPSC ring buffer question),
  so `index & (size - 1)` replaces the modulo.
- **Max message length: 31 characters** (plus a NUL terminator, so a 32-byte internal buffer).
- Longer messages must be **truncated** to fit (never overflow).

---

## Behavior Specification

### `log_init(void)`
Reset the logger to empty. After init: no stored records, and the default level threshold is
`DEBUG` (3) — i.e. everything is kept until `log_set_level` narrows it.

### `log_set_level(int threshold)`
Set the runtime severity filter. `log_write` keeps a message only if
`level <= threshold` (this severity or more severe). Example: threshold = `INFO` (2) keeps
ERR/WARN/INFO and drops DEBUG.

### `log_write(int level, unsigned int timestamp, const char *msg)`
The producer (like `printk`):
1. If `level > threshold`, **drop** the message (do nothing).
2. Otherwise store a record containing `level`, `timestamp`, and a **bounded copy** of `msg`
   (truncated to the max message length, always NUL-terminated).
3. If the ring is **full**, **overwrite the oldest** record to make room (advance the read
   position). The newest message must never be lost.

### `dmesg_read(int *out_level, unsigned int *out_timestamp, char *out_msg, int out_msg_size)`
The consumer (like `dmesg`). Pop the **oldest unread** record (FIFO):
1. If there are no unread records, return `0` (and leave outputs untouched).
2. Otherwise, write the record's `level` to `*out_level`, its `timestamp` to `*out_timestamp`,
   and copy its message into `out_msg` (at most `out_msg_size - 1` chars, NUL-terminated).
   Advance past this record and return `1`.

To read the whole log, the caller loops:
```c
int lvl; unsigned int ts; char buf[64];
while (dmesg_read(&lvl, &ts, buf, sizeof buf)) {
    /* consume one record */
}
```

---

## Examples

### FIFO order + filtering
```
log_init();
log_set_level(2);                 // INFO — drop DEBUG
log_write(0, 100, "boot fail");   // ERR  -> kept
log_write(3, 101, "trace x");     // DEBUG -> dropped
log_write(2, 102, "ready");       // INFO -> kept

dmesg_read -> (level=0, ts=100, "boot fail"), returns 1
dmesg_read -> (level=2, ts=102, "ready"),     returns 1
dmesg_read -> returns 0 (empty)
```

### Overwrite oldest when full (capacity 7)
```
log_init();                       // threshold defaults to DEBUG
// write 7 records to fill the ring: A B C D E F G
for ts in 1..7: log_write(2, ts, <letter>);   // ring full: [A B C D E F G]
log_write(2, 8, "H");             // full -> overwrite oldest (A). ring: [B C D E F G H]

dmesg_read -> "B" (ts=2)          // A was overwritten and is gone
dmesg_read -> "C"
...
dmesg_read -> "H"
dmesg_read -> returns 0
```

## Constraints
- No dynamic allocation (`malloc`/`calloc` forbidden) — use fixed static storage.
- No blocking / no I/O inside `log_write` — it just stores into the ring.
- Ring holds up to 7 records (power-of-2 array of size 8 with one sentinel slot); max message length 31 chars + NUL.
- Overwrite the **oldest** record when full (never drop the newest).
- Messages longer than the max must be truncated safely.
- Pure C, only `<string.h>` (for a bounded copy) and `<stdint.h>`-style types if you wish.

## Notes

### Interview Discussion Points (see `log_design.md` for depth):
1. **Why a ring buffer and not `printf`?** `printf`/UART blocks and isn't ISR-safe; a ring
   buffer write is bounded-time and non-blocking.
2. **Why overwrite the oldest instead of dropping the newest?** The most recent events (near a
   crash) are the most valuable; this matches `printk`.
3. **Why severity levels?** Filter noise; turn verbosity down in the field, up in the lab.
4. **What's the concurrency risk?** If an ISR and the main loop both call `log_write`, the shared
   indices can corrupt — a real logger needs a brief critical section or a lock-free SPSC design.
   (Out of scope here; see the follow-up umbrella.)
5. **Linux parallel:** `log_write` = `printk`, `dmesg_read` = `dmesg`, `log_set_level` = console
   loglevel. Same fixed-ring, newest-wins model.
