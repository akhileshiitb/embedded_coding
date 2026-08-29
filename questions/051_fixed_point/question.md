# 051. Fixed-Point Arithmetic (Q16.16)

## Difficulty: Medium

## Problem Statement

Implement **Q16.16 fixed-point arithmetic** — representing fractional numbers using 32-bit
integers, for systems with **no FPU**. A Q16.16 value stores a real number `x` as the integer
`round(x * 65536)`: 16 integer bits and 16 fractional bits.

Implement conversion to/from integers and the four core operations (add, subtract, multiply,
divide).

> **Read `fixed_point_design.md` first.** It explains Q notation, the scaling factor, why
> multiply needs a 64-bit intermediate and a `>>16`, why divide pre-shifts, and overflow/rounding.

`solution.c` has plain function stubs — you implement the arithmetic.

---

## Representation

- Type: `int32_t` holds a Q16.16 value.
- Scaling factor: `2^16 = 65536`. Real `x` ↔ fixed `round(x * 65536)`.
- `1.0` = `65536` (`0x00010000`); `0.5` = `32768`; `-1.0` = `-65536`.

```
#define FP_SHIFT  16
#define FP_ONE    (1 << FP_SHIFT)     /* 65536 = 1.0 */
```

---

## What You Must Implement

```c
#include <stdint.h>

int32_t fp_from_int(int32_t i);      /* integer -> Q16.16 */
int32_t fp_to_int(int32_t x);        /* Q16.16 -> integer (truncate toward -inf via >>) */
int32_t fp_add(int32_t a, int32_t b);
int32_t fp_sub(int32_t a, int32_t b);
int32_t fp_mul(int32_t a, int32_t b);
int32_t fp_div(int32_t a, int32_t b);
```

---

## Behavior Specification

### `fp_from_int(int32_t i)`
Convert integer `i` to Q16.16: `i << 16`.

### `fp_to_int(int32_t x)`
Convert Q16.16 `x` to an integer by arithmetic right shift: `x >> 16` (drops the fractional bits;
truncates toward negative infinity, preserving sign).

### `fp_add(a, b)` / `fp_sub(a, b)`
Same-scale integer add/subtract: `a + b` / `a - b`. (Two's-complement wrap on overflow.)

### `fp_mul(a, b)`
Multiply with a **64-bit intermediate**, then shift back by 16:
```
result = (int32_t)(((int64_t)a * (int64_t)b) >> 16);
```
Truncation (arithmetic shift) — no rounding required.

### `fp_div(a, b)`
Pre-shift the numerator by 16 (in 64 bits) then divide:
```
result = (int32_t)((((int64_t)a) << 16) / b);
```
If `b == 0`, return `0` (defined behavior for this exercise).

---

## Examples

```
fp_from_int(3)              -> 196608        // 3 * 65536
fp_to_int(196608)           -> 3
fp_to_int(FP_ONE + 32768)   -> 1             // 1.5 truncates to 1

fp_add(FP_ONE, FP_ONE)      -> 131072        // 1.0 + 1.0 = 2.0
fp_sub(FP_ONE, 32768)       -> 32768         // 1.0 - 0.5 = 0.5

fp_mul(FP_ONE*2, FP_ONE*3)  -> 393216        // 2.0 * 3.0 = 6.0 (6*65536)
fp_mul(32768, 32768)        -> 16384         // 0.5 * 0.5 = 0.25 (0.25*65536)

fp_div(FP_ONE, FP_ONE*2)    -> 32768         // 1.0 / 2.0 = 0.5
fp_div(FP_ONE*7, FP_ONE*2)  -> 229376        // 7.0 / 2.0 = 3.5 (3.5*65536)
fp_div(FP_ONE, 0)           -> 0             // divide-by-zero guard
```

## Constraints
- Q16.16 in a signed 32-bit integer; scaling factor `65536`.
- `fp_mul` and `fp_div` **must** use a 64-bit (`int64_t`) intermediate to avoid overflow.
- `fp_mul` truncates (arithmetic `>> 16`); no rounding required.
- `fp_div` by zero returns `0`.
- Works for negative values (two's complement; arithmetic shifts preserve sign).
- Pure C, `<stdint.h>` only.

## Notes

### Interview Discussion Points (see `fixed_point_design.md`):
1. **Why fixed-point at all?** No FPU → float is emulated (slow, big). Fixed-point is integer math:
   fast, deterministic, small.
2. **Why 64-bit intermediate in multiply?** `2.0 * 2.0` needs `65536 * 65536 = 2^32`, which
   overflows 32 bits. Cast to `int64_t`, multiply, then `>> 16`.
3. **Why does divide pre-shift by 16?** Division cancels one scale factor; pre-shifting the
   numerator keeps the 16 fractional bits in the quotient.
4. **Rounding vs. truncation?** `>> 16` truncates toward -inf; add `1 << 15` before shifting to
   round-to-nearest. This question truncates.
5. **Overflow/saturation?** Baseline wraps; control loops often **saturate** (clamp) instead —
   see the follow-up umbrella.
