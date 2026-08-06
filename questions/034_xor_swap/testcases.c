#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Declaration of the solution function */
extern void xor_swap(int32_t *a, int32_t *b);

int main(void) {
    int passed = 0;
    int num_tests = 18;
    int test_num = 0;

    /* ===== Basic swap cases ===== */

    /* Test 1: Basic positive swap */
    {
        test_num++;
        int32_t a = 5, b = 9;
        xor_swap(&a, &b);
        if (a == 9 && b == 5) {
            passed++;
            printf("[PASS] Test %d: Basic positive swap (5,9)\n", test_num);
        } else {
            printf("[FAIL] Test %d: Basic positive swap (5,9) | Expected: a=9, b=5 | Got: a=%d, b=%d\n",
                   test_num, a, b);
        }
    }

    /* Test 2: Swap different values */
    {
        test_num++;
        int32_t a = 100, b = 200;
        xor_swap(&a, &b);
        if (a == 200 && b == 100) {
            passed++;
            printf("[PASS] Test %d: Swap 100 and 200\n", test_num);
        } else {
            printf("[FAIL] Test %d: Swap 100 and 200 | Expected: a=200, b=100 | Got: a=%d, b=%d\n",
                   test_num, a, b);
        }
    }

    /* Test 3: Swap with one zero */
    {
        test_num++;
        int32_t a = 0, b = 7;
        xor_swap(&a, &b);
        if (a == 7 && b == 0) {
            passed++;
            printf("[PASS] Test %d: Swap zero and 7\n", test_num);
        } else {
            printf("[FAIL] Test %d: Swap zero and 7 | Expected: a=7, b=0 | Got: a=%d, b=%d\n",
                   test_num, a, b);
        }
    }

    /* Test 4: Swap both zeros (different addresses) */
    {
        test_num++;
        int32_t a = 0, b = 0;
        xor_swap(&a, &b);
        if (a == 0 && b == 0) {
            passed++;
            printf("[PASS] Test %d: Swap two zeros (different addresses)\n", test_num);
        } else {
            printf("[FAIL] Test %d: Swap two zeros (different addresses) | Expected: a=0, b=0 | Got: a=%d, b=%d\n",
                   test_num, a, b);
        }
    }

    /* Test 5: Same value, different addresses */
    {
        test_num++;
        int32_t a = 42, b = 42;
        xor_swap(&a, &b);
        if (a == 42 && b == 42) {
            passed++;
            printf("[PASS] Test %d: Same value (42), different addresses\n", test_num);
        } else {
            printf("[FAIL] Test %d: Same value (42), different addresses | Expected: a=42, b=42 | Got: a=%d, b=%d\n",
                   test_num, a, b);
        }
    }

    /* ===== ALIASING TRAP (the critical pitfall) ===== */

    /* Test 6: ALIASING - same pointer, positive value */
    {
        test_num++;
        int32_t x = 42;
        xor_swap(&x, &x);  /* SAME ADDRESS! */
        if (x == 42) {
            passed++;
            printf("[PASS] Test %d: ALIASING: same address, value 42 preserved\n", test_num);
        } else {
            printf("[FAIL] Test %d: ALIASING: same address, value 42 | Expected: x=42 | Got: x=%d (ZEROED!)\n",
                   test_num, x);
        }
    }

    /* Test 7: ALIASING - same pointer, negative value */
    {
        test_num++;
        int32_t x = -99;
        xor_swap(&x, &x);
        if (x == -99) {
            passed++;
            printf("[PASS] Test %d: ALIASING: same address, value -99 preserved\n", test_num);
        } else {
            printf("[FAIL] Test %d: ALIASING: same address, value -99 | Expected: x=-99 | Got: x=%d (ZEROED!)\n",
                   test_num, x);
        }
    }

    /* Test 8: ALIASING - same pointer, zero value */
    {
        test_num++;
        int32_t x = 0;
        xor_swap(&x, &x);
        if (x == 0) {
            passed++;
            printf("[PASS] Test %d: ALIASING: same address, value 0 preserved\n", test_num);
        } else {
            printf("[FAIL] Test %d: ALIASING: same address, value 0 | Expected: x=0 | Got: x=%d\n",
                   test_num, x);
        }
    }

    /* Test 9: ALIASING - same pointer, INT_MAX */
    {
        test_num++;
        int32_t x = 2147483647;
        xor_swap(&x, &x);
        if (x == 2147483647) {
            passed++;
            printf("[PASS] Test %d: ALIASING: same address, INT_MAX preserved\n", test_num);
        } else {
            printf("[FAIL] Test %d: ALIASING: same address, INT_MAX | Expected: x=2147483647 | Got: x=%d (ZEROED!)\n",
                   test_num, x);
        }
    }

    /* Test 10: ALIASING - same pointer, INT_MIN */
    {
        test_num++;
        int32_t x = (-2147483647 - 1);
        xor_swap(&x, &x);
        if (x == (-2147483647 - 1)) {
            passed++;
            printf("[PASS] Test %d: ALIASING: same address, INT_MIN preserved\n", test_num);
        } else {
            printf("[FAIL] Test %d: ALIASING: same address, INT_MIN | Expected: x=-2147483648 | Got: x=%d (ZEROED!)\n",
                   test_num, x);
        }
    }

    /* ===== Negative and boundary values ===== */

    /* Test 11: Swap negative values */
    {
        test_num++;
        int32_t a = -10, b = -20;
        xor_swap(&a, &b);
        if (a == -20 && b == -10) {
            passed++;
            printf("[PASS] Test %d: Swap -10 and -20\n", test_num);
        } else {
            printf("[FAIL] Test %d: Swap -10 and -20 | Expected: a=-20, b=-10 | Got: a=%d, b=%d\n",
                   test_num, a, b);
        }
    }

    /* Test 12: Swap negative and positive */
    {
        test_num++;
        int32_t a = -1, b = 1;
        xor_swap(&a, &b);
        if (a == 1 && b == -1) {
            passed++;
            printf("[PASS] Test %d: Swap -1 (0xFFFFFFFF) and 1\n", test_num);
        } else {
            printf("[FAIL] Test %d: Swap -1 and 1 | Expected: a=1, b=-1 | Got: a=%d, b=%d\n",
                   test_num, a, b);
        }
    }

    /* Test 13: Swap INT_MIN and INT_MAX */
    {
        test_num++;
        int32_t a = (-2147483647 - 1), b = 2147483647;
        xor_swap(&a, &b);
        if (a == 2147483647 && b == (-2147483647 - 1)) {
            passed++;
            printf("[PASS] Test %d: Swap INT_MIN and INT_MAX\n", test_num);
        } else {
            printf("[FAIL] Test %d: Swap INT_MIN and INT_MAX | Expected: a=2147483647, b=-2147483648 | Got: a=%d, b=%d\n",
                   test_num, a, b);
        }
    }

    /* Test 14: Swap 0xAAAAAAAA and 0x55555555 (complementary bit patterns) */
    {
        test_num++;
        int32_t a = (int32_t)0xAAAAAAAA, b = 0x55555555;
        int32_t expected_a = 0x55555555, expected_b = (int32_t)0xAAAAAAAA;
        xor_swap(&a, &b);
        if (a == expected_a && b == expected_b) {
            passed++;
            printf("[PASS] Test %d: Swap complementary bit patterns (0xAAAA.., 0x5555..)\n", test_num);
        } else {
            printf("[FAIL] Test %d: Swap complementary patterns | Expected: a=0x%08X, b=0x%08X | Got: a=0x%08X, b=0x%08X\n",
                   test_num, (uint32_t)expected_a, (uint32_t)expected_b, (uint32_t)a, (uint32_t)b);
        }
    }

    /* ===== Array element swap (realistic scenario) ===== */

    /* Test 15: Swap array elements (different indices) */
    {
        test_num++;
        int32_t arr[] = {10, 20, 30, 40, 50};
        xor_swap(&arr[1], &arr[3]);
        if (arr[1] == 40 && arr[3] == 20) {
            passed++;
            printf("[PASS] Test %d: Swap arr[1] and arr[3] in array\n", test_num);
        } else {
            printf("[FAIL] Test %d: Swap arr[1] and arr[3] | Expected: arr[1]=40, arr[3]=20 | Got: arr[1]=%d, arr[3]=%d\n",
                   test_num, arr[1], arr[3]);
        }
    }

    /* Test 16: Swap array element with itself (aliasing in sort!) */
    {
        test_num++;
        int32_t arr[] = {10, 20, 30, 40, 50};
        xor_swap(&arr[2], &arr[2]);  /* i == j in a sort! */
        if (arr[2] == 30) {
            passed++;
            printf("[PASS] Test %d: ALIASING in array: swap arr[2] with itself preserved\n", test_num);
        } else {
            printf("[FAIL] Test %d: ALIASING in array: swap arr[2] with itself | Expected: arr[2]=30 | Got: arr[2]=%d (ZEROED!)\n",
                   test_num, arr[2]);
        }
    }

    /* Test 17: Swap powers of 2 */
    {
        test_num++;
        int32_t a = 1, b = (int32_t)(1U << 31);
        xor_swap(&a, &b);
        if (a == (int32_t)(1U << 31) && b == 1) {
            passed++;
            printf("[PASS] Test %d: Swap 1 and 0x80000000\n", test_num);
        } else {
            printf("[FAIL] Test %d: Swap 1 and 0x80000000 | Expected: a=0x80000000, b=1 | Got: a=0x%08X, b=0x%08X\n",
                   test_num, (uint32_t)a, (uint32_t)b);
        }
    }

    /* Test 18: Swap adjacent values */
    {
        test_num++;
        int32_t a = 127, b = 128;
        xor_swap(&a, &b);
        if (a == 128 && b == 127) {
            passed++;
            printf("[PASS] Test %d: Swap adjacent values 127 and 128\n", test_num);
        } else {
            printf("[FAIL] Test %d: Swap 127 and 128 | Expected: a=128, b=127 | Got: a=%d, b=%d\n",
                   test_num, a, b);
        }
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);

    return (passed == num_tests) ? 0 : 1;
}
