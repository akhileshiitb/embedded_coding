#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Declaration of the solution function */
extern int check_bit(uint32_t num, int i);

/* Test case structure */
struct test_case {
    uint32_t num;
    int bit_pos;
    int expected;
    const char *description;
};

int main(void) {
    struct test_case tests[] = {
        /* Bit is set - should return 1 */
        {0x00000001, 0,  1, "Bit 0 set in 0x01"},
        {0x00000002, 1,  1, "Bit 1 set in 0x02"},
        {0x00000008, 3,  1, "Bit 3 set in 0x08"},
        {0x00000080, 7,  1, "Bit 7 set in 0x80"},
        {0x00008000, 15, 1, "Bit 15 set in 0x8000"},
        {0x80000000, 31, 1, "Bit 31 set in 0x80000000"},

        /* Bit is clear - should return 0 */
        {0x00000000, 0,  0, "Bit 0 clear in 0"},
        {0x00000000, 31, 0, "Bit 31 clear in 0"},
        {0x00000001, 1,  0, "Bit 1 clear in 0x01"},
        {0x00000001, 31, 0, "Bit 31 clear in 0x01"},
        {0xFFFFFFFE, 0,  0, "Bit 0 clear in 0xFFFFFFFE"},
        {0x7FFFFFFF, 31, 0, "Bit 31 clear in 0x7FFFFFFF"},

        /* Multi-bit numbers */
        {0x0000000F, 3,  1, "Bit 3 set in 0x0F"},
        {0x0000000F, 4,  0, "Bit 4 clear in 0x0F"},
        {0xAAAAAAAA, 0,  0, "Bit 0 clear in 0xAAAAAAAA (alternating)"},
        {0xAAAAAAAA, 1,  1, "Bit 1 set in 0xAAAAAAAA (alternating)"},
        {0x55555555, 0,  1, "Bit 0 set in 0x55555555 (alternating)"},
        {0x55555555, 1,  0, "Bit 1 clear in 0x55555555 (alternating)"},

        /* All ones */
        {0xFFFFFFFF, 0,  1, "Bit 0 set in all ones"},
        {0xFFFFFFFF, 31, 1, "Bit 31 set in all ones"},
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    for (int i = 0; i < num_tests; i++) {
        int result = check_bit(tests[i].num, tests[i].bit_pos);
        if (result == tests[i].expected) {
            passed++;
            printf("[PASS] Test %d: %s\n", i + 1, tests[i].description);
        } else {
            printf("[FAIL] Test %d: %s | Input: num=0x%08X, i=%d | Expected: %d, Got: %d\n",
                   i + 1, tests[i].description,
                   tests[i].num, tests[i].bit_pos,
                   tests[i].expected, result);
        }
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);

    return (passed == num_tests) ? 0 : 1;
}
