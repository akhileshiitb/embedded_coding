# Hints

## Hint 1

You only need to swap the bits if they are different. If both positions have the same value (both 0 or both 1), the number is already "swapped." Think about how to extract individual bits and check if they differ.

## Hint 2

XOR is the key operator for swapping bits without a temporary variable. If two bits differ, XORing both positions will flip them — effectively swapping their values. Extract bit at pos1 and bit at pos2, check if they differ, and if so, toggle both positions using XOR.

## Hint 3

Extract the bits: `bit1 = (num >> pos1) & 1` and `bit2 = (num >> pos2) & 1`. If `bit1 != bit2`, toggle both positions: `num ^= (1U << pos1) | (1U << pos2)`. This flips both bits, achieving the swap.
