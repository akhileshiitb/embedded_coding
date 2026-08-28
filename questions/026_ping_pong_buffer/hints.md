# Hints

## Hint 1

- The core idea: two buffers (e.g. `buf_a` and `buf_b`). A `write_buf` pointer
  always points to the one DMA is filling; a `read_buf` pointer points to the one
  ready for the application (or you can derive "ready" from a state flag).
- `ppbuf_swap()` is called from the ISR: it makes the current write buffer "ready"
  and switches the write pointer to the other buffer.
- `ppbuf_consume()` is called from the main loop: it marks the ready buffer as done.
- Because the consumer is always faster than the producer, you never swap onto an
  unconsumed buffer — so there is no overrun case to handle.

## Hint 2

- The state machine has just two states:
  - `IDLE`: nothing ready to process
  - `READY`: a buffer is waiting for the consumer
- Flow: `IDLE --swap()--> READY --consume()--> IDLE`
- In `ppbuf_swap()`:
  1. Swap the write/read buffer pointers (current write buffer becomes the read buffer)
  2. Set state to `READY`
- In `ppbuf_consume()`: if state is `READY`, set state back to `IDLE`; otherwise return -1.
- For write/read sample: bounds-check `index` against `buf_size`, then index into
  the write/read buffer. `read_sample` returns 0 when no buffer is ready.

## Hint 3

```c
int ppbuf_init(int size) {
    if (size < 1 || size > PPBUF_MAX_SIZE) return -1;
    buf_size  = size;
    write_buf = buf_a;
    read_buf  = buf_b;
    state     = PPBUF_STATE_IDLE;
    return 0;
}

int ppbuf_swap(void) {
    int32_t *tmp = read_buf;
    read_buf  = write_buf;   /* current write buffer is now ready */
    write_buf = tmp;         /* DMA fills the other buffer next   */
    state     = PPBUF_STATE_READY;
    return 0;
}

int ppbuf_consume(void) {
    if (state != PPBUF_STATE_READY) return -1;
    state = PPBUF_STATE_IDLE;
    return 0;
}
```
