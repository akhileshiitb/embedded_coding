# 015. Endianness Swap (Byte Swap)

## Difficulty: Easy

## Problem Statement

Given a 32-bit unsigned integer `num`, reverse the byte order (swap endianness). This converts between big-endian and little-endian representation.

For a 32-bit integer stored as bytes `[B3, B2, B1, B0]`, the result should be `[B0, B1, B2, B3]`.

Endianness conversion is essential in embedded systems when communicating between processors with different byte orders, parsing network protocols (network byte order is big-endian), and reading data from storage.

## Function Signature

```c
uint32_t swap_endianness(uint32_t num);
```

## Parameters

| Parameter | Type       | Description                       |
|-----------|------------|-----------------------------------|
| `num`     | `uint32_t` | The input 32-bit unsigned integer |

## Return Value

Return the value with its four bytes reversed in order.

## Examples

### Example 1
```
Input:  num = 0x12345678
Output: 0x78563412
Explanation: Bytes [0x12, 0x34, 0x56, 0x78] become [0x78, 0x56, 0x34, 0x12].
```

### Example 2
```
Input:  num = 0x000000FF
Output: 0xFF000000
Explanation: The lowest byte moves to the highest position.
```

### Example 3
```
Input:  num = 0xFF000000
Output: 0x000000FF
Explanation: The highest byte moves to the lowest position.
```

### Example 4
```
Input:  num = 0xAABBCCDD
Output: 0xDDCCBBAA
Explanation: Each byte swaps symmetrically around the center.
```

## Constraints

- `num` is a valid 32-bit unsigned integer

## Notes

- The operation is its own inverse: swapping endianness twice returns the original value.
- This is equivalent to the `__builtin_bswap32()` GCC built-in or the `REV` ARM instruction.
- Think about extracting each byte with masking and shifting, then reassembling in reverse order.
