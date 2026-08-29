#include <string.h>

/*
 * Sample solution — printk-style log ring buffer.
 *
 * SPSC-ring style (see question 024): a single named state struct with
 * head/tail/mask/size, power-of-2 capacity, one sentinel slot. Two logger-
 * specific additions vs. a plain SPSC buffer:
 *   1) severity filtering at the top of log_write
 *   2) overwrite-oldest (advance tail when full) instead of rejecting
 */

#define LOG_CAPACITY 8       /* power of 2; holds up to CAPACITY-1 = 7 records */
#define LOG_MSG_MAX  32      /* 31 chars + NUL */

/**
 * Log ring buffer state.
 * - buf[]:      storage array of records
 * - head:       next write position (advanced by log_write)
 * - tail:       oldest record position (advanced by dmesg_read, or by
 *               log_write on overwrite)
 * - mask:       size - 1 (for efficient wrap: index & mask)
 * - size:       number of slots in buf (power of 2)
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
    for (; i < cap - 1 && src[i] != '\0'; i++)
        dst[i] = src[i];
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

    /* If full, evict the oldest by advancing tail (overwrite policy).
     * Detecting "full" here is not to reject the write — it is to make room
     * so head never collides with tail from behind (which would look empty). */
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
