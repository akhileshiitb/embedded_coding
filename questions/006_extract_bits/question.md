# 006. Extract Value from Bit Field

## Difficulty: Medium

## Problem Statement

Given a 32-bit unsigned integer `reg` and a `mask` defining a bit field, extract the value stored in that field and return it right-shifted to bit position 0.

The mask defines which bits belong to the field (e.g., `0x00000F00` defines bits 8-11 as the field). The extracted value should be shifted down so that the field's LSB aligns with bit 0 of the result.

This is a common operation when reading hardware registers where specific bit fields encode status or configuration values.

## Function Signature

```c
uint32_t extract_bits(uint32_t reg, uint32_t mask);
```

## Parameters

| Parameter | Type       | Description                                          |
|-----------|------------|------------------------------------------------------|
| `reg`     | `uint32_t` | The register value to extract from                   |
| `mask`    | `uint32_t` | Bit mask defining the field (contiguous set bits)    |

## Return Value

Return the field value extracted from `reg` and right-shifted to start at bit 0.

## Examples

### Example 1
```
Input:  reg = 0x000000A0, mask = 0x000000F0
Output: 0x0000000A
Explanation: Mask 0xF0 covers bits 4-7. Value in bits 4-7 is 0xA (1010), shifted right by 4 gives 0x0A.
```

### Example 2
```
Input:  reg = 0xFFFF42FF, mask = 0x0000FF00
Output: 0x00000042
Explanation: Mask covers bits 8-15. Value in bits 8-15 is 0x42, shifted right by 8 gives 0x42.
```

### Example 3
```
Input:  reg = 0x1234567B, mask = 0x0000000F
Output: 0x0000000B
Explanation: Mask covers bits 0-3. Value is already at bit 0, so no shift needed.
```

### Example 4
```
Input:  reg = 0xABCDEF00, mask = 0x00FF0000
Output: 0x000000CD
Explanation: Mask covers bits 16-23. Value in bits 16-23 is 0xCD, shifted right by 16.
```

## Constraints

- `mask` contains one or more contiguous set bits (valid field mask)
- `reg` and `mask` are valid 32-bit unsigned integers

## Notes

- The operation is: `(reg & mask) >> shift` where `shift` is the position of the mask's lowest set bit.
- This is the inverse of the "Insert Value into Bit Field" operation.
- This pattern is fundamental for reading memory-mapped hardware registers (e.g., reading a status code, checking a clock divider setting).
