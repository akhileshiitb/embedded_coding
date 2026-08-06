#include <stdint.h>
#include <stdatomic.h>

#define SPSC_MAX_CAPACITY 256

/**
 * SPSC Ring Buffer State.
 * - buffer[]: storage array
 * - head: next write position (only producer modifies)
 * - tail: next read position (only consumer modifies)
 * - mask: capacity - 1 (for efficient modulo)
 * - capacity: size of the buffer array
 */
static int32_t buffer[SPSC_MAX_CAPACITY];

static struct ring_buf {
   int32_t *buf;
   uint32_t size; // number of elements in buffer
   volatile uint32_t head; // only written by producer (volatile is imp)
   volatile uint32_t tail; // only written by consumer (volatile is imp)
   uint32_t mask;
} my_circ_buf;

/**
 * @brief Initialize the SPSC ring buffer.
 *
 * @param capacity  Must be a power of 2, between 2 and SPSC_MAX_CAPACITY.
 * @return          0 on success, -1 if capacity is invalid.
 */
int spsc_init(int capacity) {
    /* TODO: Implement your solution here
     * - Validate capacity is power of 2 and in range
     * - Set mask = capacity - 1
     * - Reset head and tail to 0
     */

    if (capacity & (capacity - 1))
        return -1;

    if ((capacity < 2) || (capacity > SPSC_MAX_CAPACITY))
        return -1;

    my_circ_buf.size = capacity;
    my_circ_buf.mask = my_circ_buf.size - 1;
    my_circ_buf.head = 0;
    my_circ_buf.tail = 0;
    my_circ_buf.buf = buffer;

    return 0;
}

/**
 * @brief Check if the ring buffer is empty.
 * @return 1 if empty, 0 otherwise.
 */
int spsc_is_empty(void) {
    /* TODO: Implement your solution here */
    return my_circ_buf.head == my_circ_buf.tail;
}

/**
 * @brief Check if the ring buffer is full.
 * @return 1 if full, 0 otherwise.
 */
int spsc_is_full(void) {
    /* TODO: Implement your solution here */
    //return ((my_circ_buf.head + 1) & my_circ_buf.mask) == (my_circ_buf.tail & my_circ_buf.mask);
    return (my_circ_buf.head - my_circ_buf.tail) == (my_circ_buf.size - 1);
}

/**
 * @brief Enqueue a value (producer side).
 *
 * @param value  Value to push into the ring buffer.
 * @return       0 on success, -1 if buffer is full.
 */
int spsc_push(int32_t value) {
    /* TODO: Implement your solution here
     * - Load head (relaxed — only producer reads/writes it)
     * - Load tail (acquire — need to see consumer's latest progress)
     * - Check if full: (head + 1) & mask == tail
     * - Write data to buffer[head]
     * - Store new head with release ordering (publishes the data)
     */

    if (spsc_is_full())
        return -1;
    
    my_circ_buf.buf[my_circ_buf.head & my_circ_buf.mask] = value;

    my_circ_buf.head += 1;

    return 0;
}

/**
 * @brief Dequeue a value (consumer side).
 *
 * @param out  Pointer to store the dequeued value.
 * @return     0 on success, -1 if buffer is empty.
 */
int spsc_pop(int32_t *out) {
    /* TODO: Implement your solution here
     * - Load tail (relaxed — only consumer reads/writes it)
     * - Load head (acquire — need to see producer's latest progress)
     * - Check if empty: tail == head
     * - Read data from buffer[tail]
     * - Store new tail with release ordering
     */

    if (spsc_is_empty())
        return -1;

    *out = my_circ_buf.buf[my_circ_buf.tail & my_circ_buf.mask];

    my_circ_buf.tail += 1;

    return 0;
}


/**
 * @brief Get the number of elements in the ring buffer.
 * @return Number of elements currently stored.
 */
int spsc_count(void) {
    /* TODO: Implement your solution here */
    return my_circ_buf.head - my_circ_buf.tail;
}
