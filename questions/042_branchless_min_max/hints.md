# Hints

## Hint 1

The key idea: create a **conditional mask** that is either all-ones (0xFFFFFFFF) or all-zeros (0x00000000). In C, `x < y` evaluates to 1 or 0. Negating: `-(1)` gives `0xFFFFFFFF` (all ones in two's complement), and `-(0)` gives `0x00000000`. Use this mask to conditionally apply an XOR operation.

## Hint 2

Think about what `x ^ y` gives you: the "difference bits" between x and y. If you AND this with the all-ones mask (when x < y), you get the full XOR. If you then XOR that with `y`, you get `y ^ (x ^ y) = x`. If the mask is zero, `y ^ 0 = y`. So: `min = y ^ ((x ^ y) & mask)` where `mask = -(x < y)`.

For max, notice that `x ^ (x ^ y) = y`. So: `max = x ^ ((x ^ y) & mask)`.

## Hint 3

```c
int32_t branchless_min(int32_t x, int32_t y) {
    int32_t mask = -(x < y);   /* 0xFFFFFFFF if x < y, else 0x00000000 */
    return y ^ ((x ^ y) & mask);
}

int32_t branchless_max(int32_t x, int32_t y) {
    int32_t mask = -(x < y);   /* 0xFFFFFFFF if x < y, else 0x00000000 */
    return x ^ ((x ^ y) & mask);
}
```

Breaking it down:
- `mask` = all-ones when x is smaller, all-zeros when x is larger/equal
- `(x ^ y)` = the bits that differ between x and y (the "swap delta")
- `(x ^ y) & mask` = the delta if we need to swap, or zero if not
- `y ^ delta` = x (swapped to smaller) or y (unchanged) → min
- `x ^ delta` = y (swapped to larger) or x (unchanged) → max

No branches, no subtraction, no overflow. Works for all int32_t values including INT_MIN/INT_MAX.
