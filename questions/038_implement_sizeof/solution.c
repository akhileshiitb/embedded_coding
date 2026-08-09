#include <stdint.h>
#include <stddef.h>

/**
 * @brief Implement sizeof() using pointer arithmetic.
 *
 * The trick: casting 0 to a pointer-to-type and incrementing by 1 yields
 * the byte offset of the next element — which equals the type's size.
 *
 * MUST NOT use the sizeof operator in the macro definition.
 *
 * MY_SIZEOF(type):     computes size of a type
 * MY_SIZEOF_VAR(var):  computes size of a variable
 */

/* TODO: Implement these macros using pointer arithmetic */
#define MY_SIZEOF(type)      (0)   /* Replace with your implementation */
#define MY_SIZEOF_VAR(var)   (0)   /* Replace with your implementation */

/* Test structs — DO NOT MODIFY */
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

/* Function wrappers for testing — use MY_SIZEOF in each */
size_t sizeof_char(void) {
    return MY_SIZEOF(char);
}

size_t sizeof_short(void) {
    return MY_SIZEOF(short);
}

size_t sizeof_int(void) {
    return MY_SIZEOF(int);
}

size_t sizeof_long(void) {
    return MY_SIZEOF(long);
}

size_t sizeof_float(void) {
    return MY_SIZEOF(float);
}

size_t sizeof_double(void) {
    return MY_SIZEOF(double);
}

size_t sizeof_pointer(void) {
    return MY_SIZEOF(int *);
}

size_t sizeof_int32(void) {
    return MY_SIZEOF(int32_t);
}

size_t sizeof_int64(void) {
    return MY_SIZEOF(int64_t);
}

size_t sizeof_struct_padded(void) {
    return MY_SIZEOF(struct padded);
}

size_t sizeof_struct_packed(void) {
    return MY_SIZEOF(struct packed_s);
}

size_t sizeof_array(void) {
    int32_t arr[10];
    return MY_SIZEOF_VAR(arr);
}
