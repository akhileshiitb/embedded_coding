# Hints

## Hint 1

CRC-8/SMBus is "shift-and-XOR." Start `crc = 0x00`. For each data byte, bring it into the CRC
register (`crc ^= byte`), then process 8 bits: each bit, shift `crc` left by 1; if the bit that
shifted out (the old top bit, `crc & 0x80`) was 1, XOR the polynomial `0x07` back in. Keep `crc`
to 8 bits with a `uint8_t` (or `& 0xFF`).

## Hint 2

The inner per-bit loop is the whole algorithm:
```c
if (crc & 0x80) crc = (uint8_t)((crc << 1) ^ 0x07);
else            crc = (uint8_t)(crc << 1);
```
- `crc & 0x80` tests the top bit *before* shifting (that's the bit leaving the 8-bit register).
- The cast to `uint8_t` (or `& 0xFF`) discards bit 8 so the register stays 8 bits.
- `len == 0` never enters the loop, so it correctly returns the init value `0x00`.

`crc8_verify` is just `return crc8(data, len) == expected;`.

## Hint 3

```c
#include <stddef.h>
#include <stdint.h>

uint8_t crc8(const uint8_t *data, size_t len) {
    uint8_t crc = 0x00;                       /* INIT */
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int bit = 0; bit < 8; bit++) {
            if (crc & 0x80) crc = (uint8_t)((crc << 1) ^ 0x07);
            else            crc = (uint8_t)(crc << 1);
        }
    }
    return crc;                                /* XorOut = 0x00 */
}

int crc8_verify(const uint8_t *data, size_t len, uint8_t expected) {
    return crc8(data, len) == expected ? 1 : 0;
}
```

**Faster (table-driven) alternative** — precompute the CRC of every byte once, then process a
whole byte per step:
```c
static uint8_t table[256];
static void crc8_build_table(void) {
    for (int i = 0; i < 256; i++) {
        uint8_t c = (uint8_t)i;
        for (int b = 0; b < 8; b++)
            c = (c & 0x80) ? (uint8_t)((c << 1) ^ 0x07) : (uint8_t)(c << 1);
        table[i] = c;
    }
}
/* then: crc = table[crc ^ byte]; per data byte */
```
Trade-off: 256 bytes of memory for ~8× speed. For an interview, the bitwise version is the
expected answer; mention the table as the optimization.

**Sanity check:** `crc8("123456789", 9)` must equal `0xF4` — the canonical CRC-8/SMBus test vector.
