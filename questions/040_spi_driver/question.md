# 040. SPI Master Driver with Generic Interface

## Difficulty: Hard

## Problem Statement

Implement a **bare-metal SPI master driver** behind the same generic device driver interface used in Q039. You will write the driver against the register specification below.

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
- `-2` = device not open (read/write called before open)
- `-3` = device already open / already initialized
- `-4` = hardware error

---

## SPI IP Register Specification

**Base address:** Provided via the device handle.

### Register Map

| Offset | Name | Access | Reset Value | Description |
|--------|------|--------|-------------|-------------|
| 0x00 | CR1 | R/W | 0x00000000 | Control Register 1 |
| 0x04 | CR2 | R/W | 0x00000000 | Control Register 2 |
| 0x08 | SR | R | 0x00000002 | Status Register |
| 0x0C | DR | R/W | 0x00000000 | Data Register |

### CR1 — Control Register 1 (Offset 0x00)

| Bits | Name | Reset | Description |
|------|------|-------|-------------|
| [0] | EN | 0 | SPI peripheral enable |
| [1] | CPOL | 0 | Clock polarity. 0=idle low, 1=idle high |
| [2] | CPHA | 0 | Clock phase. 0=sample on leading edge, 1=sample on trailing edge |
| [3] | LSBFIRST | 0 | Bit order. 0=MSB first, 1=LSB first |
| [7:4] | BR | 0 | Baud rate prescaler. SCK = SystemClock / (2^(BR+1)) |
| [31:8] | — | 0 | Reserved |

### CR2 — Control Register 2 (Offset 0x04)

| Bits | Name | Reset | Description |
|------|------|-------|-------------|
| [0] | CS_LOW | 0 | Chip select output. Write 1 to assert CS (active low). Write 0 to deassert. |
| [31:1] | — | 0 | Reserved |

### SR — Status Register (Offset 0x08)

| Bits | Name | Reset | Description |
|------|------|-------|-------------|
| [0] | RXNE | 0 | RX buffer not empty. 1 = received data available in DR |
| [1] | TXE | 1 | TX buffer empty. 1 = ready to accept data in DR |
| [2] | BUSY | 0 | SPI busy (transfer in progress) |
| [31:3] | — | 0 | Reserved |

### DR — Data Register (Offset 0x0C)

| Bits | Name | Description |
|------|------|-------------|
| [7:0] | DATA | Write: byte to transmit. Read: last received byte. |
| [31:8] | — | Reserved |

**System clock:** 16,000,000 Hz (16 MHz).

---

## SPI Modes (CPOL/CPHA)

| Mode | CPOL | CPHA | Clock Idle | Sample Edge |
|------|------|------|------------|-------------|
| 0 | 0 | 0 | Low | Rising (leading) |
| 1 | 0 | 1 | Low | Falling (trailing) |
| 2 | 1 | 0 | High | Falling (leading) |
| 3 | 1 | 1 | High | Rising (trailing) |

---

## SPI Configuration

```c
enum spi_bit_order {
    SPI_MSB_FIRST = 0,
    SPI_LSB_FIRST = 1
};

struct spi_config {
    uint32_t clock_hz;     /* Desired SCK frequency */
    uint8_t  mode;         /* SPI mode 0-3 (encodes CPOL/CPHA) */
    uint8_t  bit_order;    /* enum spi_bit_order */
};
```

---

## SPI Device Handle

```c
struct spi_device {
    struct spi_regs *regs;         /* Pointer to memory-mapped registers */
    const struct driver_ops *ops;  /* Driver operations vtable */
    struct spi_config config;      /* Stored configuration */
    uint8_t initialized;           /* 1 if init() was called */
    uint8_t opened;                /* 1 if open() was called */
};
```

---

## What You Must Implement

```c
const struct driver_ops spi_ops;

int spi_init(void *handle, const void *config);
int spi_open(void *handle);
int spi_close(void *handle);
int spi_read(void *handle, uint8_t *data, size_t len);
int spi_write(void *handle, const uint8_t *data, size_t len);
int spi_deinit(void *handle);
```

---

## Behavior Specification

