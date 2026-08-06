#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Declaration of the solution function */
extern int32_t multiply(int32_t a, int32_t b);

/* Test case structure */
struct test_case {
    int32_t a;
    int32_t b;
    int32_t expected;
    const char *description;
};

int main(void) {
    struct test_case tests[] = {
        /* Basic positive cases */
        {6, 7, 42, "Basic: 6 * 7 = 42"},
        {3, 4, 12, "Basic: 3 * 4 = 12"},
        {1, 1, 1, "Basic: 1 * 1 = 1"},
        {100, 100, 10000, "Basic: 100 * 100 = 10000"},
        {255, 256, 65280, "Basic: 255 * 256 = 65280"},

        /* Identity and zero */
        {0, 0, 0, "Zero: 0 * 0 = 0"},
        {0, 12345, 0, "Zero: 0 * 12345 = 0"},
        {12345, 0, 0, "Zero: 12345 * 0 = 0"},
        {1, 2147483647, 2147483647, "Identity: 1 * INT_MAX"},
        {2147483647, 1, 2147483647, "Identity: INT_MAX * 1"},

        /* Negative numbers */
        {-3, 4, -12, "Negative: -3 * 4 = -12"},
        {3, -4, -12, "Negative: 3 * -4 = -12"},
        {-7, -6, 42, "Negative: -7 * -6 = 42 (both negative = positive)"},
        {-1, -1, 1, "Negative: -1 * -1 = 1"},
        {-1, 1, -1, "Negative: -1 * 1 = -1"},

        /* INT_MIN edge cases (the abs() trap) */
        {(-2147483647 - 1), 1, (-2147483647 - 1), "INT_MIN: INT_MIN * 1 = INT_MIN"},
        {1, (-2147483647 - 1), (-2147483647 - 1), "INT_MIN: 1 * INT_MIN = INT_MIN"},
        {(-2147483647 - 1), 0, 0, "INT_MIN: INT_MIN * 0 = 0"},
        {(-2147483647 - 1), -1, (-2147483647 - 1), "INT_MIN: INT_MIN * -1 wraps to INT_MIN"},

        /* Overflow / wrapping cases */
        {65536, 65536, 0, "Overflow: 65536 * 65536 = 2^32 wraps to 0"},
        {2147483647, 2, -2, "Overflow: INT_MAX * 2 wraps to -2"},
        {46341, 46341, 2147488281U > 2147483647U ? (int32_t)(46341U * 46341U) : 0, "Overflow: 46341^2 wraps"},
        {-2, (-2147483647 - 1), 0, "Overflow: -2 * INT_MIN = 2^32 wraps to 0"},

        /* Powers of 2 (shift equivalence) */
        {7, 8, 56, "Power: 7 * 8 (= 7 << 3)"},
        {1024, 1024, 1048576, "Power: 1024 * 1024 = 2^20"},
        {(int32_t)(1U << 16), 2, (int32_t)(1U << 17), "Power: 2^16 * 2 = 2^17"},

        /* Large values */
        {12345, 6789, (int32_t)((uint32_t)12345 * 6789), "Large: 12345 * 6789"},
        {9999, 9999, (int32_t)((uint32_t)9999 * 9999), "Large: 9999 * 9999 = 99980001"},
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    for (int i = 0; i < num_tests; i++) {
        int32_t result = multiply(tests[i].a, tests[i].b);
        if (result == tests[i].expected) {
            passed++;
            printf("[PASS] Test %d: %s\n", i + 1, tests[i].description);
        } else {
            printf("[FAIL] Test %d: %s | Input: a=%d, b=%d | Expected: %d, Got: %d\n",
                   i + 1, tests[i].description,
                   tests[i].a, tests[i].b, tests[i].expected, result);
        }
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);

    return (passed == num_tests) ? 0 : 1;
}
