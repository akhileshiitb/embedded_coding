#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Declaration of the solution function */
extern uint32_t clear_bit(uint32_t num, int i);

/* Test case structure */
struct test_case {
    uint32_t num;
    int bit_pos;
    uint32_t expected;
    const char *description;
};

int main(void) {
    struct test_case tests[] = {
        /* Basic cases - clearing a set bit */
        {0x00000001, 0,  0x00000000, "Clear bit 0 of 0x01"},
        {0x00000002, 1,  0x00000000, "Clear bit 1 of 0x02"},
        {0x00000008, 3,  0x00000000, "Clear bit 3 of 0x08"},
        {0x00000080, 7,  0x00000000, "Clear bit 7 of 0x80"},
        {0x00008000, 15, 0x00000000, "Clear bit 15 of 0x8000"},
        {0x80000000, 31, 0x00000000, "Clear bit 31 (MSB) of 0x80000000"},

        /* Clearing bit in a multi-bit number */
        {0x0000000F, 0,  0x0000000E, "Clear bit 0 of 0x0F"},
        {0x000000FF, 7,  0x0000007F, "Clear bit 7 of 0xFF"},
        {0x0000FFFF, 8,  0x0000FEFF, "Clear bit 8 of 0xFFFF"},
        {0xFFFFFFFF, 16, 0xFFFEFFFF, "Clear bit 16 of 0xFFFFFFFF"},

        /* Bit already clear - should remain unchanged */
        {0x00000000, 0,  0x00000000, "Bit 0 already clear in 0"},
        {0x00000000, 31, 0x00000000, "Bit 31 already clear in 0"},
        {0xFFFFFFFE, 0,  0xFFFFFFFE, "Bit 0 already clear in 0xFFFFFFFE"},
        {0x7FFFFFFF, 31, 0x7FFFFFFF, "Bit 31 already clear in 0x7FFFFFFF"},

        /* Edge cases */
        {0xFFFFFFFF, 0,  0xFFFFFFFE, "Clear bit 0 of all ones"},
        {0xFFFFFFFF, 31, 0x7FFFFFFF, "Clear bit 31 of all ones"},
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    for (int i = 0; i < num_tests; i++) {
        uint32_t result = clear_bit(tests[i].num, tests[i].bit_pos);
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
