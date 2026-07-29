#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* Declaration of solution functions */
extern void *aligned_malloc(size_t size, size_t alignment);
extern void  aligned_free(void *ptr);

int main(void) {
    int num_tests = 16;
    int passed = 0;
    int test_num = 0;
    void *ptr;

    /* Test 1: Basic 16-byte alignment */
    test_num++;
    ptr = aligned_malloc(100, 16);
    if (ptr != NULL && ((uintptr_t)ptr % 16) == 0) {
        passed++;
        printf("[PASS] Test %d: aligned_malloc(100, 16) returns 16-byte aligned ptr\n", test_num);
    } else {
        printf("[FAIL] Test %d: aligned_malloc(100, 16) | Expected: aligned non-NULL, Got: ptr=%p, mod=%zu\n",
               test_num, ptr, ptr ? ((uintptr_t)ptr % 16) : 0);
    }
    aligned_free(ptr);

    /* Test 2: 64-byte alignment */
    test_num++;
    ptr = aligned_malloc(64, 64);
    if (ptr != NULL && ((uintptr_t)ptr % 64) == 0) {
        passed++;
        printf("[PASS] Test %d: aligned_malloc(64, 64) returns 64-byte aligned ptr\n", test_num);
    } else {
        printf("[FAIL] Test %d: aligned_malloc(64, 64) | Expected: aligned non-NULL, Got: ptr=%p, mod=%zu\n",
               test_num, ptr, ptr ? ((uintptr_t)ptr % 64) : 0);
    }
    aligned_free(ptr);

    /* Test 3: 256-byte alignment */
    test_num++;
    ptr = aligned_malloc(32, 256);
    if (ptr != NULL && ((uintptr_t)ptr % 256) == 0) {
        passed++;
        printf("[PASS] Test %d: aligned_malloc(32, 256) returns 256-byte aligned ptr\n", test_num);
    } else {
        printf("[FAIL] Test %d: aligned_malloc(32, 256) | Expected: aligned non-NULL, Got: ptr=%p, mod=%zu\n",
               test_num, ptr, ptr ? ((uintptr_t)ptr % 256) : 0);
    }
    aligned_free(ptr);

    /* Test 4: 128-byte alignment */
    test_num++;
    ptr = aligned_malloc(200, 128);
    if (ptr != NULL && ((uintptr_t)ptr % 128) == 0) {
        passed++;
        printf("[PASS] Test %d: aligned_malloc(200, 128) returns 128-byte aligned ptr\n", test_num);
    } else {
        printf("[FAIL] Test %d: aligned_malloc(200, 128) | Expected: aligned non-NULL, Got: ptr=%p, mod=%zu\n",
               test_num, ptr, ptr ? ((uintptr_t)ptr % 128) : 0);
    }
    aligned_free(ptr);

    /* Test 5: Minimum alignment (sizeof(void*), typically 8) */
    test_num++;
    ptr = aligned_malloc(50, sizeof(void *));
    if (ptr != NULL && ((uintptr_t)ptr % sizeof(void *)) == 0) {
        passed++;
        printf("[PASS] Test %d: aligned_malloc(50, %zu) returns correctly aligned ptr\n", test_num, sizeof(void *));
    } else {
        printf("[FAIL] Test %d: aligned_malloc(50, %zu) | Expected: aligned non-NULL, Got: ptr=%p\n",
               test_num, sizeof(void *), ptr);
    }
    aligned_free(ptr);

    /* Test 6: Memory is writable for the full size */
    test_num++;
    ptr = aligned_malloc(128, 32);
    int writable = 0;
    if (ptr != NULL) {
        memset(ptr, 0xAB, 128);
        unsigned char *bytes = (unsigned char *)ptr;
        writable = (bytes[0] == 0xAB && bytes[63] == 0xAB && bytes[127] == 0xAB);
    }
    if (ptr != NULL && writable) {
        passed++;
        printf("[PASS] Test %d: Allocated memory is writable for full 128 bytes\n", test_num);
    } else {
        printf("[FAIL] Test %d: Memory write check | ptr=%p, writable=%d\n", test_num, ptr, writable);
    }
    aligned_free(ptr);

    /* Test 7: Multiple allocations are all aligned */
    test_num++;
    void *ptrs[4];
    int all_aligned = 1;
    for (int i = 0; i < 4; i++) {
        ptrs[i] = aligned_malloc(32, 64);
        if (ptrs[i] == NULL || ((uintptr_t)ptrs[i] % 64) != 0) {
            all_aligned = 0;
        }
    }
    if (all_aligned) {
        passed++;
        printf("[PASS] Test %d: 4 consecutive aligned_malloc(32, 64) all return 64-byte aligned ptrs\n", test_num);
    } else {
        printf("[FAIL] Test %d: Multiple allocations | Not all 64-byte aligned\n", test_num);
    }
    for (int i = 0; i < 4; i++) {
        aligned_free(ptrs[i]);
    }

    /* Test 8: Multiple allocations return distinct pointers */
    test_num++;
    void *p1 = aligned_malloc(64, 32);
    void *p2 = aligned_malloc(64, 32);
    if (p1 != NULL && p2 != NULL && p1 != p2) {
        passed++;
        printf("[PASS] Test %d: Two allocations return distinct pointers\n", test_num);
    } else {
        printf("[FAIL] Test %d: Distinct pointers | p1=%p, p2=%p\n", test_num, p1, p2);
    }
    aligned_free(p1);
    aligned_free(p2);

    /* Test 9: Size 0 returns NULL */
    test_num++;
    ptr = aligned_malloc(0, 16);
    if (ptr == NULL) {
        passed++;
        printf("[PASS] Test %d: aligned_malloc(0, 16) returns NULL\n", test_num);
    } else {
        printf("[FAIL] Test %d: aligned_malloc(0, 16) | Expected: NULL, Got: %p\n", test_num, ptr);
        aligned_free(ptr);
    }

    /* Test 10: Alignment not a power of 2 returns NULL */
    test_num++;
    ptr = aligned_malloc(64, 3);
    if (ptr == NULL) {
        passed++;
        printf("[PASS] Test %d: aligned_malloc(64, 3) returns NULL (alignment not power of 2)\n", test_num);
    } else {
        printf("[FAIL] Test %d: aligned_malloc(64, 3) | Expected: NULL, Got: %p\n", test_num, ptr);
        aligned_free(ptr);
    }

    /* Test 11: Alignment of 0 returns NULL */
    test_num++;
    ptr = aligned_malloc(64, 0);
    if (ptr == NULL) {
        passed++;
        printf("[PASS] Test %d: aligned_malloc(64, 0) returns NULL\n", test_num);
    } else {
        printf("[FAIL] Test %d: aligned_malloc(64, 0) | Expected: NULL, Got: %p\n", test_num, ptr);
        aligned_free(ptr);
    }

    /* Test 12: Alignment smaller than sizeof(void*) returns NULL */
    test_num++;
    ptr = aligned_malloc(64, 2);
    if (ptr == NULL) {
        passed++;
        printf("[PASS] Test %d: aligned_malloc(64, 2) returns NULL (alignment < sizeof(void*))\n", test_num);
    } else {
        printf("[FAIL] Test %d: aligned_malloc(64, 2) | Expected: NULL, Got: %p\n", test_num, ptr);
        aligned_free(ptr);
    }

    /* Test 13: aligned_free(NULL) is a no-op (should not crash) */
    test_num++;
    aligned_free(NULL);
    passed++;
    printf("[PASS] Test %d: aligned_free(NULL) does not crash\n", test_num);

    /* Test 14: Large alignment (1024 bytes) */
    test_num++;
    ptr = aligned_malloc(64, 1024);
    if (ptr != NULL && ((uintptr_t)ptr % 1024) == 0) {
        passed++;
        printf("[PASS] Test %d: aligned_malloc(64, 1024) returns 1024-byte aligned ptr\n", test_num);
    } else {
        printf("[FAIL] Test %d: aligned_malloc(64, 1024) | Expected: aligned non-NULL, Got: ptr=%p, mod=%zu\n",
               test_num, ptr, ptr ? ((uintptr_t)ptr % 1024) : 0);
    }
    aligned_free(ptr);

    /* Test 15: Large allocation size with alignment */
    test_num++;
    ptr = aligned_malloc(4096, 64);
    if (ptr != NULL && ((uintptr_t)ptr % 64) == 0) {
        passed++;
        printf("[PASS] Test %d: aligned_malloc(4096, 64) returns 64-byte aligned ptr\n", test_num);
    } else {
        printf("[FAIL] Test %d: aligned_malloc(4096, 64) | Expected: aligned non-NULL, Got: ptr=%p, mod=%zu\n",
               test_num, ptr, ptr ? ((uintptr_t)ptr % 64) : 0);
    }
    aligned_free(ptr);

    /* Test 16: Alloc, write, free, re-alloc pattern */
    test_num++;
    ptr = aligned_malloc(64, 32);
    int realloc_ok = 0;
    if (ptr != NULL && ((uintptr_t)ptr % 32) == 0) {
        memset(ptr, 0xFF, 64);
        aligned_free(ptr);
        void *ptr2 = aligned_malloc(64, 32);
        if (ptr2 != NULL && ((uintptr_t)ptr2 % 32) == 0) {
            realloc_ok = 1;
        }
        aligned_free(ptr2);
    }
    if (realloc_ok) {
        passed++;
        printf("[PASS] Test %d: Alloc-write-free-realloc pattern works correctly\n", test_num);
    } else {
        printf("[FAIL] Test %d: Alloc-write-free-realloc pattern failed\n", test_num);
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);
    return (passed == num_tests) ? 0 : 1;
}
