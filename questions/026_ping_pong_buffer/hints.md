# Hints

## Hint 1

- The core idea: two static buffers (`buf_a` and `buf_b`). A `write_buf` pointer always points to one, `read_buf` points to the other (or NULL if not ready).
- `ppbuf_swap()` is called from the ISR: it makes the current write buffer "ready" and switches the write pointer to the other buffer.
- `ppbuf_consume()` is called from the main loop: it sets `read_buf = NULL`.
- Overrun = `ppbuf_swap()` is called while `read_buf != NULL` (consumer was too slow).

## Hint 2

- State tracking:
  - `IDLE`: `read_buf == NULL` (nothing to process)
  - `READY`: `read_buf != NULL` (buffer waiting for consumer)
  - `OVERRUN`: last swap happened while `read_buf` was still valid
- In `ppbuf_swap()`:
  1. Check if `read_buf != NULL` → overrun
  2. `read_buf = write_buf` (current write buffer is now ready for reading)
  3. `write_buf = (write_buf == buf_a) ? buf_b : buf_a` (switch to other buffer)
- For write/read sample: just index into write_buf/read_buf after bounds check.

## Hint 3

```c
int ppbuf_init(int size) {
    if (size < 1 || size > PPBUF_MAX_SIZE) return -1;
    buf_size = size;
    write_buf = buf_a;
    read_buf = NULL;
    state = PPBUF_STATE_IDLE;
    overrun_count = 0;
    return 0;
}

int ppbuf_swap(void) {
    int overrun = 0;
    if (read_buf != NULL) { overrun_count++; overrun = 1; }
    read_buf = write_buf;
    write_buf = (write_buf == buf_a) ? buf_b : buf_a;
    state = overrun ? PPBUF_STATE_OVERRUN : PPBUF_STATE_READY;
    return overrun ? -1 : 0;
}

int ppbuf_consume(void) {
    if (read_buf == NULL) return -1;
    read_buf = NULL;
    state = PPBUF_STATE_IDLE;
    return 0;
}
```
