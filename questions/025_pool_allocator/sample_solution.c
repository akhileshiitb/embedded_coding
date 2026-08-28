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

union block {
    union block *next;
    uint8_t data[BLOCK_SIZE];
};

static union block  mem_pool[NUM_BLOCKS];

static union block *head;

static uint32_t free_blocks;

/**
 * @brief Initialize the fixed-block pool allocator.
 *
 * Thread every block into a singly linked free list.
 *
 * @return 0 on success.
 */
int pool_init(void) {
    // create links
    union block *curr = mem_pool;

    head = curr;

    free_blocks = NUM_BLOCKS;

    for (int i = 0; i < NUM_BLOCKS; i += 1) {
        if (i == (NUM_BLOCKS - 1)){
            // last block. terminate with null 
            curr->next = NULL;

            continue;
        }

        curr->next = curr + 1;

        curr = curr->next;
    }
    
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
    void *ret;

    if (head) {
        ret = (void *)head;
        head = head->next; 

        free_blocks -= 1;

        return ret;
    }
        
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
    union block *curr = (union block *)ptr;
    
    uintptr_t addr = (uintptr_t)ptr;
    
    // check if address is within range  
    if ((addr < (uintptr_t)mem_pool) || (addr >= ((uintptr_t)mem_pool + (NUM_BLOCKS*BLOCK_SIZE))))
        return -1;

    // TODO: Check alignments to block.

    curr->next = head; 

    head = curr;

    free_blocks += 1;

    return 0;
}

/**
 * @brief Get the number of free blocks remaining.
 *
 * @return Number of available blocks.
 */
int pool_available(void) {
    return free_blocks;
}

/**
 * @brief Check if a pointer is aligned to the given boundary.
 *
 * @param ptr       Pointer to check.
 * @param alignment Required alignment (power of 2).
 * @return 1 if aligned, 0 otherwise.
 */
int pool_is_aligned(void *ptr, int alignment) {

    return !(((uintptr_t)ptr) & (alignment - 1));
}
