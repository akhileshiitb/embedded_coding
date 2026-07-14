# 002. Clear the i-th Bit

## Difficulty: Easy

## Problem Statement

Given a 32-bit unsigned integer `num` and a bit position `i`, clear the i-th bit of `num` (force it to 0) and return the result.

Bit positions are numbered from 0 (LSB) to 31 (MSB).

## Function Signature

```c
uint32_t clear_bit(uint32_t num, int i);
```

## Parameters

| Parameter | Type       | Description                            |
|-----------|------------|----------------------------------------|
| `num`     | `uint32_t` | The input 32-bit unsigned integer      |
| `i`       | `int`      | The bit position to clear (0-indexed)  |

## Return Value

Return the value of `num` with the i-th bit cleared to 0.

## Examples

### Example 1
```
Input:  num = 0x0000000F, i = 0
Output: 0x0000000E
Explanation: Clearing bit 0 of 0xF (1111) gives 0xE (1110).
```

### Example 2
```
Input:  num = 0x000000FF, i = 7
Output: 0x0000007F
Explanation: Clearing bit 7 of 0xFF (11111111) gives 0x7F (01111111).
```

### Example 3
```
Input:  num = 0x00000000, i = 5
Output: 0x00000000
Explanation: Bit 5 is already 0, so the value remains unchanged.
```

### Example 4
```
Input:  num = 0x80000000, i = 31
Output: 0x00000000
Explanation: Clearing the MSB (bit 31) of 0x80000000 gives 0.
```

## Constraints

- `0 <= i <= 31`
- `num` is a valid 32-bit unsigned integer

## Notes

- If the i-th bit is already 0, the number should remain unchanged.
- Think about which bitwise operator can turn a bit OFF without affecting other bits.
- This is the complement operation to "Set the i-th Bit".
