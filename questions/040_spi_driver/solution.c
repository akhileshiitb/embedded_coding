#include <stdint.h>
#include <stddef.h>

/* ============================================================
 * Register Definitions (from IP specification)
 * ============================================================ */

struct spi_regs {
    volatile uint32_t CR1;   /* 0x00: Control Register 1 */
    volatile uint32_t CR2;   /* 0x04: Control Register 2 */
    volatile uint32_t SR;    /* 0x08: Status Register */
    volatile uint32_t DR;    /* 0x0C: Data Register */
};

/* CR1 register bits */
#define SPI_CR1_EN        (1U << 0)   /* SPI enable */
#define SPI_CR1_CPOL      (1U << 1)   /* Clock polarity */
#define SPI_CR1_CPHA      (1U << 2)   /* Clock phase */
#define SPI_CR1_LSBFIRST  (1U << 3)   /* LSB first */
#define SPI_CR1_BR_SHIFT  4           /* Baud rate prescaler bits [7:4] */
#define SPI_CR1_BR_MASK   (0xFU << SPI_CR1_BR_SHIFT)

/* CR2 register bits */
#define SPI_CR2_CS_LOW    (1U << 0)   /* Chip select assert (active low) */

/* SR register bits */
#define SPI_SR_RXNE       (1U << 0)   /* RX buffer not empty */
#define SPI_SR_TXE        (1U << 1)   /* TX buffer empty */
#define SPI_SR_BUSY       (1U << 2)   /* SPI busy */

/* System clock */
#define SPI_SYSTEM_CLOCK  16000000U

/* ============================================================
 * Driver Interface
 * ============================================================ */

#define DRV_OK              0
#define DRV_ERR_INVALID    -1
#define DRV_ERR_NOT_OPEN   -2
#define DRV_ERR_ALREADY    -3
#define DRV_ERR_HW         -4

struct driver_ops {
    int (*init)(void *handle, const void *config);
    int (*open)(void *handle);
    int (*close)(void *handle);
    int (*read)(void *handle, uint8_t *data, size_t len);
    int (*write)(void *handle, const uint8_t *data, size_t len);
    int (*deinit)(void *handle);
};

/* ============================================================
 * SPI Configuration & Device Handle
 * ============================================================ */

enum spi_bit_order {
    SPI_MSB_FIRST = 0,
    SPI_LSB_FIRST = 1
};

struct spi_config {
    uint32_t clock_hz;     /* Desired SCK frequency */
    uint8_t  mode;         /* SPI mode 0-3 */
    uint8_t  bit_order;    /* enum spi_bit_order */
};

struct spi_device {
    struct spi_regs *regs;
    const struct driver_ops *ops;
    struct spi_config config;
    uint8_t initialized;
    uint8_t opened;
};

/* ============================================================
 * TODO: Implement the SPI driver functions below
 * ============================================================ */

int spi_init(void *handle, const void *config) {
    /* TODO: Implement */
    (void)handle;
    (void)config;
    return DRV_ERR_INVALID;
}

int spi_open(void *handle) {
    /* TODO: Implement */
    (void)handle;
    return DRV_ERR_INVALID;
}

int spi_close(void *handle) {
    /* TODO: Implement */
    (void)handle;
    return DRV_ERR_INVALID;
}

int spi_read(void *handle, uint8_t *data, size_t len) {
    /* TODO: Implement */
    (void)handle;
    (void)data;
    (void)len;
    return DRV_ERR_INVALID;
}

int spi_write(void *handle, const uint8_t *data, size_t len) {
    /* TODO: Implement */
    (void)handle;
    (void)data;
    (void)len;
    return DRV_ERR_INVALID;
}

int spi_deinit(void *handle) {
    /* TODO: Implement */
    (void)handle;
    return DRV_ERR_INVALID;
}

/* Driver operations vtable */
const struct driver_ops spi_ops = {
    .init   = spi_init,
    .open   = spi_open,
    .close  = spi_close,
    .read   = spi_read,
    .write  = spi_write,
    .deinit = spi_deinit,
};
