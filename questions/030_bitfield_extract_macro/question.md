# 030. Extract Value from Bit Field (Macro)

## Difficulty: Medium

## Problem Statement

Write a C macro `BITFIELD_EXTRACT(reg, mask, shift)` that extracts a value from a specific bit field of a 32-bit register.

The macro should:
1. Isolate the target bit field (defined by `mask`) in `reg` using AND
2. Shift the isolated bits right by `shift` positions to align with bit 0
3. Return the extracted value

This is one of the most common operations in embedded firmware — reading a status or configuration value from a specific field of a memory-mapped hardware register.

## Macro Signature

```c
#define BITFIELD_EXTRACT(reg, mask, shift)  /* your implementation */
```

## Parameters

| Parameter | Type       | Description                                           |
|-----------|------------|-------------------------------------------------------|
| `reg`     | `uint32_t` | The register value to extract from                    |
| `mask`    | `uint32_t` | Bit mask defining the target field (e.g., `0x0000FF00`) |
| `shift`   | `int`      | Number of bits to right-shift after masking           |

## Return Value

Returns the extracted field value, right-aligned to bit 0.

## Examples

### Example 1
```
Input:  reg = 0x000000A0, mask = 0x000000F0, shift = 4
Output: 0x0000000A
Explanation: reg & mask = 0x000000A0, shifted right by 4 = 0x0A.
```

### Example 2
```
Input:  reg = 0xFFFF42FF, mask = 0x0000FF00, shift = 8
Output: 0x00000042
Explanation: reg & mask = 0x00004200, shifted right by 8 = 0x42.
```

### Example 3
```
Input:  reg = 0x1234567B, mask = 0x0000000F, shift = 0
Output: 0x0000000B
Explanation: reg & mask = 0x0000000B, shift = 0 so no change.
```

### Example 4
```
Input:  reg = 0xABCDEF00, mask = 0x00FF0000, shift = 16
Output: 0x000000CD
Explanation: reg & mask = 0x00CD0000, shifted right by 16 = 0xCD.
```

## Constraints

- `mask` contains one or more contiguous set bits (valid field mask)
- `shift` equals the position of the mask's lowest set bit
- All values are valid 32-bit unsigned integers

## Notes

- The standard pattern is: `((reg) & (mask)) >> (shift)`
- This is the inverse of the "Insert Value into Bit Field" operation (question 029).
- Real-world usage: reading an interrupt status code, checking a clock divider setting, reading an ADC channel ID from a status register.
