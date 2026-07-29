# 026. Ping-Pong (Double) Buffer for DMA/ISR

## Difficulty: Hard

## Problem Statement

Implement a ping-pong (double) buffering system for continuous DMA/ISR streaming. This is the standard technique for high-throughput data acquisition (ADC, audio, network) where hardware writes to one buffer while firmware processes the other — zero data loss with predictable latency.

The system has two buffers (A and B). At any time:
- One buffer is being "filled" by DMA/hardware (the **active** buffer)
- One buffer is available for the application to "process" (the **ready** buffer)

You must implement:
1. `ppbuf_init(int buf_size)` — Initialize the ping-pong buffer system.
2. `ppbuf_get_write_buffer()` — Return pointer to the current DMA write buffer.
3. `ppbuf_get_read_buffer()` — Return pointer to the buffer ready for processing (NULL if none ready).
4. `ppbuf_swap()` — Called by DMA complete ISR: marks current write buffer as ready and switches to the other.
5. `ppbuf_consume()` — Called by the application after processing: marks the read buffer as consumed.
6. `ppbuf_get_state()` — Return the current state of the system.
7. `ppbuf_get_overrun_count()` — Return the number of overruns.
8. `ppbuf_write_sample(int index, int32_t value)` — Simulate DMA writing a sample.
9. `ppbuf_read_sample(int index)` — Read a sample from the current read buffer.

## Function Signatures

```c
#include <stdint.h>
#include <stddef.h>

#define PPBUF_MAX_SIZE 128
#define PPBUF_STATE_IDLE     0
#define PPBUF_STATE_READY    1
#define PPBUF_STATE_OVERRUN  2

int      ppbuf_init(int buf_size);
int32_t *ppbuf_get_write_buffer(void);
int32_t *ppbuf_get_read_buffer(void);
int      ppbuf_swap(void);
int      ppbuf_consume(void);
int      ppbuf_get_state(void);
int      ppbuf_get_overrun_count(void);
int      ppbuf_write_sample(int index, int32_t value);
int32_t  ppbuf_read_sample(int index);
```

## Return Values

- `ppbuf_init`: 0 on success, -1 if buf_size invalid
- `ppbuf_get_write_buffer`: pointer to the active write buffer
- `ppbuf_get_read_buffer`: pointer to the ready buffer, or NULL if none ready
- `ppbuf_swap`: 0 on success, -1 if overrun (previous buffer not consumed)
- `ppbuf_consume`: 0 on success, -1 if no buffer to consume
- `ppbuf_get_state`: PPBUF_STATE_IDLE, PPBUF_STATE_READY, or PPBUF_STATE_OVERRUN
- `ppbuf_get_overrun_count`: cumulative overrun count
- `ppbuf_write_sample`: 0 on success, -1 if index out of range
- `ppbuf_read_sample`: sample value, or 0 if no read buffer or index out of range

## Examples

```
ppbuf_init(64);
ppbuf_write_sample(0, 1000);
ppbuf_swap();                    // buffer A ready, DMA switches to B
ppbuf_get_state();               // → PPBUF_STATE_READY
ppbuf_read_sample(0);            // → 1000
ppbuf_consume();                 // → 0 (done processing)
```

## Constraints

- `buf_size` must be between 1 and PPBUF_MAX_SIZE (128)
- Overrun increments counter but does NOT crash — old data is lost
- Only 2 buffers exist (true ping-pong)
- Write buffer and read buffer are never the same

## Notes

- **DMA ping-pong**: DMA alternates between two buffer addresses. The DMA Transfer Complete ISR calls `ppbuf_swap()`.
- **ISR vs main loop**: ISR (producer) calls `ppbuf_swap()`. Main loop (consumer) calls `ppbuf_consume()`.
- **Overrun**: If consumer is too slow, producer overwrites oldest unprocessed buffer.
- **Zero-copy**: Application processes data in-place; no memcpy needed.
- Used in: STM32 DMA double-buffer mode, I2S audio, ADC continuous conversion, Ethernet DMA.
