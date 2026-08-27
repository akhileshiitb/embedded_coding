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

#define UART_MAX_TIMEOUT    1000000

/* ============================================================
 * TODO: Implement the UART driver functions below
 * ============================================================ */

int uart_init(void *handle, const void *config) {

    struct uart_device *dev = (struct uart_device *)handle;
    struct uart_config *cfg = (struct uart_config *)config;

    if (!dev || !config)
        return DRV_ERR_INVALID;

    // validate the config
    if (cfg->baud_rate <= 0)
        return DRV_ERR_INVALID;

    if ((cfg->data_bits != 7) && (cfg->data_bits != 8))
        return DRV_ERR_INVALID;

    if ((cfg->stop_bits != 1) && (cfg->stop_bits != 2))
        return DRV_ERR_INVALID;

    if ((cfg->parity != 0) && (cfg->parity != 1) && (cfg->parity != 2))
        return DRV_ERR_INVALID;

    dev->config = *cfg;

    uint32_t div = 16000000 / cfg->baud_rate;

    dev->regs->BRR = div;

    dev->initialized = 1;

    return DRV_OK;
}

int uart_open(void *handle) {
    
    struct uart_device *dev = (struct uart_device *)handle;

    if (!dev)
        return DRV_ERR_INVALID;

    if (!dev->initialized)
        return DRV_ERR_INVALID;

    // set CR bits
    dev->regs->CR = UART_CR_EN | UART_CR_TXEN | UART_CR_RXEN;

    dev->opened = 1;

    return DRV_OK;
}

int uart_close(void *handle) {

    struct uart_device *dev = (struct uart_device *)handle;
    
    if (!dev) 
        return DRV_ERR_INVALID;
    
    if (!dev->opened)  
        return DRV_ERR_NOT_OPEN;

    uint32_t dummy_ticks = 0;
    while (dev->regs->SR & UART_SR_BUSY) {
        // wait till last byte if any gets transmitted. 
        if (dummy_ticks > UART_MAX_TIMEOUT)
            return DRV_ERR_HW;

        dummy_ticks += 1;
    }

    dev->regs->CR &= ~(UART_CR_EN | UART_CR_TXEN | UART_CR_RXEN);

    dev->opened = 0;

    return DRV_OK;
}

int uart_read(void *handle, uint8_t *data, size_t len) {

    struct uart_device *dev = (struct uart_device *)handle;
    
    if (!dev || !data || (len <= 0))    
        return DRV_ERR_INVALID;

    if (!dev->opened)
        return DRV_ERR_NOT_OPEN;

    for (size_t i = 0; i < len; i += 1) {
        uint32_t dummy_ticks = 0;
        while (!(dev->regs->SR & UART_SR_RXNE)) {

            if (dummy_ticks > UART_MAX_TIMEOUT)
                return DRV_ERR_HW;

            dummy_ticks += 1;
        }

        // read data 
        data[i] = dev->regs->DR;
    }

    return DRV_OK;
}

int uart_write(void *handle, const uint8_t *data, size_t len) {

    struct uart_device *dev = (struct uart_device *)handle;

    if (!dev || !data || (len <= 0))    
        return DRV_ERR_INVALID;

    if (!dev->opened)
        return DRV_ERR_NOT_OPEN;
    
    for (size_t i = 0; i < len; i += 1) {

        uint32_t dummy_ticks = 0;
        while (!(dev->regs->SR & UART_SR_TXE)) {

            if (dummy_ticks > UART_MAX_TIMEOUT)
                return DRV_ERR_HW;

            dummy_ticks += 1;
        }

        // write byte
        dev->regs->DR = data[i];
    }
     
    return DRV_OK;
}

int uart_deinit(void *handle) {

    struct uart_device *dev = (struct uart_device *)handle;
    
    if (!dev)
        return DRV_ERR_INVALID;

    if (!dev->initialized)
        return DRV_ERR_INVALID;

    dev->regs->CR = 0;
    dev->regs->BRR = 0;
    dev->regs->DR = 0;

    dev->initialized = 0;

    if (dev->opened)
        dev->opened = 0;

    return DRV_OK;
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
