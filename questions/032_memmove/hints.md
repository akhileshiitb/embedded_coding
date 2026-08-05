# Hints

## Hint 1

The key difference from `memcpy` is overlap safety. If you always copy forward (low to high), what goes wrong when `dest` is just past `src`? You overwrite source bytes before reading them. You need to detect this case and copy in the opposite direction.

## Hint 2

Compare `dest` and `src` as pointers:
- If `dest <= src` (or no overlap): copy **forward** from index 0 to n-1.
- If `dest > src`: copy **backward** from index n-1 down to 0.

This ensures you never overwrite a source byte before you've read it.

## Hint 3

```c
uint8_t *d = (uint8_t *)dest;
const uint8_t *s = (const uint8_t *)src;
if (d <= s || d >= s + n) {
    /* Forward copy (no overlap or dest before src) */
    for (size_t i = 0; i < n; i++) d[i] = s[i];
} else {
    /* Backward copy (dest overlaps into src region) */
    for (size_t i = n; i > 0; i--) d[i-1] = s[i-1];
}
return dest;
```
The `d >= s + n` check handles the case where dest is completely past src (no overlap at all) — forward is fine there too.
