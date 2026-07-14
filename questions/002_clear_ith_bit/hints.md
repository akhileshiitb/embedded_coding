# Hints

## Hint 1

Think about which bitwise operator can force a specific bit to 0 without affecting any other bits. The AND operator has a useful property: `1 & x = x` and `0 & x = 0`.

## Hint 2

You need a mask that has all bits set to 1 EXCEPT the bit at position `i`, which should be 0. How can you invert a mask that has only one bit set? The NOT operator (`~`) flips all bits.

## Hint 3

Create a mask with only bit `i` set: `(uint32_t)1 << i`. Invert it with NOT to get all 1s except position `i`: `~((uint32_t)1 << i)`. Then AND with the original: `num & ~((uint32_t)1 << i)`. This clears bit `i` while preserving all other bits.
