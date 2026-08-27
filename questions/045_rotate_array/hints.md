# Hints

The hints progress from a **simple-but-less-optimal** approach to the **most-optimal in-place** solution.

## Hint 1 — Concept & Normalize

First, always normalize `k`: if `k >= n`, then `k = k % n` (rotating by n is a no-op). Also handle `n <= 1` (nothing to do). Rotating **right** by `k` is the same as rotating **left** by `n - k` — so you can implement one and derive the other.

## Hint 2 — Approach 1: Temp Buffer (works, but O(n) space — NOT allowed here)

The easiest to reason about, but it violates the O(1) space constraint (shown for contrast):

```c
/* NOT acceptable for this problem — uses O(n) extra space */
void rotate_left_naive(int32_t *arr, int n, int k) {
    int32_t tmp[1000];
    k %= n;
    for (int i = 0; i < n; i++)
        tmp[(i + n - k) % n] = arr[i];
    for (int i = 0; i < n; i++)
        arr[i] = tmp[i];
}
```

Another O(1)-space-but-slow option is rotating one step at a time, k times — that's O(n·k), too slow. Both motivate a better approach.

## Hint 3 — Approach 2: Reversal Algorithm (O(n) time, O(1) space — OPTIMAL)

Rotating left by `k` = reverse first `k`, reverse rest, reverse whole. Each element is written exactly twice, no extra buffer:

```c
static void reverse(int32_t *arr, int lo, int hi) {
    while (lo < hi) {
        int32_t t = arr[lo];
        arr[lo] = arr[hi];
        arr[hi] = t;
        lo++;
        hi--;
    }
}

void rotate_left(int32_t *arr, int n, int k) {
    if (n <= 1) return;
    k %= n;
    if (k == 0) return;
    reverse(arr, 0, k - 1);       /* reverse first k */
    reverse(arr, k, n - 1);       /* reverse the rest */
    reverse(arr, 0, n - 1);       /* reverse the whole array */
}

void rotate_right(int32_t *arr, int n, int k) {
    if (n <= 1) return;
    k %= n;
    /* right by k == left by (n - k) */
    rotate_left(arr, n, n - k);
}
```

**Why this is optimal:**
- **O(n) time**: three passes, each element touched a constant number of times
- **O(1) space**: only a single temp variable in `reverse` (swap), no buffer proportional to n
- **Deterministic writes**: exactly 2n writes regardless of k — good for flash/EEPROM wear and predictable timing

**Walkthrough** — rotate `{1,2,3,4,5}` left by 2:
1. Reverse first 2: `{2,1,3,4,5}`
2. Reverse rest: `{2,1,5,4,3}`
3. Reverse whole: `{3,4,5,1,2}` ✓
