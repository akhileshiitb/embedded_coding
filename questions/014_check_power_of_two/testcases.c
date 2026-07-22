#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Declaration of the solution function */
extern int is_power_of_two(uint32_t num);

/* Test case structure */
struct test_case {
    uint32_t num;
    int expected;
    const char *description;
};

int main(void) {
    struct test_case tests[] = {
        /* Powers of two - should return 1 */
        {0x00000001, 1, "2^0 = 1"},
        {0x00000002, 1, "2^1 = 2"},
        {0x00000004, 1, "2^2 = 4"},
        {0x00000008, 1, "2^3 = 8"},
        {0x00000010, 1, "2^4 = 16"},
        {0x00000100, 1, "2^8 = 256"},
        {0x00010000, 1, "2^16 = 65536"},
        {0x80000000, 1, "2^31 = 2147483648 (largest power of 2 in uint32)"},

        /* NOT powers of two - should return 0 */
        {0x00000000, 0, "Zero is not a power of two"},
        {0x00000003, 0, "3 is not a power of two"},
        {0x00000005, 0, "5 is not a power of two"},
        {0x00000006, 0, "6 is not a power of two"},
        {0x00000007, 0, "7 is not a power of two"},
        {0x0000000F, 0, "15 is not a power of two"},
        {0xFFFFFFFF, 0, "All bits set is not a power of two"},
        {0xC0000000, 0, "Two high bits set is not a power of two"},
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    for (int i = 0; i < num_tests; i++) {
        int result = is_power_of_two(tests[i].num);
        if (result == tests[i].expected) {
            passed++;
            printf("[PASS] Test %d: %s\n", i + 1, tests[i].description);
        } else {
            printf("[FAIL] Test %d: %s | Input: num=0x%08X | Expected: %d, Got: %d\n",
                   i + 1, tests[i].description,
                   tests[i].num, tests[i].expected, result);
        }
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);

    return (passed == num_tests) ? 0 : 1;
}
