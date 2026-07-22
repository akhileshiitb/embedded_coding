# Hints

## Hint 1

You need an array of function pointers: `static isr_handler_fn vector_table[NUM_IRQS];`. You also need a default handler function that writes -1 to `irq_log[irq_num]`. In `ivt_init()`, set every entry in the vector table to this default handler and clear `irq_log`.

## Hint 2

Define a static function `default_handler(int irq_num) { irq_log[irq_num] = -1; }`. In `ivt_init()`, loop through all entries setting them to `default_handler`. In `ivt_install_handler()`, validate `irq` bounds and non-NULL handler, then store it. In `ivt_remove_handler()`, restore `default_handler`. In `ivt_dispatch()`, call `vector_table[irq](irq)`.

## Hint 3

```c
static isr_handler_fn vector_table[NUM_IRQS];

static void default_handler(int irq_num) {
    irq_log[irq_num] = -1;
}

void ivt_init(void) {
    for (int i = 0; i < NUM_IRQS; i++) {
        vector_table[i] = default_handler;
        irq_log[i] = 0;
    }
}

int ivt_install_handler(int irq, isr_handler_fn handler) {
    if (irq < 0 || irq >= NUM_IRQS || handler == NULL) return -1;
    vector_table[irq] = handler;
    return 0;
}

int ivt_remove_handler(int irq) {
    if (irq < 0 || irq >= NUM_IRQS) return -1;
    vector_table[irq] = default_handler;
    return 0;
}

int ivt_dispatch(int irq) {
    if (irq < 0 || irq >= NUM_IRQS) return -1;
    vector_table[irq](irq);
    return 0;
}
```
