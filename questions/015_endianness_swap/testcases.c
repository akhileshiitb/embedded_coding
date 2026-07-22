#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Declaration of the solution function */
extern uint32_t swap_endianness(uint32_t num);

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
        {0x12345678, 0x78563412, "Standard test pattern"},
        {0xAABBCCDD, 0xDDCCBBAA, "Distinct bytes AABBCCDD"},

        /* Single byte in each position */
        {0x000000FF, 0xFF000000, "Byte 0 moves to byte 3"},
        {0x0000FF00, 0x00FF0000, "Byte 1 moves to byte 2"},
        {0x00FF0000, 0x0000FF00, "Byte 2 moves to byte 1"},
        {0xFF000000, 0x000000FF, "Byte 3 moves to byte 0"},

        /* Single bit in each byte */
        {0x00000001, 0x01000000, "LSB byte moves to MSB position"},
        {0x80000000, 0x00000080, "MSB byte moves to LSB position"},
        {0x00000100, 0x00010000, "Byte 1 bit moves to byte 2"},
        {0x00010000, 0x00000100, "Byte 2 bit moves to byte 1"},

        /* Symmetric values (palindrome bytes) */
        {0x01020201, 0x01020201, "Byte palindrome pattern"},
        {0xABCDCDAB, 0xABCDCDAB, "Symmetric ABCDCDAB"},

        /* Network byte order examples */
        {0xC0A80001, 0x0100A8C0, "IP address 192.168.0.1 swap"},
        {0x0A000001, 0x0100000A, "IP address 10.0.0.1 swap"},
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    for (int i = 0; i < num_tests; i++) {
        uint32_t result = swap_endianness(tests[i].num);
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
