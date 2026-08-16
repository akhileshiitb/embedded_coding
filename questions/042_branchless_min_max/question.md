# 042. Branchless Minimum and Maximum

## Difficulty: Medium

## Problem Statement

Implement `branchless_min` and `branchless_max` for signed 32-bit integers using **only bitwise operations and comparison** — no `if`, no ternary (`?:`), no branch of any kind.

**Key concepts to use:**
- In C, a comparison like `x < y` evaluates to `1` (true) or `0` (false)
- Negating these: `-(1)` = `0xFFFFFFFF` (all ones), `-(0)` = `0x00000000` (all zeros)
- This gives you a **conditional mask** — all-ones or all-zeros — which you can AND with something to conditionally select it
- XOR has the property: `a ^ (a ^ b) = b` and `a ^ 0 = a`

Your task: combine these building blocks (comparison → negation → mask → XOR) into single-expression implementations of min and max that work for all `int32_t` values without overflow.

---

## ⚠️ Embedded Pitfalls & Discussion Points

### 1. Why Branchless? Branch Misprediction Penalty

On pipelined processors (ARM Cortex-A, x86), a branch misprediction flushes the pipeline:

| Processor | Pipeline Depth | Misprediction Penalty |
|-----------|---------------|----------------------|
| Cortex-M4 | 3 stages | ~3 cycles |
| Cortex-A53 | 8 stages | ~8 cycles |
| Cortex-A72 | 15 stages | ~15 cycles |
| x86 (Skylake) | 14-19 stages | ~15-20 cycles |

For data-dependent branches (like `if (x < y)`) where the outcome is essentially random (e.g., comparing sensor readings), the branch predictor has ~50% accuracy. Branchless code eliminates this penalty entirely.

**When it matters:**
- Inner loops of sorting algorithms (comparison-swap)
- Real-time DSP filters (clamping values)
- Pixel processing in image pipelines
- Any tight loop where the comparison outcome is unpredictable

### 2. The "Quick and Dirty" Version Has Overflow Bugs

An alternative formula often seen is:
```c
min = y + ((x - y) & ((x - y) >> 31))
```

This **FAILS** when `x - y` overflows! Example:
- `x = INT_MAX (2147483647)`, `y = -1`
- `x - y = 2147483648` → **signed overflow (UB!)**

The XOR-based version `y ^ ((x ^ y) & -(x < y))` is safe because:
- XOR never overflows
- The comparison `x < y` is always well-defined for int32_t
- The negation `-( )` operates on 0 or 1 (no overflow)

### 3. The Comparison Generates a Branch Internally?

You might ask: "Doesn't `x < y` compile to a branch?" On most architectures, **no**:
- ARM: `cmp` + `cset` (conditional set) — no branch
- x86: `cmp` + `setl` — no branch
- The compiler materializes the boolean as 0/1 in a register using flag-based instructions

However, if you write `if (x < y) return x; else return y;` — the compiler **might** still generate branchless code (via `cmov` on x86 or conditional select on ARM). Modern compilers are smart. The value of knowing the branchless formula is:
1. Understanding what the compiler does under the hood
2. Guaranteeing branchless codegen when the compiler doesn't optimize
3. Interview knowledge

### 4. Unsigned vs Signed

The formula `-(x < y)` works for both signed and unsigned comparisons in C — the `<` operator handles the signedness based on the operand types. However, the "arithmetic right shift" alternative (`(x - y) >> 31`) only works for signed types and assumes arithmetic shift (implementation-defined).

### 5. Use in Embedded Firmware

- **Clamping ADC values**: `value = branchless_max(value, 0); value = branchless_min(value, 4095);`
- **PWM duty cycle limiting**: ensure 0 ≤ duty ≤ max_period without branches
- **Sorting networks**: branchless compare-and-swap for fixed-size sorts (used in SIMD and GPU kernels)
- **Median filters**: branchless min/max compose into branchless median

---

## Function Signatures
```c
int32_t branchless_min(int32_t x, int32_t y);
int32_t branchless_max(int32_t x, int32_t y);
```

## Parameters
| Parameter | Type      | Description     |
|-----------|-----------|-----------------|
| `x`       | `int32_t` | First integer   |
| `y`       | `int32_t` | Second integer  |

## Return Value
- `branchless_min` returns the smaller of x and y
- `branchless_max` returns the larger of x and y

## Examples

### Example 1
```
branchless_min(5, 9) → 5
branchless_max(5, 9) → 9
Explanation: 5 < 9, so min is 5 and max is 9.
```

### Example 2
```
branchless_min(10, 3) → 3
branchless_max(10, 3) → 10
Explanation: 10 is not less than 3, so min is 3 and max is 10.
```

### Example 3
```
branchless_min(-5, 3) → -5
branchless_max(-5, 3) → 3
Explanation: -5 < 3. Signed comparison handles negative correctly.
```

### Example 4
```
branchless_min(INT_MIN, INT_MAX) → INT_MIN (-2147483648)
branchless_max(INT_MIN, INT_MAX) → INT_MAX (2147483647)
Explanation: Extreme boundary values. No overflow — XOR and mask operations are always safe.
```

## Constraints
- Must NOT use `if`, `else`, ternary `?:`, or any branching construct
- Must NOT use subtraction (avoids overflow risk)
- Must work correctly for all int32_t values including INT_MIN, INT_MAX, negative, zero
- Both functions should be single-expression or use only bitwise ops + comparison
- Equal values: `branchless_min(x, x) == x` and `branchless_max(x, x) == x`

## Notes

### Interview Discussion Points:
1. **"Why XOR-based and not subtraction-based?"** → `x - y` overflows for extreme values (INT_MAX - negative). XOR never overflows.
2. **"Does this actually generate branchless code?"** → Yes. `x < y` compiles to `cmp` + `setl`/`cset` (no branch). Verify with `gcc -O2 -S`.
3. **"When would you use this vs just writing `if`?"** → When branch prediction fails ~50% of the time (random data in inner loops). For predictable branches (e.g., error checks), `if` is fine and more readable.
4. **"Can you derive max from min?"** → `max(x,y) = x ^ y ^ min(x,y)` — since XOR of all three gives the other one. Or just swap x/y in the formula: `x ^ ((x ^ y) & -(x < y))`.
5. **"What about CMOV?"** → On x86, the compiler may emit `cmov` (conditional move) for ternary `x < y ? x : y`. This is also branchless but uses a different mechanism (conditional execution). The XOR trick is portable across all architectures.
