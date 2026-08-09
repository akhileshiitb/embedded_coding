#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

/* Declaration of the solution functions */
extern size_t sizeof_char(void);
extern size_t sizeof_short(void);
extern size_t sizeof_int(void);
extern size_t sizeof_long(void);
extern size_t sizeof_float(void);
extern size_t sizeof_double(void);
extern size_t sizeof_pointer(void);
extern size_t sizeof_int32(void);
extern size_t sizeof_int64(void);
extern size_t sizeof_struct_padded(void);
extern size_t sizeof_struct_packed(void);
extern size_t sizeof_array(void);

/* Replicate the same structs for expected value computation */
struct padded {
    char a;
    int32_t b;
    char c;
};

struct __attribute__((packed)) packed_s {
    char a;
    int32_t b;
    char c;
};

struct test_case {
    size_t (*func)(void);
    size_t expected;
    const char *description;
};

int main(void) {
    struct test_case tests[] = {
        /* Primitive types */
        {sizeof_char,    sizeof(char),    "char: always 1 byte"},
        {sizeof_short,   sizeof(short),   "short: typically 2 bytes"},
        {sizeof_int,     sizeof(int),     "int: typically 4 bytes"},
        {sizeof_long,    sizeof(long),    "long: 4 bytes (32-bit) or 8 bytes (64-bit)"},
        {sizeof_float,   sizeof(float),   "float: typically 4 bytes (IEEE 754)"},
        {sizeof_double,  sizeof(double),  "double: typically 8 bytes (IEEE 754)"},

        /* Pointer */
        {sizeof_pointer, sizeof(int *),   "pointer: 4 bytes (32-bit) or 8 bytes (64-bit)"},

        /* Fixed-width types */
        {sizeof_int32,   sizeof(int32_t), "int32_t: always 4 bytes"},
        {sizeof_int64,   sizeof(int64_t), "int64_t: always 8 bytes"},

        /* Struct with padding */
        {sizeof_struct_padded, sizeof(struct padded),
         "struct padded {char; int32_t; char}: includes alignment padding"},

        /* Packed struct (no padding) */
        {sizeof_struct_packed, sizeof(struct packed_s),
         "struct packed {char; int32_t; char}: no padding (packed)"},

        /* Array */
        {sizeof_array,   sizeof(int32_t[10]),
         "int32_t arr[10]: total array size (not pointer size)"},
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    for (int i = 0; i < num_tests; i++) {
        size_t result = tests[i].func();
        if (result == tests[i].expected) {
            passed++;
            printf("[PASS] Test %d: %s | size = %zu\n", i + 1, tests[i].description, result);
        } else {
            printf("[FAIL] Test %d: %s | Expected: %zu, Got: %zu\n",
                   i + 1, tests[i].description, tests[i].expected, result);
        }
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);

    return (passed == num_tests) ? 0 : 1;
}
