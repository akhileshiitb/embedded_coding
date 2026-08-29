# 050. CRC-8 (SMBus)

## Difficulty: Medium

## Problem Statement

Implement **CRC-8/SMBus** — an 8-bit cyclic redundancy check used to detect corruption in
serial data (SMBus, I2C PMBus, many sensor protocols). Given a byte buffer, compute its 8-bit
CRC. Also implement a verify helper.

> **Read `crc_design.md` first.** It explains what a CRC is, the shift-and-XOR (polynomial
> division) algorithm, the table-driven variant, and why CRC parameters matter.

You **design the internal approach yourself** (bitwise or table-driven) — `solution.c` has plain
function stubs only.

---

## CRC-8/SMBus Parameters

| Parameter | Value |
|-----------|-------|
| Width     | 8 bits |
| Polynomial| `0x07`  (x^8 + x^2 + x + 1) |
| Init      | `0x00` |
| RefIn     | false (MSB-first, no bit reflection) |
| RefOut    | false |
| XorOut    | `0x00` |

A well-known **check value**: the CRC-8/SMBus of the ASCII string `"123456789"` (9 bytes) is
`0xF4`. Your implementation must reproduce this.

---

## What You Must Implement

```c
#include <stddef.h>
#include <stdint.h>

/* Compute the CRC-8/SMBus of `len` bytes starting at `data`. */
uint8_t crc8(const uint8_t *data, size_t len);

/* Return 1 if crc8(data, len) equals `expected`, else 0. */
int crc8_verify(const uint8_t *data, size_t len, uint8_t expected);
```

---

## Behavior Specification

### `crc8(const uint8_t *data, size_t len)`
Compute the CRC-8/SMBus over the `len` bytes. MSB-first, polynomial `0x07`, init `0x00`, no
reflection, no final XOR. For `len == 0`, the result is the init value (`0x00`).

The standard bitwise procedure (you may also use a table):
```
crc = 0x00
for each byte b:
    crc ^= b
    repeat 8 times:
        if (crc & 0x80) crc = (uint8_t)((crc << 1) ^ 0x07);
        else            crc = (uint8_t)(crc << 1);
return crc
```

### `crc8_verify(const uint8_t *data, size_t len, uint8_t expected)`
Return `1` if `crc8(data, len) == expected`, else `0`.

---

## Examples

```
crc8("123456789", 9)  -> 0xF4      // canonical CRC-8/SMBus check value
crc8("", 0)           -> 0x00      // empty -> init value
crc8({0x00}, 1)       -> 0x00      // single zero byte
crc8_verify("123456789", 9, 0xF4) -> 1
crc8_verify("123456789", 9, 0x00) -> 0
```

## Constraints
- CRC-8/SMBus exactly: poly `0x07`, init `0x00`, MSB-first, no reflection, XorOut `0x00`.
- `data` may be any bytes (0x00..0xFF); `len` may be 0.
- Must keep the running CRC to 8 bits.
- Pure C, `<stdint.h>` / `<stddef.h>` only. Bitwise or table-driven — your choice.

## Notes

### Interview Discussion Points (see `crc_design.md`):
1. **Bitwise vs. table-driven?** Bitwise: O(8N), no memory. Table: O(N), costs 256 bytes. Pick by
   flash budget vs. throughput.
2. **Why do CRC parameters matter?** Different poly/init/reflection/XorOut give different results;
   sender and receiver must agree on the exact variant.
3. **CRC vs. checksum?** CRC's polynomial division spreads each bit across the result, catching far
   more error patterns (all single/double-bit, odd counts, short bursts) than an additive checksum.
4. **Is CRC security?** No — it detects accidental corruption only; it's not a cryptographic hash.
5. **Scaling to CRC-16/32?** Same shift-and-XOR structure at wider widths; reflected variants shift
   the other direction.
