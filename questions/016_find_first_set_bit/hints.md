# Hints

## Hint 1

The simplest approach: start from bit 0 and check each bit with `num & (1 << i)`. Return `i + 1` for the first bit that is set. If no bit is found after checking all 32, return 0.

## Hint 2

A faster approach: isolate the lowest set bit using `num & (-num)` (two's complement trick). This gives you a value with only the lowest set bit. Then count which position that bit is at using a loop or lookup table.

## Hint 3

```c
if (num == 0) return 0;
int pos = 1;
while ((num & 1) == 0) {
    num >>= 1;
    pos++;
}
return pos;
```
Keep shifting right until the LSB is 1, counting positions. The first time LSB is 1, that's your answer.
