#include <stdint.h>
#include <stddef.h>

/* ============================================================
 * Register Definitions (from IP specification)
 * ============================================================ */

struct i2c_regs {
    volatile uint32_t CR;    /* 0x00: Control Register */
    volatile uint32_t SR;    /* 0x04: Status Register */
    volatile uint32_t DR;    /* 0x08: Data Register */
    volatile uint32_t CCR;   /* 0x0C: Clock Control Register */
};

/* CR register bits */
#define I2C_CR_EN     (1U << 0)   /* Peripheral enable */
#define I2C_CR_START  (1U << 1)   /* Generate START */
#define I2C_CR_STOP   (1U << 2)   /* Generate STOP */
#define I2C_CR_ACK    (1U << 3)   /* ACK enable */

/* SR register bits */
#define I2C_SR_BUSY   (1U << 0)   /* Bus busy */
#define I2C_SR_TXE    (1U << 1)   /* TX empty */
#define I2C_SR_RXNE   (1U << 2)   /* RX not empty */
#define I2C_SR_ADDR   (1U << 3)   /* Address ACK'd */
#define I2C_SR_NACK   (1U << 4)   /* NACK received */
#define I2C_SR_TC     (1U << 5)   /* Transfer complete */

/* System clock */
#define I2C_SYSTEM_CLOCK  16000000U

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
 * I2C Configuration & Device Handle
 * ============================================================ */

struct i2c_config {
    uint32_t clock_hz;     /* 100000 (standard) or 400000 (fast) */
    uint8_t  slave_addr;   /* 7-bit slave address (0x08-0x77) */
};

struct i2c_device {
    struct i2c_regs *regs;
    const struct driver_ops *ops;
    struct i2c_config config;
    uint8_t initialized;
    uint8_t opened;
};

/* ============================================================
 * TODO: Implement the I2C driver functions below
 * ============================================================ */

int i2c_init(void *handle, const void *config) {
    /* TODO: Implement */
    (void)handle;
    (void)config;
    return DRV_ERR_INVALID;
}

int i2c_open(void *handle) {
    /* TODO: Implement */
    (void)handle;
    return DRV_ERR_INVALID;
}

int i2c_close(void *handle) {
    /* TODO: Implement */
    (void)handle;
    return DRV_ERR_INVALID;
}

int i2c_read(void *handle, uint8_t *data, size_t len) {
    /* TODO: Implement */
    (void)handle;
    (void)data;
    (void)len;
    return DRV_ERR_INVALID;
}

int i2c_write(void *handle, const uint8_t *data, size_t len) {
    /* TODO: Implement */
    (void)handle;
    (void)data;
    (void)len;
    return DRV_ERR_INVALID;
}

int i2c_deinit(void *handle) {
    /* TODO: Implement */
    (void)handle;
    return DRV_ERR_INVALID;
}

/* Driver operations vtable */
const struct driver_ops i2c_ops = {
    .init   = i2c_init,
    .open   = i2c_open,
    .close  = i2c_close,
    .read   = i2c_read,
    .write  = i2c_write,
    .deinit = i2c_deinit,
};
