#include <stddef.h>
#include <stdint.h>

// This is sample solution for this problem to refer if needed.

#define POOL_MAX_BLOCKS 64
#define POOL_MAX_BLOCK_SIZE 256

/**
 * Pool memory — statically allocated, aligned.
 * Total size = POOL_MAX_BLOCKS * POOL_MAX_BLOCK_SIZE = 16384 bytes.
 */
static _Alignas(256) uint8_t pool_memory[POOL_MAX_BLOCKS * POOL_MAX_BLOCK_SIZE];

/**
 * Free list head — points to the first available block.
 * Each free block contains a pointer to the next free block.
 */
static void *free_list;

/** Pool configuration */
static int pool_block_size;
static int pool_num_blocks;
static int free_blocks_count;
/**
 * @brief Initialize the fixed-block pool allocator.
 *
 * Sets up the free list by threading pointers through each block.
 *
 * @param block_size  Size of each block (>= 8, power of 2, <= POOL_MAX_BLOCK_SIZE).
 * @param num_blocks  Number of blocks (1 to POOL_MAX_BLOCKS).
 * @return            0 on success, -1 if parameters invalid.
 */
int pool_init(int block_size, int num_blocks) {
    /* TODO: Implement your solution here
     * - Validate parameters
     * - Thread free list: block[i]->next = block[i+1], last->next = NULL
     * - Set free_list = &block[0]
     */

    if ((block_size > POOL_MAX_BLOCK_SIZE) || (num_blocks > POOL_MAX_BLOCKS) || (block_size < 8))
        return -1;

    /* Block size must be power of 2 */
    if (block_size & (block_size - 1))
        return -1;
    
    pool_block_size = block_size;
    pool_num_blocks = num_blocks;

    free_list = (void *)pool_memory;
    
    uintptr_t next;

    /* Create linked list of free blocks */
    for (int i = 1; i < num_blocks; i += 1) {
            next = (uintptr_t)&pool_memory[i * block_size];

            *((uintptr_t *)(&pool_memory[(i - 1) * block_size])) = next;

            if (i == (num_blocks - 1)) {
                *((uintptr_t *)(&pool_memory[i * block_size])) = 0;
            }
    }

    
    free_blocks_count = num_blocks;

    return 0;
}

/**
 * @brief Allocate one block from the pool.
 *
 * @return  Pointer to allocated block, or NULL if pool is exhausted.
 */
void *pool_alloc(void) {
    /* TODO: Implement your solution here
     * - If free_list == NULL, return NULL
     * - Save free_list as the block to return
     * - Advance free_list to the next free block
     * - Return the allocated block
     */
    void *ret;

    if (free_list == NULL) 
        return NULL;
    
    ret = free_list;

    free_list = (void *)(*((uintptr_t *)free_list));
    
    free_blocks_count -= 1;

    return ret;
}

int pool_contains(void *ptr);
/**
 * @brief Free a previously allocated block back to the pool.
 *
 * @param ptr  Pointer to the block to free.
 * @return     0 on success, -1 if ptr is NULL or not from this pool.
 */
int pool_free(void *ptr) {
    /* TODO: Implement your solution here
     * - Validate ptr is not NULL and belongs to the pool
     * - Push ptr onto the free list head
     */

    if ((!ptr) || (!pool_contains(ptr)))
        return -1;

    *((uintptr_t *)ptr) = (uintptr_t)free_list;

    free_list = ptr;

    free_blocks_count += 1;

    return 0;
}

/**
 * @brief Get the number of free blocks remaining.
 *
 * @return  Number of available blocks.
 */
int pool_available(void) {
    /* TODO: Implement your solution here
     * - Walk the free list and count, OR maintain a counter
     */
    return free_blocks_count;
}

/**
 * @brief Check if a pointer belongs to this pool.
 *
 * @param ptr  Pointer to check.
 * @return     1 if ptr is a valid block start address in the pool, 0 otherwise.
 */
int pool_contains(void *ptr) {
    /* TODO: Implement your solution here
     * - Check if ptr is within pool_memory bounds
     * - Check if ptr is aligned to block_size (i.e., at a block boundary)
     */
    uintptr_t temp = (uintptr_t)ptr;

    if (temp & (pool_block_size - 1))
        return 0;

    if ((temp < (uintptr_t)&pool_memory[0]) || (temp > (uintptr_t)&pool_memory[pool_block_size * (pool_num_blocks - 1)]))
        return 0;

    return 1;
}

/**
 * @brief Check if a pointer is aligned to the given boundary.
 *
 * @param ptr        Pointer to check.
 * @param alignment  Required alignment (power of 2).
 * @return           1 if aligned, 0 otherwise.
 */
int pool_is_aligned(void *ptr, int alignment) {
    /* TODO: Implement your solution here */

    if ((uintptr_t)ptr & (alignment - 1)) 
        return 0;

    return 1;
}
