# 016. Find First Set Bit

## Difficulty: Easy

## Problem Statement

Given a 32-bit unsigned integer `num`, find the position of the least significant bit that is set to 1 (the first set bit from the right). Bit positions are numbered starting from 1 (LSB = position 1). Return 0 if no bits are set (i.e., `num` is zero).

This operation is commonly known as FFS (Find First Set) and is available as a hardware instruction on most processors. It's used in scheduling (finding the highest-priority task), memory allocation, and interrupt handling.

## Function Signature

```c
int find_first_set(uint32_t num);
```

## Parameters

| Parameter | Type       | Description                       |
|-----------|------------|-----------------------------------|
| `num`     | `uint32_t` | The input 32-bit unsigned integer |

## Return Value

Return the position (1-indexed) of the least significant set bit. Return 0 if `num` is zero.

## Examples

### Example 1
```
Input:  num = 0x00000001
Output: 1
Explanation: Bit 0 is set, position is 1 (1-indexed).
```

### Example 2
```
Input:  num = 0x00000004
Output: 3
Explanation: Bit 2 is the first set bit, position is 3 (1-indexed).
```

### Example 3
```
Input:  num = 0x00000060
Output: 6
Explanation: 0x60 = 0110 0000 in binary. Bit 5 is the lowest set bit, position = 6.
```

### Example 4
```
Input:  num = 0x00000000
Output: 0
Explanation: No bits are set, so return 0.
```

## Constraints

- `num` is a valid 32-bit unsigned integer

## Notes

- Return value is 1-indexed (LSB is position 1, not 0).
- Return 0 for the special case where `num` is zero.
- Think about how to isolate the lowest set bit.
- The expression `num & (-num)` (or `num & (~num + 1)` for unsigned) isolates the lowest set bit.
