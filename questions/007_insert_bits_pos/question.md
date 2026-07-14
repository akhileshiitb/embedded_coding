# 007. Insert Value into Bit Field (by Position)

## Difficulty: Medium

## Problem Statement

Given a 32-bit unsigned integer `reg`, a start bit position `start`, an end bit position `end`, and a `value` to insert, clear the bit field from `start` to `end` (inclusive) in `reg` and insert the `value` into that field position.

The field spans from bit `start` (low) to bit `end` (high), inclusive. The value should be shifted left by `start` to align with the field.

This is a common pattern when writing to hardware registers using bit position defines rather than pre-computed masks.

## Function Signature

```c
uint32_t insert_bits_pos(uint32_t reg, int start, int end, uint32_t value);
```

## Parameters

| Parameter | Type       | Description                                           |
|-----------|------------|-------------------------------------------------------|
| `reg`     | `uint32_t` | The original register value                           |
| `start`   | `int`      | The lowest bit position of the field (inclusive)      |
| `end`     | `int`      | The highest bit position of the field (inclusive)     |
| `value`   | `uint32_t` | The value to insert (unshifted, starting from bit 0) |

## Return Value

Return the new register value with the field (bits `start` to `end`) cleared and the value inserted at the correct position.

## Examples

### Example 1
```
Input:  reg = 0x00000000, start = 4, end = 7, value = 0x0A
Output: 0x000000A0
Explanation: Field is bits 4-7 (4 bits). Value 0xA shifted left by 4 gives 0xA0.
```

### Example 2
```
Input:  reg = 0xFFFFFFFF, start = 8, end = 15, value = 0x42
Output: 0xFFFF42FF
Explanation: Clear bits 8-15, then insert 0x42 shifted left by 8.
```

### Example 3
```
Input:  reg = 0x12345678, start = 0, end = 3, value = 0x0B
Output: 0x1234567B
Explanation: Clear bits 0-3 of reg, insert 0xB (no shift needed since start=0).
```

### Example 4
```
Input:  reg = 0xABCD0000, start = 8, end = 15, value = 0xEF
Output: 0xABCDEF00
Explanation: Clear bits 8-15, insert 0xEF shifted left by 8.
```

## Constraints

- `0 <= start <= end <= 31`
- `value` fits within the field width (`end - start + 1` bits)
- `reg` and `value` are valid 32-bit unsigned integers

## Notes

- You need to construct the mask from the bit positions: a mask covering bits `start` to `end` inclusive.
- The operation is equivalent to: build mask, clear field, shift value, insert.
- This is the position-based variant of question 005 (which takes a pre-computed mask).
