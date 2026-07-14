# 008. Extract Value from Bit Field (by Position)

## Difficulty: Medium

## Problem Statement

Given a 32-bit unsigned integer `reg`, a start bit position `start`, and an end bit position `end`, extract the value stored in the bit field from `start` to `end` (inclusive) and return it right-shifted to bit position 0.

The field spans from bit `start` (low) to bit `end` (high), inclusive. The extracted value should be shifted right by `start` so that the field's LSB aligns with bit 0 of the result.

This is a common pattern when reading hardware registers using bit position defines rather than pre-computed masks.

## Function Signature

```c
uint32_t extract_bits_pos(uint32_t reg, int start, int end);
```

## Parameters

| Parameter | Type       | Description                                           |
|-----------|------------|-------------------------------------------------------|
| `reg`     | `uint32_t` | The register value to extract from                    |
| `start`   | `int`      | The lowest bit position of the field (inclusive)      |
| `end`     | `int`      | The highest bit position of the field (inclusive)     |

## Return Value

Return the field value extracted from `reg`, right-shifted to start at bit 0.

## Examples

### Example 1
```
Input:  reg = 0x000000A0, start = 4, end = 7
Output: 0x0000000A
Explanation: Field is bits 4-7. Value 0xA0 >> 4 masked to 4 bits gives 0xA.
```

### Example 2
```
Input:  reg = 0xFFFF42FF, start = 8, end = 15
Output: 0x00000042
Explanation: Field is bits 8-15. Extract and shift right by 8 gives 0x42.
```

### Example 3
```
Input:  reg = 0x1234567B, start = 0, end = 3
Output: 0x0000000B
Explanation: Field is bits 0-3. Already at bit 0, no shift needed.
```

### Example 4
```
Input:  reg = 0xABCDEF00, start = 16, end = 23
Output: 0x000000CD
Explanation: Field is bits 16-23. Extract and shift right by 16 gives 0xCD.
```

## Constraints

- `0 <= start <= end <= 31`
- `reg` is a valid 32-bit unsigned integer

## Notes

- You need to construct a mask from the bit positions, then isolate and shift.
- The operation is: build mask for bits [start..end], AND with reg, shift right by start.
- This is the position-based variant of question 006 (which takes a pre-computed mask).
- This is the inverse of question 007 (Insert Value by Position).
