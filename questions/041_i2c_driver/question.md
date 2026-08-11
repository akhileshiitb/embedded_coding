# 041. I2C Master Driver with Generic Interface

## Difficulty: Hard

## Problem Statement

Implement a **bare-metal I2C master driver** behind the same generic device driver interface used in Q039 and Q040. You will write the driver against the register specification below.

The driver must implement the standard lifecycle: `init → open → read/write → close → deinit`

---

## Generic Driver Interface

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
- `-2` = device not open
- `-3` = device already open / already initialized
- `-4` = hardware error (NACK received, bus busy)

---

## I2C IP Register Specification

**Base address:** Provided via the device handle.

### Register Map

| Offset | Name | Access | Reset Value | Description |
|--------|------|--------|-------------|-------------|
| 0x00 | CR | R/W | 0x00000000 | Control Register |
| 0x04 | SR | R | 0x00000000 | Status Register |
| 0x08 | DR | R/W | 0x00000000 | Data Register |
| 0x0C | CCR | R/W | 0x00000000 | Clock Control Register |

### CR — Control Register (Offset 0x00)

| Bits | Name | Reset | Description |
|------|------|-------|-------------|
| [0] | EN | 0 | I2C peripheral enable |
| [1] | START | 0 | Generate START condition. Auto-clears after START is sent. |
| [2] | STOP | 0 | Generate STOP condition. Auto-clears after STOP is sent. |
| [3] | ACK | 0 | ACK enable. 1=send ACK after receiving a byte. 0=send NACK. |
| [31:4] | — | 0 | Reserved |

### SR — Status Register (Offset 0x04)

| Bits | Name | Reset | Description |
|------|------|-------|-------------|
| [0] | BUSY | 0 | Bus busy (START detected, cleared after STOP) |
| [1] | TXE | 0 | TX buffer empty. 1 = ready to write data to DR |
| [2] | RXNE | 0 | RX buffer not empty. 1 = received data in DR |
| [3] | ADDR | 0 | Address sent. Set when slave acknowledges address byte. |
| [4] | NACK | 0 | NACK received from slave. Must be cleared by software. |
| [5] | TC | 0 | Transfer complete. Set when all bytes transferred. |
| [31:6] | — | 0 | Reserved |

### DR — Data Register (Offset 0x08)

| Bits | Name | Description |
|------|------|-------------|
| [7:0] | DATA | Write: byte to transmit (address or data). Read: received byte. |
| [31:8] | — | Reserved |

### CCR — Clock Control Register (Offset 0x0C)

| Bits | Name | Description |
|------|------|-------------|
| [11:0] | CCR_VAL | Clock control value. SCL frequency = SystemClock / (2 × CCR_VAL) |
| [31:12] | — | Reserved |

**System clock:** 16,000,000 Hz (16 MHz).

---

## I2C Address Format

The first byte after START is the **address byte**:
- Bits [7:1] = 7-bit slave address
- Bit [0] = R/W̄ direction bit (0 = write, 1 = read)

So to write to slave 0x50: send `(0x50 << 1) | 0` = 0xA0
To read from slave 0x50: send `(0x50 << 1) | 1` = 0xA1

---

## I2C Configuration

```c
struct i2c_config {
    uint32_t clock_hz;     /* SCL frequency: 100000 (standard) or 400000 (fast) */
    uint8_t  slave_addr;   /* 7-bit slave address (0x08-0x77 valid range) */
};
```

---

## I2C Device Handle

```c
struct i2c_device {
    struct i2c_regs *regs;         /* Pointer to memory-mapped registers */
    const struct driver_ops *ops;  /* Driver operations vtable */
    struct i2c_config config;      /* Stored configuration */
    uint8_t initialized;           /* 1 if init() was called */
    uint8_t opened;                /* 1 if open() was called */
};
```

---

## What You Must Implement

```c
const struct driver_ops i2c_ops;

int i2c_init(void *handle, const void *config);
int i2c_open(void *handle);
int i2c_close(void *handle);
int i2c_read(void *handle, uint8_t *data, size_t len);
int i2c_write(void *handle, const uint8_t *data, size_t len);
int i2c_deinit(void *handle);
```

---

## Behavior Specification

### `i2c_init(handle, config)`
1. Validate: handle != NULL, config != NULL
2. Validate: clock_hz ∈ {100000, 400000}, slave_addr in range 0x08–0x77
3. Store config in handle
4. Compute CCR_VAL: `16000000 / (2 × clock_hz)`
5. Write CCR_VAL to CCR register
6. Mark device as initialized
7. Return 0

