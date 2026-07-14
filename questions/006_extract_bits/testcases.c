#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Declaration of the solution function */
extern uint32_t extract_bits(uint32_t reg, uint32_t mask);

/* Test case structure */
struct test_case {
    uint32_t reg;
    uint32_t mask;
    uint32_t expected;
    const char *description;
};

int main(void) {
    struct test_case tests[] = {
        /* Basic extractions - field at various positions */
        {0x0000000A, 0x0000000F, 0x0A, "Extract bits 0-3: value 0xA"},
        {0x000000A0, 0x000000F0, 0x0A, "Extract bits 4-7: value 0xA"},
        {0x00004200, 0x0000FF00, 0x42, "Extract bits 8-15: value 0x42"},
        {0x00AB0000, 0x00FF0000, 0xAB, "Extract bits 16-23: value 0xAB"},
        {0xCD000000, 0xFF000000, 0xCD, "Extract bits 24-31: value 0xCD"},

        /* Extract from register with other bits set */
        {0xFFFF42FF, 0x0000FF00, 0x42, "Extract 0x42 from all-ones surroundings"},
        {0x1234567B, 0x0000000F, 0x0B, "Extract nibble 0 from 0x1234567B"},
        {0xABCDEF00, 0x00FF0000, 0xCD, "Extract byte 2 from 0xABCDEF00"},
        {0xDEADBEEF, 0x0000FF00, 0xBE, "Extract byte 1 from 0xDEADBEEF"},

        /* All zeros in field */
        {0xFFFF00FF, 0x0000FF00, 0x00, "Extract zero field from non-zero register"},
        {0x00000000, 0x0000000F, 0x00, "Extract from zero register"},
        {0x00000000, 0xFFFF0000, 0x00, "Extract 16-bit field from zero register"},

        /* All ones in field */
        {0xFFFFFFFF, 0x0000000F, 0x0F, "Extract bits 0-3 from all ones"},
        {0xFFFFFFFF, 0x000000F0, 0x0F, "Extract bits 4-7 from all ones"},
        {0xFFFFFFFF, 0x0000FF00, 0xFF, "Extract bits 8-15 from all ones"},
        {0xFFFFFFFF, 0xFF000000, 0xFF, "Extract bits 24-31 from all ones"},

        /* Single-bit field */
        {0x00000001, 0x00000001, 0x01, "Extract single bit 0 (set)"},
        {0x00000000, 0x00000001, 0x00, "Extract single bit 0 (clear)"},
        {0x80000000, 0x80000000, 0x01, "Extract single bit 31 (set)"},
        {0x7FFFFFFF, 0x80000000, 0x00, "Extract single bit 31 (clear)"},

        /* Narrow 3-bit fields */
        {0x00000050, 0x00000070, 0x05, "Extract 3-bit field at bits 4-6: value 5"},
        {0x00001C00, 0x00001C00, 0x07, "Extract 3-bit field at bits 10-12: value 7"},

        /* Wide 16-bit fields */
        {0xDEADCAFE, 0x0000FFFF, 0xCAFE, "Extract 16-bit field bits 0-15"},
        {0xABCD5678, 0xFFFF0000, 0xABCD, "Extract 16-bit field bits 16-31"},
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    for (int i = 0; i < num_tests; i++) {
        uint32_t result = extract_bits(tests[i].reg, tests[i].mask);
        if (result == tests[i].expected) {
            passed++;
            printf("[PASS] Test %d: %s\n", i + 1, tests[i].description);
        } else {
            printf("[FAIL] Test %d: %s | Input: reg=0x%08X, mask=0x%08X | Expected: 0x%08X, Got: 0x%08X\n",
                   i + 1, tests[i].description,
                   tests[i].reg, tests[i].mask,
                   tests[i].expected, result);
        }
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);

    return (passed == num_tests) ? 0 : 1;
}
