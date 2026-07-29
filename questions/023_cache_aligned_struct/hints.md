# Hints

## Hint 1

- For struct sizes, just use `sizeof()` on the structs defined in solution.c.
- For `compute_padding`, think about what `offset % alignment` tells you.
- For `is_cache_aligned`, cast the pointer to an integer and check if the low 6 bits are zero (64 = 2^6).
- For `get_aligned_offset`, you need the standard "round up to next multiple" formula.

## Hint 2

- `compute_padding`: if `offset % alignment == 0`, padding is 0. Otherwise it's `alignment - (offset % alignment)`.
- `is_cache_aligned`: `((uintptr_t)ptr & 63) == 0` checks if the lowest 6 bits are zero.
- `get_aligned_offset`: use `(current_offset + alignment - 1) & ~(alignment - 1)` — this is the standard "align up" bitmask trick since alignment is always a power of 2.
- Remember: `~(alignment - 1)` creates a mask that clears the lower bits.

## Hint 3

```c
size_t get_padded_size(void)    { return sizeof(struct padded); }
size_t get_optimized_size(void) { return sizeof(struct optimized); }
size_t get_packed_size(void)    { return sizeof(struct packed); }

int compute_padding(int offset, int alignment) {
    int remainder = offset % alignment;
    return (remainder == 0) ? 0 : alignment - remainder;
}

int is_cache_aligned(void *ptr) {
    return ((uintptr_t)ptr & 63) == 0 ? 1 : 0;
}

int get_aligned_offset(int current_offset, int alignment) {
    return (current_offset + alignment - 1) & ~(alignment - 1);
}
```
