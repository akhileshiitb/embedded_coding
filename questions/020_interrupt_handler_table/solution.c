#include <stddef.h>

#define NUM_IRQS 8

/**
 * ISR handler function type: takes the IRQ number, returns void.
 */
typedef void (*isr_handler_fn)(int irq_num);

/**
 * Shared log array — handlers write to this to indicate they ran.
 * Index corresponds to IRQ number. Tests verify values here.
 * -1 = default handler ran, positive = custom handler ran.
 */
int irq_log[NUM_IRQS];

/**
 * @brief Initialize the interrupt vector table.
 *
 * Sets all vectors to the default handler and clears the irq_log.
 */
void ivt_init(void) {
    /* TODO: Implement your solution here */
}

/**
 * @brief Install a handler for the given IRQ number.
 *
 * @param irq      IRQ number (0 to NUM_IRQS-1).
 * @param handler  ISR function pointer (must not be NULL).
 * @return         0 on success, -1 if irq out of range or handler is NULL.
 */
int ivt_install_handler(int irq, isr_handler_fn handler) {
    /* TODO: Implement your solution here */
    (void)irq;
    (void)handler;
    return -1;
}

/**
 * @brief Remove the handler for the given IRQ, restoring the default.
 *
 * @param irq  IRQ number (0 to NUM_IRQS-1).
 * @return     0 on success, -1 if irq out of range.
 */
int ivt_remove_handler(int irq) {
    /* TODO: Implement your solution here */
    (void)irq;
    return -1;
}

/**
 * @brief Dispatch (simulate) an interrupt by calling the registered handler.
 *
 * @param irq  IRQ number (0 to NUM_IRQS-1).
 * @return     0 if handler was called, -1 if irq out of range.
 */
int ivt_dispatch(int irq) {
    /* TODO: Implement your solution here */
    (void)irq;
    return -1;
}
