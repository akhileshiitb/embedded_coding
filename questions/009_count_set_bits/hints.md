# Hints

## Hint 1

The simplest approach is to iterate through all 32 bits, checking each one. You can check the lowest bit with `num & 1` and then shift the number right. Count how many times you find a 1.

## Hint 2

Brian Kernighan's trick: the expression `num & (num - 1)` clears the lowest set bit. You can count how many times you can do this before the number becomes zero. This runs in O(k) where k is the number of set bits — faster than always checking all 32 bits.

## Hint 3

```c
int count = 0;
while (num) {
    num &= (num - 1);  // Clear lowest set bit
    count++;
}
return count;
```
Each iteration removes exactly one set bit. The loop exits when no bits remain.
