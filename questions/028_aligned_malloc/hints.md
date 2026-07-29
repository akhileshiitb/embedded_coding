# Hints

## Hint 1

You can't control where `malloc()` returns memory, but you *can* allocate more than you need and pick an aligned address within that block. The trick is: how much extra do you need to guarantee you can always find an aligned boundary? And how will `aligned_free()` know the original pointer to pass to `free()`?

## Hint 2

Allocate `size + alignment - 1 + sizeof(void*)` bytes. The `alignment - 1` extra bytes guarantee there's an aligned boundary somewhere in the block. The `sizeof(void*)` reserves space to stash the original `malloc()` pointer. To align a pointer `p` upward: use the bitmask trick `(p + alignment - 1) & ~(alignment - 1)`. Make sure you leave room before the aligned pointer to store the original address.

## Hint 3

```c
void *aligned_malloc(size_t size, size_t alignment) {
    // Validate: size > 0, alignment is power of 2 and >= sizeof(void*)
    if (size == 0 || alignment < sizeof(void*) || (alignment & (alignment - 1)) != 0)
        return NULL;

    // Over-allocate
    void *raw = malloc(size + alignment - 1 + sizeof(void *));
    if (!raw) return NULL;

    // Compute aligned address (leave sizeof(void*) room for storing raw ptr)
    uintptr_t raw_addr = (uintptr_t)raw + sizeof(void *);
    uintptr_t aligned_addr = (raw_addr + alignment - 1) & ~(alignment - 1);

    // Store original pointer just before the aligned pointer
    ((void **)aligned_addr)[-1] = raw;

    return (void *)aligned_addr;
}
```

For `aligned_free`: read back `((void **)ptr)[-1]` and pass it to `free()`.
