# Hints

## Hint 1

Start with `init`: validate clock_hz (only 100000 or 400000) and slave_addr (0x08–0x77 valid range per I2C spec). Compute `CCR = 16MHz / (2 * clock_hz)` and write to CCR register. The address is stored in the config — it will be used during every write/read transaction to form the address byte.

## Hint 2

For `write`: the I2C protocol requires: START → address byte (slave_addr<<1 | 0 for write) → data bytes → STOP. Set CR.START, poll SR.BUSY, write address to DR, check SR.NACK (return -4 if NACK), poll SR.ADDR, then loop writing data bytes (poll TXE before each), poll TC, then set CR.STOP.

For `read`: same START + address byte (slave_addr<<1 | 1 for read), but then you set CR.ACK=1, read bytes (poll RXNE), and for the **last byte** clear CR.ACK before reading (sends NACK to slave to end transfer).

## Hint 3

```c
int i2c_write(void *handle, const uint8_t *data, size_t len) {
    // ... validate ...
    dev->regs->CR |= I2C_CR_START;
    while (!(dev->regs->SR & I2C_SR_BUSY)) {}
    dev->regs->DR = (dev->config.slave_addr << 1) | 0;  // addr + W
    if (dev->regs->SR & I2C_SR_NACK) { dev->regs->CR |= I2C_CR_STOP; return DRV_ERR_HW; }
    while (!(dev->regs->SR & I2C_SR_ADDR)) {}
    for (size_t i = 0; i < len; i++) {
        while (!(dev->regs->SR & I2C_SR_TXE)) {}
        dev->regs->DR = data[i];
    }
    while (!(dev->regs->SR & I2C_SR_TC)) {}
    dev->regs->CR |= I2C_CR_STOP;
    return DRV_OK;
}

int i2c_read(void *handle, uint8_t *data, size_t len) {
    // ... validate, START, address + R ...
    dev->regs->CR |= I2C_CR_ACK;
    for (size_t i = 0; i < len; i++) {
        if (i == len - 1) dev->regs->CR &= ~I2C_CR_ACK;  // NACK last byte
        while (!(dev->regs->SR & I2C_SR_RXNE)) {}
        data[i] = (uint8_t)(dev->regs->DR & 0xFF);
    }
    dev->regs->CR |= I2C_CR_STOP;
    return DRV_OK;
}
```
