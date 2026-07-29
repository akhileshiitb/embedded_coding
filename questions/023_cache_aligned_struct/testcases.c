#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdalign.h>

/* Declaration of solution functions */
extern size_t get_padded_size(void);
extern size_t get_optimized_size(void);
extern size_t get_packed_size(void);
extern int compute_padding(int offset, int alignment);
extern int is_cache_aligned(void *ptr);
extern int get_aligned_offset(int current_offset, int alignment);

int main(void) {
    int num_tests = 16;
    int passed = 0;
    int test_num = 0;
    size_t sz;
    int result;

    /* ===== Struct size tests ===== */

    /* Test 1: Padded struct size (poorly ordered) */
    test_num++;
    sz = get_padded_size();
    if (sz == 32) {
        passed++;
        printf("[PASS] Test %d: get_padded_size() = 32\n", test_num);
    } else {
        printf("[FAIL] Test %d: get_padded_size() | Expected: 32, Got: %zu\n", test_num, sz);
    }

    /* Test 2: Optimized struct size (reordered) */
    test_num++;
    sz = get_optimized_size();
    if (sz == 16) {
        passed++;
        printf("[PASS] Test %d: get_optimized_size() = 16\n", test_num);
    } else {
        printf("[FAIL] Test %d: get_optimized_size() | Expected: 16, Got: %zu\n", test_num, sz);
    }

    /* Test 3: Packed struct size (no padding) */
    test_num++;
    sz = get_packed_size();
    if (sz == 15) {
        passed++;
        printf("[PASS] Test %d: get_packed_size() = 15\n", test_num);
    } else {
        printf("[FAIL] Test %d: get_packed_size() | Expected: 15, Got: %zu\n", test_num, sz);
    }

    /* ===== compute_padding tests ===== */

    /* Test 4: Offset 1, alignment 4 → 3 padding bytes */
    test_num++;
    result = compute_padding(1, 4);
    if (result == 3) {
        passed++;
        printf("[PASS] Test %d: compute_padding(1, 4) = 3\n", test_num);
    } else {
        printf("[FAIL] Test %d: compute_padding(1, 4) | Expected: 3, Got: %d\n", test_num, result);
    }

    /* Test 5: Already aligned → 0 padding */
    test_num++;
    result = compute_padding(4, 4);
    if (result == 0) {
        passed++;
        printf("[PASS] Test %d: compute_padding(4, 4) = 0\n", test_num);
    } else {
        printf("[FAIL] Test %d: compute_padding(4, 4) | Expected: 0, Got: %d\n", test_num, result);
    }

    /* Test 6: Offset 5, alignment 8 → 3 padding */
    test_num++;
    result = compute_padding(5, 8);
    if (result == 3) {
        passed++;
        printf("[PASS] Test %d: compute_padding(5, 8) = 3\n", test_num);
    } else {
        printf("[FAIL] Test %d: compute_padding(5, 8) | Expected: 3, Got: %d\n", test_num, result);
    }

    /* Test 7: Offset 0, alignment 8 → 0 */
    test_num++;
    result = compute_padding(0, 8);
    if (result == 0) {
        passed++;
        printf("[PASS] Test %d: compute_padding(0, 8) = 0\n", test_num);
    } else {
        printf("[FAIL] Test %d: compute_padding(0, 8) | Expected: 0, Got: %d\n", test_num, result);
    }

    /* Test 8: Offset 7, alignment 8 → 1 */
    test_num++;
    result = compute_padding(7, 8);
    if (result == 1) {
        passed++;
        printf("[PASS] Test %d: compute_padding(7, 8) = 1\n", test_num);
    } else {
        printf("[FAIL] Test %d: compute_padding(7, 8) | Expected: 1, Got: %d\n", test_num, result);
    }

    /* Test 9: Offset 13, alignment 4 → 3 */
    test_num++;
    result = compute_padding(13, 4);
    if (result == 3) {
        passed++;
        printf("[PASS] Test %d: compute_padding(13, 4) = 3\n", test_num);
    } else {
        printf("[FAIL] Test %d: compute_padding(13, 4) | Expected: 3, Got: %d\n", test_num, result);
    }

    /* ===== is_cache_aligned tests ===== */

    /* Test 10: Known aligned pointer */
    test_num++;
    _Alignas(64) char aligned_buf[128];
    result = is_cache_aligned(aligned_buf);
    if (result == 1) {
        passed++;
        printf("[PASS] Test %d: 64-byte aligned pointer detected\n", test_num);
    } else {
        printf("[FAIL] Test %d: 64-byte aligned pointer | Expected: 1, Got: %d\n", test_num, result);
    }

    /* Test 11: Unaligned pointer (offset +1) */
    test_num++;
    result = is_cache_aligned(aligned_buf + 1);
    if (result == 0) {
        passed++;
        printf("[PASS] Test %d: Unaligned pointer (offset +1) detected\n", test_num);
    } else {
        printf("[FAIL] Test %d: Unaligned pointer | Expected: 0, Got: %d\n", test_num, result);
    }

    /* Test 12: NULL pointer is 64-byte aligned (0 % 64 == 0) */
    test_num++;
    result = is_cache_aligned(NULL);
    if (result == 1) {
        passed++;
        printf("[PASS] Test %d: NULL pointer is considered aligned\n", test_num);
    } else {
        printf("[FAIL] Test %d: NULL pointer alignment | Expected: 1, Got: %d\n", test_num, result);
    }

    /* ===== get_aligned_offset tests ===== */

    /* Test 13: Offset 1, alignment 4 → 4 */
    test_num++;
    result = get_aligned_offset(1, 4);
    if (result == 4) {
        passed++;
        printf("[PASS] Test %d: get_aligned_offset(1, 4) = 4\n", test_num);
    } else {
        printf("[FAIL] Test %d: get_aligned_offset(1, 4) | Expected: 4, Got: %d\n", test_num, result);
    }

    /* Test 14: Already aligned returns same */
    test_num++;
    result = get_aligned_offset(8, 8);
    if (result == 8) {
        passed++;
        printf("[PASS] Test %d: get_aligned_offset(8, 8) = 8\n", test_num);
    } else {
        printf("[FAIL] Test %d: get_aligned_offset(8, 8) | Expected: 8, Got: %d\n", test_num, result);
    }

    /* Test 15: Cache-line alignment */
    test_num++;
    result = get_aligned_offset(63, 64);
    if (result == 64) {
        passed++;
        printf("[PASS] Test %d: get_aligned_offset(63, 64) = 64\n", test_num);
    } else {
        printf("[FAIL] Test %d: get_aligned_offset(63, 64) | Expected: 64, Got: %d\n", test_num, result);
    }

    /* Test 16: Offset 0, alignment 64 → 0 */
    test_num++;
    result = get_aligned_offset(0, 64);
    if (result == 0) {
        passed++;
        printf("[PASS] Test %d: get_aligned_offset(0, 64) = 0\n", test_num);
    } else {
        printf("[FAIL] Test %d: get_aligned_offset(0, 64) | Expected: 0, Got: %d\n", test_num, result);
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);
    return (passed == num_tests) ? 0 : 1;
}
