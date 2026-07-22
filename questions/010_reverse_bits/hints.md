# Hints

## Hint 1

One approach: build the result bit by bit. Extract the LSB of the input, place it into the result, then shift the input right and the result left. Repeat 32 times.

## Hint 2

Loop 32 times. Each iteration: shift `result` left by 1, OR in the LSB of `num` (i.e., `num & 1`), then shift `num` right by 1. This moves bit 0 of the original to bit 31 of the result, bit 1 to bit 30, etc.

## Hint 3

```c
uint32_t result = 0;
for (int i = 0; i < 32; i++) {
    result = (result << 1) | (num & 1);
    num >>= 1;
}
return result;
```
Alternatively, use a divide-and-conquer approach: swap adjacent bits, then pairs, nibbles, bytes, and halfwords using masks and shifts.
