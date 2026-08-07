# 037. Sort an Array (In-Place, Embedded-Constrained)

## Difficulty: Medium

## Problem Statement

Sort an array of `int32_t` values in **ascending order**, in-place, using **O(1) auxiliary space**.

While sorting is fundamental, this question focuses on the **engineering trade-offs** that matter in embedded systems — not just "does it produce sorted output."

## ⚠️ Embedded Systems: Why Sorting Algorithm Choice Matters

In embedded firmware, you don't just pick "the fastest sort." Constraints dictate your choice:

### 1. Stack Depth (Recursion Budget)

Quicksort's worst-case recursion depth is O(n) — on a Cortex-M0 with a 1KB stack, sorting a 200-element array can **stack overflow** and corrupt memory silently (no MMU to catch it).

| Algorithm | Stack Usage (worst) | Stack Usage (avg) |
|-----------|--------------------|--------------------|
| Quicksort | O(n) — DANGEROUS  | O(log n)           |
| Mergesort | O(n) — heap + O(log n) stack | same      |
| Heapsort  | **O(1)** — safe    | O(1)               |
| Insertion | **O(1)** — safe    | O(1)               |
| Shell sort| **O(1)** — safe    | O(1)               |

**Rule**: On stack-constrained MCUs, prefer iterative sorts or heapsort.

### 2. O(1) Auxiliary Space (No malloc)

Many embedded systems have **no heap** (`malloc` is disabled or absent). Mergesort's O(n) extra space is not an option. Your sort must work entirely within the given array.

### 3. Cache Behavior and Predictability

- **Insertion sort**: Sequential access pattern, excellent cache locality for small arrays (n < 32)
- **Heapsort**: Poor cache locality (jumps across array), but guaranteed O(n log n) worst-case
- **Quicksort**: Good locality in practice, but worst-case O(n²) breaks real-time deadlines
- **Shell sort**: Moderate locality, sub-quadratic in practice, fully iterative, O(1) space

### 4. Stability (Does it matter?)

A sort is **stable** if equal elements preserve their original order. In sensor data processing (e.g., sorting timestamped readings by priority), stability can matter. Insertion sort is stable; heapsort and quicksort are not.

### 5. Worst-Case Guarantees for Real-Time Systems

If your sort runs in an ISR or a hard-real-time loop, you need **predictable** timing:
- Quicksort: O(n²) worst case — violates timing contracts
- Heapsort: O(n log n) guaranteed — safe for real-time
- Insertion sort: O(n²) worst case, but O(n) for nearly-sorted data (common in sensor streams)

### Recommended Choice for This Problem

Implement any in-place, O(1) space sort. Good options:
- **Insertion sort** — simplest, O(1) space, stable, best for small/nearly-sorted arrays
- **Shell sort** — improved insertion sort, O(1) space, sub-quadratic for moderate n
- **Heapsort** — O(n log n) worst-case guaranteed, O(1) space, but complex to implement correctly
- **In-place quicksort** — fast in practice, but requires tail-call optimization or iterative stack to limit depth

## Function Signature
```c
void sort_array(int32_t *arr, int n);
```

## Parameters
| Parameter | Type       | Description                          |
|-----------|------------|--------------------------------------|
| `arr`     | `int32_t*` | Pointer to the array to sort in-place|
| `n`       | `int`      | Number of elements in the array      |

## Return Value
No return value. The array is sorted in ascending order in-place.

## Examples

### Example 1
```
Input:  arr = {5, 2, 9, 1, 5, 6}, n = 6
Output: arr = {1, 2, 5, 5, 6, 9}
```

### Example 2
```
Input:  arr = {3, -1, 0, -7, 4}, n = 5
Output: arr = {-7, -1, 0, 3, 4}
```

### Example 3
```
Input:  arr = {1}, n = 1
Output: arr = {1}
Explanation: Single element — already sorted.
```

### Example 4
```
Input:  arr = {5, 4, 3, 2, 1}, n = 5
Output: arr = {1, 2, 3, 4, 5}
Explanation: Reverse-sorted input — worst case for insertion sort (O(n²) comparisons).
```

## Constraints
- `1 <= n <= 1000`
- Elements can be any valid `int32_t` value (including negative, zero, INT_MIN, INT_MAX)
- Duplicates are allowed
- Must sort in-place with **O(1) auxiliary space** (no malloc, no VLAs, no extra arrays)
- Recursion is allowed but discouraged (think: 1KB stack)

## Notes

### Interview Discussion Points:
1. **"Which sort would you use on a Cortex-M0 with 1KB stack and n=500?"** → Heapsort or Shell sort (O(1) stack, no recursion, O(n log n) or better than O(n²)).
2. **"What if the data is almost sorted (sensor readings)?"** → Insertion sort — O(n) for nearly-sorted, and the branch predictor handles it well.
3. **"Why not use qsort() from libc?"** → It's often quicksort with O(n) stack worst-case. Some embedded libc versions use a hybrid (introsort) but you can't guarantee behavior. Also, the function-pointer callback in qsort() has overhead and prevents inlining.
4. **"How would you test that your sort uses O(1) space?"** → On bare-metal: paint the stack with a sentinel pattern before calling sort, then scan for the high-water mark after. If it grew more than a few words, you have a problem.
5. **"Can you swap elements using XOR swap?"** → You can (Q034), but with the aliasing guard. In practice, use a temp — it's faster on pipelined cores.
