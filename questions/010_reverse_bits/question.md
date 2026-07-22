# 010. Reverse Bits

## Difficulty: Medium

## Problem Statement

Given a 32-bit unsigned integer `num`, reverse the order of all 32 bits and return the result.

Bit reversal is commonly used in FFT (Fast Fourier Transform) algorithms, CRC calculations, and serial communication protocols where bit ordering matters.

## Function Signature

```c
uint32_t reverse_bits(uint32_t num);
```

## Parameters

| Parameter | Type       | Description                       |
|-----------|------------|-----------------------------------|
| `num`     | `uint32_t` | The input 32-bit unsigned integer |

## Return Value

Return the 32-bit unsigned integer formed by reversing all bits of `num`.

## Examples

### Example 1
```
Input:  num = 0x00000001
Output: 0x80000000
Explanation: Bit 0 becomes bit 31. Binary: 000...001 → 100...000
```

### Example 2
```
Input:  num = 0x80000000
Output: 0x00000001
Explanation: Bit 31 becomes bit 0.
```

### Example 3
```
Input:  num = 0x0000000F
Output: 0xF0000000
Explanation: Lower nibble 1111 moves to upper nibble position of MSB byte.
```

### Example 4
```
Input:  num = 0xAAAAAAAA
Output: 0x55555555
Explanation: Alternating 10101010... reverses to 01010101...
```

## Constraints

- `num` is a valid 32-bit unsigned integer

## Notes

- All 32 bits must be reversed, not just the significant ones.
- The operation is its own inverse: reversing twice gives the original value.
- Think about how to move each bit from position `i` to position `31 - i`.
