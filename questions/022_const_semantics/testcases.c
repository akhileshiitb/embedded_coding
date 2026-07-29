#include <stdio.h>
#include <stddef.h>

/* Declaration of solution functions */
extern int modify_through_ptr(const int *ptr);
extern int advance_pointer(int *const *ptr_to_const_ptr, int offset);
extern int sum_rom_table(const int *table, int len);
extern void swap_const_ptrs(const int **pp1, const int **pp2);
extern int get_readonly_lut_value(int index);

int main(void) {
    int num_tests = 16;
    int passed = 0;
    int result;
    int test_num = 0;

    /* ===== modify_through_ptr tests ===== */

    /* Test 1: Basic doubling */
    test_num++;
    int val1 = 5;
    result = modify_through_ptr(&val1);
    if (result == 10 && val1 == 5) {
        passed++;
        printf("[PASS] Test %d: modify_through_ptr(&5) returns 10, original unchanged\n", test_num);
    } else {
        printf("[FAIL] Test %d: modify_through_ptr(&5) | Expected: result=10, val=5 | Got: result=%d, val=%d\n", test_num, result, val1);
    }

    /* Test 2: Zero value */
    test_num++;
    int val2 = 0;
    result = modify_through_ptr(&val2);
    if (result == 0 && val2 == 0) {
        passed++;
        printf("[PASS] Test %d: modify_through_ptr(&0) returns 0\n", test_num);
    } else {
        printf("[FAIL] Test %d: modify_through_ptr(&0) | Expected: 0, Got: %d\n", test_num, result);
    }

    /* Test 3: Negative value */
    test_num++;
    int val3 = -7;
    result = modify_through_ptr(&val3);
    if (result == -14 && val3 == -7) {
        passed++;
        printf("[PASS] Test %d: modify_through_ptr(&-7) returns -14, original unchanged\n", test_num);
    } else {
        printf("[FAIL] Test %d: modify_through_ptr(&-7) | Expected: result=-14, val=-7 | Got: result=%d, val=%d\n", test_num, result, val3);
    }

    /* ===== advance_pointer tests ===== */

    /* Test 4: Read at offset 0 */
    test_num++;
    int arr1[] = {100, 200, 300, 400};
    int *p1 = arr1;
    result = advance_pointer(&p1, 0);
    if (result == 100) {
        passed++;
        printf("[PASS] Test %d: advance_pointer offset=0 reads first element\n", test_num);
    } else {
        printf("[FAIL] Test %d: advance_pointer offset=0 | Expected: 100, Got: %d\n", test_num, result);
    }

    /* Test 5: Read at offset 2 */
    test_num++;
    result = advance_pointer(&p1, 2);
    if (result == 300) {
        passed++;
        printf("[PASS] Test %d: advance_pointer offset=2 reads third element\n", test_num);
    } else {
        printf("[FAIL] Test %d: advance_pointer offset=2 | Expected: 300, Got: %d\n", test_num, result);
    }

    /* Test 6: Read at offset 3 */
    test_num++;
    result = advance_pointer(&p1, 3);
    if (result == 400) {
        passed++;
        printf("[PASS] Test %d: advance_pointer offset=3 reads fourth element\n", test_num);
    } else {
        printf("[FAIL] Test %d: advance_pointer offset=3 | Expected: 400, Got: %d\n", test_num, result);
    }

    /* ===== sum_rom_table tests ===== */

    /* Test 7: Sum of 4 elements */
    test_num++;
    const int rom1[] = {1, 2, 3, 4};
    result = sum_rom_table(rom1, 4);
    if (result == 10) {
        passed++;
        printf("[PASS] Test %d: sum_rom_table({1,2,3,4}) = 10\n", test_num);
    } else {
        printf("[FAIL] Test %d: sum_rom_table({1,2,3,4}) | Expected: 10, Got: %d\n", test_num, result);
    }

    /* Test 8: Single element */
    test_num++;
    const int rom2[] = {42};
    result = sum_rom_table(rom2, 1);
    if (result == 42) {
        passed++;
        printf("[PASS] Test %d: sum_rom_table({42}) = 42\n", test_num);
    } else {
        printf("[FAIL] Test %d: sum_rom_table({42}) | Expected: 42, Got: %d\n", test_num, result);
    }

    /* Test 9: Empty table (len=0) */
    test_num++;
    result = sum_rom_table(NULL, 0);
    if (result == 0) {
        passed++;
        printf("[PASS] Test %d: sum_rom_table(NULL, 0) = 0\n", test_num);
    } else {
        printf("[FAIL] Test %d: sum_rom_table(NULL, 0) | Expected: 0, Got: %d\n", test_num, result);
    }

    /* Test 10: Negative values */
    test_num++;
    const int rom3[] = {-5, 10, -3, 8};
    result = sum_rom_table(rom3, 4);
    if (result == 10) {
        passed++;
        printf("[PASS] Test %d: sum_rom_table({-5,10,-3,8}) = 10\n", test_num);
    } else {
        printf("[FAIL] Test %d: sum_rom_table({-5,10,-3,8}) | Expected: 10, Got: %d\n", test_num, result);
    }

    /* ===== swap_const_ptrs tests ===== */

    /* Test 11: Basic swap */
    test_num++;
    const int sa = 100, sb = 200;
    const int *pa = &sa, *pb = &sb;
    swap_const_ptrs(&pa, &pb);
    if (*pa == 200 && *pb == 100) {
        passed++;
        printf("[PASS] Test %d: swap_const_ptrs swaps pointers correctly\n", test_num);
    } else {
        printf("[FAIL] Test %d: swap_const_ptrs | Expected: *pa=200, *pb=100 | Got: *pa=%d, *pb=%d\n", test_num, *pa, *pb);
    }

    /* Test 12: Swap back (double swap) */
    test_num++;
    swap_const_ptrs(&pa, &pb);
    if (*pa == 100 && *pb == 200) {
        passed++;
        printf("[PASS] Test %d: Double swap restores original\n", test_num);
    } else {
        printf("[FAIL] Test %d: Double swap | Expected: *pa=100, *pb=200 | Got: *pa=%d, *pb=%d\n", test_num, *pa, *pb);
    }

    /* Test 13: Swap with same pointer (self-swap) */
    test_num++;
    const int sc = 999;
    const int *pc = &sc;
    const int *pc_copy = pc;
    swap_const_ptrs(&pc, &pc);
    if (*pc == 999 && pc == pc_copy) {
        passed++;
        printf("[PASS] Test %d: Self-swap preserves value\n", test_num);
    } else {
        printf("[FAIL] Test %d: Self-swap | Expected: *pc=999 | Got: *pc=%d\n", test_num, *pc);
    }

    /* ===== get_readonly_lut_value tests ===== */

    /* Test 14: First element */
    test_num++;
    result = get_readonly_lut_value(0);
    if (result == 10) {
        passed++;
        printf("[PASS] Test %d: LUT[0] = 10\n", test_num);
    } else {
        printf("[FAIL] Test %d: LUT[0] | Expected: 10, Got: %d\n", test_num, result);
    }

    /* Test 15: Last valid element */
    test_num++;
    result = get_readonly_lut_value(7);
    if (result == 80) {
        passed++;
        printf("[PASS] Test %d: LUT[7] = 80\n", test_num);
    } else {
        printf("[FAIL] Test %d: LUT[7] | Expected: 80, Got: %d\n", test_num, result);
    }

    /* Test 16: Out of range */
    test_num++;
    result = get_readonly_lut_value(8);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: LUT[8] out of range returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: LUT[8] | Expected: -1, Got: %d\n", test_num, result);
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);
    return (passed == num_tests) ? 0 : 1;
}
