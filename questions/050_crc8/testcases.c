#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

extern uint8_t crc8(const uint8_t *data, size_t len);
extern int     crc8_verify(const uint8_t *data, size_t len, uint8_t expected);

int main(void) {
    int passed = 0;
    int num_tests = 12;
    int test_num = 0;

    /* === Test 1: canonical check value "123456789" -> 0xF4 === */
    {
        test_num++;
        const uint8_t d[] = "123456789";
        uint8_t r = crc8(d, 9);
        if (r == 0xF4) { passed++; printf("[PASS] Test %d: crc8(\"123456789\")=0x%02X\n", test_num, r); }
        else printf("[FAIL] Test %d: got 0x%02X (expected 0xF4)\n", test_num, r);
    }

    /* === Test 2: empty buffer -> init 0x00 === */
    {
        test_num++;
        uint8_t r = crc8((const uint8_t *)"", 0);
        if (r == 0x00) { passed++; printf("[PASS] Test %d: crc8(empty)=0x00\n", test_num); }
        else printf("[FAIL] Test %d: got 0x%02X (expected 0x00)\n", test_num, r);
    }

    /* === Test 3: single zero byte -> 0x00 === */
    {
        test_num++;
        uint8_t d[] = {0x00};
        uint8_t r = crc8(d, 1);
        if (r == 0x00) { passed++; printf("[PASS] Test %d: crc8({0x00})=0x00\n", test_num); }
        else printf("[FAIL] Test %d: got 0x%02X (expected 0x00)\n", test_num, r);
    }

    /* === Test 4: single 0x01 byte -> 0x07 (one byte = poly after 8 shifts) === */
    {
        test_num++;
        uint8_t d[] = {0x01};
        uint8_t r = crc8(d, 1);
        /* 0x01 -> after 8 iterations equals the polynomial 0x07 */
        if (r == 0x07) { passed++; printf("[PASS] Test %d: crc8({0x01})=0x%02X\n", test_num, r); }
        else printf("[FAIL] Test %d: got 0x%02X (expected 0x07)\n", test_num, r);
    }

    /* === Test 5: single 0xFF byte -> known value 0xF3 === */
    {
        test_num++;
        uint8_t d[] = {0xFF};
        uint8_t r = crc8(d, 1);
        if (r == 0xF3) { passed++; printf("[PASS] Test %d: crc8({0xFF})=0x%02X\n", test_num, r); }
        else printf("[FAIL] Test %d: got 0x%02X (expected 0xF3)\n", test_num, r);
    }

    /* === Test 6: two bytes {0x00,0x00} -> 0x00 === */
    {
        test_num++;
        uint8_t d[] = {0x00, 0x00};
        uint8_t r = crc8(d, 2);
        if (r == 0x00) { passed++; printf("[PASS] Test %d: crc8({0,0})=0x00\n", test_num); }
        else printf("[FAIL] Test %d: got 0x%02X\n", test_num, r);
    }

    /* === Test 7: verify matches correct CRC === */
    {
        test_num++;
        const uint8_t d[] = "123456789";
        if (crc8_verify(d, 9, 0xF4) == 1) { passed++; printf("[PASS] Test %d: verify accepts correct CRC\n", test_num); }
        else printf("[FAIL] Test %d: verify rejected correct CRC\n", test_num);
    }

    /* === Test 8: verify rejects wrong CRC === */
    {
        test_num++;
        const uint8_t d[] = "123456789";
        if (crc8_verify(d, 9, 0x00) == 0) { passed++; printf("[PASS] Test %d: verify rejects wrong CRC\n", test_num); }
        else printf("[FAIL] Test %d: verify accepted wrong CRC\n", test_num);
    }

    /* === Test 9: single-bit corruption changes CRC === */
    {
        test_num++;
        uint8_t a[] = {0x12, 0x34, 0x56, 0x78};
        uint8_t b[] = {0x12, 0x34, 0x56, 0x79};   /* one bit flipped in last byte */
        uint8_t ca = crc8(a, 4);
        uint8_t cb = crc8(b, 4);
        if (ca != cb) { passed++; printf("[PASS] Test %d: single-bit change alters CRC (0x%02X vs 0x%02X)\n", test_num, ca, cb); }
        else printf("[FAIL] Test %d: CRC unchanged by bit flip (0x%02X)\n", test_num, ca);
    }

    /* === Test 10: order sensitivity (CRC differs on reorder) === */
    {
        test_num++;
        uint8_t a[] = {0x01, 0x02};
        uint8_t b[] = {0x02, 0x01};
        if (crc8(a, 2) != crc8(b, 2)) { passed++; printf("[PASS] Test %d: CRC is order-sensitive\n", test_num); }
        else printf("[FAIL] Test %d: CRC not order-sensitive\n", test_num);
    }

    /* === Test 11: longer known vector {0xDE,0xAD,0xBE,0xEF} === */
    {
        test_num++;
        uint8_t d[] = {0xDE, 0xAD, 0xBE, 0xEF};
        uint8_t r = crc8(d, 4);
        /* Reference value computed with CRC-8/SMBus */
        if (r == 0xCA) { passed++; printf("[PASS] Test %d: crc8({DE,AD,BE,EF})=0x%02X\n", test_num, r); }
        else printf("[FAIL] Test %d: got 0x%02X (expected 0xCA)\n", test_num, r);
    }

    /* === Test 12: all 0xFF x4 known vector === */
    {
        test_num++;
        uint8_t d[] = {0xFF, 0xFF, 0xFF, 0xFF};
        uint8_t r = crc8(d, 4);
        if (r == 0xDE) { passed++; printf("[PASS] Test %d: crc8({FF,FF,FF,FF})=0x%02X\n", test_num, r); }
        else printf("[FAIL] Test %d: got 0x%02X (expected 0xDE)\n", test_num, r);
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);
    return (passed == num_tests) ? 0 : 1;
}
