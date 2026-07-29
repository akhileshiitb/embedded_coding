#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* Declaration of solution functions */
extern int   pool_init(int block_size, int num_blocks);
extern void *pool_alloc(void);
extern int   pool_free(void *ptr);
extern int   pool_available(void);
extern int   pool_contains(void *ptr);
extern int   pool_is_aligned(void *ptr, int alignment);

int main(void) {
    int num_tests = 16;
    int passed = 0;
    int result;
    int test_num = 0;

    /* Test 1: Init with valid parameters */
    test_num++;
    result = pool_init(16, 4);
    if (result == 0) {
        passed++;
        printf("[PASS] Test %d: pool_init(16, 4) succeeds\n", test_num);
    } else {
        printf("[FAIL] Test %d: pool_init(16, 4) | Expected: 0, Got: %d\n", test_num, result);
    }

    /* Test 2: Available = num_blocks after init */
    test_num++;
    result = pool_available();
    if (result == 4) {
        passed++;
        printf("[PASS] Test %d: pool_available() = 4 after init\n", test_num);
    } else {
        printf("[FAIL] Test %d: pool_available() | Expected: 4, Got: %d\n", test_num, result);
    }

    /* Test 3: Alloc returns non-NULL */
    test_num++;
    void *blk1 = pool_alloc();
    if (blk1 != NULL) {
        passed++;
        printf("[PASS] Test %d: pool_alloc() returns non-NULL\n", test_num);
    } else {
        printf("[FAIL] Test %d: pool_alloc() returned NULL\n", test_num);
    }

    /* Test 4: Available decreases after alloc */
    test_num++;
    result = pool_available();
    if (result == 3) {
        passed++;
        printf("[PASS] Test %d: pool_available() = 3 after 1 alloc\n", test_num);
    } else {
        printf("[FAIL] Test %d: pool_available() | Expected: 3, Got: %d\n", test_num, result);
    }

    /* Test 5: Allocated block is aligned to block_size */
    test_num++;
    result = pool_is_aligned(blk1, 16);
    if (result == 1) {
        passed++;
        printf("[PASS] Test %d: Allocated block is 16-byte aligned\n", test_num);
    } else {
        printf("[FAIL] Test %d: Alignment check | Expected: 1, Got: %d (ptr=%p)\n", test_num, result, blk1);
    }

    /* Test 6: Allocate all remaining blocks */
    test_num++;
    void *blk2 = pool_alloc();
    void *blk3 = pool_alloc();
    void *blk4 = pool_alloc();
    if (blk2 != NULL && blk3 != NULL && blk4 != NULL && pool_available() == 0) {
        passed++;
        printf("[PASS] Test %d: All 4 blocks allocated, available=0\n", test_num);
    } else {
        printf("[FAIL] Test %d: Alloc all blocks | avail=%d\n", test_num, pool_available());
    }

    /* Test 7: Alloc from exhausted pool returns NULL */
    test_num++;
    void *blk5 = pool_alloc();
    if (blk5 == NULL) {
        passed++;
        printf("[PASS] Test %d: pool_alloc() returns NULL when exhausted\n", test_num);
    } else {
        printf("[FAIL] Test %d: Alloc from exhausted pool | Expected: NULL, Got: %p\n", test_num, blk5);
    }

    /* Test 8: pool_contains on valid block */
    test_num++;
    result = pool_contains(blk1);
    if (result == 1) {
        passed++;
        printf("[PASS] Test %d: pool_contains(blk1) = 1\n", test_num);
    } else {
        printf("[FAIL] Test %d: pool_contains(blk1) | Expected: 1, Got: %d\n", test_num, result);
    }

    /* Test 9: pool_contains on invalid pointer */
    test_num++;
    int stack_var = 42;
    result = pool_contains(&stack_var);
    if (result == 0) {
        passed++;
        printf("[PASS] Test %d: pool_contains(stack_var) = 0\n", test_num);
    } else {
        printf("[FAIL] Test %d: pool_contains(stack_var) | Expected: 0, Got: %d\n", test_num, result);
    }

    /* Test 10: pool_contains on NULL */
    test_num++;
    result = pool_contains(NULL);
    if (result == 0) {
        passed++;
        printf("[PASS] Test %d: pool_contains(NULL) = 0\n", test_num);
    } else {
        printf("[FAIL] Test %d: pool_contains(NULL) | Expected: 0, Got: %d\n", test_num, result);
    }

    /* Test 11: Free returns block to pool */
    test_num++;
    result = pool_free(blk2);
    if (result == 0 && pool_available() == 1) {
        passed++;
        printf("[PASS] Test %d: pool_free(blk2) succeeds, available=1\n", test_num);
    } else {
        printf("[FAIL] Test %d: pool_free(blk2) | result=%d, avail=%d\n", test_num, result, pool_available());
    }

    /* Test 12: Can re-alloc after free */
    test_num++;
    void *blk_realloc = pool_alloc();
    if (blk_realloc != NULL && pool_available() == 0) {
        passed++;
        printf("[PASS] Test %d: Re-alloc after free succeeds\n", test_num);
    } else {
        printf("[FAIL] Test %d: Re-alloc | ptr=%p, avail=%d\n", test_num, blk_realloc, pool_available());
    }

    /* Test 13: Free NULL returns -1 */
    test_num++;
    result = pool_free(NULL);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: pool_free(NULL) returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: pool_free(NULL) | Expected: -1, Got: %d\n", test_num, result);
    }

    /* Test 14: Free pointer not from pool returns -1 */
    test_num++;
    result = pool_free(&stack_var);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: pool_free(stack_var) returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: pool_free(stack_var) | Expected: -1, Got: %d\n", test_num, result);
    }

    /* Test 15: Init with invalid block_size (not power of 2) */
    test_num++;
    result = pool_init(12, 4);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: pool_init(12, 4) fails (block_size not power of 2)\n", test_num);
    } else {
        printf("[FAIL] Test %d: pool_init(12, 4) | Expected: -1, Got: %d\n", test_num, result);
    }

    /* Test 16: Init with block_size < 8 fails */
    test_num++;
    result = pool_init(4, 4);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: pool_init(4, 4) fails (block_size < 8)\n", test_num);
    } else {
        printf("[FAIL] Test %d: pool_init(4, 4) | Expected: -1, Got: %d\n", test_num, result);
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);
    return (passed == num_tests) ? 0 : 1;
}
