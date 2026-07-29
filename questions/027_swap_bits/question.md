# 027. Swap Bits at Two Positions

## Difficulty: Easy

## Problem Statement

Given a 32-bit unsigned integer `num`, and two bit positions `pos1` and `pos2`, write a function to swap the bits at these two positions. The bit positions are 0-indexed (bit 0 is the LSB).

If both bits are the same (both 0 or both 1), the number remains unchanged after swapping.

## Function Signature
```c
uint32_t swap_bits(uint32_t num, int pos1, int pos2);
```

## Parameters
| Parameter | Type       | Description                              |
|-----------|------------|------------------------------------------|
| `num`     | `uint32_t` | The input 32-bit unsigned integer        |
| `pos1`    | `int`      | First bit position to swap (0-indexed)   |
| `pos2`    | `int`      | Second bit position to swap (0-indexed)  |

## Return Value
Return the value of `num` with the bits at positions `pos1` and `pos2` swapped.

## Examples

### Example 1
```
Input:  num = 0x00000001, pos1 = 0, pos2 = 1
Output: 0x00000002
Explanation: Bit 0 is 1, bit 1 is 0. After swapping: bit 0 becomes 0, bit 1 becomes 1.
             Binary: 01 → 10, so 0x01 → 0x02.
```

### Example 2
```
Input:  num = 0x00000003, pos1 = 0, pos2 = 1
Output: 0x00000003
Explanation: Bit 0 is 1, bit 1 is 1. Both bits are the same, so swapping has no effect.
```

### Example 3
```
Input:  num = 0x80000000, pos1 = 0, pos2 = 31
Output: 0x00000001
Explanation: Bit 31 is 1, bit 0 is 0. After swapping: bit 31 becomes 0, bit 0 becomes 1.
             Result: 0x80000000 → 0x00000001.
```

### Example 4
```
Input:  num = 0xAAAAAAAA, pos1 = 0, pos2 = 1
Output: 0xAAAAAAA9
Explanation: 0xAAAAAAAA = ...1010 1010. Bit 0 is 0, bit 1 is 1.
             After swap: ...1010 1001 = 0xAAAAAAA9.
```

## Constraints
- `0 <= pos1 <= 31`
- `0 <= pos2 <= 31`
- `num` is a valid 32-bit unsigned integer

## Notes
- If `pos1 == pos2`, the number should remain unchanged.
- Only swap when the two bits differ; if they are the same, no change is needed.
- Think about how XOR can be used to toggle specific bits.
