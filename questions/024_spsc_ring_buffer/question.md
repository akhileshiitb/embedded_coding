# 024. Lock-Free SPSC Ring Buffer

## Difficulty: Hard

## Problem Statement

Implement a lock-free Single-Producer Single-Consumer (SPSC) circular ring buffer using C11 atomics. This is a fundamental data structure in embedded systems for passing data between an ISR (producer) and a main-loop task (consumer), or between two threads, without using mutexes.

The ring buffer stores `int32_t` values and has a fixed capacity (power of 2 for efficient modulo via bitmask).

You must implement:
1. `spsc_init(int capacity)` — Initialize the ring buffer with given capacity (power of 2).
2. `spsc_push(int32_t value)` — Producer: enqueue a value. Returns 0 on success, -1 if full.
3. `spsc_pop(int32_t *out)` — Consumer: dequeue a value. Returns 0 on success, -1 if empty.
4. `spsc_is_empty()` — Returns 1 if the buffer is empty, 0 otherwise.
5. `spsc_is_full()` — Returns 1 if the buffer is full, 0 otherwise.
6. `spsc_count()` — Returns the number of elements currently in the buffer.

## Function Signatures

```c
#include <stdint.h>
#include <stdatomic.h>

#define SPSC_MAX_CAPACITY 256

int  spsc_init(int capacity);
int  spsc_push(int32_t value);
int  spsc_pop(int32_t *out);
int  spsc_is_empty(void);
int  spsc_is_full(void);
int  spsc_count(void);
```

## Parameters

### spsc_init
| Parameter  | Type  | Description                              |
|------------|-------|------------------------------------------|
| `capacity` | `int` | Buffer capacity (must be power of 2, <= SPSC_MAX_CAPACITY) |

### spsc_push
| Parameter | Type      | Description            |
|-----------|-----------|------------------------|
| `value`   | `int32_t` | Value to enqueue       |

### spsc_pop
| Parameter | Type       | Description                           |
|-----------|------------|---------------------------------------|
| `out`     | `int32_t *`| Pointer to store the dequeued value   |

## Return Values

- `spsc_init`: 0 on success, -1 if capacity is invalid
- `spsc_push`: 0 on success, -1 if buffer is full
- `spsc_pop`: 0 on success, -1 if buffer is empty
- `spsc_is_empty`: 1 if empty, 0 otherwise
- `spsc_is_full`: 1 if full, 0 otherwise
- `spsc_count`: number of elements in the buffer (0 to capacity-1)

## Examples

```
spsc_init(8);              // capacity = 8
spsc_is_empty();           // → 1
spsc_push(42);             // → 0 (success)
spsc_push(99);             // → 0 (success)
spsc_count();              // → 2
spsc_is_full();            // → 0

int32_t val;
spsc_pop(&val);            // → 0, val = 42 (FIFO order)
spsc_pop(&val);            // → 0, val = 99
spsc_pop(&val);            // → -1 (empty)
```

## Constraints

- `capacity` must be a power of 2, between 2 and SPSC_MAX_CAPACITY (256)
- The buffer holds at most `capacity - 1` elements (one slot is sacrificed to distinguish full from empty)
- Only one thread pushes, only one thread pops (SPSC contract)
- Use `atomic_load_explicit` / `atomic_store_explicit` with appropriate memory ordering:
  - Producer writes data, then releases head
  - Consumer reads data, then releases tail

## Notes

- **Why power of 2?** `index % capacity` becomes `index & (capacity - 1)` — a single AND instruction instead of expensive division.
- **Why one slot wasted?** To distinguish full from empty without an extra counter. Full: `(head + 1) & mask == tail`. Empty: `head == tail`.
- **Memory ordering**: The producer uses `memory_order_release` when advancing head (ensures data write is visible before the index update). The consumer uses `memory_order_acquire` when reading head (ensures it sees the producer's data).
- **No locks needed** because head is only written by the producer and tail is only written by the consumer — there is no contention on any single variable.
- This pattern is ubiquitous: Linux `kfifo`, DPDK ring, audio driver DMA buffers.
