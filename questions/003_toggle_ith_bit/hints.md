# Hints

## Hint 1

Think about which bitwise operator flips a bit regardless of its current value. There's one operator that gives 1 when inputs differ and 0 when inputs are the same.

## Hint 2

The XOR operator (`^`) has the property: `x ^ 1 = ~x` (flips the bit) and `x ^ 0 = x` (keeps the bit). You need a mask with exactly one bit set at position `i` to flip only that bit.

## Hint 3

Create a mask by shifting 1 to the left by `i` positions: `(uint32_t)1 << i`. Then XOR this mask with the original number: `num ^ ((uint32_t)1 << i)`. This flips bit `i` while leaving all other bits unchanged.
