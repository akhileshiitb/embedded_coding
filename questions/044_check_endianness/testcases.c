#include <stdio.h>
#include <stdint.h>

extern int is_little_endian(void);

/*
 * Independently determine the host endianness using a different method
 * than the candidate is likely to use, so we compare two approaches.
 *
 * We use a union here (the test harness's ground truth).
 */
static int host_is_little_endian_reference(void) {
    union {
        uint32_t u32;
        uint8_t bytes[4];
    } probe;
    probe.u32 = 0x01020304U;
    /* Little-endian: lowest address holds 0x04 (LSB) */
    return (probe.bytes[0] == 0x04) ? 1 : 0;
}

int main(void) {
    int passed = 0;
    int num_tests = 5;
    int test_num = 0;

    int reference = host_is_little_endian_reference();
    printf("(Host detected as %s-endian by reference method)\n\n",
           reference ? "LITTLE" : "BIG");

    /* === Test 1: Result matches independent reference === */
    {
        test_num++;
        int result = is_little_endian();
        if (result == reference) {
            passed++;
            printf("[PASS] Test %d: Result matches host endianness (%d)\n", test_num, result);
        } else {
            printf("[FAIL] Test %d: Result=%d, reference=%d (mismatch)\n",
                   test_num, result, reference);
        }
    }

    /* === Test 2: Result is strictly 0 or 1 === */
    {
        test_num++;
        int result = is_little_endian();
        if (result == 0 || result == 1) {
            passed++;
            printf("[PASS] Test %d: Result is a valid boolean (0 or 1): %d\n", test_num, result);
        } else {
            printf("[FAIL] Test %d: Result=%d is not 0 or 1\n", test_num, result);
        }
    }

    /* === Test 3: Deterministic — repeated calls give same answer === */
    {
        test_num++;
        int r1 = is_little_endian();
        int r2 = is_little_endian();
        int r3 = is_little_endian();
        if (r1 == r2 && r2 == r3) {
            passed++;
            printf("[PASS] Test %d: Deterministic across repeated calls (%d)\n", test_num, r1);
        } else {
            printf("[FAIL] Test %d: Non-deterministic: %d, %d, %d\n", test_num, r1, r2, r3);
        }
    }

    /* === Test 4: Cross-check with pointer-cast method === */
    {
        test_num++;
        uint32_t value = 1;
        uint8_t first_byte = *(uint8_t *)&value;
        /* Little-endian: first byte of value 1 is 0x01 */
        int pointer_method = (first_byte == 1) ? 1 : 0;
        int result = is_little_endian();
        if (result == pointer_method) {
            passed++;
            printf("[PASS] Test %d: Matches pointer-cast method (%d)\n", test_num, result);
        } else {
            printf("[FAIL] Test %d: Result=%d, pointer-method=%d\n",
                   test_num, result, pointer_method);
        }
    }

    /* === Test 5: Cross-check with 16-bit union method === */
    {
        test_num++;
        union {
            uint16_t u16;
            uint8_t bytes[2];
        } probe;
        probe.u16 = 0x0001U;
        /* Little-endian: byte[0] == 0x01 */
        int u16_method = (probe.bytes[0] == 0x01) ? 1 : 0;
        int result = is_little_endian();
        if (result == u16_method) {
            passed++;
            printf("[PASS] Test %d: Matches 16-bit union method (%d)\n", test_num, result);
        } else {
            printf("[FAIL] Test %d: Result=%d, u16-method=%d\n",
                   test_num, result, u16_method);
        }
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);

    return (passed == num_tests) ? 0 : 1;
}
