#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Declaration of the solution function */
extern uint32_t reverse_bits(uint32_t num);

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
        {0x00000001, 0x80000000, "Bit 0 becomes bit 31"},
        {0x80000000, 0x00000001, "Bit 31 becomes bit 0"},
        {0x00000002, 0x40000000, "Bit 1 becomes bit 30"},

        /* Nibble and byte patterns */
        {0x0000000F, 0xF0000000, "Lower nibble reverses to upper"},
        {0xF0000000, 0x0000000F, "Upper nibble reverses to lower"},
        {0x000000FF, 0xFF000000, "Lower byte reverses to upper"},
        {0xFF000000, 0x000000FF, "Upper byte reverses to lower"},

        /* Alternating patterns */
        {0xAAAAAAAA, 0x55555555, "10101010... becomes 01010101..."},
        {0x55555555, 0xAAAAAAAA, "01010101... becomes 10101010..."},

        /* Symmetric values (palindromes) */
        {0xFF0000FF, 0xFF0000FF, "Symmetric byte pattern"},
        {0x81000081, 0x81000081, "Symmetric sparse pattern"},

        /* Asymmetric values */
        {0x12345678, 0x1E6A2C48, "Arbitrary value 0x12345678"},
        {0x0000FFFF, 0xFFFF0000, "Lower half set"},
        {0x00FF00FF, 0xFF00FF00, "Alternating bytes set"},
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    for (int i = 0; i < num_tests; i++) {
        uint32_t result = reverse_bits(tests[i].num);
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
