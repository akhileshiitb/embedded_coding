#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Declaration of the solution function */
extern uint32_t insert_bits(uint32_t reg, uint32_t mask, uint32_t value);

/* Test case structure */
struct test_case {
    uint32_t reg;
    uint32_t mask;
    uint32_t value;
    uint32_t expected;
    const char *description;
};

int main(void) {
    struct test_case tests[] = {
        /* Basic insertions into zero register */
        {0x00000000, 0x0000000F, 0x0A, 0x0000000A, "Insert 0xA into bits 0-3 of 0"},
        {0x00000000, 0x000000F0, 0x0A, 0x000000A0, "Insert 0xA into bits 4-7 of 0"},
        {0x00000000, 0x0000FF00, 0x42, 0x00004200, "Insert 0x42 into bits 8-15 of 0"},
        {0x00000000, 0x00FF0000, 0xAB, 0x00AB0000, "Insert 0xAB into bits 16-23 of 0"},
        {0x00000000, 0xFF000000, 0xCD, 0xCD000000, "Insert 0xCD into bits 24-31 of 0"},

        /* Insert overwriting existing field */
        {0xFFFFFFFF, 0x0000000F, 0x05, 0xFFFFFFF5, "Insert 0x5 into bits 0-3 of all ones"},
        {0xFFFFFFFF, 0x0000FF00, 0x42, 0xFFFF42FF, "Insert 0x42 into bits 8-15 of all ones"},
        {0xFFFFFFFF, 0xFF000000, 0x00, 0x00FFFFFF, "Insert 0x00 into bits 24-31 of all ones"},

        /* Insert into mixed register */
        {0x12345678, 0x0000000F, 0x0B, 0x1234567B, "Insert 0xB into bits 0-3 of 0x12345678"},
        {0xABCD0000, 0x0000FF00, 0xEF, 0xABCDEF00, "Insert 0xEF into bits 8-15 of 0xABCD0000"},
        {0xDEADBEEF, 0x00FF0000, 0xCA, 0xDECABEEF, "Insert 0xCA into bits 16-23 of 0xDEADBEEF"},

        /* Single-bit field (equivalent to set/clear) */
        {0x00000000, 0x00000001, 0x01, 0x00000001, "Insert 1 into single bit field at bit 0"},
        {0xFFFFFFFF, 0x80000000, 0x00, 0x7FFFFFFF, "Insert 0 into single bit field at bit 31"},

        /* Narrow fields */
        {0x00000000, 0x00000070, 0x05, 0x00000050, "Insert 0x5 into 3-bit field at bits 4-6"},
        {0x00000000, 0x00001C00, 0x07, 0x00001C00, "Insert 0x7 into 3-bit field at bits 10-12"},

        /* Value is zero (clear the field) */
        {0xFFFFFFFF, 0x000000FF, 0x00, 0xFFFFFF00, "Insert 0 to clear bits 0-7"},
        {0xFFFFFFFF, 0xFFFF0000, 0x0000, 0x0000FFFF, "Insert 0 to clear bits 16-31"},

        /* Full-width field */
        {0xDEADBEEF, 0x0000FFFF, 0xCAFE, 0xDEADCAFE, "Insert 0xCAFE into 16-bit field bits 0-15"},
        {0x12345678, 0xFFFF0000, 0xABCD, 0xABCD5678, "Insert 0xABCD into 16-bit field bits 16-31"},
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    for (int i = 0; i < num_tests; i++) {
        uint32_t result = insert_bits(tests[i].reg, tests[i].mask, tests[i].value);
        if (result == tests[i].expected) {
            passed++;
            printf("[PASS] Test %d: %s\n", i + 1, tests[i].description);
        } else {
            printf("[FAIL] Test %d: %s | Input: reg=0x%08X, mask=0x%08X, value=0x%X | Expected: 0x%08X, Got: 0x%08X\n",
                   i + 1, tests[i].description,
                   tests[i].reg, tests[i].mask, tests[i].value,
                   tests[i].expected, result);
        }
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);

    return (passed == num_tests) ? 0 : 1;
}
