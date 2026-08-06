#include <stdint.h>

/**
 * @brief Multiply two integers without using *, /, or % operators.
 *
 * Uses the Russian Peasant (shift-and-add) algorithm: for each set bit
 * in the multiplier, shift the multiplicand left by that position and accumulate.
 *
 * PITFALLS:
 * - abs(INT_MIN) overflows int32_t — must handle via uint32_t
 * - Result wraps on overflow (lower 32 bits), matching hardware behavior
 * - O(32) iterations; O(n²) if addition is also bitwise
 *
 * @param a  Multiplicand
 * @param b  Multiplier
 * @return   The product a * b (lower 32 bits)
 */
int32_t multiply(int32_t a, int32_t b) {
    /* TODO: Implement your solution here */
    (void)a;
    (void)b;
    return 0;
}
