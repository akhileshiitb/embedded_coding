#include <stdint.h>

/**
 * @brief Find two non-repeating elements in an array where all other elements appear twice.
 *
 * Uses XOR bit partitioning: XOR all elements to get combined XOR of the two unique values,
 * then use a distinguishing bit to partition elements into two groups.
 *
 * @param arr   Pointer to the input array of integers
 * @param n     Number of elements in the array
 * @param out1  Output pointer for the smaller unique value
 * @param out2  Output pointer for the larger unique value
 */
void find_two_non_repeating(const int32_t *arr, int n, int32_t *out1, int32_t *out2) {
    /* TODO: Implement your solution here */
    (void)arr;
    (void)n;
    *out1 = 0;
    *out2 = 0;
}
