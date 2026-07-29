# 028. Aligned Malloc

## Difficulty: Medium

## Problem Statement

Implement `aligned_malloc()` and `aligned_free()` — wrappers around the standard `malloc()` and `free()` that guarantee the returned pointer is aligned to a specified power-of-2 boundary.

In embedded systems, aligned memory is critical for:
- **DMA transfers** — many DMA controllers require source/destination buffers to be aligned to 4, 8, or even 64 bytes
- **SIMD/vector operations** — NEON, SSE require 16-byte alignment
- **Cache line optimization** — 64-byte alignment avoids false sharing
- **Hardware registers** — memory-mapped I/O often requires natural alignment

Standard `malloc()` only guarantees alignment suitable for any fundamental type (typically 8 or 16 bytes). When you need stricter alignment (e.g., 64, 128, 256 bytes), you need a custom allocator.

**Key insight**: Allocate extra space, then return a pointer that is aligned within that space. Store metadata just before the aligned pointer so `aligned_free()` can find the original `malloc()` pointer.

## Function Signatures

```c
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

void *aligned_malloc(size_t size, size_t alignment);
void  aligned_free(void *ptr);
```

## Parameters

### aligned_malloc
| Parameter   | Type     | Description                                              |
|-------------|----------|----------------------------------------------------------|
| `size`      | `size_t` | Number of bytes to allocate (> 0)                        |
| `alignment` | `size_t` | Required alignment in bytes (must be a power of 2, >= sizeof(void*)) |

### aligned_free
| Parameter | Type     | Description                              |
|-----------|----------|------------------------------------------|
| `ptr`     | `void *` | Pointer returned by `aligned_malloc`, or NULL |

## Return Values

- `aligned_malloc`: Pointer to aligned memory block, or NULL on failure (invalid params or malloc failure)
- `aligned_free`: No return value. Frees the underlying allocation. Passing NULL is a no-op.

## Examples

### Example 1
```
Input:  aligned_malloc(100, 64)
Output: pointer p where ((uintptr_t)p % 64) == 0
Explanation: Allocates at least 100 bytes, returned pointer is 64-byte aligned.
```

### Example 2
```
Input:  aligned_malloc(32, 256)
Output: pointer p where ((uintptr_t)p % 256) == 0
Explanation: Even though we only need 32 bytes, the pointer is 256-byte aligned.
```

### Example 3
```
Input:  aligned_malloc(0, 16)
Output: NULL
Explanation: Size of 0 is invalid, returns NULL.
```

### Example 4
```
Input:  aligned_malloc(64, 3)
Output: NULL
Explanation: Alignment is not a power of 2, returns NULL.
```

## Constraints

- `alignment` must be a power of 2 and >= `sizeof(void*)`
- `size` must be > 0
- Return NULL if parameters are invalid
- Return NULL if internal `malloc()` fails
- `aligned_free(NULL)` must be a safe no-op
- The allocated memory must be usable (writable) for `size` bytes from the returned pointer
- You may use standard `malloc()` and `free()` internally

## Notes

- **Over-allocation strategy**: To guarantee alignment, allocate `size + alignment - 1 + sizeof(void*)` bytes. The extra `alignment - 1` bytes give room to shift the pointer forward. The `sizeof(void*)` stores the original pointer.
- **Storing the original pointer**: Place the original `malloc()` pointer in the `sizeof(void*)` bytes immediately before the aligned pointer you return. This lets `aligned_free()` recover it.
- **Alignment math**: Given a pointer `p`, the next aligned address is `(p + alignment - 1) & ~(alignment - 1)`. This is the standard bitmask trick for rounding up to a power-of-2 boundary.
- **Real-world equivalents**: POSIX `posix_memalign()`, C11 `aligned_alloc()`, Windows `_aligned_malloc()`. This exercise implements the same concept from scratch.
