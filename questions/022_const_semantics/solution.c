#include <stddef.h>

/**
 * @brief Return the value pointed to by ptr, doubled.
 *        ptr is pointer-to-const — you must not modify *ptr.
 *
 * @param ptr  Pointer to a const int.
 * @return     *ptr * 2
 */
int modify_through_ptr(const int *ptr) {
    /* TODO: Implement your solution here */
    (void)ptr;
    return 0;
}

/**
 * @brief Read a value at an offset from a const pointer.
 *        ptr_to_const_ptr points to a pointer that cannot be reassigned.
 *
 * @param ptr_to_const_ptr  Pointer to a const pointer to int.
 * @param offset            Array offset to read from.
 * @return                  Value at (*ptr_to_const_ptr)[offset].
 */
int advance_pointer(int *const *ptr_to_const_ptr, int offset) {
    /* TODO: Implement your solution here */
    (void)ptr_to_const_ptr;
    (void)offset;
    return 0;
}

/**
 * @brief Sum all elements in a const (ROM-like) table.
 *
 * @param table  Pointer to a read-only array of ints.
 * @param len    Number of elements.
 * @return       Sum of all elements.
 */
int sum_rom_table(const int *table, int len) {
    /* TODO: Implement your solution here */
    (void)table;
    (void)len;
    return 0;
}

/**
 * @brief Swap what two pointer-to-const-int variables point to.
 *        The pointed-to values are const, but the pointers themselves
 *        can be reassigned.
 *
 * @param pp1  Pointer to first const int pointer.
 * @param pp2  Pointer to second const int pointer.
 */
void swap_const_ptrs(const int **pp1, const int **pp2) {
    /* TODO: Implement your solution here */
    (void)pp1;
    (void)pp2;
}

/**
 * @brief Return a value from a static const lookup table.
 *        Demonstrates .rodata placement.
 *        LUT: {10, 20, 30, 40, 50, 60, 70, 80}
 *
 * @param index  Index into the LUT (0-7 valid).
 * @return       LUT value, or -1 if index out of range.
 */
int get_readonly_lut_value(int index) {
    /* TODO: Implement your solution here */
    (void)index;
    return -1;
}
