# Hints

## Hint 1

Think of how a hardware adder works: XOR gives you the "sum" of two bits without considering carry (like a half-adder). AND gives you the bits where both inputs are 1 — these are positions that generate a carry. The carry needs to be added to the next higher bit position (shift left by 1). Repeat until there's no more carry.

## Hint 2

The loop pattern is: compute `sum = a ^ b` (XOR) and `carry = (a & b) << 1`. Then set `a = sum` and `b = carry`, and repeat until `carry == 0`. But **beware**: if `a & b` has the MSB set (negative in signed), left-shifting is undefined behavior in C. You must cast to `uint32_t` before the shift.

## Hint 3

```c
int32_t add(int32_t a, int32_t b) {
    uint32_t ua = (uint32_t)a, ub = (uint32_t)b;
    while (ub != 0) {
        uint32_t carry = (ua & ub) << 1;  /* Safe: unsigned shift */
        ua = ua ^ ub;                      /* Sum without carry */
        ub = carry;                        /* Carry becomes new b */
    }
    return (int32_t)ua;
}
```
The key insight: work entirely in `uint32_t` to avoid signed-shift UB. Two's complement addition produces identical bit patterns for signed and unsigned, so casting back to `int32_t` at the end gives the correct signed result.