### `i2c_open(handle)`
1. Validate: handle != NULL, device must be initialized
2. Set CR.EN = 1
3. Mark device as opened
4. Return 0

### `i2c_write(handle, data, len)`
Performs a complete I2C write transaction: START → ADDR+W → DATA bytes → STOP

1. Validate: handle != NULL, data != NULL, len > 0, device must be open
2. Generate START: set CR.START = 1, wait for SR.BUSY = 1
3. Send address byte (write): write `(slave_addr << 1) | 0` to DR
4. Wait for SR.ADDR = 1 (slave ACK'd address). If SR.NACK = 1, generate STOP and return -4
5. For each data byte:
   - Wait for SR.TXE = 1
   - Write byte to DR
   - If SR.NACK = 1 at any point, generate STOP and return -4
6. Wait for SR.TC = 1 (transfer complete)
7. Generate STOP: set CR.STOP = 1
8. Return 0

### `i2c_read(handle, data, len)`
Performs a complete I2C read transaction: START → ADDR+R → READ bytes (ACK/NACK) → STOP

1. Validate: handle != NULL, data != NULL, len > 0, device must be open
2. Generate START: set CR.START = 1, wait for SR.BUSY = 1
3. Set CR.ACK = 1 (to ACK received bytes)
4. Send address byte (read): write `(slave_addr << 1) | 1` to DR
5. Wait for SR.ADDR = 1. If SR.NACK = 1, generate STOP and return -4
6. For each byte except the last:
   - Wait for SR.RXNE = 1
   - Read byte from DR (ACK is sent automatically since CR.ACK = 1)
7. For the last byte:
   - Clear CR.ACK = 0 (send NACK to signal end of read)
   - Wait for SR.RXNE = 1
   - Read last byte from DR
8. Generate STOP: set CR.STOP = 1
9. Return 0

### `i2c_close(handle)`
1. Validate: handle != NULL, device must be open
2. Clear CR.EN = 0
3. Mark device as closed
4. Return 0

### `i2c_deinit(handle)`
1. Validate: handle != NULL, device must be initialized
2. If still open, close it first
3. Clear CCR to 0
4. Clear CR to 0
5. Reset all state
6. Return 0

---

## Examples

### Example: Write 2 bytes to slave 0x50 at 100 kHz
```
i2c_init(dev, &cfg);     // cfg = {100000, 0x50}
                          // CCR = 16000000/(2×100000) = 80
i2c_open(dev);            // CR.EN = 1
i2c_write(dev, buf, 2);  // START → 0xA0 → data[0] → data[1] → STOP
i2c_close(dev);           // CR.EN = 0
```

### Example: Read 3 bytes from slave 0x68
```
cfg = {400000, 0x68}      // CCR = 16000000/(2×400000) = 20
i2c_read(dev, buf, 3);   // START → 0xD1 → [ACK]read → [ACK]read → [NACK]read → STOP
```

---

## Constraints
- Access registers via `handle->regs` (volatile pointer)
- Validate all parameters and return error codes
- Enforce lifecycle: init before open, open before read/write
- Send NACK before the last read byte (standard I2C protocol)
- System clock is 16 MHz
- Polling only — no interrupts, no DMA

## Notes

### Interview Discussion Points:
1. **"Why NACK the last byte?"** → The master must signal the slave to stop sending. Without NACK, the slave will keep driving SDA and the master can't generate STOP cleanly.
2. **"What's the difference between repeated START and STOP+START?"** → Repeated START doesn't release the bus (no other master can arbitrate). Used for atomic read-after-write (e.g., set register address then read data).
3. **"Why is the slave address shifted left by 1?"** → The I2C standard packs the 7-bit address in bits [7:1] and the R/W direction in bit [0]. Some datasheets give the "8-bit address" (already shifted); others give the "7-bit address" (needs shifting). Always clarify.
4. **"How would you handle clock stretching?"** → Slave holds SCL low to pause the master. The hardware handles this transparently in most I2C peripherals — the master's clock counter pauses while SCL is held low.
5. **"What happens on bus contention (multi-master)?"** → Arbitration loss: the master detects another master driving SDA low when it expected high. Must abort and retry. Our simple driver doesn't handle this (single-master assumption).
