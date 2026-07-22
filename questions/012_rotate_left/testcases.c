#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Declaration of the solution function */
extern uint32_t rotate_left(uint32_t num, int n);

/* Test case structure */
struct test_case {
    uint32_t num;
    int n;
    uint32_t expected;
    const char *description;
};

int main(void) {
    struct test_case tests[] = {
        /* Basic cases */
        {0x00000001, 1,  0x00000002, "Rotate 1 left by 1"},
        {0x00000001, 4,  0x00000010, "Rotate 1 left by 4"},
        {0x00000001, 31, 0x80000000, "Rotate 1 left by 31 (to MSB)"},
        {0x80000000, 1,  0x00000001, "MSB wraps to LSB"},

        /* Rotate by 0 (identity) */
        {0x12345678, 0,  0x12345678, "Rotate by 0 is identity"},
        {0x00000000, 5,  0x00000000, "Rotating zero gives zero"},
        {0xFFFFFFFF, 7,  0xFFFFFFFF, "Rotating all ones gives all ones"},

        /* Nibble-aligned rotations */
        {0x12345678, 4,  0x23456781, "Rotate left by 4 (one nibble)"},
        {0x12345678, 8,  0x34567812, "Rotate left by 8 (one byte)"},
        {0x12345678, 16, 0x56781234, "Rotate left by 16 (two bytes)"},
        {0x12345678, 24, 0x78123456, "Rotate left by 24 (three bytes)"},

        /* Single bit tracking */
        {0x00000002, 1,  0x00000004, "Bit 1 moves to bit 2"},
        {0x40000000, 1,  0x80000000, "Bit 30 moves to bit 31"},
        {0x40000000, 2,  0x00000001, "Bit 30 wraps past MSB"},

        /* Patterns */
        {0xAAAAAAAA, 1,  0x55555555, "Alternating 10 rotated by 1"},
        {0x0000FFFF, 16, 0xFFFF0000, "Lower half rotates to upper half"},
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    for (int i = 0; i < num_tests; i++) {
        uint32_t result = rotate_left(tests[i].num, tests[i].n);
        if (result == tests[i].expected) {
            passed++;
            printf("[PASS] Test %d: %s\n", i + 1, tests[i].description);
        } else {
            printf("[FAIL] Test %d: %s | Input: num=0x%08X, n=%d | Expected: 0x%08X, Got: 0x%08X\n",
                   i + 1, tests[i].description,
                   tests[i].num, tests[i].n, tests[i].expected, result);
        }
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);

    return (passed == num_tests) ? 0 : 1;
}
