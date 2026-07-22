#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Declaration of the solution function */
extern uint32_t rotate_right(uint32_t num, int n);

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
        {0x00000001, 1,  0x80000000, "LSB wraps to MSB"},
        {0x80000000, 1,  0x40000000, "MSB moves to bit 30"},
        {0x00000010, 4,  0x00000001, "Rotate 0x10 right by 4"},
        {0x00000002, 1,  0x00000001, "Bit 1 moves to bit 0"},

        /* Rotate by 0 (identity) */
        {0x12345678, 0,  0x12345678, "Rotate by 0 is identity"},
        {0x00000000, 5,  0x00000000, "Rotating zero gives zero"},
        {0xFFFFFFFF, 7,  0xFFFFFFFF, "Rotating all ones gives all ones"},

        /* Nibble-aligned rotations */
        {0x12345678, 4,  0x81234567, "Rotate right by 4 (one nibble)"},
        {0x12345678, 8,  0x78123456, "Rotate right by 8 (one byte)"},
        {0x12345678, 16, 0x56781234, "Rotate right by 16 (two bytes)"},
        {0x12345678, 24, 0x34567812, "Rotate right by 24 (three bytes)"},

        /* Single bit tracking */
        {0x00000004, 1,  0x00000002, "Bit 2 moves to bit 1"},
        {0x00000004, 2,  0x00000001, "Bit 2 moves to bit 0"},
        {0x00000004, 3,  0x80000000, "Bit 2 wraps to MSB"},

        /* Patterns */
        {0x55555555, 1,  0xAAAAAAAA, "Alternating 01 rotated right by 1"},
        {0xFFFF0000, 16, 0x0000FFFF, "Upper half rotates to lower half"},
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    for (int i = 0; i < num_tests; i++) {
        uint32_t result = rotate_right(tests[i].num, tests[i].n);
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
