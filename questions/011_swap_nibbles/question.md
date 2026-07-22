# 011. Swap Nibbles

## Difficulty: Easy

## Problem Statement

Given a 32-bit unsigned integer `num`, swap the upper and lower nibbles (4-bit halves) of each byte. A nibble is a 4-bit group — each byte has a high nibble (bits 7-4) and a low nibble (bits 3-0).

For example, byte `0xAB` becomes `0xBA` after swapping nibbles.

This operation is useful in BCD (Binary-Coded Decimal) manipulation and certain display driver protocols.

## Function Signature

```c
uint32_t swap_nibbles(uint32_t num);
```

## Parameters

| Parameter | Type       | Description                       |
|-----------|------------|-----------------------------------|
| `num`     | `uint32_t` | The input 32-bit unsigned integer |

## Return Value

Return the value with the upper and lower nibbles swapped within each byte.

## Examples

### Example 1
```
Input:  num = 0x12345678
Output: 0x21436587
Explanation: Each byte has its nibbles swapped: 12→21, 34→43, 56→65, 78→87.
```

### Example 2
```
Input:  num = 0x0000000F
Output: 0x000000F0
Explanation: Low nibble (0xF) swaps with high nibble (0x0) in the lowest byte.
```

### Example 3
```
Input:  num = 0x000000AB
Output: 0x000000BA
Explanation: The byte 0xAB becomes 0xBA after nibble swap.
```

### Example 4
```
Input:  num = 0xFFFFFFFF
Output: 0xFFFFFFFF
Explanation: All bits are 1, so swapping nibbles has no visible effect.
```

## Constraints

- `num` is a valid 32-bit unsigned integer

## Notes

- Each byte is treated independently — nibbles swap within their own byte.
- Think about using masks to isolate the high and low nibbles, then shifting.
- The operation is its own inverse: swapping nibbles twice returns the original value.
