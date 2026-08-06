# 033. Find Two Non-Repeating Elements

## Difficulty: Medium

## Problem Statement

Given an array of integers where every element appears **exactly twice** except for **two elements** that appear exactly once, find and return both non-repeating (unique) elements.

You must solve this in **O(n) time** and **O(1) extra space** using only bitwise operations — no hash tables or sorting allowed.

The two unique values should be returned via output pointers, with the smaller value in `*out1` and the larger value in `*out2`.

## Function Signature
```c
void find_two_non_repeating(const int32_t *arr, int n, int32_t *out1, int32_t *out2);
```

## Parameters
| Parameter | Type             | Description                                      |
|-----------|------------------|--------------------------------------------------|
| `arr`     | `const int32_t*` | Input array of integers                          |
| `n`       | `int`            | Number of elements in the array (always even + 2)|
| `out1`    | `int32_t*`       | Output pointer for the smaller unique value      |
| `out2`    | `int32_t*`       | Output pointer for the larger unique value       |

## Return Value
No return value. Results are written to `*out1` (smaller) and `*out2` (larger).

## Examples

### Example 1
```
Input:  arr = {2, 3, 7, 9, 11, 2, 3, 11}, n = 8
Output: *out1 = 7, *out2 = 9
Explanation: All elements appear twice except 7 and 9.
             XOR of all = 7^9 = 0xE (1110). Rightmost set bit = 0x2 (bit 1).
             Partition by bit 1: group0={3,9,3} → XOR=9, group1={2,7,11,2,11} → XOR=7.
             Sorted: out1=7, out2=9.
```

### Example 2
```
Input:  arr = {1, 2, 3, 2, 1, 4}, n = 6
Output: *out1 = 3, *out2 = 4
Explanation: 3 and 4 are the only non-repeating elements.
             XOR of all = 3^4 = 7 (0111). Rightmost set bit = 1 (bit 0).
             Partition by bit 0: group with bit0=1: {1,3,1}→3, group with bit0=0: {2,2,4}→4.
```

### Example 3
```
Input:  arr = {-1, 5, -1, 10, 5, 99, 10, -42}, n = 8
Output: *out1 = -42, *out2 = 99
Explanation: -42 and 99 are the unique elements. Negative numbers work with two's complement XOR.
```

### Example 4
```
Input:  arr = {0, 2147483647, 0, -2147483648}, n = 4
Output: *out1 = -2147483648, *out2 = 2147483647
Explanation: INT_MIN and INT_MAX are the two unique elements.
```

## Constraints
- `4 <= n <= 1000` (n is always even)
- There are exactly two elements that appear once; all others appear exactly twice
- Elements can be any valid `int32_t` value (including negative, zero, INT_MIN, INT_MAX)
- The array contains at least one pair besides the two unique elements

## Notes
- XOR of identical values is zero: `a ^ a = 0`
- XOR of all elements leaves only the XOR of the two unique values
- The key insight: use the rightmost set bit of the combined XOR to **partition** elements into two groups — each group contains exactly one unique element
- The expression `x & (-x)` isolates the rightmost set bit (works correctly for all non-zero values including negative numbers, since `-x` is the two's complement)
- This technique is used in firmware for identifying which of multiple interrupt sources fired simultaneously
