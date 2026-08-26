# Hints

The hints below progress from a **working-but-less-optimal** approach to the **most idiomatic/optimal** one. Each is correct — but they differ in readability, safety, and portability.

## Hint 1 — Concept

Store a known multi-byte value (like the 32-bit integer `1`) in memory, then look at the **first byte** (lowest address). If that byte is `1`, the least-significant byte came first → little-endian. If it's `0`, the most-significant byte came first → big-endian. You just need a way to inspect an individual byte of a multi-byte value.

## Hint 2 — Approach 1: Pointer Cast (works, but strict-aliasing caveats)

The most direct approach: take the address of an `int`, cast it to a `char *`, and read the first byte.

```c
int is_little_endian(void) {
    int x = 1;
    char *p = (char *)&x;    /* point at the first byte of x */
    return (*p == 1) ? 1 : 0;
}
```

**Why it's less-optimal:** casting between unrelated pointer types can raise eyebrows about strict aliasing. It happens to be safe here because `char *` is explicitly allowed to alias any type — but it reads as a "hack" and relies on `sizeof(int) >= 2`. Some MISRA-C rules flag pointer casts.

## Hint 3 — Approach 2: Union (cleaner, most idiomatic)

A `union` lets you view the same memory as either a 32-bit integer or an array of bytes — no pointer casting needed. This is the classic, readable, self-documenting solution:

```c
int is_little_endian(void) {
    union {
        uint32_t value;
        uint8_t  bytes[4];
    } probe;

    probe.value = 1;          /* store 0x00000001 */
    return (probe.bytes[0] == 1) ? 1 : 0;
}
```

**Why it's the most optimal (runtime) choice:**
- No pointer-cast / strict-aliasing concerns (type punning through a union is well-defined in C)
- Self-documenting: the intent ("view these bytes") is explicit
- Compiles to a couple of instructions at `-O2` (often just a constant on a known target)
- Fixed-width types make the byte layout unambiguous

### Bonus — Compile-Time (Zero Runtime Cost)

If you know your compiler and don't need runtime detection, the truly optimal answer costs **zero** cycles because it resolves at compile time:

```c
int is_little_endian(void) {
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    return 1;
#else
    return 0;
#endif
}
```

The problem asks for runtime detection, so submit the **union** version (Hint 3). But mentioning the compile-time macro in an interview shows depth — it's what you'd actually use in production firmware where the target is fixed.
