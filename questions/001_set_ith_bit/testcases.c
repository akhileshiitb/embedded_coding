#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Declaration of the solution function */
extern uint32_t set_bit(uint32_t num, int i);

/* Test case structure */
struct test_case {
    uint32_t num;
    int bit_pos;
    uint32_t expected;
    const char *description;
};

int main(void) {
    struct test_case tests[] = {
        /* Basic cases - setting bit on zero */
        {0x00000000, 0,  0x00000001, "Set bit 0 of 0"},
        {0x00000000, 1,  0x00000002, "Set bit 1 of 0"},
        {0x00000000, 3,  0x00000008, "Set bit 3 of 0"},
        {0x00000000, 7,  0x00000080, "Set bit 7 of 0"},
        {0x00000000, 15, 0x00008000, "Set bit 15 of 0"},
        {0x00000000, 31, 0x80000000, "Set bit 31 (MSB) of 0"},

        /* Already set bit - should remain unchanged */
        {0x00000001, 0,  0x00000001, "Bit 0 already set"},
        {0x0000000F, 3,  0x0000000F, "Bit 3 already set in 0xF"},
        {0x80000000, 31, 0x80000000, "Bit 31 already set"},
        {0xFFFFFFFF, 16, 0xFFFFFFFF, "All bits set, set bit 16"},

        /* Setting bit on non-zero numbers */
        {0x00000001, 7,  0x00000081, "Set bit 7 of 0x01"},
        {0x000000FF, 8,  0x000001FF, "Set bit 8 of 0xFF"},
        {0xAAAAAAAA, 0,  0xAAAAAAAA | 0x01, "Set bit 0 of 0xAAAAAAAA"},
        {0x55555555, 1,  0x55555555 | 0x02, "Set bit 1 of 0x55555555"},

        /* Edge cases */
        {0xFFFFFFFE, 0,  0xFFFFFFFF, "Set bit 0 to make all ones"},
        {0x7FFFFFFF, 31, 0xFFFFFFFF, "Set bit 31 to make all ones"},
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    for (int i = 0; i < num_tests; i++) {
        uint32_t result = set_bit(tests[i].num, tests[i].bit_pos);
        if (result == tests[i].expected) {
            passed++;
            printf("[PASS] Test %d: %s\n", i + 1, tests[i].description);
        } else {
            printf("[FAIL] Test %d: %s | Input: num=0x%08X, i=%d | Expected: 0x%08X, Got: 0x%08X\n",
                   i + 1, tests[i].description,
                   tests[i].num, tests[i].bit_pos,
                   tests[i].expected, result);
        }
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);

    return (passed == num_tests) ? 0 : 1;
}
