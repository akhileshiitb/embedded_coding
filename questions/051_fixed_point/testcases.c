#include <stdio.h>
#include <stdint.h>

extern int32_t fp_from_int(int32_t i);
extern int32_t fp_to_int(int32_t x);
extern int32_t fp_add(int32_t a, int32_t b);
extern int32_t fp_sub(int32_t a, int32_t b);
extern int32_t fp_mul(int32_t a, int32_t b);
extern int32_t fp_div(int32_t a, int32_t b);

#define FP_ONE (1 << 16)   /* 65536 = 1.0 */

int main(void) {
    int passed = 0;
    int num_tests = 16;
    int test_num = 0;

    /* === conversions === */
    { test_num++;
      if (fp_from_int(3) == 196608) { passed++; printf("[PASS] Test %d: fp_from_int(3)=196608\n", test_num); }
      else printf("[FAIL] Test %d: got %d\n", test_num, fp_from_int(3)); }

    { test_num++;
      if (fp_from_int(-2) == -131072) { passed++; printf("[PASS] Test %d: fp_from_int(-2)=-131072\n", test_num); }
      else printf("[FAIL] Test %d: got %d\n", test_num, fp_from_int(-2)); }

    { test_num++;
      if (fp_to_int(196608) == 3) { passed++; printf("[PASS] Test %d: fp_to_int(196608)=3\n", test_num); }
      else printf("[FAIL] Test %d: got %d\n", test_num, fp_to_int(196608)); }

    { test_num++;
      /* 1.5 (FP_ONE + 32768) truncates to 1 */
      if (fp_to_int(FP_ONE + 32768) == 1) { passed++; printf("[PASS] Test %d: fp_to_int(1.5)=1 (truncate)\n", test_num); }
      else printf("[FAIL] Test %d: got %d\n", test_num, fp_to_int(FP_ONE + 32768)); }

    /* === add / sub === */
    { test_num++;
      if (fp_add(FP_ONE, FP_ONE) == 131072) { passed++; printf("[PASS] Test %d: 1.0+1.0=2.0\n", test_num); }
      else printf("[FAIL] Test %d: got %d\n", test_num, fp_add(FP_ONE, FP_ONE)); }

    { test_num++;
      if (fp_sub(FP_ONE, 32768) == 32768) { passed++; printf("[PASS] Test %d: 1.0-0.5=0.5\n", test_num); }
      else printf("[FAIL] Test %d: got %d\n", test_num, fp_sub(FP_ONE, 32768)); }

    { test_num++;
      /* negative result: 0.5 - 1.0 = -0.5 */
      if (fp_sub(32768, FP_ONE) == -32768) { passed++; printf("[PASS] Test %d: 0.5-1.0=-0.5\n", test_num); }
      else printf("[FAIL] Test %d: got %d\n", test_num, fp_sub(32768, FP_ONE)); }

    /* === multiply === */
    { test_num++;
      /* 2.0 * 3.0 = 6.0 -> 6*65536 = 393216 (also proves >32bit intermediate) */
      if (fp_mul(FP_ONE*2, FP_ONE*3) == 393216) { passed++; printf("[PASS] Test %d: 2.0*3.0=6.0\n", test_num); }
      else printf("[FAIL] Test %d: got %d\n", test_num, fp_mul(FP_ONE*2, FP_ONE*3)); }

    { test_num++;
      /* 0.5 * 0.5 = 0.25 -> 0.25*65536 = 16384 */
      if (fp_mul(32768, 32768) == 16384) { passed++; printf("[PASS] Test %d: 0.5*0.5=0.25\n", test_num); }
      else printf("[FAIL] Test %d: got %d\n", test_num, fp_mul(32768, 32768)); }

    { test_num++;
      /* negative: -2.0 * 3.0 = -6.0 */
      if (fp_mul(-FP_ONE*2, FP_ONE*3) == -393216) { passed++; printf("[PASS] Test %d: -2.0*3.0=-6.0\n", test_num); }
      else printf("[FAIL] Test %d: got %d\n", test_num, fp_mul(-FP_ONE*2, FP_ONE*3)); }

    { test_num++;
      /* mul by 1.0 is identity */
      if (fp_mul(12345, FP_ONE) == 12345) { passed++; printf("[PASS] Test %d: x*1.0=x\n", test_num); }
      else printf("[FAIL] Test %d: got %d\n", test_num, fp_mul(12345, FP_ONE)); }

    /* === divide === */
    { test_num++;
      /* 1.0 / 2.0 = 0.5 */
      if (fp_div(FP_ONE, FP_ONE*2) == 32768) { passed++; printf("[PASS] Test %d: 1.0/2.0=0.5\n", test_num); }
      else printf("[FAIL] Test %d: got %d\n", test_num, fp_div(FP_ONE, FP_ONE*2)); }

    { test_num++;
      /* 7.0 / 2.0 = 3.5 -> 3.5*65536 = 229376 */
      if (fp_div(FP_ONE*7, FP_ONE*2) == 229376) { passed++; printf("[PASS] Test %d: 7.0/2.0=3.5\n", test_num); }
      else printf("[FAIL] Test %d: got %d\n", test_num, fp_div(FP_ONE*7, FP_ONE*2)); }

    { test_num++;
      /* negative: -6.0 / 3.0 = -2.0 */
      if (fp_div(-FP_ONE*6, FP_ONE*3) == -131072) { passed++; printf("[PASS] Test %d: -6.0/3.0=-2.0\n", test_num); }
      else printf("[FAIL] Test %d: got %d\n", test_num, fp_div(-FP_ONE*6, FP_ONE*3)); }

    { test_num++;
      /* divide by zero -> 0 */
      if (fp_div(FP_ONE, 0) == 0) { passed++; printf("[PASS] Test %d: divide-by-zero returns 0\n", test_num); }
      else printf("[FAIL] Test %d: got %d\n", test_num, fp_div(FP_ONE, 0)); }

    { test_num++;
      /* round trip: mul then div inverse. (3.0 * 2.5) / 2.5 = 3.0 */
      int32_t v = fp_mul(FP_ONE*3, FP_ONE*2 + 32768);   /* 3.0 * 2.5 = 7.5 */
      int32_t back = fp_div(v, FP_ONE*2 + 32768);        /* 7.5 / 2.5 = 3.0 */
      if (back == FP_ONE*3) { passed++; printf("[PASS] Test %d: (3.0*2.5)/2.5=3.0 round trip\n", test_num); }
      else printf("[FAIL] Test %d: got %d (expected %d)\n", test_num, back, FP_ONE*3); }

    printf("\nResults: %d/%d passed\n", passed, num_tests);
    return (passed == num_tests) ? 0 : 1;
}
