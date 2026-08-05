#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

/* Declaration of the solution function */
extern void *my_memmove(void *dest, const void *src, size_t n);

/* Helper: compare memory and return 1 if equal */
static int mem_equal(const void *a, const void *b, size_t n) {
    const uint8_t *pa = (const uint8_t *)a;
    const uint8_t *pb = (const uint8_t *)b;
    for (size_t i = 0; i < n; i++) {
        if (pa[i] != pb[i]) return 0;
    }
    return 1;
}

/* Helper: print buffer in hex */
static void print_hex(const uint8_t *buf, size_t n) {
    for (size_t i = 0; i < n; i++) {
        printf("%02X ", buf[i]);
    }
}

int main(void) {
    int num_tests = 18;
    int passed = 0;
    int test_num = 0;

    uint8_t buf[256] __attribute__((aligned(16)));
    uint8_t ref[256];

    /* ===== Test 1: Zero-length move ===== */
    test_num++;
    {
        memset(buf, 0xAA, 16);
        uint8_t expected[16];
        memset(expected, 0xAA, 16);

        void *ret = my_memmove(buf, buf + 4, 0);

        if (ret == buf && mem_equal(buf, expected, 16)) {
            passed++;
            printf("[PASS] Test %d: Zero-length move (n=0) — buffer unchanged\n", test_num);
        } else {
            printf("[FAIL] Test %d: Zero-length move (n=0) — buffer modified or wrong return\n", test_num);
        }
    }

    /* ===== Test 2: Non-overlapping copy (separate buffers) ===== */
    test_num++;
    {
        uint8_t src[16] __attribute__((aligned(16)));
        uint8_t dst[16] __attribute__((aligned(16)));
        for (int i = 0; i < 16; i++) src[i] = (uint8_t)(0x10 + i);
        memset(dst, 0, 16);

        void *ret = my_memmove(dst, src, 16);

        if (ret == dst && mem_equal(dst, src, 16)) {
            passed++;
            printf("[PASS] Test %d: Non-overlapping copy, 16 bytes\n", test_num);
        } else {
            printf("[FAIL] Test %d: Non-overlapping copy, 16 bytes — data mismatch\n", test_num);
        }
    }

    /* ===== Test 3: Forward overlap — dest < src, overlap by 4 ===== */
    test_num++;
    {
        /* buf = [1,2,3,4,5,6,7,8,9,10,11,12] */
        for (int i = 0; i < 12; i++) buf[i] = (uint8_t)(i + 1);

        /* Move src=buf+4 to dest=buf+2, n=6 (overlap: dest < src, safe forward) */
        /* Expected: copy buf[4..9] to buf[2..7] */
        /* Before: 1 2 3 4 5 6 7 8 9 10 11 12 */
        /* After:  1 2 5 6 7 8 9 10 9 10 11 12 */
        uint8_t expected[] = {1, 2, 5, 6, 7, 8, 9, 10, 9, 10, 11, 12};

        void *ret = my_memmove(buf + 2, buf + 4, 6);

        if (ret == (buf + 2) && mem_equal(buf, expected, 12)) {
            passed++;
            printf("[PASS] Test %d: Forward overlap (dest < src), move 6 bytes\n", test_num);
        } else {
            printf("[FAIL] Test %d: Forward overlap (dest < src) | Expected: ", test_num);
            print_hex(expected, 12);
            printf("| Got: ");
            print_hex(buf, 12);
            printf("\n");
        }
    }

    /* ===== Test 4: Backward overlap — dest > src, overlap by 4 ===== */
    test_num++;
    {
        /* buf = [1,2,3,4,5,6,7,8,9,10,11,12] */
        for (int i = 0; i < 12; i++) buf[i] = (uint8_t)(i + 1);

        /* Move src=buf+2 to dest=buf+4, n=6 (overlap: dest > src, need backward) */
        /* Copy buf[2..7] to buf[4..9] */
        /* Before: 1 2 3 4 5 6 7 8 9 10 11 12 */
        /* After:  1 2 3 4 3 4 5 6 7 8  11 12 */
        uint8_t expected[] = {1, 2, 3, 4, 3, 4, 5, 6, 7, 8, 11, 12};

        void *ret = my_memmove(buf + 4, buf + 2, 6);

        if (ret == (buf + 4) && mem_equal(buf, expected, 12)) {
            passed++;
            printf("[PASS] Test %d: Backward overlap (dest > src), move 6 bytes\n", test_num);
        } else {
            printf("[FAIL] Test %d: Backward overlap (dest > src) | Expected: ", test_num);
            print_hex(expected, 12);
            printf("| Got: ");
            print_hex(buf, 12);
            printf("\n");
        }
    }

    /* ===== Test 5: Complete overlap (dest == src) ===== */
    test_num++;
    {
        for (int i = 0; i < 8; i++) buf[i] = (uint8_t)(0xA0 + i);
        uint8_t expected[8];
        memcpy(expected, buf, 8);

        void *ret = my_memmove(buf, buf, 8);

        if (ret == buf && mem_equal(buf, expected, 8)) {
            passed++;
            printf("[PASS] Test %d: Complete overlap (dest == src), 8 bytes unchanged\n", test_num);
        } else {
            printf("[FAIL] Test %d: Complete overlap — buffer was corrupted\n", test_num);
        }
    }

    /* ===== Test 6: Overlap by 1 byte forward (dest = src - 1) ===== */
    test_num++;
    {
        for (int i = 0; i < 10; i++) buf[i] = (uint8_t)(i + 1);

        /* src=buf+1, dest=buf+0, n=5: copy [2,3,4,5,6] to positions [0..4] */
        /* Before: 1 2 3 4 5 6 7 8 9 10 */
        /* After:  2 3 4 5 6 6 7 8 9 10 */
        uint8_t expected[] = {2, 3, 4, 5, 6, 6, 7, 8, 9, 10};

        void *ret = my_memmove(buf, buf + 1, 5);

        if (ret == buf && mem_equal(buf, expected, 10)) {
            passed++;
            printf("[PASS] Test %d: Overlap by 1 forward (shift left by 1)\n", test_num);
        } else {
            printf("[FAIL] Test %d: Overlap by 1 forward | Expected: ", test_num);
            print_hex(expected, 10);
            printf("| Got: ");
            print_hex(buf, 10);
            printf("\n");
        }
    }

    /* ===== Test 7: Overlap by 1 byte backward (dest = src + 1) ===== */
    test_num++;
    {
        for (int i = 0; i < 10; i++) buf[i] = (uint8_t)(i + 1);

        /* src=buf+0, dest=buf+1, n=5: copy [1,2,3,4,5] to positions [1..5] */
        /* Before: 1 2 3 4 5 6 7 8 9 10 */
        /* After:  1 1 2 3 4 5 7 8 9 10 */
        uint8_t expected[] = {1, 1, 2, 3, 4, 5, 7, 8, 9, 10};

        void *ret = my_memmove(buf + 1, buf, 5);

        if (ret == (buf + 1) && mem_equal(buf, expected, 10)) {
            passed++;
            printf("[PASS] Test %d: Overlap by 1 backward (shift right by 1)\n", test_num);
        } else {
            printf("[FAIL] Test %d: Overlap by 1 backward | Expected: ", test_num);
            print_hex(expected, 10);
            printf("| Got: ");
            print_hex(buf, 10);
            printf("\n");
        }
    }

    /* ===== Test 8: Large forward overlap ===== */
    test_num++;
    {
        for (int i = 0; i < 64; i++) buf[i] = (uint8_t)(i + 1);

        /* src=buf+4, dest=buf+0, n=32: shift 32 bytes left by 4 */
        /* Use libc memmove as reference */
        memcpy(ref, buf, 64);
        memmove(ref, ref + 4, 32);

        void *ret = my_memmove(buf, buf + 4, 32);

        if (ret == buf && mem_equal(buf, ref, 64)) {
            passed++;
            printf("[PASS] Test %d: Large forward overlap (shift left by 4, 32 bytes)\n", test_num);
        } else {
            printf("[FAIL] Test %d: Large forward overlap — data mismatch\n", test_num);
        }
    }

    /* ===== Test 9: Large backward overlap ===== */
    test_num++;
    {
        for (int i = 0; i < 64; i++) buf[i] = (uint8_t)(i + 1);

        /* src=buf+0, dest=buf+4, n=32: shift 32 bytes right by 4 */
        memcpy(ref, buf, 64);
        memmove(ref + 4, ref, 32);

        void *ret = my_memmove(buf + 4, buf, 32);

        if (ret == (buf + 4) && mem_equal(buf, ref, 64)) {
            passed++;
            printf("[PASS] Test %d: Large backward overlap (shift right by 4, 32 bytes)\n", test_num);
        } else {
            printf("[FAIL] Test %d: Large backward overlap — data mismatch\n", test_num);
        }
    }

    /* ===== Test 10: Almost complete overlap (dest = src + 1, n = 31) ===== */
    test_num++;
    {
        for (int i = 0; i < 64; i++) buf[i] = (uint8_t)(0x40 + i);

        memcpy(ref, buf, 64);
        memmove(ref + 1, ref, 31);

        void *ret = my_memmove(buf + 1, buf, 31);

        if (ret == (buf + 1) && mem_equal(buf, ref, 64)) {
            passed++;
            printf("[PASS] Test %d: Almost complete overlap (shift right by 1, 31 bytes)\n", test_num);
        } else {
            printf("[FAIL] Test %d: Almost complete overlap | Expected: ", test_num);
            print_hex(ref, 32);
            printf("| Got: ");
            print_hex(buf, 32);
            printf("\n");
        }
    }

    /* ===== Test 11: Single byte ===== */
    test_num++;
    {
        buf[0] = 0x55;
        buf[1] = 0xAA;

        void *ret = my_memmove(buf + 1, buf, 1);

        if (ret == (buf + 1) && buf[1] == 0x55) {
            passed++;
            printf("[PASS] Test %d: Single byte move (adjacent, overlap)\n", test_num);
        } else {
            printf("[FAIL] Test %d: Single byte move | Expected buf[1]=0x55, Got: 0x%02X\n",
                   test_num, buf[1]);
        }
    }

    /* ===== Test 12: Return value check ===== */
    test_num++;
    {
        for (int i = 0; i < 8; i++) buf[i] = (uint8_t)i;

        void *ret = my_memmove(buf + 2, buf, 4);

        if (ret == (buf + 2)) {
            passed++;
            printf("[PASS] Test %d: Return value equals dest\n", test_num);
        } else {
            printf("[FAIL] Test %d: Return value | Expected: %p, Got: %p\n",
                   test_num, (void *)(buf + 2), ret);
        }
    }

    /* ===== Test 13: Non-overlapping large (128 bytes) ===== */
    test_num++;
    {
        uint8_t src[128] __attribute__((aligned(16)));
        uint8_t dst[128] __attribute__((aligned(16)));
        for (int i = 0; i < 128; i++) src[i] = (uint8_t)(i * 3);
        memset(dst, 0, 128);

        void *ret = my_memmove(dst, src, 128);

        if (ret == dst && mem_equal(dst, src, 128)) {
            passed++;
            printf("[PASS] Test %d: Non-overlapping large copy (128 bytes)\n", test_num);
        } else {
            printf("[FAIL] Test %d: Non-overlapping large copy — data mismatch\n", test_num);
        }
    }

    /* ===== Test 14: Forward overlap, half-size overlap ===== */
    test_num++;
    {
        /* 16 bytes, move src=buf+8 to dest=buf+4, n=8 (overlap 4 bytes) */
        for (int i = 0; i < 20; i++) buf[i] = (uint8_t)(0x10 + i);

        memcpy(ref, buf, 20);
        memmove(ref + 4, ref + 8, 8);

        void *ret = my_memmove(buf + 4, buf + 8, 8);

        if (ret == (buf + 4) && mem_equal(buf, ref, 20)) {
            passed++;
            printf("[PASS] Test %d: Forward overlap, half-size (8 bytes, overlap 4)\n", test_num);
        } else {
            printf("[FAIL] Test %d: Forward overlap, half-size — data mismatch\n", test_num);
        }
    }

    /* ===== Test 15: Backward overlap, half-size overlap ===== */
    test_num++;
    {
        /* 16 bytes, move src=buf+4 to dest=buf+8, n=8 (overlap 4 bytes) */
        for (int i = 0; i < 20; i++) buf[i] = (uint8_t)(0x20 + i);

        memcpy(ref, buf, 20);
        memmove(ref + 8, ref + 4, 8);

        void *ret = my_memmove(buf + 8, buf + 4, 8);

        if (ret == (buf + 8) && mem_equal(buf, ref, 20)) {
            passed++;
            printf("[PASS] Test %d: Backward overlap, half-size (8 bytes, overlap 4)\n", test_num);
        } else {
            printf("[FAIL] Test %d: Backward overlap, half-size — data mismatch\n", test_num);
        }
    }

    /* ===== Test 16: All same bytes (overlap should not matter) ===== */
    test_num++;
    {
        memset(buf, 0x77, 32);
        uint8_t expected[32];
        memset(expected, 0x77, 32);

        void *ret = my_memmove(buf + 4, buf, 16);

        if (ret == (buf + 4) && mem_equal(buf, expected, 32)) {
            passed++;
            printf("[PASS] Test %d: All same bytes (0x77), overlap doesn't matter\n", test_num);
        } else {
            printf("[FAIL] Test %d: All same bytes — unexpected corruption\n", test_num);
        }
    }

    /* ===== Test 17: Overlap where entire src is within dest ===== */
    test_num++;
    {
        for (int i = 0; i < 16; i++) buf[i] = (uint8_t)(i + 1);

        /* src=buf+2, dest=buf+0, n=12: src region [2..13] fully within [0..11] range too */
        memcpy(ref, buf, 16);
        memmove(ref, ref + 2, 12);

        void *ret = my_memmove(buf, buf + 2, 12);

        if (ret == buf && mem_equal(buf, ref, 16)) {
            passed++;
            printf("[PASS] Test %d: Source fully within extended dest region (forward)\n", test_num);
        } else {
            printf("[FAIL] Test %d: Source fully within dest region | Expected: ", test_num);
            print_hex(ref, 16);
            printf("| Got: ");
            print_hex(buf, 16);
            printf("\n");
        }
    }

    /* ===== Test 18: Large backward overlap (shift right by 2, 64 bytes) ===== */
    test_num++;
    {
        for (int i = 0; i < 128; i++) buf[i] = (uint8_t)(i);

        memcpy(ref, buf, 128);
        memmove(ref + 2, ref, 64);

        void *ret = my_memmove(buf + 2, buf, 64);

        if (ret == (buf + 2) && mem_equal(buf, ref, 128)) {
            passed++;
            printf("[PASS] Test %d: Large backward overlap (shift right by 2, 64 bytes)\n", test_num);
        } else {
            printf("[FAIL] Test %d: Large backward overlap — data mismatch\n", test_num);
        }
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);

    return (passed == num_tests) ? 0 : 1;
}
