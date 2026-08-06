#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Declaration of the solution function */
extern int32_t add(int32_t a, int32_t b);

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
        {5, 3, 8, "Basic: 5 + 3 = 8"},
        {1, 1, 2, "Basic: 1 + 1 = 2"},
        {100, 200, 300, "Basic: 100 + 200 = 300"},
        {0, 0, 0, "Zero: 0 + 0 = 0"},
        {0, 42, 42, "Identity: 0 + 42 = 42"},
        {42, 0, 42, "Identity: 42 + 0 = 42"},

        /* Negative numbers */
        {-1, 1, 0, "Cancel: -1 + 1 = 0 (carry propagates all 32 bits)"},
        {-5, -3, -8, "Negative: -5 + -3 = -8"},
        {-1, -1, -2, "Negative: -1 + -1 = -2"},
        {10, -3, 7, "Mixed: 10 + (-3) = 7"},
        {-10, 3, -7, "Mixed: -10 + 3 = -7"},

        /* Boundary values - the UB traps */
        {2147483647, 0, 2147483647, "Boundary: INT_MAX + 0"},
        {(-2147483647 - 1), 0, (-2147483647 - 1), "Boundary: INT_MIN + 0"},
        {2147483647, 1, (-2147483647 - 1), "Overflow: INT_MAX + 1 wraps to INT_MIN"},
        {(-2147483647 - 1), -1, 2147483647, "Overflow: INT_MIN + (-1) wraps to INT_MAX"},
        {(-2147483647 - 1), (-2147483647 - 1), 0, "Overflow: INT_MIN + INT_MIN wraps to 0"},
        {2147483647, 2147483647, -2, "Overflow: INT_MAX + INT_MAX wraps to -2"},

        /* Carry propagation stress tests */
        {1, (int32_t)0x7FFFFFFE, 2147483647, "Carry: 1 + 0x7FFFFFFE = INT_MAX"},
        {(int32_t)0xFFFFFFFF, (int32_t)0xFFFFFFFF, -2, "Carry: -1 + -1 = -2 (all bits carry)"},
        {(int32_t)0x55555555, (int32_t)0xAAAAAAAA, -1, "Complementary: 0x5555... + 0xAAAA... = 0xFFFF... = -1"},

        /* Powers of 2 */
        {1024, 1024, 2048, "Powers: 1024 + 1024 = 2048"},
        {(int32_t)(1U << 30), (int32_t)(1U << 30), (-2147483647 - 1), "Powers: 2^30 + 2^30 = 2^31 (INT_MIN via overflow)"},
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    for (int i = 0; i < num_tests; i++) {
        int32_t result = add(tests[i].a, tests[i].b);
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
