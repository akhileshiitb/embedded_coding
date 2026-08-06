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
    
    char *p_dest = (char *)dest, *p_src = (char *)src;

    if (p_dest == p_src)
        return dest; // no copy needed.

    if (p_dest > p_src) {
        /* backward copy */ 
        size_t i = n - 1;
        while (n--) {
            *(p_dest + i)  = *(p_src + i);
            i -= 1;
        }
    } else {
        /* forward copy */ 
        while (n--) {
            *p_dest = *p_src;
             p_dest++;
             p_src++;
        }
    }

    return dest;
}
