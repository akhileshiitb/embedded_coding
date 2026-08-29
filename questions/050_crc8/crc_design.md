# CRC (Cyclic Redundancy Check) — Design Concepts

A reference for CRC error detection in embedded systems, focused on CRC-8. Read this before the
coding question.

---

## 1. What Problem Does CRC Solve?

When you send bytes over a noisy channel (UART, SPI, I2C, RF, CAN) or store them in flash, bits
can flip. You need to **detect** corruption cheaply at the receiver. A **CRC** is a small check
value (8, 16, or 32 bits) computed from the data. The sender appends it; the receiver recomputes
it and compares. If they differ, the data is corrupt.

CRC is **error-detecting, not error-correcting** — it tells you something is wrong, not how to fix
it. It's far stronger than a simple checksum (sum of bytes): a CRC catches all single-bit errors,
all double-bit errors (for a good polynomial), any odd number of bit errors, and all burst errors
shorter than the CRC width.

---

## 2. The Core Idea — Polynomial Division

CRC treats the message as a big binary number and computes the **remainder** when dividing it by a
fixed **generator polynomial**, using **carry-less (XOR) arithmetic** (called GF(2) — binary
field arithmetic). The remainder is the CRC.

- "Polynomial" `0x07` means the bit pattern of the divisor. For CRC-8-SMBus the polynomial is
  `x^8 + x^2 + x + 1` → binary `1_0000_0111`; the low 8 bits `0000_0111` = `0x07` are what you XOR
  with (the top `x^8` bit is implicit).
- Division in GF(2) is just **shift and XOR**: no borrows, no carries. Subtraction = XOR.

You don't need the deep algebra to implement it — the algorithm below is purely shifts and XORs.

---

## 3. The Bitwise Algorithm (Shift-Register Simulation)

This mirrors how CRC hardware works — a shift register with XOR taps at the polynomial's set bits:

```
crc = INIT
for each byte b in data:
    crc ^= b                       # bring the byte into the register (MSB-first variant)
    repeat 8 times:                # one iteration per bit
        if (crc & 0x80):           # top bit set?
            crc = (crc << 1) ^ POLY
        else:
            crc = (crc << 1)
    crc &= 0xFF                    # keep it 8-bit
return crc ^ XOROUT
```

For **CRC-8-SMBus**: `POLY = 0x07`, `INIT = 0x00`, `XOROUT = 0x00`, MSB-first (no reflection).
Each bit: shift left; if the bit shifted out was 1, XOR in the polynomial. That's the whole thing.

Time: O(8 × N) — 8 iterations per byte. Fine for small messages.

---

## 4. The Table-Driven Algorithm (Faster)

Processing one bit at a time is slow. You can **precompute** the 8-bit CRC of every possible byte
(0..255) into a 256-entry table, then process one **byte** per step:

```
crc = INIT
for each byte b in data:
    crc = table[crc ^ b]           # (MSB-first, width 8)
return crc ^ XOROUT
```

- **Trade-off**: 256 bytes of flash/RAM for the table vs. ~8× faster computation.
- The table itself is generated once by running the bitwise algorithm on each of 0..255.
- Larger CRCs (CRC-16/32) use a 256-entry table of 16/32-bit values (512 B / 1 KB).

On tiny MCUs you might keep the bitwise version to save flash; on faster parts you use the table.

---

## 5. CRC Parameters (Why There Are So Many "CRC-8"s)

A CRC is fully specified by a handful of parameters ("Rocksoft model"):
- **Width** — 8, 16, 32 bits.
- **Polynomial** — the generator (e.g., CRC-8-SMBus `0x07`, CRC-8-CCITT `0x8D`).
- **Init** — starting register value (0x00 or 0xFF are common).
- **RefIn / RefOut** — whether input bytes and the output are **bit-reflected** (LSB-first). Some
  standards process bits LSB-first to match serial hardware.
- **XorOut** — a final value XORed with the result.

Two implementations that use different parameters produce **different** CRCs for the same data.
This is a classic bug: sender and receiver must agree on **all** parameters. Always cite the exact
CRC variant (e.g., "CRC-8/SMBus"), not just "CRC-8".

---

## 6. Why Not Just a Checksum?

A simple additive checksum (sum bytes mod 256) is cheap but weak: it misses byte-reordering, many
multi-bit errors, and can't catch a `+1/-1` pair. CRC's polynomial division spreads each bit's
influence across the whole check value, giving vastly better error coverage for a few extra XORs.

---

## 7. Verifying at the Receiver

Two equivalent styles:
- **Compute-and-compare**: receiver computes CRC over the payload and checks it equals the received
  CRC byte.
- **Compute-over-all**: compute the CRC over payload **plus** the received CRC; for many CRC
  definitions the result is a fixed constant (often 0) when there's no error. Convenient in
  hardware.

This question uses compute-and-compare (compute over the payload, compare to the expected byte).

---

## 8. Follow-Up Topics (Umbrella)

- **Table generation at build/boot**: generate the 256-entry table with the bitwise routine at
  startup (saves flash) vs. hardcoding a const table (saves boot time).
- **Reflected CRCs**: CRC-8/MAXIM, CRC-16/MODBUS, CRC-32 (Ethernet/zlib) reflect input and output;
  implement via reflected shift (`>>` and test the low bit) or reflected tables.
- **CRC-16 / CRC-32**: same shift-and-XOR structure at wider widths; CRC-32 is the workhorse for
  Ethernet, ZIP, and flash images.
- **Hardware CRC units**: many MCUs (STM32, etc.) have a CRC peripheral — you feed bytes and read
  the result, offloading the CPU.
- **Incremental / streaming CRC**: keep the running `crc` across calls so you can CRC a stream in
  chunks without buffering the whole message.
- **Not for security**: CRC detects *accidental* corruption. It is **not** a cryptographic hash —
  an attacker can forge data with a matching CRC. Use a MAC/hash for integrity against tampering.
