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

/* Create a ring buffer */

// to store 7 log_entry
#define RING_BUF_SIZE   8
#define MSG_MAX_SIZE    32

#define    LOG_LEVEL_ERR    0
#define    LOG_LEVEL_WARN   1
#define    LOG_LEVEL_INFO   2
#define    LOG_LEVEL_DEBUG  3

struct log_entry {
    int level;
    unsigned int timestamp;
    char msg[MSG_MAX_SIZE];
};

struct ringbuff {
    struct log_entry log_buf[RING_BUF_SIZE];

    unsigned int write; // write pointer
    unsigned int read; // read pointer
    unsigned int size; // size of ring buffer/log buffer
    unsigned int mask; // mask to calculate indexes
    int ll; // current log level
};


// initantiate log buffer
struct ringbuff buff;

/* Reset the logger to empty; default threshold = DEBUG (3). */
void log_init(void) {
    /* TODO: Implement */
    buff.write = 0;
    buff.read = 0;
    buff.size = RING_BUF_SIZE;
    buff.mask = buff.size - 1; // size is power of 2
    buff.ll = LOG_LEVEL_DEBUG;
}

// 1 --> empty ring buffer
static int is_empty(void) {
    return buff.read == buff.write;
}

// 1 --> full ring buffer
static int is_full(void) {
    unsigned int mask = buff.mask;
    return ((buff.write + 1) & mask) == (buff.read & mask);
}

/* Set the runtime severity filter (keep messages with level <= threshold). */
void log_set_level(int threshold) {
    /* TODO: Implement */
    buff.ll = threshold;
}

/* Producer (like printk): store the message unless filtered; overwrite oldest if full. */
void log_write(int level, unsigned int timestamp, const char *msg) {
    /* TODO: NULL pointer check on msg */

    unsigned int mask = buff.mask;
    unsigned int write_idx = buff.write & mask;

    if (level > buff.ll) 
        return;

    if (is_full()) {
        // override
        buff.read += 1; 
    }
    

    buff.log_buf[write_idx].timestamp = timestamp;
    buff.log_buf[write_idx].level = level;

    // copy message
    for (int i = 0; i < 31; i += 1) {
        buff.log_buf[write_idx].msg[i] = msg[i];
        if (msg[i] == '\0')
            break;
    }


    // NULL terminate
    buff.log_buf[write_idx].msg[31] = '\0';

    buff.write += 1;
    
}

/* Consumer (like dmesg): pop the oldest unread record.
 * Returns 1 and fills the outputs if a record was available, 0 if empty. */
int dmesg_read(int *out_level, unsigned int *out_timestamp,
               char *out_msg, int out_msg_size) {

    /* TODO: NULL pointer checks on inputs */

    unsigned int mask = buff.mask;
    unsigned int read_idx = buff.read & mask;

    if (is_empty())
        return 0;
    
    // read message
    *out_level = buff.log_buf[read_idx].level;
    *out_timestamp = buff.log_buf[read_idx].timestamp;

    for (int i = 0; i < (out_msg_size - 1); i += 1) {
        out_msg[i] = buff.log_buf[read_idx].msg[i];
        if (!out_msg[i])
            break;
    }

    // force null terminate.
    out_msg[out_msg_size - 1] = '\0';

    buff.read += 1;

    return 1;
}
