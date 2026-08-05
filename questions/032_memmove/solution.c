#include <stdint.h>
#include <stddef.h>

/**
 * @brief Copy n bytes from src to dest, handling overlapping regions.
 *
 * Copies forward when dest < src, backward when dest > src,
 * to ensure correctness even when memory regions overlap.
 *
 * @param dest  Pointer to destination memory.
 * @param src   Pointer to source memory.
 * @param n     Number of bytes to copy.
 * @return      Pointer to dest.
 */
void *my_memmove(void *dest, const void *src, size_t n) {
    /* TODO: Implement your solution here */
    (void)dest;
    (void)src;
    (void)n;
    return dest;
}
