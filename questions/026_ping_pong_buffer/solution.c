#include <stdint.h>
#include <stddef.h>

#define PPBUF_MAX_SIZE 128

#define PPBUF_STATE_IDLE     0
#define PPBUF_STATE_READY    1
#define PPBUF_STATE_OVERRUN  2

static int32_t buf_a[PPBUF_MAX_SIZE];
static int32_t buf_b[PPBUF_MAX_SIZE];

static int32_t *write_buf;
static int32_t *read_buf;
static int buf_size;
static int state;
static int overrun_count;

/**
 * @brief Initialize the ping-pong buffer system.
 * @param size  Samples per buffer (1 to PPBUF_MAX_SIZE).
 * @return      0 on success, -1 if invalid.
 */
int ppbuf_init(int size) {
    /* TODO: Implement */
    (void)size;
    return -1;
}

/**
 * @brief Get pointer to the current DMA write buffer.
 */
int32_t *ppbuf_get_write_buffer(void) {
    /* TODO: Implement */
    return NULL;
}

/**
 * @brief Get pointer to the buffer ready for processing.
 * @return Pointer to ready buffer, or NULL if none.
 */
int32_t *ppbuf_get_read_buffer(void) {
    /* TODO: Implement */
    return NULL;
}

/**
 * @brief Swap buffers (called from DMA complete ISR).
 * @return 0 on success, -1 if overrun.
 */
int ppbuf_swap(void) {
    /* TODO: Implement */
    return -1;
}

/**
 * @brief Mark the read buffer as consumed.
 * @return 0 on success, -1 if no buffer to consume.
 */
int ppbuf_consume(void) {
    /* TODO: Implement */
    return -1;
}

/**
 * @brief Get current state.
 */
int ppbuf_get_state(void) {
    /* TODO: Implement */
    return PPBUF_STATE_IDLE;
}

/**
 * @brief Get cumulative overrun count.
 */
int ppbuf_get_overrun_count(void) {
    /* TODO: Implement */
    return 0;
}

/**
 * @brief Write a sample to the current write buffer.
 * @param index  Sample index (0 to buf_size-1).
 * @param value  Value to write.
 * @return       0 on success, -1 if out of range.
 */
int ppbuf_write_sample(int index, int32_t value) {
    /* TODO: Implement */
    (void)index;
    (void)value;
    return -1;
}

/**
 * @brief Read a sample from the current read buffer.
 * @param index  Sample index (0 to buf_size-1).
 * @return       Value, or 0 if no read buffer or OOB.
 */
int32_t ppbuf_read_sample(int index) {
    /* TODO: Implement */
    (void)index;
    return 0;
}
