# 014. Check Power of Two

## Difficulty: Easy

## Problem Statement

Given a 32-bit unsigned integer `num`, determine whether it is a power of two using bitwise operations.

A number is a power of two if it has exactly one bit set in its binary representation (e.g., 1, 2, 4, 8, 16...). Zero is NOT a power of two.

This check is frequently used in embedded systems for memory alignment verification, buffer size validation, and hardware register configuration.

## Function Signature

```c
int is_power_of_two(uint32_t num);
```

## Parameters

| Parameter | Type       | Description                       |
|-----------|------------|-----------------------------------|
| `num`     | `uint32_t` | The input 32-bit unsigned integer |

## Return Value

Return `1` if `num` is a power of two, `0` otherwise.

## Examples

### Example 1
```
Input:  num = 0x00000001
Output: 1
Explanation: 1 = 2^0, which is a power of two.
```

### Example 2
```
Input:  num = 0x00000010
Output: 1
Explanation: 16 = 2^4, which is a power of two (binary: 10000).
```

### Example 3
```
Input:  num = 0x00000006
Output: 0
Explanation: 6 = 110 in binary has two bits set, so it is NOT a power of two.
```

### Example 4
```
Input:  num = 0x00000000
Output: 0
Explanation: Zero is not a power of two (no bits are set).
```

## Constraints

- `num` is a valid 32-bit unsigned integer

## Notes

- Zero must be handled as a special case — it is NOT a power of two.
- A number with exactly one bit set is a power of two.
- Think about what `num - 1` looks like in binary when `num` is a power of two.
