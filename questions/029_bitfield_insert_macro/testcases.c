#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Include the macro definition from solution */
#include "solution.c"

/* Test case structure */
struct test_case {
    uint32_t reg;
    uint32_t mask;
    int shift;
    uint32_t value;
    uint32_t expected;
    const char *description;
};

int main(void) {
    struct test_case tests[] = {
        /* Basic insertions at various positions */
        {0x00000000, 0x0000000F, 0, 0x0A, 0x0000000A,
         "Insert 0xA into bits 0-3 of zero reg"},
        {0x00000000, 0x000000F0, 4, 0x0A, 0x000000A0,
         "Insert 0xA into bits 4-7 of zero reg"},
        {0x00000000, 0x0000FF00, 8, 0x42, 0x00004200,
         "Insert 0x42 into bits 8-15 of zero reg"},
        {0x00000000, 0x00FF0000, 16, 0xAB, 0x00AB0000,
         "Insert 0xAB into bits 16-23 of zero reg"},
        {0x00000000, 0xFF000000, 24, 0xCD, 0xCD000000,
         "Insert 0xCD into bits 24-31 of zero reg"},

        /* Insertion into register with other bits set */
        {0xFFFFFFFF, 0x0000FF00, 8, 0x42, 0xFFFF42FF,
         "Insert 0x42 into all-ones reg bits 8-15"},
        {0x12345678, 0x0000000F, 0, 0x0B, 0x1234567B,
         "Insert 0xB into bits 0-3 of 0x12345678"},
        {0xABCD0000, 0x00FF0000, 16, 0xEF, 0xABEF0000,
         "Insert 0xEF into bits 16-23 of 0xABCD0000"},
        {0xDEADBEEF, 0x0000FF00, 8, 0x00, 0xDEAD00EF,
         "Insert 0x00 into bits 8-15 (clear field)"},

        /* Overwrite existing field value */
        {0x000000A0, 0x000000F0, 4, 0x05, 0x00000050,
         "Overwrite 0xA with 0x5 in bits 4-7"},
        {0xFFFF42FF, 0x0000FF00, 8, 0xBE, 0xFFFFBEFF,
         "Overwrite 0x42 with 0xBE in bits 8-15"},

        /* Insert zero (clear field) */
        {0xFFFFFFFF, 0x0000000F, 0, 0x00, 0xFFFFFFF0,
         "Insert 0 into bits 0-3 (clear nibble)"},
        {0xFFFFFFFF, 0xFF000000, 24, 0x00, 0x00FFFFFF,
         "Insert 0 into bits 24-31 (clear top byte)"},

        /* Insert max field value */
        {0x00000000, 0x0000000F, 0, 0x0F, 0x0000000F,
         "Insert max 4-bit value 0xF"},
        {0x00000000, 0x0000FF00, 8, 0xFF, 0x0000FF00,
         "Insert max 8-bit value 0xFF"},

        /* Single-bit fields */
        {0x00000000, 0x00000001, 0, 0x01, 0x00000001,
         "Set single bit 0"},
        {0xFFFFFFFF, 0x80000000, 31, 0x00, 0x7FFFFFFF,
         "Clear single bit 31"},
        {0x00000000, 0x80000000, 31, 0x01, 0x80000000,
         "Set single bit 31"},

        /* Wide 16-bit fields */
        {0x00000000, 0x0000FFFF, 0, 0xCAFE, 0x0000CAFE,
         "Insert 16-bit value 0xCAFE into bits 0-15"},
        {0xDEAD0000, 0xFFFF0000, 16, 0xBEEF, 0xBEEF0000,
         "Insert 16-bit value 0xBEEF into bits 16-31"},

        /* Preserve surrounding bits */
        {0xAA55AA55, 0x0000FF00, 8, 0x77, 0xAA557755,
         "Insert 0x77 preserving pattern 0xAA55AA55"},
        {0x12345678, 0x00FF0000, 16, 0x99, 0x12995678,
         "Insert 0x99 preserving 0x12345678"},
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    for (int i = 0; i < num_tests; i++) {
        uint32_t result = (uint32_t)BITFIELD_INSERT(tests[i].reg, tests[i].mask,
                                                     tests[i].shift, tests[i].value);
        if (result == tests[i].expected) {
            passed++;
            printf("[PASS] Test %d: %s\n", i + 1, tests[i].description);
        } else {
            printf("[FAIL] Test %d: %s | Input: reg=0x%08X, mask=0x%08X, shift=%d, value=0x%X | Expected: 0x%08X, Got: 0x%08X\n",
                   i + 1, tests[i].description,
                   tests[i].reg, tests[i].mask, tests[i].shift, tests[i].value,
                   tests[i].expected, result);
        }
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);

    return (passed == num_tests) ? 0 : 1;
}
