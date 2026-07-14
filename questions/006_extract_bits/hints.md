# Hints

## Hint 1

This is a two-step operation: first isolate the field bits using AND with the mask, then shift the result right to align with bit 0. Think about how to determine the shift amount from the mask.

## Hint 2

Use `reg & mask` to isolate the field bits (all other bits become 0). Then you need to shift right by the number of trailing zeros in the mask — i.e., the position of the mask's lowest set bit.

## Hint 3

Find the shift: `int shift = 0; uint32_t m = mask; while (!(m & 1)) { m >>= 1; shift++; }`. Then extract: `(reg & mask) >> shift`. This isolates the field and right-aligns it to bit 0.
