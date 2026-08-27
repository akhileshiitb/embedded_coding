# 045. Rotate an Array by K Positions (Left and Right)

## Difficulty: Medium

## Problem Statement

Implement two functions that rotate an array of integers **in-place** by `k` positions:

- `rotate_left(arr, n, k)` — shift elements toward the front; elements that fall off the front wrap around to the back
- `rotate_right(arr, n, k)` — shift elements toward the back; elements that fall off the back wrap around to the front

Both must operate **in-place** using **O(1) auxiliary space** (no temporary array proportional to n).

## Function Signatures
```c
void rotate_left(int32_t *arr, int n, int k);
void rotate_right(int32_t *arr, int n, int k);
```

## Parameters
| Parameter | Type       | Description                          |
|-----------|------------|--------------------------------------|
| `arr`     | `int32_t*` | Pointer to the array to rotate       |
| `n`       | `int`      | Number of elements in the array      |
| `k`       | `int`      | Number of positions to rotate        |

## Return Value
No return value. The array is rotated in-place.

## Examples

### Example 1: Rotate Left
```
Input:  arr = {1, 2, 3, 4, 5}, n = 5, k = 2
Output: arr = {3, 4, 5, 1, 2}
Explanation: Each element moves 2 positions toward the front.
             1 and 2 wrap around to the back.
```

### Example 2: Rotate Right
```
Input:  arr = {1, 2, 3, 4, 5}, n = 5, k = 2
Output: arr = {4, 5, 1, 2, 3}
Explanation: Each element moves 2 positions toward the back.
             4 and 5 wrap around to the front.
```

### Example 3: k larger than n
```
Input:  arr = {1, 2, 3}, n = 3, k = 5
Output (left):  {3, 1, 2}
Explanation: k = 5 is equivalent to k = 5 % 3 = 2 rotations.
```

### Example 4: k = 0 or k = n
```
Input:  arr = {1, 2, 3, 4}, n = 4, k = 4
Output: {1, 2, 3, 4}  (unchanged — a full rotation returns to the original)
```

## Constraints
- `1 <= n <= 1000`
- `0 <= k` (k may be larger than n — normalize with `k % n`)
- Elements can be any valid `int32_t` value
- Must rotate **in-place** with **O(1) auxiliary space**
- Handle edge cases: `k = 0`, `k = n`, `k > n`, single-element array

## Notes

### Why In-Place Rotation Matters in Embedded Systems:

1. **No spare RAM for a copy**: On an MCU with 8–64 KB of RAM, allocating a second buffer the size of your data array may be impossible. In-place rotation uses O(1) extra memory.

2. **Circular buffer index management**: Rotating a fixed sample window (e.g., a moving-average filter over the last N ADC readings) is conceptually a rotation. Understanding rotation algorithms informs efficient ring-buffer design.

3. **DMA double-buffering**: Shifting data windows for streaming (audio, sensor fusion) often needs rotation without a full copy.

4. **Cache/flash wear**: Fewer memory writes = less power and, for some non-volatile media, less wear. The reversal algorithm does exactly `2n` writes regardless of k.

### The Key Insight — Reversal Algorithm:

Rotating left by `k` can be done with three array reversals and **no extra buffer**:
1. Reverse the first `k` elements
2. Reverse the remaining `n - k` elements
3. Reverse the whole array

This is O(n) time, O(1) space, and each element is written exactly twice. Right rotation is the same idea with `k` adjusted.

### Interview Discussion Points:
1. **"What's the naive approach and why avoid it?"** → Rotating one position at a time is O(n·k). A temporary array is O(n) space. The reversal trick is O(n) time and O(1) space.
2. **"How do left and right rotation relate?"** → Rotating right by `k` equals rotating left by `n - k`. You can implement one in terms of the other.
3. **"Why normalize k with modulo?"** → If `k >= n`, rotating by `k` is the same as rotating by `k % n`. Skipping this wastes work or causes out-of-bounds access.
4. **"What about the juggling / cyclic-replacement algorithm?"** → It also achieves O(n)/O(1) using GCD cycles, but it's harder to get right and has worse cache locality than the reversal method.
5. **"Edge cases?"** → `k = 0` (no-op), `k = n` (no-op), `n = 1` (no-op), and `k > n` (normalize first).
