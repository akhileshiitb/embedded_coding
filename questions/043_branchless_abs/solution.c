#include <stdint.h>

/**
 * @brief Branchless absolute value of a signed 32-bit integer.
 *
 * Must use arithmetic right shift to create a sign mask, then
 * combine addition and XOR to conditionally negate.
 *
 * No branches, no ternary, no abs() calls.
 *
 * Note: abs(INT_MIN) wraps to INT_MIN — this is expected.
 *
 * @param v  The input signed integer
 * @return   The absolute value of v
 */
int32_t branchless_abs(int32_t v) {
    /* TODO: Implement your solution here */
    (void)v;
    return 0;
}
