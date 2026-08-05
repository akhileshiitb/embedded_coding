# Hints

## Hint 1

Start with the simplest correct solution: cast both pointers to `uint8_t *` and copy byte-by-byte in a loop. Don't forget to save `dest` before incrementing — you need to return the original pointer.

## Hint 2

To optimize, check if both `src` and `dest` are 4-byte aligned using `(uintptr_t)ptr & 0x3`. If they are, you can copy 4 bytes at a time by casting to `uint32_t *`. Handle any leading unaligned bytes first, then the aligned bulk, then trailing bytes.

## Hint 3

```c
uint8_t *d = dest; const uint8_t *s = src;
/* Byte-copy until both are 4-byte aligned */
while (n && ((uintptr_t)d & 0x3)) { *d++ = *s++; n--; }
/* Word-copy the aligned middle */
uint32_t *dw = (uint32_t *)d; const uint32_t *sw = (const uint32_t *)s;
while (n >= 4) { *dw++ = *sw++; n -= 4; }
/* Byte-copy trailing remainder */
d = (uint8_t *)dw; s = (const uint8_t *)sw;
while (n--) { *d++ = *s++; }
return dest;
```
Note: word-copy is only safe when BOTH pointers are aligned. If only dest becomes aligned but src isn't, fall back to byte copy (or accept unaligned access on platforms that support it).
