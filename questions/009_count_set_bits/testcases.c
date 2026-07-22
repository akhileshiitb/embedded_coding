#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Declaration of the solution function */
extern int count_set_bits(uint32_t num);

/* Test case structure */
struct test_case {
    uint32_t num;
    int expected;
    const char *description;
};

int main(void) {
    struct test_case tests[] = {
        /* Basic cases */
        {0x00000000, 0,  "Zero has no bits set"},
        {0x00000001, 1,  "Only bit 0 set"},
        {0x00000003, 2,  "Bits 0 and 1 set"},
        {0x0000000F, 4,  "Lower nibble all set (4 bits)"},
        {0x000000FF, 8,  "Lower byte all set (8 bits)"},

        /* Powers of two (single bit set) */
        {0x00000010, 1,  "Bit 4 set (power of 2)"},
        {0x00000100, 1,  "Bit 8 set (power of 2)"},
        {0x80000000, 1,  "Only MSB set"},

        /* Alternating patterns */
        {0xAAAAAAAA, 16, "Alternating 10 pattern (16 bits)"},
        {0x55555555, 16, "Alternating 01 pattern (16 bits)"},

        /* All bits set */
        {0xFFFFFFFF, 32, "All 32 bits set"},
        {0x0000FFFF, 16, "Lower 16 bits set"},
        {0xFFFF0000, 16, "Upper 16 bits set"},

        /* Sparse and dense values */
        {0x00010001, 2,  "Two bits set far apart"},
        {0x7FFFFFFF, 31, "All bits set except MSB"},
        {0xFFFFFFFE, 31, "All bits set except LSB"},
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    for (int i = 0; i < num_tests; i++) {
        int result = count_set_bits(tests[i].num);
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
