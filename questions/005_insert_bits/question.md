# 005. Insert Value into Bit Field

## Difficulty: Medium

## Problem Statement

Given a 32-bit unsigned integer `reg`, a `mask` defining a bit field, and a `value` to insert, clear the bit field (defined by the mask) in `reg` and insert the `value` into that field position.

The mask defines which bits belong to the field (e.g., `0x00000F00` defines bits 8-11 as the field). The value should be placed at the field's position — i.e., shifted to align with the mask's lowest set bit.

This is a common operation when writing to hardware registers where specific bit fields control different peripherals or settings.

## Function Signature

```c
uint32_t insert_bits(uint32_t reg, uint32_t mask, uint32_t value);
```

## Parameters

| Parameter | Type       | Description                                          |
|-----------|------------|------------------------------------------------------|
| `reg`     | `uint32_t` | The original register value                          |
| `mask`    | `uint32_t` | Bit mask defining the field (contiguous set bits)    |
| `value`   | `uint32_t` | The value to insert (unshifted, starting from bit 0) |

## Return Value

Return the new register value with the field (defined by mask) cleared and the value inserted at the correct bit position.

## Examples

### Example 1
```
Input:  reg = 0x00000000, mask = 0x000000F0, value = 0x0A
Output: 0x000000A0
Explanation: Mask 0xF0 covers bits 4-7. Value 0x0A (1010) shifted to bits 4-7 gives 0xA0.
```

### Example 2
```
Input:  reg = 0xFFFFFFFF, mask = 0x0000FF00, value = 0x42
Output: 0xFFFF42FF
Explanation: Clear bits 8-15, then insert 0x42 shifted left by 8.
```

### Example 3
```
Input:  reg = 0x12345678, mask = 0x0000000F, value = 0x0B
Output: 0x1234567B
Explanation: Clear bits 0-3 of reg, insert 0x0B into bits 0-3.
```

### Example 4
```
Input:  reg = 0xABCD0000, mask = 0x0000FF00, value = 0xEF
Output: 0xABCDEF00
Explanation: Clear bits 8-15, insert 0xEF shifted left by 8.
```

## Constraints

- `mask` contains one or more contiguous set bits (valid field mask)
- `value` fits within the field width defined by the mask
- `reg`, `mask`, and `value` are valid 32-bit unsigned integers

## Notes

- The operation is: `(reg & ~mask) | ((value << shift) & mask)` where `shift` is the position of the mask's lowest set bit.
- You need to determine how far to shift the value based on the mask position.
- This pattern is fundamental for writing to memory-mapped hardware registers (e.g., setting a clock divider field, configuring GPIO mode bits).
