# Hints

## Hint 1

Q16.16 stores a real number as `int32_t = round(x * 65536)`. The scale factor is `2^16`, so:
- **int → fixed**: shift left by 16 (`i << 16`) — multiply by 65536.
- **fixed → int**: arithmetic shift right by 16 (`x >> 16`) — divide by 65536, dropping the
  fractional bits (signed shift keeps the sign).
- **add / sub**: both operands share the same scale, so just `a + b` / `a - b`.

## Hint 2

**Multiply** and **divide** are where the scale factor bites, and where you need a 64-bit
intermediate:
- `a * b` multiplies the scales too: `(x·2^16)(y·2^16) = xy·2^32`. That's scaled by `2^32`, so
  shift right by 16 to return to Q16.16. And `a*b` overflows 32 bits (2.0×2.0 alone needs 2^32),
  so cast to `int64_t` first: `(int32_t)(((int64_t)a * b) >> 16)`.
- `a / b` cancels one scale factor (`(x·2^16)/(y·2^16) = x/y`, unscaled), so **pre-shift the
  numerator left by 16** to keep the fraction: `(int32_t)((((int64_t)a) << 16) / b)`. Guard
  `b == 0` (return 0 here).

## Hint 3

```c
#include <stdint.h>
#define FP_SHIFT 16

int32_t fp_from_int(int32_t i) { return i << FP_SHIFT; }
int32_t fp_to_int(int32_t x)   { return x >> FP_SHIFT; }   /* arithmetic shift */

int32_t fp_add(int32_t a, int32_t b) { return a + b; }
int32_t fp_sub(int32_t a, int32_t b) { return a - b; }

int32_t fp_mul(int32_t a, int32_t b) {
    return (int32_t)(((int64_t)a * (int64_t)b) >> FP_SHIFT);
}

int32_t fp_div(int32_t a, int32_t b) {
    if (b == 0) return 0;
    return (int32_t)((((int64_t)a) << FP_SHIFT) / b);
}
```

**Key reasoning:**
- The `int64_t` casts in `mul`/`div` are the crux — without them the intermediate overflows 32
  bits and you get garbage for anything but tiny values. This is the #1 thing interviewers check.
- `mul` shifts *right* by 16 (remove the extra scale); `div` shifts the numerator *left* by 16
  (preserve the fraction). They're mirror images.
- Arithmetic right shift on the signed `int32_t` keeps negative values correct.
- To round-to-nearest instead of truncating in `mul`, add half an LSB before shifting:
  `(((int64_t)a*b) + (1<<15)) >> 16`. This question truncates.
