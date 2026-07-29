# Hints

## Hint 1

- The key insight: use a power-of-2 capacity so `index & (capacity - 1)` replaces `index % capacity`.
- **Empty condition**: `head == tail`
- **Full condition**: `(head + 1) & mask == tail` (one slot is always empty as a sentinel)
- The producer only writes `head`; the consumer only writes `tail`. This is why no lock is needed.

## Hint 2

- Use `atomic_load_explicit` with `memory_order_acquire` when reading the *other* thread's variable (producer reads tail, consumer reads head).
- Use `atomic_store_explicit` with `memory_order_release` when advancing your own index (ensures data is visible before the index update).
- Use `memory_order_relaxed` when reading your own index (no need for synchronization with yourself).
- `spsc_count` = `(head - tail) & mask`.

## Hint 3

```c
int spsc_push(int32_t value) {
    unsigned int h = atomic_load_explicit(&head, memory_order_relaxed);
    unsigned int next_h = (h + 1) & mask;
    unsigned int t = atomic_load_explicit(&tail, memory_order_acquire);
    if (next_h == t) return -1;  /* full */
    buffer[h] = value;
    atomic_store_explicit(&head, next_h, memory_order_release);
    return 0;
}

int spsc_pop(int32_t *out) {
    unsigned int t = atomic_load_explicit(&tail, memory_order_relaxed);
    unsigned int h = atomic_load_explicit(&head, memory_order_acquire);
    if (t == h) return -1;  /* empty */
    *out = buffer[t];
    atomic_store_explicit(&tail, (t + 1) & mask, memory_order_release);
    return 0;
}
```
