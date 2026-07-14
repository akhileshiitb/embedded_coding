# Hints

## Hint 1

Think about which bitwise operator can turn a specific bit ON without affecting any other bits in the number. There are four main bitwise operators: AND, OR, XOR, and NOT.

## Hint 2

The OR operator (`|`) sets a bit to 1 if either operand has a 1 in that position. You need to create a "mask" — a number that has exactly one bit set at position `i`. How can you create such a mask using the shift operator?

## Hint 3

Create a mask by shifting 1 to the left by `i` positions: `(1 << i)`. Then OR this mask with the original number: `num | (1 << i)`. This sets bit `i` to 1 while leaving all other bits unchanged. Note: use `(uint32_t)1` to avoid undefined behavior when `i = 31`.
