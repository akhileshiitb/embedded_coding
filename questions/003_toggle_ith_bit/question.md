# 003. Toggle the i-th Bit

## Difficulty: Easy

## Problem Statement

Given a 32-bit unsigned integer `num` and a bit position `i`, toggle the i-th bit of `num` (flip it: 0 becomes 1, 1 becomes 0) and return the result.

Bit positions are numbered from 0 (LSB) to 31 (MSB).

## Function Signature

```c
uint32_t toggle_bit(uint32_t num, int i);
```

## Parameters

| Parameter | Type       | Description                             |
|-----------|------------|-----------------------------------------|
| `num`     | `uint32_t` | The input 32-bit unsigned integer       |
| `i`       | `int`      | The bit position to toggle (0-indexed)  |

## Return Value

Return the value of `num` with the i-th bit flipped.

## Examples

### Example 1
```
Input:  num = 0x00000000, i = 0
Output: 0x00000001
Explanation: Toggling bit 0 of 0 (bit was 0, becomes 1) gives 1.
```

### Example 2
```
Input:  num = 0x00000001, i = 0
Output: 0x00000000
Explanation: Toggling bit 0 of 1 (bit was 1, becomes 0) gives 0.
```

### Example 3
```
Input:  num = 0x0000000F, i = 2
Output: 0x0000000B
Explanation: Toggling bit 2 of 0xF (1111) gives 0xB (1011).
```

### Example 4
```
Input:  num = 0x00000000, i = 31
Output: 0x80000000
Explanation: Toggling the MSB (bit 31) of 0 gives 0x80000000.
```

## Constraints

- `0 <= i <= 31`
- `num` is a valid 32-bit unsigned integer

## Notes

- Unlike SET and CLEAR, toggle does not care about the current state of the bit — it always flips it.
- Think about which bitwise operator flips a bit regardless of its current value.
- Toggling the same bit twice returns the original value.
