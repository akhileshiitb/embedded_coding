# Hints

## Hint 1

You need to move each of the four bytes to the opposite end of the 32-bit value. Extract each byte individually using masking (AND with 0xFF at the right position), then place them in their new positions using shifts.

## Hint 2

Extract each byte: byte 0 = `num & 0xFF`, byte 1 = `(num >> 8) & 0xFF`, byte 2 = `(num >> 16) & 0xFF`, byte 3 = `(num >> 24) & 0xFF`. Then shift each to its new position (byte 0 goes to position 24, byte 3 goes to position 0) and OR them together.

## Hint 3

```c
return ((num & 0x000000FF) << 24) |
       ((num & 0x0000FF00) << 8)  |
       ((num & 0x00FF0000) >> 8)  |
       ((num & 0xFF000000) >> 24);
```
Each byte is masked out and shifted to its mirror position. The four shifted bytes are combined with OR.
