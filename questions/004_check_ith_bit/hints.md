# Hints

## Hint 1

Think about how to isolate a single bit from a number. The AND operator can "mask off" all bits except the one you're interested in. If the result is non-zero, that bit was set.

## Hint 2

Create a mask with only bit `i` set: `(uint32_t)1 << i`. AND this mask with `num`. If bit `i` was 1, the result is non-zero; if bit `i` was 0, the result is zero. But you need to return exactly 0 or 1...

## Hint 3

Use `(num >> i) & 1` to shift bit `i` down to position 0, then mask with 1. This always gives exactly 0 or 1. Alternatively: `(num & ((uint32_t)1 << i)) != 0` uses a comparison to normalize to 0/1.
