# 004. Check the i-th Bit

## Difficulty: Easy

## Problem Statement

Given a 32-bit unsigned integer `num` and a bit position `i`, check if the i-th bit of `num` is set (1) or clear (0). Return 1 if the bit is set, 0 if the bit is clear.

Bit positions are numbered from 0 (LSB) to 31 (MSB).

## Function Signature

```c
int check_bit(uint32_t num, int i);
```

## Parameters

| Parameter | Type       | Description                            |
|-----------|------------|----------------------------------------|
| `num`     | `uint32_t` | The input 32-bit unsigned integer      |
| `i`       | `int`      | The bit position to check (0-indexed)  |

## Return Value

Return `1` if the i-th bit is set (1), or `0` if the i-th bit is clear (0).

## Examples

### Example 1
```
Input:  num = 0x00000001, i = 0
Output: 1
Explanation: Bit 0 of 0x01 (0001) is 1.
```

### Example 2
```
Input:  num = 0x00000001, i = 1
Output: 0
Explanation: Bit 1 of 0x01 (0001) is 0.
```

### Example 3
```
Input:  num = 0x0000000F, i = 3
Output: 1
Explanation: Bit 3 of 0xF (1111) is 1.
```

### Example 4
```
Input:  num = 0x80000000, i = 31
Output: 1
Explanation: The MSB (bit 31) of 0x80000000 is 1.
```

## Constraints

- `0 <= i <= 31`
- `num` is a valid 32-bit unsigned integer

## Notes

- The return value must be exactly 0 or 1 (not any non-zero value for "set").
- This is a read-only operation — the original number is not modified.
- Think about how to isolate a single bit and normalize the result.
