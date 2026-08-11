# 039. UART Driver with Generic Interface

## Difficulty: Hard

## Problem Statement

Implement a **bare-metal UART driver** behind a generic device driver interface. You will write the driver against the register specification below — just as you would when reading a real MCU datasheet.

The driver must implement the standard lifecycle: `init → open → read/write → close → deinit`

---

## Generic Driver Interface

All peripheral drivers share this common interface:

```c
struct driver_ops {
    int (*init)(void *handle, const void *config);
    int (*open)(void *handle);
    int (*close)(void *handle);
    int (*read)(void *handle, uint8_t *data, size_t len);
    int (*write)(void *handle, const uint8_t *data, size_t len);
    int (*deinit)(void *handle);
};
```

**Return conventions:**
- `0` = success
- `-1` = invalid parameter (NULL pointer, invalid config)
- `-2` = device not open (read/write called before open)
- `-3` = device already open / already initialized
- `-4` = hardware error (timeout, overrun)

---

## UART IP Register Specification

**Base address:** Provided via the device handle (not hardcoded).

### Register Map

| Offset | Name | Access | Reset Value | Description |
|--------|------|--------|-------------|-------------|
| 0x00 | CR | R/W | 0x00000000 | Control Register |
| 0x04 | SR | R | 0x00000001 | Status Register |
| 0x08 | DR | R/W | 0x00000000 | Data Register |
| 0x0C | BRR | R/W | 0x00000000 | Baud Rate Register |

### CR — Control Register (Offset 0x00)

| Bits | Name | Reset | Description |
|------|------|-------|-------------|
| [0] | EN | 0 | Peripheral enable. Must be set before any TX/RX. |
| [1] | TXEN | 0 | Transmit enable. |
| [2] | RXEN | 0 | Receive enable. |
| [31:3] | — | 0 | Reserved. Write as 0. |

### SR — Status Register (Offset 0x04)

| Bits | Name | Reset | Description |
|------|------|-------|-------------|
| [0] | TXE | 1 | TX buffer empty. 1 = ready to accept data in DR. |
| [1] | RXNE | 0 | RX buffer not empty. 1 = data available in DR. |
| [2] | BUSY | 0 | Transmitter busy. |
| [3] | ORE | 0 | Overrun error. Set if new data arrives while RXNE=1. |
| [31:4] | — | 0 | Reserved. |

### DR — Data Register (Offset 0x08)

| Bits | Name | Description |
|------|------|-------------|
| [7:0] | DATA | Write: byte to transmit. Read: received byte. |
| [31:8] | — | Reserved. Read as 0. |

### BRR — Baud Rate Register (Offset 0x0C)

| Bits | Name | Description |
|------|------|-------------|
| [15:0] | DIV | Baud rate divisor. BaudRate = SystemClock / DIV. |
| [31:16] | — | Reserved. Write as 0. |

**System clock:** Assume 16,000,000 Hz (16 MHz).

---

## UART Configuration

```c
enum uart_parity {
    UART_PARITY_NONE = 0,
    UART_PARITY_ODD  = 1,
    UART_PARITY_EVEN = 2
};

struct uart_config {
    uint32_t baud_rate;    /* 9600, 19200, 38400, 57600, 115200 */
    uint8_t  data_bits;    /* 7 or 8 */
    uint8_t  stop_bits;    /* 1 or 2 */
    uint8_t  parity;       /* enum uart_parity */
};
```

---

## UART Device Handle

```c
struct uart_device {
    struct uart_regs *regs;         /* Pointer to memory-mapped registers */
    const struct driver_ops *ops;   /* Driver operations vtable */
    struct uart_config config;      /* Stored configuration */
    uint8_t initialized;            /* 1 if init() was called */
    uint8_t opened;                 /* 1 if open() was called */
};
```

---

## What You Must Implement

```c
/* The driver_ops vtable populated with your functions */
const struct driver_ops uart_ops;

/* Individual driver functions */
int uart_init(void *handle, const void *config);
int uart_open(void *handle);
int uart_close(void *handle);
int uart_read(void *handle, uint8_t *data, size_t len);
int uart_write(void *handle, const uint8_t *data, size_t len);
int uart_deinit(void *handle);
```

---

## Behavior Specification

### `uart_init(handle, config)`
1. Validate: handle != NULL, config != NULL
2. Validate config: baud_rate > 0, data_bits ∈ {7,8}, stop_bits ∈ {1,2}, parity ∈ {0,1,2}
3. Store config in handle
4. Compute divisor: `DIV = 16000000 / baud_rate`
5. Write DIV to BRR register
6. Mark device as initialized
7. Return 0 on success

### `uart_open(handle)`
1. Validate: handle != NULL, device must be initialized
2. Set CR bits: EN=1, TXEN=1, RXEN=1
3. Mark device as opened
4. Return 0 on success

### `uart_write(handle, data, len)`
1. Validate: handle != NULL, data != NULL, len > 0, device must be open
2. For each byte:
   - Poll SR.TXE until it equals 1 (TX buffer ready)
   - Write byte to DR
3. Return 0 on success

### `uart_read(handle, data, len)`
1. Validate: handle != NULL, data != NULL, len > 0, device must be open
2. For each byte:
   - Poll SR.RXNE until it equals 1 (data available)
   - Read byte from DR
3. Return 0 on success

### `uart_close(handle)`
1. Validate: handle != NULL, device must be open
2. Clear CR bits: EN=0, TXEN=0, RXEN=0
3. Mark device as closed
4. Return 0 on success

### `uart_deinit(handle)`
1. Validate: handle != NULL, device must be initialized
2. If device is still open, close it first
3. Clear BRR to 0
4. Reset all state
5. Return 0 on success

---

## Examples

### Example: Init + Write "Hi"
```
uart_init(dev, &cfg);         // BRR = 16000000/115200 = 138
uart_open(dev);               // CR = 0x07 (EN|TXEN|RXEN)
uart_write(dev, "Hi", 2);    // Polls TXE, writes 'H' then 'i' to DR
uart_close(dev);              // CR = 0x00
```

### Example: Error — write before open
```
uart_init(dev, &cfg);
uart_write(dev, "X", 1);     // Returns -2 (device not open)
```

---

## Constraints
- Must NOT hardcode register base address — access via `handle->regs`
- All register accesses must go through the `volatile` pointer
- Must validate all parameters and return appropriate error codes
- Must enforce lifecycle: init before open, open before read/write
- System clock is 16 MHz (constant)
- Polling only — no interrupts, no DMA

## Notes

### Interview Discussion Points:
1. **"Why volatile for registers?"** → Prevents compiler from caching register reads or reordering accesses. Without it, polling loops may be optimized away.
2. **"Why a vtable (driver_ops) pattern?"** → Enables polymorphism: upper layers call `dev->ops->write()` without knowing if it's UART, SPI, or I2C underneath. Same pattern as Linux file_operations.
3. **"Why check lifecycle state?"** → Writing to an uninitialized peripheral can corrupt hardware state, trigger faults, or enable unintended DMA transfers. Defensive APIs catch bugs early.
4. **"What would you add for production?"** → Timeout on poll loops, interrupt-driven TX/RX with ring buffers, DMA support, mutex for thread safety, power management (clock gating on close).
