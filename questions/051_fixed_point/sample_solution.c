#include <stdint.h>

/* Sample solution — Q16.16 fixed-point arithmetic. */

#define FP_SHIFT 16

int32_t fp_from_int(int32_t i) {
    return i << FP_SHIFT;
}

int32_t fp_to_int(int32_t x) {
    return x >> FP_SHIFT;               /* arithmetic shift preserves sign */
}

int32_t fp_add(int32_t a, int32_t b) {
    return a + b;
}

int32_t fp_sub(int32_t a, int32_t b) {
    return a - b;
}

int32_t fp_mul(int32_t a, int32_t b) {
    return (int32_t)(((int64_t)a * (int64_t)b) >> FP_SHIFT);
}

int32_t fp_div(int32_t a, int32_t b) {
    if (b == 0) return 0;               /* divide-by-zero guard */
    return (int32_t)((((int64_t)a) << FP_SHIFT) / b);
}
