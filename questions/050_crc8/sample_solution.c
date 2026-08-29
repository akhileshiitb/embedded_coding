#include <stddef.h>
#include <stdint.h>

/*
 * Sample solution — CRC-8/SMBus, bitwise (shift-register) implementation.
 *   poly=0x07, init=0x00, MSB-first, no reflection, XorOut=0x00.
 */

uint8_t crc8(const uint8_t *data, size_t len) {
    uint8_t crc = 0x00;                 /* INIT */
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int bit = 0; bit < 8; bit++) {
            if (crc & 0x80)
                crc = (uint8_t)((crc << 1) ^ 0x07);
            else
                crc = (uint8_t)(crc << 1);
        }
    }
    return crc;                          /* XorOut = 0x00 */
}

int crc8_verify(const uint8_t *data, size_t len, uint8_t expected) {
    return crc8(data, len) == expected ? 1 : 0;
}
