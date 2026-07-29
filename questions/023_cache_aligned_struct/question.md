# 023. Cache-Conscious Data Layout

## Difficulty: Medium

## Problem Statement

In embedded and performance-critical systems, understanding struct padding, member ordering, and cache line alignment is essential for minimizing memory waste and avoiding false sharing on multi-core systems.

This question tests your ability to:
1. Calculate struct sizes considering natural alignment/padding rules
2. Reorder struct members to minimize padding waste
3. Understand packed structs and their trade-offs
4. Work with explicit cache-line alignment

You must implement these functions:

1. `get_padded_size()` — Return the size (in bytes) of a struct with poor member ordering:
   ```c
   struct padded { char a; int b; char c; double d; char e; };
   ```

2. `get_optimized_size()` — Return the size of a reordered struct that minimizes padding:
   ```c
   struct optimized { double d; int b; char a; char c; char e; /* + padding to align */ };
   ```

3. `get_packed_size()` — Return the size of the same struct with `__attribute__((packed))`:
   ```c
   struct packed { char a; int b; char c; double d; char e; } __attribute__((packed));
   ```

4. `compute_padding(int offset, int alignment)` — Given a current offset and a member's alignment requirement, compute how many padding bytes are needed.

5. `is_cache_aligned(void *ptr)` — Return 1 if the pointer is 64-byte (cache-line) aligned, 0 otherwise.

6. `get_aligned_offset(int current_offset, int alignment)` — Return the next offset that satisfies the given alignment.

## Function Signatures

```c
#include <stddef.h>
#include <stdint.h>

size_t get_padded_size(void);
size_t get_optimized_size(void);
size_t get_packed_size(void);
int compute_padding(int offset, int alignment);
int is_cache_aligned(void *ptr);
int get_aligned_offset(int current_offset, int alignment);
```

## Parameters

### compute_padding
| Parameter   | Type  | Description                              |
|-------------|-------|------------------------------------------|
| `offset`    | `int` | Current byte offset within the struct    |
| `alignment` | `int` | Required alignment (always power of 2)   |

### is_cache_aligned
| Parameter | Type     | Description                    |
|-----------|----------|--------------------------------|
| `ptr`     | `void *` | Pointer to check alignment of  |

### get_aligned_offset
| Parameter        | Type  | Description                            |
|------------------|-------|----------------------------------------|
| `current_offset` | `int` | Current byte offset                    |
| `alignment`      | `int` | Required alignment (always power of 2) |

## Return Values

- `get_padded_size()`: sizeof the poorly-ordered struct (with natural padding)
- `get_optimized_size()`: sizeof the reordered struct (minimum padding)
- `get_packed_size()`: sizeof the packed struct (no padding)
- `compute_padding(offset, alignment)`: number of padding bytes needed (0 if already aligned)
- `is_cache_aligned(ptr)`: 1 if ptr is 64-byte aligned, 0 otherwise
- `get_aligned_offset(current, align)`: smallest offset >= current that is a multiple of align

## Examples

### Example 1
```
// Assuming 64-bit system (char=1, int=4, double=8):
get_padded_size();    → 32 (significant waste from poor ordering)
get_optimized_size(); → 16 (members reordered large-to-small)
get_packed_size();    → 15 (no padding, but potential unaligned access penalties)
```

### Example 2
```
compute_padding(1, 4);  → 3 (next 4-byte boundary from offset 1 is offset 4)
compute_padding(4, 4);  → 0 (already aligned)
compute_padding(5, 8);  → 3 (next 8-byte boundary from offset 5 is offset 8)
```

### Example 3
```
get_aligned_offset(1, 4);   → 4
get_aligned_offset(8, 8);   → 8
get_aligned_offset(63, 64); → 64
```

## Constraints

- Assume a 64-bit system: char=1, short=2, int=4, long=8, double=8, pointer=8
- Alignment is always a power of 2
- offset >= 0
- Cache line size is 64 bytes

## Notes

- **Natural alignment rule**: a member of size N must be placed at an offset divisible by N.
- **Struct alignment**: the overall struct is padded to a multiple of its largest member's alignment.
- **`#pragma pack` / `__attribute__((packed))`**: eliminates padding but may cause unaligned memory accesses (hardware faults on some architectures, performance penalty on others).
- **False sharing**: when two threads access different variables that share the same cache line, cache coherence traffic destroys performance.
- On ARM Cortex-M with no cache, packing is often acceptable. On Cortex-A with L1/L2 cache, alignment matters significantly.
