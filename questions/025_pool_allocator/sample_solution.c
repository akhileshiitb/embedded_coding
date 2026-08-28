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

/* Compile-time enforcement of the block-size invariants. */
_Static_assert(BLOCK_SIZE >= sizeof(void *), "BLOCK_SIZE must hold a pointer");
_Static_assert((BLOCK_SIZE & (BLOCK_SIZE - 1)) == 0, "BLOCK_SIZE must be a power of 2");
_Static_assert(NUM_BLOCKS >= 1, "NUM_BLOCKS must be >= 1");

/*
 * A block is either:
 *   - free:      its storage holds a pointer to the next free block
 *   - allocated: its storage is raw bytes owned by the caller
 * The union captures this dual use in a single type. `next` and `bytes`
 * share the same memory; only one meaning is live at a time.
 */
typedef union block {
    union block *next;          /* free-list link (valid only while free) */
    uint8_t      bytes[BLOCK_SIZE];
} block_t;

/* Pool memory — statically allocated, naturally aligned to a block. */
static _Alignas(BLOCK_SIZE) block_t pool_memory[NUM_BLOCKS];

/* Free list head — points to the first available block, or NULL if empty. */
static block_t *free_list;

/* Number of free blocks remaining. */
static int free_blocks_count;

/**
 * @brief Initialize the fixed-block pool allocator.
 *
 * Threads every block into a singly linked free list, with the last
 * block's `next` set to NULL.
 *
 * @return 0 on success.
 */
int pool_init(void) {
    for (int i = 0; i < NUM_BLOCKS - 1; i++) {
        pool_memory[i].next = &pool_memory[i + 1];
    }
    pool_memory[NUM_BLOCKS - 1].next = NULL;

    free_list = &pool_memory[0];
    free_blocks_count = NUM_BLOCKS;

    return 0;
}

/**
 * @brief Allocate one block from the pool.
 *
 * Pops the head of the free list (O(1)).
 *
 * @return Pointer to allocated block, or NULL if the pool is exhausted.
 */
void *pool_alloc(void) {
    if (free_list == NULL)
        return NULL;

    block_t *blk = free_list;
    free_list = blk->next;      /* read link out before handing block over */
    free_blocks_count -= 1;

    return blk;
}

/**
 * @brief Free a previously allocated block back to the pool.
 *
 * Validates that ptr is a real block-start address within the pool, then
 * pushes the block onto the head of the free list (O(1)).
 *
 * @param ptr Pointer to the block to free.
 * @return 0 on success, -1 if ptr is NULL or not from this pool.
 */
int pool_free(void *ptr) {
    if (ptr == NULL)
        return -1;

    uintptr_t p     = (uintptr_t)ptr;
    uintptr_t start = (uintptr_t)&pool_memory[0];
    uintptr_t end   = (uintptr_t)&pool_memory[NUM_BLOCKS - 1];

    /* Must lie within the pool and sit exactly on a block boundary. */
    if (p < start || p > end || (p - start) % BLOCK_SIZE != 0)
        return -1;

    block_t *blk = (block_t *)ptr;
    blk->next = free_list;      /* repurpose storage as free-list link */
    free_list = blk;
    free_blocks_count += 1;

    return 0;
}

/**
 * @brief Get the number of free blocks remaining.
 *
 * @return Number of available blocks.
 */
int pool_available(void) {
    return free_blocks_count;
}

/**
 * @brief Check if a pointer is aligned to the given boundary.
 *
 * @param ptr       Pointer to check.
 * @param alignment Required alignment (power of 2).
 * @return 1 if aligned, 0 otherwise.
 */
int pool_is_aligned(void *ptr, int alignment) {
    if ((uintptr_t)ptr & (uintptr_t)(alignment - 1))
        return 0;

    return 1;
}
