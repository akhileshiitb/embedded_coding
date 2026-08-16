#include <stdio.h>
#include <stdint.h>

extern int32_t branchless_abs(int32_t v);

struct test_case {
    int32_t input;
    int32_t expected;
    const char *description;
};

int main(void) {
    struct test_case tests[] = {
        /* Positive values — unchanged */
        {5, 5, "Positive: abs(5)=5"},
        {1, 1, "Positive: abs(1)=1"},
        {100, 100, "Positive: abs(100)=100"},
        {2147483647, 2147483647, "Positive: abs(INT_MAX)=INT_MAX"},

        /* Zero */
        {0, 0, "Zero: abs(0)=0"},

        /* Negative values — negated */
        {-1, 1, "Negative: abs(-1)=1"},
        {-5, 5, "Negative: abs(-5)=5"},
        {-100, 100, "Negative: abs(-100)=100"},
        {-2147483647, 2147483647, "Negative: abs(-INT_MAX)=INT_MAX"},

        /* INT_MIN — the UB edge case (wraps to itself) */
        {(-2147483647 - 1), (-2147483647 - 1), "INT_MIN: abs(INT_MIN) wraps to INT_MIN"},

        /* Values near zero */
        {-1, 1, "Near zero: abs(-1)=1"},
        {1, 1, "Near zero: abs(1)=1"},
        {-2, 2, "Near zero: abs(-2)=2"},
        {2, 2, "Near zero: abs(2)=2"},

        /* Powers of 2 */
        {-1024, 1024, "Power: abs(-1024)=1024"},
        {-65536, 65536, "Power: abs(-65536)=65536"},
        {-(1 << 30), (1 << 30), "Power: abs(-2^30)=2^30"},

        /* Alternating bit patterns */
        {(int32_t)0xAAAAAAAA, 0x55555556, "Pattern: abs(0xAAAAAAAA) = abs(-1431655766)"},
        {0x55555555, 0x55555555, "Pattern: abs(0x55555555) = 1431655765 (positive, unchanged)"},

        /* Near INT_MIN/INT_MAX boundary */
        {-2147483647, 2147483647, "Boundary: abs(-2147483647)=2147483647"},
        {-2147483646, 2147483646, "Boundary: abs(-2147483646)=2147483646"},
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    for (int i = 0; i < num_tests; i++) {
        int32_t result = branchless_abs(tests[i].input);
        if (result == tests[i].expected) {
            passed++;
            printf("[PASS] Test %d: %s\n", i + 1, tests[i].description);
        } else {
            printf("[FAIL] Test %d: %s | Input: %d | Expected: %d, Got: %d\n",
                   i + 1, tests[i].description,
                   tests[i].input, tests[i].expected, result);
        }
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);

    return (passed == num_tests) ? 0 : 1;
}
