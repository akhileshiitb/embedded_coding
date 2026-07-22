# Hints

## Hint 1

A left rotation can be decomposed into two parts: the bits that stay within the 32-bit range (left shift) and the bits that overflow and need to wrap around (right shift of the overflow portion).

## Hint 2

Left shift the number by `n` to move most bits into position. The bits that fell off the left end are the top `n` bits of the original number — you can recover them by right-shifting the original by `(32 - n)`.

## Hint 3

```c
return (num << n) | (num >> (32 - n));
```
The left shift moves bits up by `n`, and the right shift by `(32 - n)` captures the bits that wrapped around. OR combines both parts. Handle the `n == 0` case if needed (shifting by 32 is undefined in C).
