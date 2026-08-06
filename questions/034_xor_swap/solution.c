#include <stdint.h>

/**
 * @brief Swap two integers using XOR operations (no temporary variable).
 *
 * CRITICAL: Must handle the aliasing case where a == b (same address).
 * Naive XOR swap zeroes both values when pointers alias.
 *
 * @param a  Pointer to the first integer
 * @param b  Pointer to the second integer
 */
void xor_swap(int32_t *a, int32_t *b) {
    /* TODO: Implement your solution here */
    (void)a;
    (void)b;
}
