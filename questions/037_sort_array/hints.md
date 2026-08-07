# Hints

## Hint 1

For embedded systems with O(1) space constraint, the simplest correct approach is **insertion sort**: iterate from index 1 to n-1, and for each element, shift it left until it's in the correct position. It's stable, uses no extra memory, and is O(n) on nearly-sorted data (common in sensor streams). The downside: O(n²) worst case on reverse-sorted input.

## Hint 2

Insertion sort inner loop: save `arr[i]` as `key`, then scan backwards from `j = i-1` while `arr[j] > key`, shifting each element right by one. When the scan stops, place `key` at `arr[j+1]`. This is essentially "insert into a sorted subarray."

For better performance on larger arrays, consider **Shell sort**: run insertion sort with decreasing gap sequences (e.g., gaps = n/2, n/4, ..., 1). Same O(1) space, but reduces comparisons to O(n^(3/2)) or better.

## Hint 3

```c
void sort_array(int32_t *arr, int n) {
    /* Insertion sort: O(1) space, stable, O(n) for nearly-sorted */
    for (int i = 1; i < n; i++) {
        int32_t key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}
```
This handles all edge cases: single element (loop doesn't execute), negatives (comparison works naturally with int32_t), duplicates (stable — equal elements don't move past each other), and INT_MIN/INT_MAX (no arithmetic overflow in comparisons).
