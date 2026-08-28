#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* Pool is statically configured as BLOCK_SIZE=16, NUM_BLOCKS=4 in solution.c */
#define BLOCK_SIZE 16
#define NUM_BLOCKS 4

/* Declaration of solution functions */
extern int   pool_init(void);
extern void *pool_alloc(void);
extern int   pool_free(void *ptr);
extern int   pool_available(void);
extern int   pool_is_aligned(void *ptr, int alignment);

int main(void) {
    int num_tests = 13;
    int passed = 0;
    int result;
    int test_num = 0;

    /* Test 1: Init succeeds */
    test_num++;
    result = pool_init();
    if (result == 0) {
        passed++;
        printf("[PASS] Test %d: pool_init() succeeds\n", test_num);
    } else {
        printf("[FAIL] Test %d: pool_init() | Expected: 0, Got: %d\n", test_num, result);
    }

    /* Test 2: Available = NUM_BLOCKS after init */
    test_num++;
    result = pool_available();
    if (result == NUM_BLOCKS) {
        passed++;
        printf("[PASS] Test %d: pool_available() = %d after init\n", test_num, NUM_BLOCKS);
    } else {
        printf("[FAIL] Test %d: pool_available() | Expected: %d, Got: %d\n", test_num, NUM_BLOCKS, result);
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
    if (result == NUM_BLOCKS - 1) {
        passed++;
        printf("[PASS] Test %d: pool_available() = %d after 1 alloc\n", test_num, NUM_BLOCKS - 1);
    } else {
        printf("[FAIL] Test %d: pool_available() | Expected: %d, Got: %d\n", test_num, NUM_BLOCKS - 1, result);
    }

    /* Test 5: Allocated block is aligned to BLOCK_SIZE */
    test_num++;
    result = pool_is_aligned(blk1, BLOCK_SIZE);
    if (result == 1) {
        passed++;
        printf("[PASS] Test %d: Allocated block is %d-byte aligned\n", test_num, BLOCK_SIZE);
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
        printf("[PASS] Test %d: All %d blocks allocated, available=0\n", test_num, NUM_BLOCKS);
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

    /* Test 8: Free returns block to pool */
    test_num++;
    result = pool_free(blk2);
    if (result == 0 && pool_available() == 1) {
        passed++;
        printf("[PASS] Test %d: pool_free(blk2) succeeds, available=1\n", test_num);
    } else {
        printf("[FAIL] Test %d: pool_free(blk2) | result=%d, avail=%d\n", test_num, result, pool_available());
    }

    /* Test 9: Can re-alloc after free */
    test_num++;
    void *blk_realloc = pool_alloc();
    if (blk_realloc != NULL && pool_available() == 0) {
        passed++;
        printf("[PASS] Test %d: Re-alloc after free succeeds\n", test_num);
    } else {
        printf("[FAIL] Test %d: Re-alloc | ptr=%p, avail=%d\n", test_num, blk_realloc, pool_available());
    }

    /* Test 10: Free NULL returns -1 */
    test_num++;
    result = pool_free(NULL);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: pool_free(NULL) returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: pool_free(NULL) | Expected: -1, Got: %d\n", test_num, result);
    }

    /* Test 11: Free pointer not from pool returns -1 */
    test_num++;
    int stack_var = 42;
    result = pool_free(&stack_var);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: pool_free(stack_var) returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: pool_free(stack_var) | Expected: -1, Got: %d\n", test_num, result);
    }

    /* Test 12: Re-init resets the pool to full availability */
    test_num++;
    pool_init();
    result = pool_available();
    if (result == NUM_BLOCKS) {
        passed++;
        printf("[PASS] Test %d: pool_init() re-init restores available=%d\n", test_num, NUM_BLOCKS);
    } else {
        printf("[FAIL] Test %d: re-init | Expected: %d, Got: %d\n", test_num, NUM_BLOCKS, result);
    }

    /* Test 13: Full alloc/free cycle leaves pool consistent */
    test_num++;
    void *ptrs[NUM_BLOCKS];
    for (int i = 0; i < NUM_BLOCKS; i++) ptrs[i] = pool_alloc();
    for (int i = 0; i < NUM_BLOCKS; i++) pool_free(ptrs[i]);
    if (pool_available() == NUM_BLOCKS && pool_alloc() != NULL) {
        passed++;
        printf("[PASS] Test %d: Full alloc/free cycle consistent\n", test_num);
    } else {
        printf("[FAIL] Test %d: cycle | avail=%d\n", test_num, pool_available());
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);
    return (passed == num_tests) ? 0 : 1;
}
