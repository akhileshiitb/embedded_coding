#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Declaration of the solution function */
extern int find_first_set(uint32_t num);

/* Test case structure */
struct test_case {
    uint32_t num;
    int expected;
    const char *description;
};

int main(void) {
    struct test_case tests[] = {
        /* Zero case */
        {0x00000000, 0,  "Zero returns 0 (no bits set)"},

        /* Single bit set */
        {0x00000001, 1,  "Bit 0 set → position 1"},
        {0x00000002, 2,  "Bit 1 set → position 2"},
        {0x00000004, 3,  "Bit 2 set → position 3"},
        {0x00000008, 4,  "Bit 3 set → position 4"},
        {0x00000010, 5,  "Bit 4 set → position 5"},
        {0x00000100, 9,  "Bit 8 set → position 9"},
        {0x00010000, 17, "Bit 16 set → position 17"},
        {0x80000000, 32, "Bit 31 set → position 32"},

        /* Multiple bits set (should return lowest) */
        {0x00000060, 6,  "0x60: bits 5,6 set → position 6"},
        {0x0000000F, 1,  "Lower nibble set → position 1"},
        {0xFFFFFFFF, 1,  "All bits set → position 1"},
        {0xFFFF0000, 17, "Upper 16 bits set → position 17"},
        {0x00000018, 4,  "0x18: bits 3,4 set → position 4"},

        /* Sparse patterns */
        {0x80000004, 3,  "Bits 2 and 31 set → position 3"},
        {0x40002000, 14, "Bits 13 and 30 set → position 14"},
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    for (int i = 0; i < num_tests; i++) {
        int result = find_first_set(tests[i].num);
        if (result == tests[i].expected) {
            passed++;
            printf("[PASS] Test %d: %s\n", i + 1, tests[i].description);
        } else {
            printf("[FAIL] Test %d: %s | Input: num=0x%08X | Expected: %d, Got: %d\n",
                   i + 1, tests[i].description,
                   tests[i].num, tests[i].expected, result);
        }
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);

    return (passed == num_tests) ? 0 : 1;
}
