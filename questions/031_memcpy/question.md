# 031. Implement memcpy()

## Difficulty: Medium

## Problem Statement

Implement the standard C library function `memcpy()` — copy `n` bytes from source memory to destination memory.

Your implementation should be **optimized for performance** by copying word-aligned data (4 bytes at a time using `uint32_t`) when both source and destination are aligned, falling back to byte-by-byte copy for unaligned portions.

Behavior is **undefined if source and destination overlap** — you do not need to handle overlapping regions (that's `memmove`'s job).

## Function Signature

```c
void *my_memcpy(void *dest, const void *src, size_t n);
```

## Parameters

| Parameter | Type           | Description                              |
|-----------|----------------|------------------------------------------|
| `dest`    | `void *`       | Pointer to destination memory            |
| `src`     | `const void *` | Pointer to source memory (not modified)  |
| `n`       | `size_t`       | Number of bytes to copy                  |

## Return Value

Return `dest` (pointer to the destination buffer).

## Examples

### Example 1
```
Input:  src = "Hello", n = 6
Output: dest contains "Hello\0"
Explanation: Copies all 6 bytes including null terminator.
```

### Example 2
```
Input:  src = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}, n = 8
Output: dest = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}
Explanation: 8 bytes, word-aligned — can copy two 4-byte words.
```

### Example 3
```
Input:  src points to offset +1 (unaligned), n = 5
Output: dest contains the 5 bytes starting from src
Explanation: Unaligned source — must handle leading bytes before word copy.
```

## Constraints

- `dest` and `src` are valid non-NULL pointers (when `n > 0`)
- `dest` and `src` do NOT overlap
- `0 <= n <= 4096` (for test purposes)
- Alignment optimization should use 4-byte (`uint32_t`) word copies

## Notes

- A naive byte-by-byte loop is correct but suboptimal on 32-bit embedded systems.
- The optimized approach: copy leading unaligned bytes, then copy aligned words (4 bytes at a time), then copy trailing bytes.
- Check alignment with: `((uintptr_t)ptr & 0x3) == 0` (aligned to 4-byte boundary).
- Both src and dest must be aligned to use word copies safely.
- This is one of the most frequently asked embedded C interview questions.
- Real implementations often also handle 8-byte (64-bit) alignment or use DMA for very large copies.
