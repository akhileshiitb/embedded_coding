# Hints

## Hint 1

The insert operation has two parts: first remove the old field value, then place the new one. Think about which bitwise operators can clear specific bits and which can set them.

## Hint 2

Use `reg & ~mask` to clear the target field (inverted mask turns off exactly those bits). Then shift the value left by `shift` and use OR to place it. Apply `& mask` to the shifted value as a safety clip.

## Hint 3

The complete expression is: `(reg & ~mask) | (((value) << (shift)) & (mask))`. The `~mask` clears the field, `<< shift` positions the value, `& mask` clips it, and `|` merges it in.
