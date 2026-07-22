# 013. Rotate Right

## Difficulty: Easy

## Problem Statement

Given a 32-bit unsigned integer `num` and a rotation count `n`, perform a right circular rotation by `n` positions and return the result.

In a right rotation, bits shifted out from the LSB (bit 0) wrap around to the MSB (bit 31). This is the complement of left rotation.

Right rotation is used in ARM instruction encoding (barrel shifter), hash functions, and cryptographic primitives.

## Function Signature

```c
uint32_t rotate_right(uint32_t num, int n);
```

## Parameters

| Parameter | Type       | Description                                 |
|-----------|------------|---------------------------------------------|
| `num`     | `uint32_t` | The input 32-bit unsigned integer           |
| `n`       | `int`      | Number of positions to rotate right (0-31)  |

## Return Value

Return the value of `num` rotated right by `n` bit positions.

## Examples

### Example 1
```
Input:  num = 0x00000001, n = 1
Output: 0x80000000
Explanation: LSB wraps around to become MSB.
```

### Example 2
```
Input:  num = 0x80000000, n = 1
Output: 0x40000000
Explanation: MSB (bit 31) moves down to bit 30.
```

### Example 3
```
Input:  num = 0x12345678, n = 4
Output: 0x81234567
Explanation: Rotating right by 4 moves the bottom nibble to the top.
```

### Example 4
```
Input:  num = 0x12345678, n = 8
Output: 0x78123456
Explanation: Rotating right by 8 moves the bottom byte to the top.
```

## Constraints

- `0 <= n <= 31`
- `num` is a valid 32-bit unsigned integer

## Notes

- Rotating by 0 returns the original value unchanged.
- Rotating right by `n` is equivalent to rotating left by `32 - n`.
- Think about combining a right shift with a left shift to capture the wrapped bits.
