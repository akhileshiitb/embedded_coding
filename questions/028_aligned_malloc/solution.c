#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * @brief Allocate memory aligned to a specified power-of-2 boundary.
 *
 * Allocates at least 'size' bytes of memory with the returned pointer
 * aligned to 'alignment' bytes. Uses malloc() internally with over-allocation.
 *
 * @param size       Number of bytes to allocate (must be > 0).
 * @param alignment  Required alignment in bytes (power of 2, >= sizeof(void*)).
 * @return           Aligned pointer on success, NULL on failure or invalid params.
 */
void *aligned_malloc(size_t size, size_t alignment) {
    /* TODO: Implement your solution here
     * 1. Validate parameters (size > 0, alignment is power of 2 and >= sizeof(void*))
     * 2. Allocate size + alignment - 1 + sizeof(void*) bytes with malloc()
     * 3. Compute the aligned pointer within the allocated block
     * 4. Store the original malloc pointer just before the aligned pointer
     * 5. Return the aligned pointer
     */
    (void)size;
    (void)alignment;
    return NULL;
}

/**
 * @brief Free memory allocated by aligned_malloc.
 *
 * Retrieves the original malloc pointer stored before the aligned pointer
 * and calls free() on it.
 *
 * @param ptr  Pointer previously returned by aligned_malloc, or NULL (no-op).
 */
void aligned_free(void *ptr) {
    /* TODO: Implement your solution here
     * 1. If ptr is NULL, return (no-op)
     * 2. Retrieve the original malloc pointer stored at ((void**)ptr)[-1]
     * 3. Call free() on the original pointer
     */
    (void)ptr;
}
