#include <stdint.h>

/*
 * Q16.16 fixed-point arithmetic (no FPU).
 *   A real value x is stored as the int32_t  round(x * 65536).
 *   16 integer bits, 16 fractional bits. Scaling factor = 2^16 = 65536.
 *
 *   FP_SHIFT = 16, FP_ONE = 1 << 16 = 65536 = 1.0
 *
 * mul/div MUST use a 64-bit intermediate to avoid overflow. See
 * fixed_point_design.md for the theory.
 */

/* integer -> Q16.16 */
int32_t fp_from_int(int32_t i) {
    /* TODO: Implement */
    (void)i;
    return 0;
}

/* Q16.16 -> integer (arithmetic shift; truncates toward -inf) */
int32_t fp_to_int(int32_t x) {
    /* TODO: Implement */
    (void)x;
    return 0;
}

int32_t fp_add(int32_t a, int32_t b) {
    /* TODO: Implement */
    (void)a; (void)b;
    return 0;
}

int32_t fp_sub(int32_t a, int32_t b) {
    /* TODO: Implement */
    (void)a; (void)b;
    return 0;
}

/* multiply: use int64_t intermediate, then >> 16 */
int32_t fp_mul(int32_t a, int32_t b) {
    /* TODO: Implement */
    (void)a; (void)b;
    return 0;
}

/* divide: pre-shift numerator by 16 in int64_t, then divide. b==0 returns 0. */
int32_t fp_div(int32_t a, int32_t b) {
    /* TODO: Implement */
    (void)a; (void)b;
    return 0;
}
