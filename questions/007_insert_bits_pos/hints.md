# Hints

## Hint 1

The key challenge is constructing a mask from the start and end positions. Once you have the mask, this becomes the same as the mask-based insert: clear the field, shift the value, OR it in.

## Hint 2

To build a mask covering bits `start` to `end`: create a value with (`end - start + 1`) ones, then shift it left by `start`. One way to get N ones: `(1 << N) - 1` gives N consecutive 1-bits starting from bit 0. Watch out for the case where the width is 32 (shift overflow).

## Hint 3

Build the mask: `uint32_t mask = (((uint32_t)1 << (end - start + 1)) - 1) << start;` (handle width=32 specially: mask = `0xFFFFFFFF`). Then: `(reg & ~mask) | ((value << start) & mask)`. The final `& mask` clips the value to the field width for safety.
