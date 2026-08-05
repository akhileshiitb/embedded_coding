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

static inline int is_aligned(uintptr_t ptr) {
    return !(ptr & (sizeof(uint32_t) - 1));
}

void *my_memcpy(void *dest, const void *src, size_t n) {
    /* TODO: Implement your solution here */
    uintptr_t p_dest = (uintptr_t)dest, p_src = (uintptr_t)src;

    while (n) {
    
       if (is_aligned(p_src) && is_aligned(p_dest) && ((n / sizeof(uint32_t)) > 0)) {
            /* src and dest alinged --> do word copy */ 

           for (unsigned long i = 0; i < (n / sizeof(uint32_t)) ; i++) {
               /* Copy full words */
               *((uint32_t *)p_dest) = *((uint32_t *)p_src);

               p_dest += sizeof(uint32_t);
               p_src += sizeof(uint32_t);

               n -= sizeof(uint32_t);
           
           }
           
       } else {
            /* either or both src/dest not alinged, do byte copy */ 
           *((uint8_t *)p_dest) = *((uint8_t *)p_src);
            
            p_src += sizeof(uint8_t);
            p_dest += sizeof(uint8_t);     

            n -= sizeof(uint8_t); // copied 1 byte
       }
    
    }

    return dest;
}
