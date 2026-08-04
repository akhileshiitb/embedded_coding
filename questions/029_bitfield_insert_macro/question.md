# 029. Insert Value into Bit Field (Macro)

## Difficulty: Medium

## Problem Statement

Write a C macro `BITFIELD_INSERT(reg, mask, shift, value)` that inserts a value into a specific bit field of a 32-bit register.

The macro should:
1. Clear the target bit field (defined by `mask`) in `reg`
2. Shift `value` left by `shift` positions
3. Insert the shifted value into the cleared field
4. Return the resulting 32-bit value

This is one of the most common operations in embedded firmware — writing a configuration value into a specific field of a memory-mapped hardware register without disturbing other fields.

## Macro Signature

```c
#define BITFIELD_INSERT(reg, mask, shift, value)  /* your implementation */
```

## Parameters

| Parameter | Type       | Description                                           |
|-----------|------------|-------------------------------------------------------|
| `reg`     | `uint32_t` | The original register value                           |
| `mask`    | `uint32_t` | Bit mask defining the target field (e.g., `0x0000FF00`) |
| `shift`   | `int`      | Number of bits to left-shift the value before insert  |
| `value`   | `uint32_t` | The value to insert (pre-shift, right-aligned)        |

## Return Value

Returns the new 32-bit register value with the field updated.

## Examples

### Example 1
```
Input:  reg = 0x00000000, mask = 0x000000F0, shift = 4, value = 0xA
Output: 0x000000A0
Explanation: Clear bits 4-7 (already 0), shift 0xA left by 4 → 0xA0, OR into reg.
```

### Example 2
```
Input:  reg = 0xFFFFFFFF, mask = 0x0000FF00, shift = 8, value = 0x42
Output: 0xFFFF42FF
Explanation: Clear bits 8-15 → 0xFFFF00FF, shift 0x42 left by 8 → 0x4200, OR in → 0xFFFF42FF.
```

### Example 3
```
Input:  reg = 0x12345678, mask = 0x0000000F, shift = 0, value = 0xB
Output: 0x1234567B
Explanation: Clear bits 0-3 → 0x12345670, value 0xB needs no shift, OR in → 0x1234567B.
```

### Example 4
```
Input:  reg = 0xABCD0000, mask = 0x00FF0000, shift = 16, value = 0xEF
Output: 0xABEF0000
Explanation: Clear bits 16-23 → 0xAB000000, shift 0xEF left by 16 → 0x00EF0000, OR in → 0xABEF0000.
```

## Constraints

- `mask` contains one or more contiguous set bits (valid field mask)
- `shift` equals the position of the mask's lowest set bit
- `value` fits within the field width (no overflow check required)
- All values are valid 32-bit unsigned integers

## Notes

- The standard pattern is: `(reg & ~mask) | ((value << shift) & mask)`
- The `& mask` on the inserted value is defensive — it clips overflow if value is too large for the field.
- This is the inverse of the "Extract from Bit Field" operation.
- Real-world usage: writing a clock divider, setting an interrupt priority level, configuring a peripheral mode.
