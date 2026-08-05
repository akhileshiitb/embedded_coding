#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

/* Declaration of the solution function */
extern void *my_memcpy(void *dest, const void *src, size_t n);

/* Helper: compare memory and return 1 if equal */
static int mem_equal(const void *a, const void *b, size_t n) {
    const uint8_t *pa = (const uint8_t *)a;
    const uint8_t *pb = (const uint8_t *)b;
    for (size_t i = 0; i < n; i++) {
        if (pa[i] != pb[i]) return 0;
    }
    return 1;
}

/* Helper: fill buffer with pattern */
static void fill_pattern(uint8_t *buf, size_t n, uint8_t start) {
    for (size_t i = 0; i < n; i++) {
        buf[i] = (uint8_t)(start + i);
    }
}

/* Aligned buffer type */
typedef struct {
    uint8_t data[4096] __attribute__((aligned(16)));
} aligned_buf_t;

int main(void) {
    int num_tests = 18;
    int passed = 0;
    int test_num = 0;

    aligned_buf_t src_buf, dst_buf, ref_buf;

    /* ===== Test 1: Zero-length copy ===== */
    test_num++;
    {
        memset(dst_buf.data, 0xAA, 16);
        uint8_t expected_pattern[16];
        memset(expected_pattern, 0xAA, 16);

        void *ret = my_memcpy(dst_buf.data, src_buf.data, 0);

        if (ret == dst_buf.data && mem_equal(dst_buf.data, expected_pattern, 16)) {
            passed++;
            printf("[PASS] Test %d: Zero-length copy (n=0) — dest unchanged\n", test_num);
        } else {
            printf("[FAIL] Test %d: Zero-length copy (n=0) — dest was modified or wrong return\n", test_num);
        }
    }

    /* ===== Test 2: Single byte copy ===== */
    test_num++;
    {
        src_buf.data[0] = 0x42;
        memset(dst_buf.data, 0, 16);

        void *ret = my_memcpy(dst_buf.data, src_buf.data, 1);

        if (ret == dst_buf.data && dst_buf.data[0] == 0x42) {
            passed++;
            printf("[PASS] Test %d: Single byte copy (n=1)\n", test_num);
        } else {
            printf("[FAIL] Test %d: Single byte copy (n=1) | Expected: 0x42, Got: 0x%02X\n",
                   test_num, dst_buf.data[0]);
        }
    }

    /* ===== Test 3: Small copy (3 bytes, less than one word) ===== */
    test_num++;
    {
        src_buf.data[0] = 0xDE; src_buf.data[1] = 0xAD; src_buf.data[2] = 0xBE;
        memset(dst_buf.data, 0, 16);

        void *ret = my_memcpy(dst_buf.data, src_buf.data, 3);

        if (ret == dst_buf.data && dst_buf.data[0] == 0xDE &&
            dst_buf.data[1] == 0xAD && dst_buf.data[2] == 0xBE) {
            passed++;
            printf("[PASS] Test %d: Small copy (n=3, less than one word)\n", test_num);
        } else {
            printf("[FAIL] Test %d: Small copy (n=3) | Got: %02X %02X %02X\n",
                   test_num, dst_buf.data[0], dst_buf.data[1], dst_buf.data[2]);
        }
    }

    /* ===== Test 4: Exactly 4 bytes, aligned (one word) ===== */
    test_num++;
    {
        uint8_t src_data[4] = {0x11, 0x22, 0x33, 0x44};
        uint8_t dst_data[4] __attribute__((aligned(4))) = {0};

        /* Ensure src is also aligned */
        uint8_t aligned_src[4] __attribute__((aligned(4)));
        memcpy(aligned_src, src_data, 4);

        void *ret = my_memcpy(dst_data, aligned_src, 4);

        if (ret == dst_data && mem_equal(dst_data, src_data, 4)) {
            passed++;
            printf("[PASS] Test %d: Exactly 4 bytes, aligned (one word)\n", test_num);
        } else {
            printf("[FAIL] Test %d: Exactly 4 bytes, aligned | Got: %02X %02X %02X %02X\n",
                   test_num, dst_data[0], dst_data[1], dst_data[2], dst_data[3]);
        }
    }

    /* ===== Test 5: 8 bytes, aligned (two words) ===== */
    test_num++;
    {
        fill_pattern(src_buf.data, 8, 0x10);
        memset(dst_buf.data, 0, 16);

        void *ret = my_memcpy(dst_buf.data, src_buf.data, 8);

        if (ret == dst_buf.data && mem_equal(dst_buf.data, src_buf.data, 8)) {
            passed++;
            printf("[PASS] Test %d: 8 bytes, aligned (two words)\n", test_num);
        } else {
            printf("[FAIL] Test %d: 8 bytes, aligned — data mismatch\n", test_num);
        }
    }

    /* ===== Test 6: 16 bytes, aligned (four words) ===== */
    test_num++;
    {
        fill_pattern(src_buf.data, 16, 0xA0);
        memset(dst_buf.data, 0, 32);

        void *ret = my_memcpy(dst_buf.data, src_buf.data, 16);

        if (ret == dst_buf.data && mem_equal(dst_buf.data, src_buf.data, 16)) {
            passed++;
            printf("[PASS] Test %d: 16 bytes, aligned (four words)\n", test_num);
        } else {
            printf("[FAIL] Test %d: 16 bytes, aligned — data mismatch\n", test_num);
        }
    }

    /* ===== Test 7: Unaligned source (src offset +1) ===== */
    test_num++;
    {
        fill_pattern(src_buf.data, 32, 0x50);
        memset(dst_buf.data, 0, 32);

        /* src starts at offset 1 (unaligned) */
        void *ret = my_memcpy(dst_buf.data, src_buf.data + 1, 12);

        if (ret == dst_buf.data && mem_equal(dst_buf.data, src_buf.data + 1, 12)) {
            passed++;
            printf("[PASS] Test %d: Unaligned source (offset +1), 12 bytes\n", test_num);
        } else {
            printf("[FAIL] Test %d: Unaligned source (offset +1), 12 bytes — data mismatch\n", test_num);
        }
    }

    /* ===== Test 8: Unaligned destination (dest offset +1) ===== */
    test_num++;
    {
        fill_pattern(src_buf.data, 32, 0x70);
        memset(dst_buf.data, 0, 32);

        /* dest starts at offset 1 (unaligned) */
        void *ret = my_memcpy(dst_buf.data + 1, src_buf.data, 12);

        if (ret == (dst_buf.data + 1) && mem_equal(dst_buf.data + 1, src_buf.data, 12)) {
            passed++;
            printf("[PASS] Test %d: Unaligned destination (offset +1), 12 bytes\n", test_num);
        } else {
            printf("[FAIL] Test %d: Unaligned destination (offset +1), 12 bytes — data mismatch\n", test_num);
        }
    }

    /* ===== Test 9: Both src and dest unaligned (offset +3) ===== */
    test_num++;
    {
        fill_pattern(src_buf.data, 32, 0x30);
        memset(dst_buf.data, 0, 32);

        void *ret = my_memcpy(dst_buf.data + 3, src_buf.data + 3, 10);

        if (ret == (dst_buf.data + 3) && mem_equal(dst_buf.data + 3, src_buf.data + 3, 10)) {
            passed++;
            printf("[PASS] Test %d: Both unaligned (offset +3), 10 bytes\n", test_num);
        } else {
            printf("[FAIL] Test %d: Both unaligned (offset +3), 10 bytes — data mismatch\n", test_num);
        }
    }

    /* ===== Test 10: Large aligned copy (256 bytes) ===== */
    test_num++;
    {
        fill_pattern(src_buf.data, 256, 0x00);
        memset(dst_buf.data, 0xFF, 256);

        void *ret = my_memcpy(dst_buf.data, src_buf.data, 256);

        if (ret == dst_buf.data && mem_equal(dst_buf.data, src_buf.data, 256)) {
            passed++;
            printf("[PASS] Test %d: Large aligned copy (256 bytes)\n", test_num);
        } else {
            printf("[FAIL] Test %d: Large aligned copy (256 bytes) — data mismatch\n", test_num);
        }
    }

    /* ===== Test 11: Large aligned copy (1024 bytes) ===== */
    test_num++;
    {
        fill_pattern(src_buf.data, 1024, 0x01);
        memset(dst_buf.data, 0, 1024);

        void *ret = my_memcpy(dst_buf.data, src_buf.data, 1024);

        if (ret == dst_buf.data && mem_equal(dst_buf.data, src_buf.data, 1024)) {
            passed++;
            printf("[PASS] Test %d: Large aligned copy (1024 bytes)\n", test_num);
        } else {
            printf("[FAIL] Test %d: Large aligned copy (1024 bytes) — data mismatch\n", test_num);
        }
    }

    /* ===== Test 12: Copy with trailing bytes (13 bytes = 3 words + 1 byte) ===== */
    test_num++;
    {
        fill_pattern(src_buf.data, 16, 0xC0);
        memset(dst_buf.data, 0, 16);

        void *ret = my_memcpy(dst_buf.data, src_buf.data, 13);

        if (ret == dst_buf.data && mem_equal(dst_buf.data, src_buf.data, 13)) {
            passed++;
            printf("[PASS] Test %d: 13 bytes aligned (3 words + 1 trailing byte)\n", test_num);
        } else {
            printf("[FAIL] Test %d: 13 bytes aligned — data mismatch\n", test_num);
        }
    }

    /* ===== Test 13: Verify return value is dest ===== */
    test_num++;
    {
        fill_pattern(src_buf.data, 8, 0xF0);
        memset(dst_buf.data, 0, 8);

        void *ret = my_memcpy(dst_buf.data, src_buf.data, 8);

        if (ret == dst_buf.data) {
            passed++;
            printf("[PASS] Test %d: Return value equals dest pointer\n", test_num);
        } else {
            printf("[FAIL] Test %d: Return value equals dest pointer | Expected: %p, Got: %p\n",
                   test_num, (void *)dst_buf.data, ret);
        }
    }

    /* ===== Test 14: Source not modified ===== */
    test_num++;
    {
        fill_pattern(src_buf.data, 16, 0xAA);
        fill_pattern(ref_buf.data, 16, 0xAA); /* reference copy */
        memset(dst_buf.data, 0, 16);

        my_memcpy(dst_buf.data, src_buf.data, 16);

        if (mem_equal(src_buf.data, ref_buf.data, 16)) {
            passed++;
            printf("[PASS] Test %d: Source buffer not modified after copy\n", test_num);
        } else {
            printf("[FAIL] Test %d: Source buffer was modified during copy!\n", test_num);
        }
    }

    /* ===== Test 15: Dest bytes beyond n not modified ===== */
    test_num++;
    {
        fill_pattern(src_buf.data, 8, 0x10);
        memset(dst_buf.data, 0xBB, 16); /* fill dest with sentinel */

        my_memcpy(dst_buf.data, src_buf.data, 8);

        /* Bytes 8-15 should still be 0xBB */
        int beyond_ok = 1;
        for (int i = 8; i < 16; i++) {
            if (dst_buf.data[i] != 0xBB) { beyond_ok = 0; break; }
        }

        if (beyond_ok && mem_equal(dst_buf.data, src_buf.data, 8)) {
            passed++;
            printf("[PASS] Test %d: Bytes beyond n not modified\n", test_num);
        } else {
            printf("[FAIL] Test %d: Bytes beyond n were modified or copy incorrect\n", test_num);
        }
    }

    /* ===== Test 16: Copy all 0xFF bytes ===== */
    test_num++;
    {
        memset(src_buf.data, 0xFF, 32);
        memset(dst_buf.data, 0x00, 32);

        void *ret = my_memcpy(dst_buf.data, src_buf.data, 32);

        uint8_t expected[32];
        memset(expected, 0xFF, 32);

        if (ret == dst_buf.data && mem_equal(dst_buf.data, expected, 32)) {
            passed++;
            printf("[PASS] Test %d: Copy 32 bytes of 0xFF\n", test_num);
        } else {
            printf("[FAIL] Test %d: Copy 32 bytes of 0xFF — data mismatch\n", test_num);
        }
    }

    /* ===== Test 17: Copy all 0x00 bytes ===== */
    test_num++;
    {
        memset(src_buf.data, 0x00, 32);
        memset(dst_buf.data, 0xFF, 32);

        void *ret = my_memcpy(dst_buf.data, src_buf.data, 32);

        uint8_t expected[32];
        memset(expected, 0x00, 32);

        if (ret == dst_buf.data && mem_equal(dst_buf.data, expected, 32)) {
            passed++;
            printf("[PASS] Test %d: Copy 32 bytes of 0x00\n", test_num);
        } else {
            printf("[FAIL] Test %d: Copy 32 bytes of 0x00 — data mismatch\n", test_num);
        }
    }

    /* ===== Test 18: Mismatched alignment (src aligned, dest +2) ===== */
    test_num++;
    {
        fill_pattern(src_buf.data, 64, 0x20);
        memset(dst_buf.data, 0, 64);

        /* src is aligned (offset 0), dest is at +2 */
        void *ret = my_memcpy(dst_buf.data + 2, src_buf.data, 20);

        if (ret == (dst_buf.data + 2) && mem_equal(dst_buf.data + 2, src_buf.data, 20)) {
            passed++;
            printf("[PASS] Test %d: Mismatched alignment (src aligned, dest +2), 20 bytes\n", test_num);
        } else {
            printf("[FAIL] Test %d: Mismatched alignment (src aligned, dest +2) — data mismatch\n", test_num);
        }
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);

    return (passed == num_tests) ? 0 : 1;
}
