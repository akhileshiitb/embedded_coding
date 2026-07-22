# 012. Rotate Left

## Difficulty: Easy

## Problem Statement

Given a 32-bit unsigned integer `num` and a rotation count `n`, perform a left circular rotation by `n` positions and return the result.

In a left rotation, bits shifted out from the MSB (bit 31) wrap around to the LSB (bit 0). This is different from a logical left shift where bits shifted out are lost.

Circular rotations are fundamental in cryptographic algorithms (SHA, AES key schedule) and hash functions.

## Function Signature

```c
uint32_t rotate_left(uint32_t num, int n);
```

## Parameters

| Parameter | Type       | Description                                |
|-----------|------------|--------------------------------------------|
| `num`     | `uint32_t` | The input 32-bit unsigned integer          |
| `n`       | `int`      | Number of positions to rotate left (0-31)  |

## Return Value

Return the value of `num` rotated left by `n` bit positions.

## Examples

### Example 1
```
Input:  num = 0x00000001, n = 1
Output: 0x00000002
Explanation: Bit 0 moves to bit 1 position.
```

### Example 2
```
Input:  num = 0x80000000, n = 1
Output: 0x00000001
Explanation: MSB wraps around to become LSB.
```

### Example 3
```
Input:  num = 0x12345678, n = 4
Output: 0x23456781
Explanation: Rotating left by 4 moves the top nibble to the bottom.
```

### Example 4
```
Input:  num = 0x00000001, n = 31
Output: 0x80000000
Explanation: Bit 0 rotates all the way to become the MSB.
```

## Constraints

- `0 <= n <= 31`
- `num` is a valid 32-bit unsigned integer

## Notes

- Rotating by 0 returns the original value unchanged.
- Rotating by 32 is equivalent to rotating by 0 (full cycle).
- Think about combining a left shift with a right shift to capture the wrapped bits.