### `spi_init(handle, config)`
1. Validate: handle != NULL, config != NULL
2. Validate: mode ∈ {0,1,2,3}, clock_hz > 0, bit_order ∈ {0,1}
3. Store config in handle
4. Compute prescaler: find smallest BR (0-15) such that `16MHz / 2^(BR+1) <= clock_hz`
5. Write CR1: set CPOL (bit 1) from mode bit 1, CPHA (bit 2) from mode bit 0, LSBFIRST (bit 3) from bit_order, BR (bits 7:4) from computed prescaler. Do NOT set EN yet.
6. Mark device as initialized
7. Return 0

### `spi_open(handle)`
1. Validate: handle != NULL, device must be initialized
2. Set CR1.EN = 1 (keep other CR1 bits unchanged)
3. Mark device as opened
4. Return 0

### `spi_write(handle, data, len)`
1. Validate: handle != NULL, data != NULL, len > 0, device must be open
2. Assert chip select: set CR2.CS_LOW = 1
3. For each byte:
   - Poll SR.TXE until 1
   - Write byte to DR
4. Poll SR.BUSY until 0 (wait for last byte to finish)
5. Deassert chip select: set CR2.CS_LOW = 0
6. Return 0

### `spi_read(handle, data, len)`
SPI is full-duplex: to read, you must clock out dummy bytes (0xFF).

1. Validate: handle != NULL, data != NULL, len > 0, device must be open
2. Assert chip select: CR2.CS_LOW = 1
3. For each byte:
   - Poll SR.TXE until 1
   - Write 0xFF to DR (dummy byte to generate clock)
   - Poll SR.RXNE until 1
   - Read byte from DR into data buffer
4. Deassert chip select: CR2.CS_LOW = 0
5. Return 0

### `spi_close(handle)`
1. Validate: handle != NULL, device must be open
2. Clear CR1.EN = 0 (disable SPI, keep config bits)
3. Ensure CS is deasserted: CR2.CS_LOW = 0
4. Mark device as closed
5. Return 0

### `spi_deinit(handle)`
1. Validate: handle != NULL, device must be initialized
2. If device is still open, close it first
3. Clear CR1 to 0 (reset all config)
4. Clear CR2 to 0
5. Reset all state
6. Return 0

---

## Examples

### Example: Init Mode 0, 1 MHz
```
spi_init(dev, &cfg);    // cfg = {1000000, 0, MSB_FIRST}
                         // BR=3 → 16MHz/2^4 = 1MHz
                         // CR1 = 0b00110000 = 0x30 (BR=3, CPOL=0, CPHA=0, LSB=0)
spi_open(dev);           // CR1 |= EN → CR1 = 0x31
spi_write(dev, buf, 3);  // CS_LOW=1, write 3 bytes, CS_LOW=0
spi_close(dev);          // CR1 &= ~EN → CR1 = 0x30, CS=0
```

### Example: Init Mode 3, LSB first
```
cfg = {2000000, 3, LSB_FIRST}
// Mode 3: CPOL=1, CPHA=1
// BR=2 → 16MHz/2^3 = 2MHz
// CR1 = LSBFIRST(bit3) | CPHA(bit2) | CPOL(bit1) | BR=2(bits7:4)
// CR1 = 0x2E (0b00101110)
```

---

## Constraints
- Access registers via `handle->regs` (volatile pointer)
- Validate all parameters and return error codes
- Enforce lifecycle: init before open, open before read/write
- CS must be asserted before transfer and deasserted after
- System clock is 16 MHz
- Polling only — no interrupts, no DMA

## Notes

### Interview Discussion Points:
1. **"Why manage CS in software?"** → Some SPI peripherals auto-manage CS, but for multi-byte transfers you need CS held low across all bytes. Software CS gives full control (important for SPI flash command sequences).
2. **"Why is SPI full-duplex?"** → SPI clocks data simultaneously in both directions. Even for "read-only" operations, the master must send clock cycles (dummy TX bytes) to receive data. The read function writes 0xFF as dummy.
3. **"What happens if you deassert CS between bytes?"** → Most SPI slaves interpret CS rising as "transaction complete." Multi-byte commands (like SPI flash read) will fail if CS bounces mid-transfer.
4. **"Why poll BUSY after the last byte?"** → TXE means the TX buffer is empty (byte moved to shift register), not that transmission is complete. BUSY clears when the shift register finishes clocking out the last bit.
5. **"How would you handle multiple slaves?"** → Each slave has its own CS pin. The driver would take a CS pin number in config and assert/deassert the correct pin. Never assert two CS lines simultaneously.
