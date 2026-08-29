#include <string.h>

/*
 * Logging framework — printk-style ring buffer.
 *
 * Implement the four functions below. You must DESIGN all internal state
 * yourself: the log record representation, the fixed ring buffer storage,
 * the read/write indices, and the bounded message copy.
 *
 * Contract (see question.md for full spec):
 *   Levels: 0=ERR, 1=WARN, 2=INFO, 3=DEBUG   (lower = more severe)
 *   Ring capacity: 4 records
 *   Max message length: 31 chars + NUL
 *   Keep a message only if level <= threshold.
 *   When the ring is full, overwrite the OLDEST record.
 *   Default threshold after log_init() is DEBUG (3).
 */

/* Reset the logger to empty; default threshold = DEBUG (3). */
void log_init(void) {
    /* TODO: Implement */
}

/* Set the runtime severity filter (keep messages with level <= threshold). */
void log_set_level(int threshold) {
    /* TODO: Implement */
    (void)threshold;
}

/* Producer (like printk): store the message unless filtered; overwrite oldest if full. */
void log_write(int level, unsigned int timestamp, const char *msg) {
    /* TODO: Implement */
    (void)level;
    (void)timestamp;
    (void)msg;
}

/* Consumer (like dmesg): pop the oldest unread record.
 * Returns 1 and fills the outputs if a record was available, 0 if empty. */
int dmesg_read(int *out_level, unsigned int *out_timestamp,
               char *out_msg, int out_msg_size) {
    /* TODO: Implement */
    (void)out_level;
    (void)out_timestamp;
    (void)out_msg;
    (void)out_msg_size;
    return 0;
}
