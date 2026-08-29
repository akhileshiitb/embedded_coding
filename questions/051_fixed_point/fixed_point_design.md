# Fixed-Point Arithmetic — Design Concepts

A reference for fixed-point math on embedded systems, focused on Q16.16. Read this before the
coding question.

---

## 1. Why Fixed-Point? (The No-FPU Problem)

Many MCUs have **no floating-point unit (FPU)**. On those parts, `float`/`double` operations are
emulated in software — often **10–100× slower** than integer math, and they pull in large library
code. Yet you still need fractional values: sensor scaling, filter coefficients, PID gains,
positions, ratios.

**Fixed-point** represents fractional numbers using plain **integers**, by agreeing that a fixed
number of the integer's bits represent the fraction. All arithmetic is then just integer add,
subtract, shift, and multiply — fast, deterministic, and FPU-free.

---

## 2. Q Notation and Q16.16

`Qm.n` means: `m` integer bits and `n` fractional bits. **Q16.16** packs a signed value into a
32-bit integer with **16 integer bits** and **16 fractional bits**:

```
 bit: 31                16 15                 0
      [   integer part    ][  fractional part  ]
              (signed)          (1/65536 units)
```

- The **scaling factor** is `2^16 = 65536`. A real value `x` is stored as the integer
  `round(x * 65536)`.
- Example: `1.0` → `1 * 65536 = 65536 = 0x00010000`. `0.5` → `32768 = 0x00008000`.
  `-1.0` → `-65536` (two's complement).
- Range: roughly `-32768.0 .. +32767.99998`, resolution `1/65536 ≈ 0.0000153`.

We use a signed 32-bit type for the fixed value and a signed 64-bit type for intermediate
products (see multiply).

---

## 3. Converting To/From Fixed-Point

- **int → fixed**: `fixed = i << 16` (i.e. `i * 65536`).
- **fixed → int** (truncate toward zero-ish): `i = fixed >> 16` (arithmetic shift keeps sign).
- **float → fixed** (host/debug only): `fixed = (int32_t)(f * 65536.0)`.
- **fixed → float**: `f = fixed / 65536.0`.

On the device you usually avoid float entirely; the float conversions are for host tooling/tests.

---

## 4. The Four Operations

### Add / Subtract — trivial
Two Q16.16 values share the same scale, so you just add/subtract the underlying integers:
```
add(a, b) = a + b
sub(a, b) = a - b
```
(Overflow if the true result exceeds the 32-bit range — see §5.)

### Multiply — the important one
Multiplying two Q16.16 values multiplies their scales too: `(a·S)·(b·S) = (a·b)·S²`. The raw
product is scaled by `2^32`, so you must **shift right by 16** to get back to Q16.16:
```
mul(a, b) = (int32_t)(((int64_t)a * (int64_t)b) >> 16)
```
- **Use a 64-bit intermediate.** `a * b` in 32 bits overflows almost immediately (e.g. 2.0 × 2.0
  needs the product `65536 × 65536 = 2^32`, which does not fit in 32 bits). Cast to `int64_t`
  first, multiply, then shift.
- The `>> 16` removes the extra scale factor.

### Divide — the mirror image
Division divides the scales away (`(a·S)/(b·S) = a/b`, unscaled), so you must **pre-shift the
numerator left by 16** to keep the fractional bits:
```
div(a, b) = (int32_t)((((int64_t)a) << 16) / b)
```
- Again a 64-bit intermediate: `a << 16` can exceed 32 bits.
- Divide-by-zero must be guarded (return a sentinel/saturate, or document it as UB — this question
  defines a specific behavior).

---

## 5. Overflow, Rounding, and Saturation

- **Overflow**: Q16.16 holds about ±32768. `mul`/`add` of large values can exceed that. Options:
  let it wrap (fast, like hardware), or **saturate** (clamp to min/max — safer for control loops).
  This question uses simple two's-complement wrap for add/sub and truncation for mul/div (the
  common baseline); saturation is a follow-up.
- **Rounding**: `>> 16` **truncates** toward negative infinity (arithmetic shift). To round to
  nearest, add half an LSB before shifting: `((a*b) + (1 << 15)) >> 16`. Truncation is simpler and
  is what this question expects.
- **Sign**: use **arithmetic** right shift on signed types so the sign bit is preserved. (C leaves
  signed `>>` implementation-defined, but every mainstream embedded compiler arithmetic-shifts.)

---

## 6. Choosing the Q Format

- **More fractional bits (e.g. Q8.24)** → finer resolution, smaller integer range.
- **More integer bits (e.g. Q24.8)** → larger range, coarser resolution.
- **Q16.16** is a popular balance in 32-bit systems: symmetric, easy mental math (shift by 16),
  decent range and resolution. DSP code often uses **Q15** (16-bit, all fraction, range ±1) for
  filter coefficients.

The format is a contract: every value in a computation must use the **same** Q format, or you must
explicitly rescale when mixing.

---

## 7. Follow-Up Topics (Umbrella)

- **Saturating arithmetic**: clamp results to `INT32_MAX/MIN` instead of wrapping — essential in
  control loops where a wrapped value causes wild actuator commands.
- **Rounding modes**: round-to-nearest (add half LSB), round-to-even, vs. truncation; matters for
  bias in accumulating filters.
- **Q15 / DSP formats**: 16-bit fixed-point for MAC-heavy filter code; the ARM CMSIS-DSP library is
  built around Q7/Q15/Q31.
- **Mixed formats & rescaling**: multiplying a Q16.16 by a Q8.24 requires aligning scales; shifting
  by the difference in fractional bits.
- **Overflow detection**: check the 64-bit product against the 32-bit range before narrowing, to
  flag/saturate instead of silently wrapping.
- **Reciprocal / sqrt / trig in fixed-point**: implemented via Newton's method, CORDIC, or lookup
  tables — no libm on the device.
- **Why not just scale everything by 1000?** Decimal fixed-point (scale by 10^k) is possible but
  binary scaling (`2^n`) lets you use cheap shifts instead of multiply/divide for the scale.
