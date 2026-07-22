# 009. Count Set Bits (Popcount / Hamming Weight)

## Difficulty: Easy

## Problem Statement

Given a 32-bit unsigned integer `num`, count the number of bits that are set to 1 (also known as the population count or Hamming weight).

This is a fundamental operation in embedded systems used for error detection, data compression, and hardware diagnostics.

## Function Signature

```c
int count_set_bits(uint32_t num);
```

## Parameters

| Parameter | Type       | Description                       |
|-----------|------------|-----------------------------------|
| `num`     | `uint32_t` | The input 32-bit unsigned integer |

## Return Value

Return the number of bits set to 1 in `num` (a value between 0 and 32).

## Examples

### Example 1
```
Input:  num = 0x00000000
Output: 0
Explanation: Zero has no bits set.
```

### Example 2
```
Input:  num = 0x00000001
Output: 1
Explanation: Only bit 0 is set.
```

### Example 3
```
Input:  num = 0x0000000F
Output: 4
Explanation: 0xF = 1111 in binary, so 4 bits are set.
```

### Example 4
```
Input:  num = 0xFFFFFFFF
Output: 32
Explanation: All 32 bits are set.
```

## Constraints

- `num` is a valid 32-bit unsigned integer

## Notes

- The simplest approach checks each bit one at a time.
- Brian Kernighan's algorithm is a well-known optimization.
- Many CPUs have a hardware `POPCNT` instruction for this operation.
