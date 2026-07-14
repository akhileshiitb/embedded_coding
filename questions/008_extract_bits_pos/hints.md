# Hints

## Hint 1

Two approaches: (1) build a mask for the field, AND with reg, then shift right; or (2) shift right first, then mask off the upper bits. Either way you need to know the field width: `end - start + 1`.

## Hint 2

Simpler approach — shift first, then mask: shift `reg` right by `start` to bring the field down to bit 0, then AND with a mask of `width` ones to discard bits above the field. A mask of N ones is `(1 << N) - 1`.

## Hint 3

`(reg >> start) & (((uint32_t)1 << (end - start + 1)) - 1)`. For the edge case where the field is all 32 bits (start=0, end=31), `(1 << 32)` overflows — handle it by returning `reg` directly or using `width == 32 ? 0xFFFFFFFF : ((uint32_t)1 << width) - 1` as the mask.
