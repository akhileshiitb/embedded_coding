# Hints

## Hint 1

The XOR swap uses the property that `x ^ x = 0` and `x ^ 0 = x`. Three XOR-assign operations swap two values: `*a ^= *b; *b ^= *a; *a ^= *b;`. But there's a case where `x ^ x = 0` works **against** you — what happens when both pointers reference the same memory location? You need a guard before the swap logic.

## Hint 2

Check if `a == b` (pointer comparison, not value comparison). If the pointers are equal, the values are already "swapped" (same location = same value), so just return immediately. This single pointer comparison prevents the catastrophic zeroing. The XOR swap itself is three sequential `^=` operations — each depends on the result of the previous one (this is the pipeline stall the problem mentions).

## Hint 3

```c
void xor_swap(int32_t *a, int32_t *b) {
    if (a == b) return;   /* Aliasing guard — prevents zeroing */
    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}
```
That's it — the guard is the senior differentiator, not the XOR mechanics. In an interview, explain **why** this guard is needed, **when** aliasing happens in real firmware (sort swap with i==j), and **why** you'd still prefer a temp-based swap in production (pipeline parallelism, readability, MISRA compliance).
