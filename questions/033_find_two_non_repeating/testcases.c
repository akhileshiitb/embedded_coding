#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Declaration of the solution function */
extern void find_two_non_repeating(const int32_t *arr, int n, int32_t *out1, int32_t *out2);

/* Test case structure */
struct test_case {
    const int32_t *arr;
    int n;
    int32_t expected_out1;  /* smaller value */
    int32_t expected_out2;  /* larger value */
    const char *description;
};

int main(void) {
    /* Test data arrays */

    /* Basic cases */
    int32_t arr1[] = {2, 3, 7, 9, 11, 2, 3, 11};
    int32_t arr2[] = {1, 2, 3, 2, 1, 4};
    int32_t arr3[] = {4, 1, 4, 6, 1, 3, 6, 8};
    int32_t arr4[] = {10, 20, 30, 10, 20, 40};

    /* Edge cases with negative numbers */
    int32_t arr5[] = {-1, 5, -1, 10, 5, 99, 10, -42};
    int32_t arr6[] = {-5, -3, -5, -7, -3, -9};
    int32_t arr7[] = {-100, 100, 50, -100, 50, -50};

    /* Boundary values */
    int32_t arr8[] = {0, 2147483647, 0, -2147483648LL};
    int32_t arr9[] = {0, 1, 2, 2, 1, 3};
    int32_t arr10[] = {2147483647, 5, 5, 2147483646, 10, 10};

    /* Adjacent/similar values */
    int32_t arr11[] = {7, 8, 100, 100, 200, 200};
    int32_t arr12[] = {1, 2, 1, 3, 2, 3, 4, 5};

    /* Values differing by one bit */
    int32_t arr13[] = {0x0F, 0x0E, 0xAA, 0xAA, 0xBB, 0xBB};
    int32_t arr14[] = {0xFF, 0x00, 0x12, 0x12, 0x34, 0x34};

    /* Large array with many duplicates */
    int32_t arr15[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8};
    int32_t arr16[] = {42, -42, 100, 200, 100, 200, 300, 300};

    /* Minimum size array */
    int32_t arr17[] = {5, 5, 11, 22};

    /* Both unique elements are powers of 2 */
    int32_t arr18[] = {16, 32, 3, 3, 7, 7, 9, 9};

    struct test_case tests[] = {
        /* Basic cases */
        {arr1, 8, 7, 9, "Basic: two unique in mixed array"},
        {arr2, 6, 3, 4, "Basic: small array with unique 3 and 4"},
        {arr3, 8, 3, 8, "Basic: unique elements not adjacent"},
        {arr4, 6, 30, 40, "Basic: unique values are multiples of 10"},

        /* Edge cases with negative numbers */
        {arr5, 8, -42, 99, "Negative: one negative, one positive unique"},
        {arr6, 6, -9, -7, "Negative: both unique are negative"},
        {arr7, 6, -50, 100, "Negative: mixed sign unique values"},

        /* Boundary values */
        {arr8, 4, -2147483648LL, 2147483647, "Boundary: INT_MIN and INT_MAX"},
        {arr9, 6, 0, 3, "Boundary: zero is one of the unique values"},
        {arr10, 6, 2147483646, 2147483647, "Boundary: two large adjacent values"},

        /* Adjacent/similar values */
        {arr11, 6, 7, 8, "Adjacent: unique values differ by 1"},
        {arr12, 8, 4, 5, "Adjacent: many pairs, unique at end"},

        /* Values differing by specific bits */
        {arr13, 6, 14, 15, "Bits: unique values differ by 1 bit (0x0E vs 0x0F)"},
        {arr14, 6, 0, 255, "Bits: zero and 0xFF as unique"},

        /* Larger arrays */
        {arr15, 18, 9, 10, "Large: 9 pairs + two unique"},
        {arr16, 8, -42, 42, "Sign: negation pair as unique values"},

        /* Minimum size */
        {arr17, 4, 11, 22, "MinSize: smallest valid array (one pair + two unique)"},

        /* Powers of 2 */
        {arr18, 8, 16, 32, "Powers: both unique are powers of 2"},
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    for (int i = 0; i < num_tests; i++) {
        int32_t out1 = 0, out2 = 0;
        find_two_non_repeating(tests[i].arr, tests[i].n, &out1, &out2);

        if (out1 == tests[i].expected_out1 && out2 == tests[i].expected_out2) {
            passed++;
            printf("[PASS] Test %d: %s\n", i + 1, tests[i].description);
        } else {
            printf("[FAIL] Test %d: %s | Expected: out1=%d, out2=%d | Got: out1=%d, out2=%d\n",
                   i + 1, tests[i].description,
                   tests[i].expected_out1, tests[i].expected_out2,
                   out1, out2);
        }
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);

    return (passed == num_tests) ? 0 : 1;
}
