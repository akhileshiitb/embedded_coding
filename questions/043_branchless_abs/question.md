# 043. Branchless Absolute Value

## Difficulty: Medium

## Problem Statement

Implement `branchless_abs` — compute the absolute value of a signed 32-bit integer using **only bitwise operations and addition** — no `if`, no ternary (`?:`), no branch, and no `abs()` or standard library calls.

**Key concepts to use:**
- Arithmetic right shift of a signed integer by 31 positions replicates the sign bit across all 32 bits
- For negative values: this produces `0xFFFFFFFF` (all ones = -1 in two's complement)
- For positive/zero values: this produces `0x00000000` (all zeros)
- This "sign mask" can be used to conditionally negate a value without branching
- Two's complement negation: `-v = (~v) + 1 = (v ^ 0xFFFFFFFF) + 1`

Your task: use the arithmetic right shift to create a sign mask, then combine addition and XOR to conditionally negate negative inputs.

---

## ⚠️ Embedded Pitfalls & Discussion Points (Senior-Level)

### 1. Arithmetic Right Shift is Implementation-Defined

The C standard (C11 §6.5.7) states:

> "If E1 has a signed type and a negative value, the resulting value is **implementation-defined**."

This means `v >> 31` is NOT guaranteed to sign-extend. On a hypothetical machine, it could zero-fill instead (logical shift). In practice:
- **GCC, Clang, MSVC, ARM compilers**: all use arithmetic shift for signed types
- **The C standard does NOT guarantee it**

For strictly portable code, you'd write:
```c
int32_t mask = -(v < 0);  /* Guaranteed portable */
```

But in embedded firmware targeting a known compiler/architecture (ARM + GCC), `v >> 31` is safe and avoids the comparison instruction.

### 2. abs(INT_MIN) is Undefined Behavior

`INT_MIN = -2147483648`. Its absolute value would be `+2147483648`, which **cannot be represented** in `int32_t` (max is `2147483647`). This is signed overflow → **undefined behavior**.

In practice, on two's complement hardware: `abs(INT_MIN)` wraps to `INT_MIN` (the bit pattern stays the same). Your branchless implementation will exhibit this same behavior — and that's acceptable for this exercise. But in an interview, you must **acknowledge this edge case unprompted**.

### 3. How the Trick Works (Two's Complement Insight)

For negative `v`:
```
mask = v >> 31 = 0xFFFFFFFF (all ones = -1)
(v + mask) = v + (-1) = v - 1
(v + mask) ^ mask = (v - 1) ^ 0xFFFFFFFF = ~(v - 1) = -v
```

For positive/zero `v`:
```
mask = v >> 31 = 0x00000000
(v + mask) = v + 0 = v
(v + mask) ^ mask = v ^ 0 = v
```

This is equivalent to: if negative, negate (via two's complement identity `~(v-1) = -v`); if positive, leave unchanged.

### 4. Why Branchless Abs Matters in Embedded

- **DSP signal processing**: Computing magnitude of audio samples in real-time (no branch penalty)
- **Motor control**: Absolute error calculation in PID loops running at 10+ kHz
- **Image processing**: SAD (Sum of Absolute Differences) for block matching — inner loop called millions of times
- **Sensor calibration**: Absolute deviation from target in feedback controllers

### 5. Alternative: The Subtraction-Based Version

Another common version:
```c
mask = v >> 31;
r = (v ^ mask) - mask;
```

This works because `(v ^ 0xFFFFFFFF) - (-1) = ~v + 1 = -v`. Both versions are equivalent; interviewers may ask you to derive both and explain why they're the same.

---

## Function Signature
```c
int32_t branchless_abs(int32_t v);
```

## Parameters
| Parameter | Type      | Description                    |
|-----------|-----------|--------------------------------|
| `v`       | `int32_t` | The input signed integer       |

## Return Value
The absolute value of `v`. For `INT_MIN`, the result wraps to `INT_MIN` (same as hardware behavior).

## Examples

### Example 1
```
branchless_abs(5) → 5
Explanation: Positive value — unchanged.
```

### Example 2
```
branchless_abs(-5) → 5
Explanation: Negative value — negated to positive.
```

### Example 3
```
branchless_abs(0) → 0
Explanation: Zero — unchanged.
```

### Example 4
```
branchless_abs(INT_MIN) → INT_MIN (-2147483648)
Explanation: abs(INT_MIN) cannot be represented in int32_t.
             Result wraps — this is expected and matches hardware behavior.
```

## Constraints
- Must NOT use `if`, `else`, ternary `?:`, or any branching construct
- Must NOT call `abs()`, `labs()`, or any library function
- Must use arithmetic right shift to create the sign mask
- Must work for all int32_t values (INT_MIN wraps to itself — accepted)
- Single expression or 2-3 lines max (mask + result)

## Notes

### Interview Discussion Points:
1. **"Is arithmetic right shift portable?"** → No, it's implementation-defined. But all mainstream embedded compilers (GCC/Clang on ARM, x86, RISC-V) use arithmetic shift for signed types. For strict portability, use `-(v < 0)` instead.
2. **"What happens with abs(INT_MIN)?"** → UB per the standard. In practice, wraps to INT_MIN on two's complement hardware. A senior candidate mentions this unprompted.
3. **"Show me two equivalent formulations"** → `(v + mask) ^ mask` and `(v ^ mask) - mask` are both correct. Derive one from the other using `~v = v ^ 0xFFFFFFFF` and `-1 = 0xFFFFFFFF`.
4. **"Why not just use a branch?"** → In a tight DSP loop computing abs() on every sample at 48kHz × 1024 channels, branch misprediction (~50% on audio signals crossing zero) costs 8-15 cycles per call. Branchless = deterministic timing.
5. **"What does the compiler generate for abs()?"** → With `-O2`, GCC on ARM generates: `eor mask, v, v asr #31; sub r, v, mask` — which IS this branchless trick. The compiler already knows it. But you need to demonstrate you understand it.
