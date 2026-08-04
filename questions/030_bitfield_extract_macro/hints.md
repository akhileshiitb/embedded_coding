# Hints

## Hint 1

Extraction is a two-step process: first isolate the bits you care about (turn all other bits to zero), then move them down to position 0. Think about AND and right-shift.

## Hint 2

Use `reg & mask` to zero out everything except the field bits. Then shift the result right by `shift` to align the field's LSB with bit 0 of the output.

## Hint 3

The complete expression is: `(((reg) & (mask)) >> (shift))`. The AND isolates the field, and the right-shift moves it to bit 0. Parenthesize all macro arguments to avoid operator precedence issues.
