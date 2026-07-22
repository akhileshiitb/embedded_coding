# Hints

## Hint 1

A right rotation is the mirror of left rotation. Bits shifted off the right end need to appear at the left end. You can decompose this into two shift operations combined with OR.

## Hint 2

Right shift the number by `n` to move most bits into position. The bits that fell off the right end are the bottom `n` bits of the original number — you can move them to the top by left-shifting the original by `(32 - n)`.

## Hint 3

```c
return (num >> n) | (num << (32 - n));
```
The right shift moves bits down by `n`, and the left shift by `(32 - n)` captures the bits that wrapped around. OR combines both parts. Handle the `n == 0` case if needed (shifting by 32 is undefined in C).
