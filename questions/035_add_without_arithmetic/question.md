# 035. Addition Without Arithmetic Operators

## Difficulty: Medium

## Problem Statement

Implement integer addition of two `int32_t` values using **only bitwise operations** — no `+`, `-`, `*`, `/`, or `%` operators allowed.

The approach uses the half-adder principle: XOR computes the sum without carry, AND followed by left-shift computes the carry, and you iterate until the carry is zero.

## ⚠️ Critical Embedded Pitfalls (Senior-Level Differentiators)

### 1. Signed Left-Shift is Undefined Behavior

The carry computation `(a & b) << 1` triggers **undefined behavior** in C when `a & b` is negative (i.e., has the MSB set). Per C11 §6.5.7:

> "If E1 has a signed type and nonnegative value, and E1 × 2^E2 is representable in the result type, then that is the resulting value; **otherwise, the behavior is undefined**."

This means left-shifting a negative `int32_t` is UB. The compiler is free to:
- Produce garbage results
- Optimize away your entire loop (assuming UB "never happens")
- Generate different code at `-O0` vs `-O2`

**Fix**: Cast to `uint32_t` before shifting the carry, then cast back.

### 2. Infinite Loop with INT_MIN

Consider adding `-1` (0xFFFFFFFF) and `1`:
- XOR = 0xFFFFFFFE, Carry = 0x00000002
- The carry propagates through all 32 bits...
- If you don't cast to unsigned, the compiler might "optimize" the loop into non-termination because it assumes signed overflow never happens.

With proper `uint32_t` casts, the carry always eventually becomes zero (in at most 32 iterations) because each iteration shifts the carry left, and bits "fall off" the top.

### 3. Two's Complement — It Just Works (if you avoid UB)

The beauty of two's complement addition is that signed and unsigned addition produce **identical bit patterns**. By casting to `uint32_t`, performing the XOR/carry loop, and casting the result back to `int32_t`, you get correct signed results including negative numbers — no special sign handling needed.

### 4. Embedded Relevance

- **ALU design**: This is literally how a ripple-carry adder works in hardware (XOR = sum, AND = carry, shift = propagate)
- **DSP processors without barrel shifters**: Some minimal MCUs (PIC, early 8051) lack a hardware multiply and use this pattern for multi-byte addition
- **Compiler verification**: Understanding this helps verify that compiler-generated add instructions match expected behavior on new silicon

## Function Signature
```c
int32_t add(int32_t a, int32_t b);
```

## Parameters
| Parameter | Type      | Description          |
|-----------|-----------|----------------------|
| `a`       | `int32_t` | First integer        |
| `b`       | `int32_t` | Second integer       |

## Return Value
Return the sum `a + b`, computed using only bitwise operations.

## Examples

### Example 1
```
Input:  a = 5, b = 3
Output: 8
Explanation: Binary 0101 + 0011:
             XOR (sum without carry) = 0110 (6)
             AND (carry bits)        = 0001, shift left = 0010 (2)
             Next iteration: XOR(6,2)=4, AND(6,2)=2, shift=4
             Next: XOR(4,4)=0, AND(4,4)=4, shift=8
             Next: XOR(0,8)=8, AND(0,8)=0 → carry is zero, done. Result = 8
```

### Example 2
```
Input:  a = -1, b = 1
Output: 0
Explanation: 0xFFFFFFFF + 0x00000001 = 0x00000000 (two's complement wraparound).
             Carry propagates through all 32 bits then drops off.
```

### Example 3
```
Input:  a = -5, b = -3
Output: -8
Explanation: Two negative numbers. Bit patterns add correctly in two's complement.
```

### Example 4
```
Input:  a = INT_MIN (-2147483648), b = 0
Output: -2147483648
Explanation: Adding zero — carry is immediately zero. No UB triggered.
```

## Constraints
- `a` and `b` are any valid `int32_t` values (including INT_MIN, INT_MAX, negative, zero)
- You must NOT use `+`, `-`, `*`, `/`, or `%` operators
- You MUST cast to `uint32_t` before left-shifting carry to avoid undefined behavior
- The result wraps on overflow (same as hardware addition — no overflow detection required)

## Notes

### Interview Follow-Up Questions:
1. **"Why must you cast to unsigned before shifting?"** → Left-shift of negative signed values is UB in C. The compiler can assume it never happens and optimize your loop away.
2. **"What's the maximum number of iterations?"** → At most 32, because each iteration shifts carry left by 1, and after 32 shifts all carry bits fall off the 32-bit width.
3. **"Is this O(1) or O(n)?"** → O(n) where n is the bit width (32). Each iteration propagates carry by at least one position.
4. **"How does this relate to hardware?"** → This IS a ripple-carry adder. A carry-lookahead adder (CLA) computes all carries in parallel using generate/propagate logic — O(log n) gate delay.
5. **"Can you use this to implement subtraction?"** → Yes: `subtract(a, b) = add(a, add(~b, 1))` — negate b using two's complement, then add.
