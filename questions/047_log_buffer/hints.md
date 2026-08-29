# Hints

## Hint 1

- Design a single **state struct** holding the ring array plus its indices, and one static
  instance of it (like `my_circ_buf` in the SPSC question).
- Use a **power-of-2 capacity** so `index & (size - 1)` replaces `index % size`.
- Each log record needs three fields: `level`, `timestamp`, and a fixed `msg[]` buffer
  (31 chars + NUL).
- **Empty condition**: `head == tail`.
- **Overwrite policy**: unlike a plain SPSC buffer that *rejects* on full, this logger keeps a
  free "sentinel" slot and, when full, advances `tail` to **drop the oldest** before writing.
- Copy the message with a **bounded copy** so it never overflows and is always NUL-terminated.

## Hint 2

- Keep the producer/consumer split like the SPSC buffer: `log_write` advances `head`,
  `dmesg_read` advances `tail`. The only extra logic is the overwrite step in `log_write`.
- **Count** = `(head - tail) & mask`.
- **Full** (before overwrite) = count equals `size - 1` (one sentinel slot always free), i.e.
  `((head + 1) & mask) == tail`.
- On a full write: bump `tail = (tail + 1) & mask` first (discard oldest), then write at `head`
  and advance `head`.
- `log_write` filters first: `if (level > threshold) return;`
- `log_set_level` just stores the threshold; `log_init` resets `head`, `tail`, and sets the
  default threshold to DEBUG (3).

> **Why detect "full" if we never reject a write?** Unlike the SPSC buffer (where full → *reject*
> the write and return -1), this logger always accepts the write and **overwrites the oldest**.
> But we still need the full check — for a *different* purpose: to **advance `tail` (evict the
> oldest) before writing**. If you skip it and only ever advance `head`, then after enough writes
> `head` wraps around and collides with `tail` from behind, making `head == tail` — which the
> reader interprets as **empty**, silently losing *all* records instead of just the oldest one.
> So the rule flips: SPSC = "detect full → reject"; logger = "detect full → evict oldest".

## Hint 3

```c
#include <string.h>

#define LOG_CAPACITY 8       /* power of 2; holds up to CAPACITY-1 records */
#define LOG_MSG_MAX  32      /* 31 chars + NUL */

/**
 * Log ring buffer state.
 * - buf[]:      storage array of records
 * - head:       next write position (advanced by log_write)
 * - tail:       oldest record position (advanced by dmesg_read, or by log_write on overwrite)
 * - mask:       size - 1 (for efficient wrap)
 * - size:       number of slots in buf
 * - threshold:  current severity filter (keep level <= threshold)
 */
static struct log_buf {
    struct {
        int          level;
        unsigned int timestamp;
        char         msg[LOG_MSG_MAX];
    } buf[LOG_CAPACITY];
    unsigned int head;
    unsigned int tail;
    unsigned int mask;
    unsigned int size;
    int          threshold;
} my_log_buf;

/* bounded copy: at most cap-1 chars, always NUL-terminated */
static void bounded_copy(char *dst, const char *src, int cap) {
    int i = 0;
    if (cap <= 0) return;
    for (; i < cap - 1 && src[i] != '\0'; i++) dst[i] = src[i];
    dst[i] = '\0';
}

void log_init(void) {
    my_log_buf.size = LOG_CAPACITY;
    my_log_buf.mask = my_log_buf.size - 1;
    my_log_buf.head = 0;
    my_log_buf.tail = 0;
    my_log_buf.threshold = 3;   /* DEBUG: keep everything by default */
}

void log_set_level(int threshold) {
    my_log_buf.threshold = threshold;
}

void log_write(int level, unsigned int timestamp, const char *msg) {
    if (level > my_log_buf.threshold)
        return;                                  /* filtered out */

    /* If full, drop the oldest by advancing tail (overwrite policy). */
    if (((my_log_buf.head + 1) & my_log_buf.mask) == my_log_buf.tail)
        my_log_buf.tail = (my_log_buf.tail + 1) & my_log_buf.mask;

    unsigned int h = my_log_buf.head & my_log_buf.mask;
    my_log_buf.buf[h].level = level;
    my_log_buf.buf[h].timestamp = timestamp;
    bounded_copy(my_log_buf.buf[h].msg, msg, LOG_MSG_MAX);

    my_log_buf.head = (my_log_buf.head + 1) & my_log_buf.mask;
}

int dmesg_read(int *out_level, unsigned int *out_timestamp,
               char *out_msg, int out_msg_size) {
    if (my_log_buf.head == my_log_buf.tail)
        return 0;                                /* empty */

    unsigned int t = my_log_buf.tail & my_log_buf.mask;
    if (out_level)     *out_level = my_log_buf.buf[t].level;
    if (out_timestamp) *out_timestamp = my_log_buf.buf[t].timestamp;
    if (out_msg && out_msg_size > 0)
        bounded_copy(out_msg, my_log_buf.buf[t].msg, out_msg_size);

    my_log_buf.tail = (my_log_buf.tail + 1) & my_log_buf.mask;
    return 1;
}
```

**Key reasoning:** this is the SPSC ring-buffer skeleton (`head`/`tail`/`mask`, power-of-2 wrap,
one sentinel slot) with two additions specific to a logger: a **severity filter** at the top of
`log_write`, and an **overwrite-oldest** step (advance `tail` when full) instead of rejecting.
Two bounded copies — into the ring on write, out of the ring on read — guarantee no overflow at
either boundary. This is the printk/dmesg model.

> Note: with a power-of-2 capacity and one sentinel slot, `LOG_CAPACITY = 8` stores up to 7
> records. If you want to store exactly N records, size the array to the next power of 2 above N.
