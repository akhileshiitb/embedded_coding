#include <stdint.h>
#include <stddef.h>

/**
 * @brief Copy n bytes from src to dest (non-overlapping).
 *
 * Optimized implementation that copies word-aligned data using
 * 4-byte transfers when possible, falling back to byte copy
 * for unaligned portions.
 *
 * @param dest  Pointer to destination memory.
 * @param src   Pointer to source memory (not modified).
 * @param n     Number of bytes to copy.
 * @return      Pointer to dest.
 */
void *my_memcpy(void *dest, const void *src, size_t n) {
    /* TODO: Implement your solution here */
    (void)dest;
    (void)src;
    (void)n;
    return dest;
}
