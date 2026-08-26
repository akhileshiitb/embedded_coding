# 044. Check System Endianness

## Difficulty: Easy

## Problem Statement

Write a function that determines whether the CPU running the code is **little-endian** or **big-endian** at runtime.

- **Little-endian**: the least-significant byte is stored at the lowest memory address (x86, most ARM, RISC-V)
- **Big-endian**: the most-significant byte is stored at the lowest memory address (network byte order, some MIPS/PowerPC, older SPARC)

Return `1` if the system is little-endian, `0` if big-endian.

## Function Signature
```c
int is_little_endian(void);
```

## Parameters
None.

## Return Value
- `1` if the system is little-endian
- `0` if the system is big-endian

## Examples

### Example 1 (running on x86 or ARM little-endian)
```
is_little_endian() → 1
Explanation: Store the 32-bit value 0x00000001 in memory. On a little-endian
             machine, the byte at the lowest address is 0x01. Reading the first
             byte gives 1, so the function returns 1.
```

### Example 2 (running on a big-endian machine)
```
is_little_endian() → 0
Explanation: For the value 0x00000001, the byte at the lowest address is 0x00
             (the most-significant byte). Reading the first byte gives 0.
```

## How Endianness Works

Consider storing the 32-bit integer `0x01020304` at memory address `A`:

| Address | Little-Endian | Big-Endian |
|---------|---------------|------------|
| A+0     | 0x04          | 0x01       |
| A+1     | 0x03          | 0x02       |
| A+2     | 0x02          | 0x03       |
| A+3     | 0x01          | 0x04       |

Little-endian stores the "little end" (least-significant byte) first. Big-endian stores the "big end" (most-significant byte) first. The trick to detect this is to store a known multi-byte value and inspect the byte at the lowest address.

## Constraints
- Must determine endianness at **runtime** (not using compiler macros like `__BYTE_ORDER__`)
- Must work on both 32-bit and 64-bit platforms
- No standard library calls required
- The function takes no arguments and returns an int

## Notes

### Why Endianness Matters in Embedded Systems:

1. **Network byte order**: TCP/IP protocols use big-endian ("network byte order"). A little-endian device (most ARM MCUs) must byte-swap multi-byte fields before transmitting. Functions like `htons()`/`htonl()` handle this — but on bare metal you often implement them yourself.

2. **Cross-platform data exchange**: When two devices with different endianness share binary data (over UART, SPI, CAN, or shared memory), the receiver may interpret multi-byte values incorrectly unless a byte-order convention is agreed upon.

3. **Memory-mapped peripherals**: Some peripherals expose registers in a fixed byte order that may differ from the CPU. Reading a 32-bit register into a struct can produce wrong values without byte-swapping.

4. **File formats and flash storage**: Binary file formats (ELF, PNG, network captures) specify a byte order. Firmware reading/writing these must match.

5. **Serialization bugs**: The classic bug — writing a `uint32_t` to flash on a little-endian device, then reading it on a big-endian device (or vice versa) — produces garbage. Always serialize with an explicit byte order.

### Interview Discussion Points:
1. **"How many ways can you detect endianness?"** → union trick, pointer cast, and (compile-time) preprocessor macros. Be ready to discuss trade-offs.
2. **"Which method is most idiomatic?"** → The union of `uint32_t` and `uint8_t[4]` is the classic, readable approach and avoids strict-aliasing concerns.
3. **"Can this be done at compile time?"** → Yes, with `__BYTE_ORDER__` / `__ORDER_LITTLE_ENDIAN__` (GCC/Clang) — zero runtime cost. But the problem asks for runtime detection.
4. **"What about mixed/middle endian?"** → Rare historically (PDP-11). Modern systems are pure little or big endian. You can ignore mixed-endian for this problem.
5. **"Does `char*` casting violate strict aliasing?"** → No — `char*` (and `unsigned char*`) is explicitly allowed to alias any type per the C standard. This is why the pointer-cast method is safe.
