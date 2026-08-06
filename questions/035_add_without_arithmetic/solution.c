#include <stdint.h>

/**
 * @brief Add two integers using only bitwise operations (no +, -, *, /, %).
 *
 * Uses the half-adder principle: XOR for sum-without-carry, AND+shift for carry.
 * CRITICAL: Must cast to uint32_t before left-shifting carry to avoid
 * undefined behavior when the carry value is negative.
 *
 * @param a  First integer
 * @param b  Second integer
 * @return   The sum a + b
 */
int32_t add(int32_t a, int32_t b) {
    /* TODO: Implement your solution here */
    (void)a;
    (void)b;
    return 0;
}
