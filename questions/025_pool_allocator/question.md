# 025. Fixed-Block Pool Allocator

## Difficulty: Hard

## Problem Statement

Implement a fixed-block memory pool allocator — the standard approach to dynamic memory management in embedded systems where `malloc()`/`free()` is too unpredictable (heap fragmentation, non-deterministic timing).

A pool allocator pre-allocates a contiguous block of memory and divides it into fixed-size blocks. A free list (embedded within the unused blocks themselves) tracks which blocks are available. Allocation and deallocation are both O(1).

You must implement:
1. `pool_init(int block_size, int num_blocks)` — Initialize the pool with the given block size and count.
2. `pool_alloc()` — Allocate one block from the pool. Returns pointer or NULL if exhausted.
3. `pool_free(void *ptr)` — Return a block to the pool.
4. `pool_available()` — Return the number of free blocks remaining.
5. `pool_contains(void *ptr)` — Check if a pointer belongs to this pool.
6. `pool_is_aligned(void *ptr, int alignment)` — Check if the returned pointer meets the given alignment.

## Function Signatures

```c
#include <stddef.h>
#include <stdint.h>

#define POOL_MAX_BLOCKS 64
#define POOL_MAX_BLOCK_SIZE 256

int   pool_init(int block_size, int num_blocks);
void *pool_alloc(void);
int   pool_free(void *ptr);
int   pool_available(void);
int   pool_contains(void *ptr);
int   pool_is_aligned(void *ptr, int alignment);
```

## Parameters

### pool_init
| Parameter    | Type  | Description                                             |
|--------------|-------|---------------------------------------------------------|
| `block_size` | `int` | Size of each block in bytes (>= 8, must be power of 2) |
| `num_blocks` | `int` | Number of blocks in the pool (1 to POOL_MAX_BLOCKS)     |

### pool_free
| Parameter | Type     | Description                            |
|-----------|----------|----------------------------------------|
| `ptr`     | `void *` | Pointer to a previously allocated block |

### pool_contains
| Parameter | Type     | Description                    |
|-----------|----------|--------------------------------|
| `ptr`     | `void *` | Pointer to check               |

### pool_is_aligned
| Parameter   | Type     | Description                   |
|-------------|----------|-------------------------------|
| `ptr`       | `void *` | Pointer to check              |
| `alignment` | `int`    | Required alignment (power of 2) |

## Return Values

- `pool_init`: 0 on success, -1 if parameters invalid
- `pool_alloc`: pointer to allocated block, or NULL if pool exhausted
- `pool_free`: 0 on success, -1 if ptr is NULL or not from this pool
- `pool_available`: number of free blocks (0 to num_blocks)
- `pool_contains`: 1 if ptr points to a valid block start within the pool, 0 otherwise
- `pool_is_aligned`: 1 if ptr is aligned to the given boundary, 0 otherwise

## Examples

```
pool_init(16, 4);        // 4 blocks of 16 bytes each
pool_available();        // → 4

void *a = pool_alloc(); // → valid pointer
void *b = pool_alloc(); // → valid pointer
pool_available();        // → 2

pool_free(a);            // → 0 (success)
pool_available();        // → 3

pool_contains(a);        // → 1 (still a valid block address)
pool_contains((void*)0x1); // → 0 (not in pool)
```

## Constraints

- `block_size` must be >= 8 (to hold a pointer for the free list) and a power of 2
- `block_size` must be <= POOL_MAX_BLOCK_SIZE (256)
- `num_blocks` must be between 1 and POOL_MAX_BLOCKS (64)
- Freeing NULL or a pointer not from the pool returns -1
- Double-free is undefined (not required to detect, but must not crash)
- Blocks returned by `pool_alloc` must be aligned to `block_size`

## Notes

- **Free list trick**: Each free block stores a pointer to the next free block in its first `sizeof(void*)` bytes. No extra metadata memory needed.
- **O(1) alloc/free**: `pool_alloc` pops the head of the free list. `pool_free` pushes onto the head.
- **Deterministic**: Unlike `malloc`, timing is always constant regardless of allocation history.
- **Alignment guarantee**: By choosing block_size as a power of 2 and aligning the pool start, all blocks are naturally aligned.
- This is the pattern used in RTOS kernels (FreeRTOS memory pools), network stacks (packet buffer pools), and DMA descriptor rings.
