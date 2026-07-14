# 001. Set the i-th Bit

## Difficulty: Easy

## Problem Statement

Given a 32-bit unsigned integer `num` and a bit position `i`, set the i-th bit of `num` to 1 and return the result.

Bit positions are numbered from 0 (LSB) to 31 (MSB).

## Function Signature

```c
uint32_t set_bit(uint32_t num, int i);
```

## Parameters

| Parameter | Type       | Description                          |
|-----------|------------|--------------------------------------|
| `num`     | `uint32_t` | The input 32-bit unsigned integer    |
| `i`       | `int`      | The bit position to set (0-indexed)  |

## Return Value

Return the value of `num` with the i-th bit set to 1.

## Examples

### Example 1
```
Input:  num = 0x00000000, i = 0
Output: 0x00000001
Explanation: Setting bit 0 of 0 gives 1.
```

### Example 2
```
Input:  num = 0x00000000, i = 3
Output: 0x00000008
Explanation: Setting bit 3 of 0 gives 8 (binary: 1000).
```

### Example 3
```
Input:  num = 0x0000000F, i = 3
Output: 0x0000000F
Explanation: Bit 3 is already set, so the value remains unchanged.
```

### Example 4
```
Input:  num = 0x00000000, i = 31
Output: 0x80000000
Explanation: Setting the MSB (bit 31) of 0.
```

## Constraints

- `0 <= i <= 31`
- `num` is a valid 32-bit unsigned integer

## Notes

- If the i-th bit is already set, the number should remain unchanged.
- Think about which bitwise operator can turn a bit ON without affecting other bits.
