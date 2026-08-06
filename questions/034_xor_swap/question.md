# 034. XOR Swap with Aliasing Guard

## Difficulty: Medium

## Problem Statement

Implement `xor_swap` — a function that swaps two integers using only XOR operations (no temporary variable).

**However**, your implementation must handle a critical embedded systems pitfall:

### ⚠️ The Aliasing Trap (Senior-Level Differentiator)

The naive XOR swap (`a^=b; b^=a; a^=b;`) **fails catastrophically** when both pointers point to the same memory location. If `a == b` (same address), the first `*a ^= *b` zeros out the value, and the remaining operations propagate zero — **both values are permanently destroyed**.

In embedded firmware, this aliasing scenario occurs more often than you'd expect:
- Sorting algorithms calling `swap(&arr[i], &arr[j])` when `i == j`
- DMA scatter-gather descriptors referencing the same buffer
- Register bank operations where source and destination are the same register

Your implementation **must check for pointer aliasing** before performing the XOR swap.

### ⚠️ Why XOR Swap is Avoided in Production Firmware

Even when aliasing is handled, XOR swap has real drawbacks on modern embedded processors:

1. **Pipeline stalls**: The three sequential XOR operations create **Read-After-Write (RAW) data dependencies** — each operation depends on the result of the previous one. On pipelined ARM Cortex-M/A cores, this prevents instruction-level parallelism and causes pipeline bubbles.

2. **No performance benefit**: A temp-based swap compiles to simple register moves (which execute in parallel), while XOR swap forces serial execution. On ARM Cortex-M4+, temp swap is typically **2-3 cycles faster**.

3. **Readability and maintainability**: XOR swap obscures intent, making code review and debugging harder in safety-critical firmware (MISRA C discourages "clever" constructs).

**Your task**: Implement the XOR swap correctly with the aliasing guard, understanding that this is an interview exercise — in production, you'd use a temp variable.

## Function Signature
```c
void xor_swap(int32_t *a, int32_t *b);
```

## Parameters
| Parameter | Type       | Description                        |
|-----------|------------|------------------------------------|
| `a`       | `int32_t*` | Pointer to the first integer       |
| `b`       | `int32_t*` | Pointer to the second integer      |

## Return Value
No return value. The values at `*a` and `*b` are swapped in-place.

## Examples

### Example 1: Normal swap
```
Input:  *a = 5, *b = 9   (a != b, different addresses)
Output: *a = 9, *b = 5
Explanation: XOR swap proceeds normally.
             *a ^= *b → *a = 5^9 = 12
             *b ^= *a → *b = 9^12 = 5
             *a ^= *b → *a = 12^5 = 9
```

### Example 2: Aliasing (CRITICAL)
```
Input:  *a = 42, a == b (SAME address)
Output: *a = 42 (value preserved)
Explanation: Pointers alias the same location. Without the guard,
             *a ^= *b → *a = 42^42 = 0  ← VALUE DESTROYED!
             With the guard: detect a == b, return immediately.
```

### Example 3: Swapping zeros
```
Input:  *a = 0, *b = 7
Output: *a = 7, *b = 0
Explanation: XOR with zero is identity. Works correctly.
```

### Example 4: Negative values
```
Input:  *a = -1 (0xFFFFFFFF), *b = 1 (0x00000001)
Output: *a = 1, *b = -1
Explanation: XOR swap works on the bit patterns regardless of sign.
```

## Constraints
- `a` and `b` are valid non-NULL pointers to `int32_t`
- Pointers **may alias** (point to the same memory location) — you must handle this
- Values can be any valid `int32_t` including 0, INT_MIN, INT_MAX, negative numbers
- You must NOT use a temporary variable, arithmetic operators (+, -, *), or any operator other than `^=` for the swap logic

## Notes

### Interview Discussion Points (What makes this a senior question):

1. **Aliasing awareness**: Can you identify *when* XOR swap breaks without being told?
2. **Pipeline analysis**: Can you explain the RAW dependency chain on a 5-stage ARM pipeline?
3. **Production judgment**: Would you use XOR swap in shipping firmware? (Answer: No — use a temp or compiler `__builtin_swap` / `std::swap`)
4. **Follow-up**: "What if I told you the compiler will optimize a temp-based swap into register renames with zero cost?" (It does — compilers are smarter than XOR tricks)
5. **Strict aliasing rule**: In C, `int32_t *a` and `int32_t *b` of the same type *can* legally alias. But if the types differed, the compiler might assume no aliasing under `-fstrict-aliasing` and reorder the XOR operations, causing silent corruption.
