# 038. Implement sizeof() Using Pointer Arithmetic

## Difficulty: Medium

## Problem Statement

Implement a macro `MY_SIZEOF(type)` that computes the size (in bytes) of a given type using **only pointer arithmetic** — without using the built-in `sizeof` operator.

The trick: if you cast `0` (null pointer) to a pointer-to-type, then advance it by 1 element, the resulting pointer value equals the size of that type in bytes (since pointer arithmetic moves in units of the pointed-to type).

Additionally, implement `MY_SIZEOF_VAR(var)` to compute the size of a variable.

## ⚠️ Embedded Pitfalls & Discussion Points

### 1. sizeof is a Compile-Time Operator (Not a Function!)

`sizeof` is evaluated **at compile time** by the compiler (except for VLAs in C99+). It never generates runtime code. Your `MY_SIZEOF` macro is a clever trick that also resolves at compile time (the compiler constant-folds the pointer arithmetic). But understanding *why* sizeof works this way is critical:

- **No runtime cost**: `sizeof(struct sensor_reading)` in a tight ISR loop costs zero cycles
- **Array sizing**: `#define BUFFER_COUNT (BUFFER_SIZE / sizeof(element_t))` — computed by the compiler, not at runtime
- **Static assertions**: `_Static_assert(sizeof(struct packet) == 64, "packet size mismatch")` — catches struct layout bugs at build time

### 2. Struct Padding and Alignment

`sizeof(struct)` is NOT the sum of member sizes. The compiler inserts **padding bytes** for alignment:

```c
struct example {
    char a;      // 1 byte + 3 bytes padding
    int32_t b;   // 4 bytes (aligned to 4-byte boundary)
    char c;      // 1 byte + 3 bytes padding (struct size rounded up)
};
// sizeof = 12, NOT 6!
```

In embedded systems, this matters for:
- **Memory-mapped hardware registers**: Struct layout must match register map exactly — use `__attribute__((packed))` or manual padding
- **Network protocol packets**: Padding ruins wire format — must pack or serialize manually
- **DMA buffer sizing**: DMA transfer size must match actual struct size including padding
- **Flash storage**: Wasting padding bytes in EEPROM/flash configs burns precious space

### 3. Pointer Arithmetic Fundamentals

When you increment a `T*` pointer by 1, the actual address advances by `sizeof(T)` bytes:

```c
int *p = (int *)0;
int *q = p + 1;      // q == 4 (on 32-bit int platforms)
// (char *)q - (char *)p == sizeof(int)
```

This is the foundation of array indexing: `arr[i]` is defined as `*(arr + i)`, which moves `i * sizeof(*arr)` bytes forward.

### 4. Why This Matters on Different Architectures

| Architecture | char | short | int | long | pointer |
|-------------|------|-------|-----|------|---------|
| ARM Cortex-M (32-bit) | 1 | 2 | 4 | 4 | 4 |
| x86_64 | 1 | 2 | 4 | 8 | 8 |
| AVR (8-bit) | 1 | 2 | 2 | 4 | 2 |
| MSP430 (16-bit) | 1 | 2 | 2 | 4 | 2 |

Code that assumes `sizeof(int) == 4` breaks on 8/16-bit MCUs. Always use `<stdint.h>` fixed-width types in firmware.

### 5. The `__attribute__((packed))` Trap

On ARM, accessing a misaligned 32-bit value causes a **HardFault** (or silent data corruption with unaligned access enabled). Packed structs remove padding but create misaligned accesses:

```c
struct __attribute__((packed)) sensor_packet {
    uint8_t  id;        // offset 0
    uint32_t timestamp; // offset 1 — MISALIGNED on ARM!
    uint16_t value;     // offset 5 — MISALIGNED!
};
```

The compiler generates byte-by-byte access for packed members (slower), or you get faults if `-mno-unaligned-access` is set.

## Function Signature
```c
/* Macro to compute size of a type (in bytes) */
#define MY_SIZEOF(type)  /* your implementation */

/* Macro to compute size of a variable (in bytes) */
#define MY_SIZEOF_VAR(var)  /* your implementation */

/* Function wrappers for testing */
size_t sizeof_char(void);
size_t sizeof_short(void);
size_t sizeof_int(void);
size_t sizeof_long(void);
size_t sizeof_float(void);
size_t sizeof_double(void);
size_t sizeof_pointer(void);
size_t sizeof_int32(void);
size_t sizeof_int64(void);
size_t sizeof_struct_padded(void);
size_t sizeof_struct_packed(void);
size_t sizeof_array(void);
```

## Parameters
The macros take a type name or variable. The test wrapper functions take no parameters.

## Return Value
Each function returns the size in bytes of the specified type, computed using the `MY_SIZEOF` macro (not the built-in `sizeof`).

## Examples

### Example 1
```
MY_SIZEOF(int) → 4 (on 32-bit and 64-bit platforms)
Explanation: (size_t)((int *)0 + 1) == 4
```

### Example 2
```
MY_SIZEOF(char) → 1 (always, by definition)
Explanation: sizeof(char) is guaranteed to be 1 by the C standard.
```

### Example 3
```
struct padded { char a; int b; char c; };
MY_SIZEOF(struct padded) → 12 (typically, with 4-byte int alignment)
Explanation: 1 + 3(pad) + 4 + 1 + 3(pad) = 12. Struct size is rounded
             up to alignment of its largest member.
```

### Example 4
```
int arr[10];
MY_SIZEOF_VAR(arr) → 40 (10 * 4 bytes)
Explanation: The macro computes the total array size, not just pointer size.
```

## Constraints
- Must NOT use the `sizeof` operator anywhere in your macro implementation
- Must work for primitive types, structs, arrays, and pointers
- `MY_SIZEOF(char)` must return 1
- Results must match the built-in `sizeof` for all test cases
- Implementation should be a compile-time constant expression

## Notes

### Interview Discussion Points:
1. **"Is sizeof a function or operator?"** → Operator. Evaluated at compile time. No parentheses needed for variables: `sizeof x` is valid (but `sizeof(int)` needs parens because `int` is a type, not an expression).
2. **"When is sizeof NOT compile-time?"** → C99 VLAs: `int arr[n]; sizeof(arr)` is evaluated at runtime because `n` isn't known at compile time.
3. **"What does sizeof return for an array vs a pointer?"** → Array: total size in bytes. Pointer: size of the pointer itself (4 or 8). This distinction is lost when arrays decay to pointers (function parameters).
4. **"How do you compute array element count?"** → `#define ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))` — but this silently gives wrong results if `a` has decayed to a pointer.
5. **"What's the alignment of a struct?"** → The alignment of its most-aligned member. `sizeof(struct)` is always a multiple of this alignment (trailing padding is added).
