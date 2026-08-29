#include <stddef.h>
#include <stdint.h>

/*
 * CRC-8/SMBus.
 *   Width 8, polynomial 0x07, init 0x00, MSB-first (no reflection), XorOut 0x00.
 *   Canonical check: crc8("123456789", 9) == 0xF4.
 *
 * You choose the internal approach (bitwise shift-and-XOR, or a 256-entry
 * table). See crc_design.md for both.
 */

/* Compute the CRC-8/SMBus of `len` bytes at `data`. len==0 returns 0x00. */
uint8_t crc8(const uint8_t *data, size_t len) {
    /* TODO: Implement */
    (void)data;
    (void)len;
    return 0;
}

/* Return 1 if crc8(data, len) == expected, else 0. */
int crc8_verify(const uint8_t *data, size_t len, uint8_t expected) {
    /* TODO: Implement */
    (void)data;
    (void)len;
    (void)expected;
    return 0;
}
