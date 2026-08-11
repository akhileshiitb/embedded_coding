# Hints

## Hint 1

Start with `init`: validate the config struct fields (baud > 0, data_bits ∈ {7,8}, stop_bits ∈ {1,2}, parity ∈ {0,1,2}), then compute the baud rate divisor as `SYSTEM_CLOCK / baud_rate` and write it to `regs->BRR`. Remember to set the `initialized` flag. Every other function should check this flag before proceeding.

## Hint 2

The lifecycle enforcement is the key design pattern: `open` checks `initialized==1`, `read`/`write` check `opened==1`, `close` checks `opened==1`. For `open`, set all three CR bits (`EN | TXEN | RXEN`). For `write`, poll `regs->SR & TXE` in a loop, then write each byte to `regs->DR`. For `read`, poll `regs->SR & RXNE`, then read `regs->DR` into the buffer.

## Hint 3

```c
int uart_init(void *handle, const void *config) {
    if (!handle || !config) return DRV_ERR_INVALID;
    struct uart_device *dev = (struct uart_device *)handle;
    const struct uart_config *cfg = (const struct uart_config *)config;
    if (cfg->baud_rate == 0) return DRV_ERR_INVALID;
    if (cfg->data_bits != 7 && cfg->data_bits != 8) return DRV_ERR_INVALID;
    if (cfg->stop_bits != 1 && cfg->stop_bits != 2) return DRV_ERR_INVALID;
    if (cfg->parity > 2) return DRV_ERR_INVALID;
    dev->config = *cfg;
    dev->regs->BRR = UART_SYSTEM_CLOCK / cfg->baud_rate;
    dev->initialized = 1;
    return DRV_OK;
}

int uart_open(void *handle) {
    if (!handle) return DRV_ERR_INVALID;
    struct uart_device *dev = (struct uart_device *)handle;
    if (!dev->initialized) return DRV_ERR_INVALID;
    dev->regs->CR = UART_CR_EN | UART_CR_TXEN | UART_CR_RXEN;
    dev->opened = 1;
    return DRV_OK;
}

int uart_write(void *handle, const uint8_t *data, size_t len) {
    if (!handle || !data || len == 0) return DRV_ERR_INVALID;
    struct uart_device *dev = (struct uart_device *)handle;
    if (!dev->opened) return DRV_ERR_NOT_OPEN;
    for (size_t i = 0; i < len; i++) {
        while (!(dev->regs->SR & UART_SR_TXE)) { /* poll */ }
        dev->regs->DR = data[i];
    }
    return DRV_OK;
}
// ... similar for read (poll RXNE, read DR), close (clear CR), deinit (clear all).
```
