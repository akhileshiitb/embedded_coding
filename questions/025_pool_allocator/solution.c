#include <stddef.h>
#include <stdint.h>

/*
 * Pool configuration — statically defined at compile time.
 *
 * BLOCK_SIZE must be a power of 2 and >= sizeof(void *) so that a free
 * block can hold the free-list "next" pointer.
 * NUM_BLOCKS is the fixed number of blocks in the pool.
 */
#define BLOCK_SIZE 16
#define NUM_BLOCKS 4

/*
 * Design the pool's backing storage and bookkeeping yourself.
 * Hint: a block can be modeled as a union of a "next free block" pointer
 * and a raw byte buffer, since a block is never both free and allocated
 * at the same time.
 */

/**
 * @brief Initialize the fixed-block pool allocator.
 *
 * Thread every block into a singly linked free list.
 *
 * @return 0 on success.
 */
int pool_init(void) {
    /* TODO: Implement your solution here */
    return 0;
}

/**
 * @brief Allocate one block from the pool.
 *
 * Should pop the head of the free list (O(1)).
 *
 * @return Pointer to allocated block, or NULL if the pool is exhausted.
 */
void *pool_alloc(void) {
    /* TODO: Implement your solution here */
    return NULL;
}

/**
 * @brief Free a previously allocated block back to the pool.
 *
 * Validate that ptr is a real block-start address within the pool, then
 * push the block onto the head of the free list (O(1)).
 *
 * @param ptr Pointer to the block to free.
 * @return 0 on success, -1 if ptr is NULL or not from this pool.
 */
int pool_free(void *ptr) {
    /* TODO: Implement your solution here */
    (void)ptr;
    return -1;
}

/**
 * @brief Get the number of free blocks remaining.
 *
 * @return Number of available blocks.
 */
int pool_available(void) {
    /* TODO: Implement your solution here */
    return 0;
}

/**
 * @brief Check if a pointer is aligned to the given boundary.
 *
 * @param ptr       Pointer to check.
 * @param alignment Required alignment (power of 2).
 * @return 1 if aligned, 0 otherwise.
 */
int pool_is_aligned(void *ptr, int alignment) {
    /* TODO: Implement your solution here */
    (void)ptr;
    (void)alignment;
    return 0;
}
