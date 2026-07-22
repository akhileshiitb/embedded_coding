#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Declaration of the solution function */
extern uint32_t swap_nibbles(uint32_t num);

/* Test case structure */
struct test_case {
    uint32_t num;
    uint32_t expected;
    const char *description;
};

int main(void) {
    struct test_case tests[] = {
        /* Basic cases */
        {0x00000000, 0x00000000, "Zero remains zero"},
        {0xFFFFFFFF, 0xFFFFFFFF, "All ones remains all ones"},
        {0x0000000F, 0x000000F0, "Low nibble swaps to high"},
        {0x000000F0, 0x0000000F, "High nibble swaps to low"},
        {0x000000AB, 0x000000BA, "Single byte nibble swap (0xAB)"},

        /* Multi-byte patterns */
        {0x12345678, 0x21436587, "All four bytes swapped"},
        {0xABCDEF01, 0xBADCFE10, "All four bytes swapped (hex letters)"},
        {0x11223344, 0x11223344, "Symmetric nibbles unchanged"},

        /* Byte boundary independence */
        {0x0F0F0F0F, 0xF0F0F0F0, "All low nibbles become high"},
        {0xF0F0F0F0, 0x0F0F0F0F, "All high nibbles become low"},

        /* Edge cases */
        {0x00000001, 0x00000010, "Single bit in low nibble"},
        {0x00000080, 0x00000008, "Single bit in high nibble"},
        {0x80000000, 0x08000000, "MSB moves within MSB byte"},
        {0x10203040, 0x01020304, "Each byte has one nibble set"},

        /* Idempotent check pattern */
        {0xA5A5A5A5, 0x5A5A5A5A, "Mixed pattern A5"},
        {0x5A5A5A5A, 0xA5A5A5A5, "Mixed pattern 5A"},
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    for (int i = 0; i < num_tests; i++) {
        uint32_t result = swap_nibbles(tests[i].num);
        if (result == tests[i].expected) {
            passed++;
            printf("[PASS] Test %d: %s\n", i + 1, tests[i].description);
        } else {
            printf("[FAIL] Test %d: %s | Input: num=0x%08X | Expected: 0x%08X, Got: 0x%08X\n",
                   i + 1, tests[i].description,
                   tests[i].num, tests[i].expected, result);
        }
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);

    return (passed == num_tests) ? 0 : 1;
}
