# Hints

## Hint 1

- The free list is embedded *inside* the free blocks themselves — each free block's first `sizeof(void*)` bytes hold a pointer to the next free block.
- `pool_alloc()` pops the head of the free list (O(1)).
- `pool_free()` pushes the block onto the head of the free list (O(1)).
- During `pool_init()`, thread all blocks together into a linked list.

## Hint 2

- Use a `union` or cast to embed the next pointer: `*(void **)block = next_block;`
- To validate a pointer belongs to the pool:
  1. Check it's >= pool_memory start
  2. Check it's < pool_memory + (block_size * num_blocks)
  3. Check `(ptr - start) % block_size == 0` (on a block boundary)
- For alignment check: `((uintptr_t)ptr & (alignment - 1)) == 0`

## Hint 3

```c
int pool_init(int block_size, int num_blocks) {
    if (block_size < 8 || (block_size & (block_size - 1)) != 0) return -1;
    if (block_size > POOL_MAX_BLOCK_SIZE) return -1;
    if (num_blocks < 1 || num_blocks > POOL_MAX_BLOCKS) return -1;

    pool_block_size = block_size;
    pool_num_blocks = num_blocks;

    /* Thread free list */
    free_list = (void *)pool_memory;
    for (int i = 0; i < num_blocks - 1; i++) {
        uint8_t *current = pool_memory + (i * block_size);
        uint8_t *next = current + block_size;
        *(void **)current = (void *)next;
    }
    uint8_t *last = pool_memory + ((num_blocks - 1) * block_size);
    *(void **)last = NULL;
    return 0;
}

void *pool_alloc(void) {
    if (free_list == NULL) return NULL;
    void *block = free_list;
    free_list = *(void **)block;
    return block;
}

int pool_free(void *ptr) {
    if (ptr == NULL || !pool_contains(ptr)) return -1;
    *(void **)ptr = free_list;
    free_list = ptr;
    return 0;
}
```
