# 025. Fixed-Block Pool Allocator

## Difficulty: Medium

## Problem Statement

Implement a fixed-block memory pool allocator — the standard approach to dynamic memory management in embedded systems where `malloc()`/`free()` is too unpredictable (heap fragmentation, non-deterministic timing).

A pool allocator pre-allocates a contiguous block of memory and divides it into fixed-size blocks. A free list (embedded within the unused blocks themselves) tracks which blocks are available. Allocation and deallocation are both O(1).

In this version, the block size and block count are **compile-time constants** (`BLOCK_SIZE` and `NUM_BLOCKS`), not runtime parameters. This lets you model each block as a `union` — either a free-list link or raw storage — and keeps the implementation simple and fully static.

You must implement:
1. `pool_init(void)` — Thread all blocks into the free list.
2. `pool_alloc()` — Allocate one block from the pool. Returns pointer or NULL if exhausted.
3. `pool_free(void *ptr)` — Return a block to the pool.
4. `pool_available()` — Return the number of free blocks remaining.
5. `pool_is_aligned(void *ptr, int alignment)` — Check if the returned pointer meets the given alignment.

## Function Signatures

```c
#include <stddef.h>
#include <stdint.h>

#define BLOCK_SIZE 16   /* power of 2, >= sizeof(void *) */
#define NUM_BLOCKS 4    /* >= 1 */

int   pool_init(void);
void *pool_alloc(void);
int   pool_free(void *ptr);
int   pool_available(void);
int   pool_is_aligned(void *ptr, int alignment);
```

## Return Values

- `pool_init`: 0 on success
- `pool_alloc`: pointer to allocated block, or NULL if pool exhausted
- `pool_free`: 0 on success, -1 if ptr is NULL or not from this pool
- `pool_available`: number of free blocks (0 to NUM_BLOCKS)
- `pool_is_aligned`: 1 if ptr is aligned to the given boundary, 0 otherwise

## Examples

```
pool_init();             // thread the free list
pool_available();        // → 4

void *a = pool_alloc(); // → valid pointer
void *b = pool_alloc(); // → valid pointer
pool_available();        // → 2

pool_free(a);            // → 0 (success)
pool_available();        // → 3

pool_free((void*)0x1);  // → -1 (not in pool)
```

## Constraints

- `BLOCK_SIZE` must be a power of 2 and >= `sizeof(void *)` (to hold the free-list pointer). Enforce with `_Static_assert`.
- `NUM_BLOCKS` must be >= 1.
- Freeing NULL or a pointer not from the pool returns -1.
- Double-free is undefined (not required to detect, but must not crash).
- Blocks returned by `pool_alloc` must be aligned to `BLOCK_SIZE`.

## Notes

- **Union block trick**: Model a block as
  ```c
  typedef union block {
      union block *next;        /* used while the block is free   */
      uint8_t bytes[BLOCK_SIZE]; /* used while the block is in use */
  } block_t;
  ```
  Only one member is live at a time: `next` while the block sits on the free list, `bytes` once the caller owns it. This is why no extra metadata memory is needed.
- **O(1) alloc/free**: `pool_alloc` pops the head of the free list. `pool_free` pushes onto the head.
- **Deterministic**: Unlike `malloc`, timing is always constant regardless of allocation history.
- **Alignment guarantee**: `BLOCK_SIZE` is a power of 2 and the backing array is `_Alignas(BLOCK_SIZE)`, so all blocks are naturally aligned.
- This is the pattern used in RTOS kernels (FreeRTOS memory pools), network stacks (packet buffer pools), and DMA descriptor rings.
