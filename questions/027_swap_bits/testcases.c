#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Declaration of the solution function */
extern uint32_t swap_bits(uint32_t num, int pos1, int pos2);

/* Test case structure */
struct test_case {
    uint32_t num;
    int pos1;
    int pos2;
    uint32_t expected;
    const char *description;
};

int main(void) {
    struct test_case tests[] = {
        /* Basic cases - bits differ, swap needed */
        {0x00000001, 0, 1, 0x00000002, "Swap bit 0 (1) and bit 1 (0) of 0x01"},
        {0x00000002, 0, 1, 0x00000001, "Swap bit 0 (0) and bit 1 (1) of 0x02"},
        {0x00000001, 0, 7, 0x00000080, "Swap bit 0 (1) and bit 7 (0) of 0x01"},
        {0x00000080, 0, 7, 0x00000001, "Swap bit 0 (0) and bit 7 (1) of 0x80"},
        {0x80000000, 0, 31, 0x00000001, "Swap bit 0 (0) and bit 31 (1) - MSB to LSB"},

        /* Bits are the same - no change expected */
        {0x00000003, 0, 1, 0x00000003, "Both bits are 1, no change"},
        {0x00000000, 0, 1, 0x00000000, "Both bits are 0, no change on zero"},
        {0xFFFFFFFF, 5, 20, 0xFFFFFFFF, "All bits set, swap has no effect"},
        {0x00000000, 15, 31, 0x00000000, "Zero input, swap any positions"},

        /* Same position - no change */
        {0x00000001, 0, 0, 0x00000001, "Same position (0,0), no change"},
        {0xABCDEF01, 16, 16, 0xABCDEF01, "Same position (16,16), no change"},

        /* Alternating bit patterns */
        {0xAAAAAAAA, 0, 1, 0xAAAAAAA9, "Swap in alternating 1010 pattern"},
        {0x55555555, 0, 1, 0x55555556, "Swap in alternating 0101 pattern"},
        {0xAAAAAAAA, 30, 31, 0x6AAAAAAA, "Swap top 2 bits of 0xAAAAAAAA"},

        /* Edge cases with boundary positions */
        {0x00000001, 0, 31, 0x80000000, "Move LSB to MSB position"},
        {0x40000000, 30, 0, 0x00000001, "Swap bit 30 (1) with bit 0 (0)"},
        {0x00008000, 15, 16, 0x00010000, "Swap across byte boundary (bit 15 and 16)"},

        /* Random values */
        {0xDEADBEEF, 4, 28, 0xCEADBEFF, "Swap bit 4 and bit 28 of 0xDEADBEEF"},
        {0x12345678, 0, 4, 0x12345669, "Swap bit 0 (0) and bit 4 (1) of 0x12345678"},
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    for (int i = 0; i < num_tests; i++) {
        uint32_t result = swap_bits(tests[i].num, tests[i].pos1, tests[i].pos2);
        if (result == tests[i].expected) {
            passed++;
            printf("[PASS] Test %d: %s\n", i + 1, tests[i].description);
        } else {
            printf("[FAIL] Test %d: %s | Input: num=0x%08X, pos1=%d, pos2=%d | Expected: 0x%08X, Got: 0x%08X\n",
                   i + 1, tests[i].description,
                   tests[i].num, tests[i].pos1, tests[i].pos2,
                   tests[i].expected, result);
        }
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);

    return (passed == num_tests) ? 0 : 1;
}
