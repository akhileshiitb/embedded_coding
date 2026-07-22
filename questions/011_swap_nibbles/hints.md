# Hints

## Hint 1

Think about how to isolate the high nibbles and low nibbles across all four bytes simultaneously. The mask `0xF0F0F0F0` selects all high nibbles, and `0x0F0F0F0F` selects all low nibbles.

## Hint 2

Once you have the high nibbles and low nibbles separated using AND masks, you need to shift one group right by 4 and the other left by 4, then combine them with OR.

## Hint 3

```c
return ((num & 0xF0F0F0F0) >> 4) | ((num & 0x0F0F0F0F) << 4);
```
This shifts all high nibbles down by 4 bits and all low nibbles up by 4 bits within their respective bytes, then combines them.
