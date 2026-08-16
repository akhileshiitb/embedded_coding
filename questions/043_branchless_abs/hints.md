# Hints

## Hint 1

Arithmetic right shift of a signed integer by 31 replicates the sign bit into all 32 positions. For negative numbers this gives `0xFFFFFFFF` (-1), for positive/zero it gives `0x00000000`. Store this in a variable called `mask`. Now you have a conditional mask — use it with addition and XOR to selectively negate.

## Hint 2

Two's complement negation: `-v = ~v + 1 = (v ^ 0xFFFFFFFF) + 1`. Rearranging: `-v = (v + (-1)) ^ (-1)` — which is `(v + 0xFFFFFFFF) ^ 0xFFFFFFFF`. Notice that `0xFFFFFFFF` is exactly what `mask` equals when v is negative. So: `(v + mask) ^ mask` gives `-v` when negative, and `(v + 0) ^ 0 = v` when positive.

## Hint 3

```c
int32_t branchless_abs(int32_t v) {
    int32_t mask = v >> 31;       /* 0xFFFFFFFF if negative, 0x00000000 if positive */
    return (v + mask) ^ mask;
}
```

Step by step:
- Negative v: `mask = -1`, so `(v + (-1)) ^ (-1)` = `(v - 1) ^ 0xFFFFFFFF` = `~(v - 1)` = `-v`
- Positive v: `mask = 0`, so `(v + 0) ^ 0` = `v`

Alternative equivalent: `(v ^ mask) - mask` (uses subtraction instead of addition + XOR).

**Edge case**: `abs(INT_MIN)` → mask = -1, result = `(INT_MIN - 1) ^ -1` = `(INT_MAX) ^ -1` = `INT_MIN` (wraps). This is expected — acknowledge it in interviews.
