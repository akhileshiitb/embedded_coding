# Hints

## Hint 1

- The free list is embedded *inside* the free blocks themselves. Model each block as a `union` so the same storage can be either a "next" pointer (while free) or raw bytes (while allocated).
- `pool_alloc()` pops the head of the free list (O(1)).
- `pool_free()` pushes the block onto the head of the free list (O(1)).
- During `pool_init()`, thread all blocks together into a linked list.

## Hint 2

- Because `BLOCK_SIZE` and `NUM_BLOCKS` are compile-time constants, use `_Static_assert` to enforce the invariants instead of runtime checks:
  ```c
  _Static_assert(BLOCK_SIZE >= sizeof(void *), "BLOCK_SIZE must hold a pointer");
  _Static_assert((BLOCK_SIZE & (BLOCK_SIZE - 1)) == 0, "BLOCK_SIZE must be a power of 2");
  ```
- To validate a pointer in `pool_free` (must belong to the pool):
  1. Reject NULL.
  2. Check it's within `[&pool_memory[0], &pool_memory[NUM_BLOCKS - 1]]`.
  3. Check `((uintptr_t)ptr - start) % BLOCK_SIZE == 0` (on a block boundary).
- For alignment check: `((uintptr_t)ptr & (alignment - 1)) == 0`

## Hint 3

```c
#define BLOCK_SIZE 16
#define NUM_BLOCKS 4

typedef union block {
    union block *next;
    uint8_t      bytes[BLOCK_SIZE];
} block_t;

static _Alignas(BLOCK_SIZE) block_t pool_memory[NUM_BLOCKS];
static block_t *free_list;
static int free_blocks_count;

int pool_init(void) {
    for (int i = 0; i < NUM_BLOCKS - 1; i++)
        pool_memory[i].next = &pool_memory[i + 1];
    pool_memory[NUM_BLOCKS - 1].next = NULL;
    free_list = &pool_memory[0];
    free_blocks_count = NUM_BLOCKS;
    return 0;
}

void *pool_alloc(void) {
    if (free_list == NULL) return NULL;
    block_t *blk = free_list;
    free_list = blk->next;
    free_blocks_count--;
    return blk;
}

int pool_free(void *ptr) {
    if (ptr == NULL) return -1;
    uintptr_t p     = (uintptr_t)ptr;
    uintptr_t start = (uintptr_t)&pool_memory[0];
    uintptr_t end   = (uintptr_t)&pool_memory[NUM_BLOCKS - 1];
    if (p < start || p > end || (p - start) % BLOCK_SIZE != 0)
        return -1;
    block_t *blk = (block_t *)ptr;
    blk->next = free_list;
    free_list = blk;
    free_blocks_count++;
    return 0;
}
```
