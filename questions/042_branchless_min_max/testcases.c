#include <stdio.h>
#include <stdint.h>

extern int32_t branchless_min(int32_t x, int32_t y);
extern int32_t branchless_max(int32_t x, int32_t y);

struct test_case {
    int32_t x;
    int32_t y;
    int32_t expected_min;
    int32_t expected_max;
    const char *description;
};

int main(void) {
    struct test_case tests[] = {
        /* Basic positive */
        {5, 9, 5, 9, "Basic: min(5,9)=5, max(5,9)=9"},
        {10, 3, 3, 10, "Basic: min(10,3)=3, max(10,3)=10"},
        {1, 1, 1, 1, "Equal: min(1,1)=1, max(1,1)=1"},
        {0, 0, 0, 0, "Zero: min(0,0)=0, max(0,0)=0"},
        {100, 200, 100, 200, "Basic: min(100,200)=100"},

        /* Negative numbers */
        {-5, 3, -5, 3, "Negative: min(-5,3)=-5"},
        {-10, -3, -10, -3, "Both neg: min(-10,-3)=-10"},
        {-1, -1, -1, -1, "Equal neg: min(-1,-1)=-1"},
        {-100, 0, -100, 0, "Neg and zero: min(-100,0)=-100"},
        {5, -5, -5, 5, "Symmetric: min(5,-5)=-5"},

        /* Boundary values — the overflow trap */
        {2147483647, 0, 0, 2147483647, "Boundary: min(INT_MAX,0)=0"},
        {(-2147483647-1), 0, (-2147483647-1), 0, "Boundary: min(INT_MIN,0)=INT_MIN"},
        {2147483647, (-2147483647-1), (-2147483647-1), 2147483647, "Boundary: min(INT_MAX,INT_MIN)=INT_MIN"},
        {2147483647, 2147483646, 2147483646, 2147483647, "Boundary: adjacent near INT_MAX"},
        {(-2147483647-1), (-2147483647), (-2147483647-1), (-2147483647), "Boundary: adjacent near INT_MIN"},

        /* Cases that break the subtraction-based version */
        {2147483647, -1, -1, 2147483647, "Overflow-trap: INT_MAX and -1 (x-y overflows)"},
        {(-2147483647-1), 1, (-2147483647-1), 1, "Overflow-trap: INT_MIN and 1 (x-y overflows)"},
        {2147483647, -2147483647, -2147483647, 2147483647, "Overflow-trap: max range spread"},

        /* Zero involvement */
        {0, 1, 0, 1, "Zero: min(0,1)=0"},
        {0, -1, -1, 0, "Zero: min(0,-1)=-1"},

        /* Powers of 2 */
        {1024, 512, 512, 1024, "Powers: min(1024,512)=512"},
        {-1024, -512, -1024, -512, "Powers neg: min(-1024,-512)=-1024"},
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    for (int i = 0; i < num_tests; i++) {
        int32_t got_min = branchless_min(tests[i].x, tests[i].y);
        int32_t got_max = branchless_max(tests[i].x, tests[i].y);

        if (got_min == tests[i].expected_min && got_max == tests[i].expected_max) {
            passed++;
            printf("[PASS] Test %d: %s\n", i + 1, tests[i].description);
        } else {
            printf("[FAIL] Test %d: %s | Input: x=%d, y=%d | "
                   "Expected: min=%d, max=%d | Got: min=%d, max=%d\n",
                   i + 1, tests[i].description,
                   tests[i].x, tests[i].y,
                   tests[i].expected_min, tests[i].expected_max,
                   got_min, got_max);
        }
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);

    return (passed == num_tests) ? 0 : 1;
}
