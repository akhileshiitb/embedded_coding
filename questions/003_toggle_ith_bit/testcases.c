#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Declaration of the solution function */
extern uint32_t toggle_bit(uint32_t num, int i);

/* Test case structure */
struct test_case {
    uint32_t num;
    int bit_pos;
    uint32_t expected;
    const char *description;
};

int main(void) {
    struct test_case tests[] = {
        /* Toggle bit from 0 to 1 */
        {0x00000000, 0,  0x00000001, "Toggle bit 0 of 0 (0->1)"},
        {0x00000000, 1,  0x00000002, "Toggle bit 1 of 0 (0->1)"},
        {0x00000000, 3,  0x00000008, "Toggle bit 3 of 0 (0->1)"},
        {0x00000000, 7,  0x00000080, "Toggle bit 7 of 0 (0->1)"},
        {0x00000000, 15, 0x00008000, "Toggle bit 15 of 0 (0->1)"},
        {0x00000000, 31, 0x80000000, "Toggle bit 31 of 0 (0->1)"},

        /* Toggle bit from 1 to 0 */
        {0x00000001, 0,  0x00000000, "Toggle bit 0 of 0x01 (1->0)"},
        {0x00000002, 1,  0x00000000, "Toggle bit 1 of 0x02 (1->0)"},
        {0x00000080, 7,  0x00000000, "Toggle bit 7 of 0x80 (1->0)"},
        {0x80000000, 31, 0x00000000, "Toggle bit 31 of 0x80000000 (1->0)"},

        /* Toggle in multi-bit numbers */
        {0x0000000F, 2,  0x0000000B, "Toggle bit 2 of 0x0F (1->0)"},
        {0x0000000F, 4,  0x0000001F, "Toggle bit 4 of 0x0F (0->1)"},
        {0x000000FF, 7,  0x0000007F, "Toggle bit 7 of 0xFF (1->0)"},
        {0x000000FF, 8,  0x000001FF, "Toggle bit 8 of 0xFF (0->1)"},

        /* Edge cases with all ones */
        {0xFFFFFFFF, 0,  0xFFFFFFFE, "Toggle bit 0 of all ones (1->0)"},
        {0xFFFFFFFF, 31, 0x7FFFFFFF, "Toggle bit 31 of all ones (1->0)"},
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    for (int i = 0; i < num_tests; i++) {
        uint32_t result = toggle_bit(tests[i].num, tests[i].bit_pos);
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
