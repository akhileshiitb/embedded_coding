#include <stdint.h>
#include <stddef.h>

#define PPBUF_MAX_SIZE 128

#define PPBUF_STATE_IDLE     0
#define PPBUF_STATE_READY    1

struct ppbuff_t {
    int32_t buff_a[PPBUF_MAX_SIZE];
    int32_t buff_b[PPBUF_MAX_SIZE];
    int32_t size;
    int32_t state;
    int32_t *write_ptr;
    int32_t *read_ptr;
} ppbuff;

/*
 * Assumption: the consumer is always faster than the producer, so an overrun
 * (swap while a buffer is still unconsumed) never happens. You do not need to
 * detect or count overruns.
 *
 * Design decision is yours: declare whatever buffers, pointers, and state
 * variable(s) you need to implement a two-buffer ping-pong scheme.
 */

/**
 * @brief Initialize the ping-pong buffer system.
 * @param size  Samples per buffer (1 to PPBUF_MAX_SIZE).
 * @return      0 on success, -1 if invalid.
 */
int ppbuf_init(int size) {
    /* TODO: Implement */

    if (size <= 0 || size > PPBUF_MAX_SIZE)
        return -1;

    ppbuff.size = size;
    ppbuff.state = PPBUF_STATE_IDLE;
    ppbuff.write_ptr = ppbuff.buff_a;
    ppbuff.read_ptr = ppbuff.buff_b;

    return 0;
}

/**
 * @brief Get pointer to the current DMA write buffer.
 */
int32_t *ppbuf_get_write_buffer(void) {
    /* TODO: Implement */
    return ppbuff.write_ptr;
}

/**
 * @brief Get pointer to the buffer ready for processing.
 * @return Pointer to ready buffer, or NULL if none.
 */
int32_t *ppbuf_get_read_buffer(void) {
    /* TODO: Implement */
    if (ppbuff.state == PPBUF_STATE_IDLE)
        return NULL;

    return ppbuff.read_ptr;
}

/**
 * @brief Swap buffers (called from DMA complete ISR).
 * @return 0 on success.
 */
int ppbuf_swap(void) {
    /* TODO: Implement */
    int32_t *temp = ppbuff.write_ptr; 
    ppbuff.write_ptr =  ppbuff.read_ptr;
    ppbuff.read_ptr = temp;

    ppbuff.state = PPBUF_STATE_READY;

    return 0;
}

/**
 * @brief Mark the read buffer as consumed.
 * @return 0 on success, -1 if no buffer to consume.
 */
int ppbuf_consume(void) {
    /* TODO: Implement */

    if (ppbuff.state == PPBUF_STATE_READY) {
        ppbuff.state = PPBUF_STATE_IDLE;
        return 0; 
    }

    return -1;
}

/**
 * @brief Get current state.
 */
int ppbuf_get_state(void) {
    /* TODO: Implement */
    return ppbuff.state;
}

/**
 * @brief Write a sample to the current write buffer.
 * @param index  Sample index (0 to buf_size-1).
 * @param value  Value to write.
 * @return       0 on success, -1 if out of range.
 */
int ppbuf_write_sample(int index, int32_t value) {
    /* TODO: Implement */
    if (index < 0 || (index > (ppbuff.size - 1)))
        return -1;

    ppbuff.write_ptr[index] = value;

    return 0;
}

/**
 * @brief Read a sample from the current read buffer.
 * @param index  Sample index (0 to buf_size-1).
 * @return       Value, or 0 if no read buffer or OOB.
 */
int32_t ppbuf_read_sample(int index) {
    /* TODO: Implement */
    if (index < 0 || (index > (ppbuff.size - 1)))
        return 0;

    if (ppbuff.state == PPBUF_STATE_IDLE)
        return 0;

    return ppbuff.read_ptr[index];
}
