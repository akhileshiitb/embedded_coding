# Hints

## Hint 1

Think about how you'd multiply on paper in binary: for each `1` bit in the multiplier, you add the multiplicand shifted left by that bit's position. This is the "Russian Peasant" or "shift-and-add" method. But handling signed values is tricky — can you work with unsigned magnitudes and apply the sign at the end? (Watch out for INT_MIN's absolute value!)

## Hint 2

The cleanest approach for signed multiply: work entirely in `uint32_t`. Cast both operands to unsigned, multiply the unsigned values using shift-and-add, and cast the result back. Two's complement multiplication gives the same lower 32 bits regardless of whether you treat the operands as signed or unsigned — so you can skip all sign logic entirely!

The loop: while the multiplier (`ub`) is non-zero, if its LSB is set, accumulate the multiplicand (`ua`) into the result. Then shift `ua` left by 1 and shift `ub` right by 1.

## Hint 3

```c
int32_t multiply(int32_t a, int32_t b) {
    uint32_t ua = (uint32_t)a;
    uint32_t ub = (uint32_t)b;
    uint32_t result = 0;

    while (ub != 0) {
        if (ub & 1) {
            result += ua;   /* Or use bitwise add from Q035 */
        }
        ua <<= 1;
        ub >>= 1;
    }
    return (int32_t)result;
}
```
Key insight: since we only want the lower 32 bits of the product, unsigned multiplication produces the correct bit pattern for signed results too (two's complement property). No sign handling needed!
