#include <stdint.h>
#include <stddef.h>

/* ============================================================
 * Register Definitions (from IP specification)
 * ============================================================ */

/* UART Register structure — memory-mapped */
struct uart_regs {
    volatile uint32_t CR;    /* 0x00: Control Register */
    volatile uint32_t SR;    /* 0x04: Status Register */
    volatile uint32_t DR;    /* 0x08: Data Register */
    volatile uint32_t BRR;   /* 0x0C: Baud Rate Register */
};

/* CR register bits */
#define UART_CR_EN      (1U << 0)   /* Peripheral enable */
#define UART_CR_TXEN    (1U << 1)   /* Transmit enable */
#define UART_CR_RXEN    (1U << 2)   /* Receive enable */

/* SR register bits */
#define UART_SR_TXE     (1U << 0)   /* TX buffer empty */
#define UART_SR_RXNE    (1U << 1)   /* RX buffer not empty */
#define UART_SR_BUSY    (1U << 2)   /* Transmitter busy */
#define UART_SR_ORE     (1U << 3)   /* Overrun error */

/* System clock */
#define UART_SYSTEM_CLOCK   16000000U

/* ============================================================
 * Driver Interface
 * ============================================================ */

/* Error codes */
#define DRV_OK              0
#define DRV_ERR_INVALID    -1   /* Invalid parameter */
#define DRV_ERR_NOT_OPEN   -2   /* Device not open */
#define DRV_ERR_ALREADY    -3   /* Already initialized/open */
#define DRV_ERR_HW         -4   /* Hardware error */

/* Generic driver operations vtable */
struct driver_ops {
    int (*init)(void *handle, const void *config);
    int (*open)(void *handle);
    int (*close)(void *handle);
    int (*read)(void *handle, uint8_t *data, size_t len);
    int (*write)(void *handle, const uint8_t *data, size_t len);
    int (*deinit)(void *handle);
};

/* ============================================================
 * UART Configuration & Device Handle
 * ============================================================ */

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

struct uart_device {
    struct uart_regs *regs;         /* Pointer to HW registers */
    const struct driver_ops *ops;   /* Driver vtable */
    struct uart_config config;      /* Stored configuration */
    uint8_t initialized;            /* 1 if init() called */
    uint8_t opened;                 /* 1 if open() called */
};

/* ============================================================
 * TODO: Implement the UART driver functions below
 * ============================================================ */

int uart_init(void *handle, const void *config) {
    /* TODO: Implement */
    (void)handle;
    (void)config;
    return DRV_ERR_INVALID;
}

int uart_open(void *handle) {
    /* TODO: Implement */
    (void)handle;
    return DRV_ERR_INVALID;
}

int uart_close(void *handle) {
    /* TODO: Implement */
    (void)handle;
    return DRV_ERR_INVALID;
}

int uart_read(void *handle, uint8_t *data, size_t len) {
    /* TODO: Implement */
    (void)handle;
    (void)data;
    (void)len;
    return DRV_ERR_INVALID;
}

int uart_write(void *handle, const uint8_t *data, size_t len) {
    /* TODO: Implement */
    (void)handle;
    (void)data;
    (void)len;
    return DRV_ERR_INVALID;
}

int uart_deinit(void *handle) {
    /* TODO: Implement */
    (void)handle;
    return DRV_ERR_INVALID;
}

/* Driver operations vtable — populate with your functions */
const struct driver_ops uart_ops = {
    .init   = uart_init,
    .open   = uart_open,
    .close  = uart_close,
    .read   = uart_read,
    .write  = uart_write,
    .deinit = uart_deinit,
};
