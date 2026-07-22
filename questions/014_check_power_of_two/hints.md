# Hints

## Hint 1

A power of two has exactly one bit set in its binary representation. Think about what happens when you subtract 1 from such a number. For example: 8 = `1000`, 7 = `0111`. Notice the relationship?

## Hint 2

When `num` is a power of two, `num - 1` has all the bits below the set bit turned ON and the set bit itself turned OFF. This means `num & (num - 1)` will be zero for powers of two (but also for zero itself!).

## Hint 3

```c
return (num != 0) && ((num & (num - 1)) == 0);
```
First check that `num` is not zero (zero is not a power of two). Then check that `num & (num - 1)` equals zero, which confirms exactly one bit is set.
