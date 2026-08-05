# 032. Implement memmove()

## Difficulty: Medium

## Problem Statement

Implement the standard C library function `memmove()` — copy `n` bytes from source memory to destination memory, correctly handling the case where source and destination regions **overlap**.

Unlike `memcpy()` (which has undefined behavior on overlap), `memmove()` must produce the correct result regardless of how `src` and `dest` relate in memory. The key insight is choosing the **direction of copy**:

- If `dest < src`: copy **forward** (low address to high) — safe because you read bytes before overwriting them.
- If `dest > src`: copy **backward** (high address to low) — prevents overwriting source bytes you haven't copied yet.

## Function Signature

```c
void *my_memmove(void *dest, const void *src, size_t n);
```

## Parameters

| Parameter | Type           | Description                              |
|-----------|----------------|------------------------------------------|
| `dest`    | `void *`       | Pointer to destination memory            |
| `src`     | `const void *` | Pointer to source memory                 |
| `n`       | `size_t`       | Number of bytes to copy                  |

## Return Value

Return `dest` (pointer to the destination buffer).

## Examples

### Example 1: No overlap
```
Memory: [A B C D E F G H]
         src---^     ^---dest
Copy 4 bytes from src to dest (no overlap): straightforward copy.
Result: dest contains [A B C D]
```

### Example 2: Forward overlap (dest < src)
```
Before: buf = [1 2 3 4 5 6 7 8]
        dest = buf+0, src = buf+2, n = 4
        Copy forward: buf[0]=buf[2], buf[1]=buf[3], buf[2]=buf[4], buf[3]=buf[5]
After:  buf = [3 4 5 6 5 6 7 8]
```

### Example 3: Backward overlap (dest > src)
```
Before: buf = [1 2 3 4 5 6 7 8]
        dest = buf+2, src = buf+0, n = 4
        Must copy backward: buf[5]=buf[3], buf[4]=buf[2], buf[3]=buf[1], buf[2]=buf[0]
After:  buf = [1 2 1 2 3 4 7 8]
```

### Example 4: Complete overlap (dest == src)
```
Before: buf = [A B C D]
        dest = src = buf, n = 4
After:  buf = [A B C D]  (no change needed)
```

## Constraints

- `dest` and `src` are valid non-NULL pointers (when `n > 0`)
- `dest` and `src` **may overlap** in any configuration
- `0 <= n <= 4096` (for test purposes)

## Notes

- The critical question in interviews: "What happens if you always copy forward when dest > src within the overlap region?" — Answer: you overwrite source bytes before reading them, corrupting the result.
- A simple correct solution needs only two loops: forward and backward, selected by comparing `dest` and `src`.
- Optimization (word-aligned copies) is a bonus but correctness with overlap is the primary goal.
- `memmove` is the safe default in production code when you can't guarantee non-overlap.
