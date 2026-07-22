# 020. Interrupt Vector Table

## Difficulty: Medium

## Problem Statement

Implement an interrupt vector table (IVT) system — a core component of any embedded system. The IVT is an array of function pointers, one for each possible interrupt source. When an interrupt fires, the system uses the IRQ number to index into the table and call the appropriate handler.

You must implement:
1. `ivt_init()` — initialize all vectors to a default handler
2. `ivt_install_handler()` — install a handler for a given IRQ number
3. `ivt_remove_handler()` — restore the default handler for a given IRQ
4. `ivt_dispatch()` — simulate dispatching an interrupt (call the handler at IRQ index)

The system supports IRQ numbers 0 through `NUM_IRQS - 1` (where `NUM_IRQS` is 8).

Handler signature: `void (*)(int irq_num)` — a pointer to a function that takes an `int` IRQ number and returns void.

Since handlers return void, we verify behavior via a shared `irq_log` array that handlers write to.

## Function Signatures

```c
#define NUM_IRQS 8

void ivt_init(void);
int ivt_install_handler(int irq, void (*handler)(int irq_num));
int ivt_remove_handler(int irq);
int ivt_dispatch(int irq);
```

## Parameters

### ivt_install_handler
| Parameter | Type                      | Description                          |
|-----------|---------------------------|--------------------------------------|
| `irq`     | `int`                     | IRQ number (0 to NUM_IRQS-1)        |
| `handler` | `void (*)(int irq_num)`   | Function pointer to the ISR          |

### ivt_remove_handler / ivt_dispatch
| Parameter | Type  | Description              |
|-----------|-------|--------------------------|
| `irq`     | `int` | IRQ number (0 to NUM_IRQS-1) |

## Return Values

- `ivt_init`: void
- `ivt_install_handler`: 0 on success, -1 if IRQ out of range or handler is NULL
- `ivt_remove_handler`: 0 on success, -1 if IRQ out of range
- `ivt_dispatch`: 0 if handler was called, -1 if IRQ out of range

## Examples

```
ivt_init();                              // all vectors → default handler
ivt_install_handler(3, my_uart_isr);     // → 0 (success)
ivt_dispatch(3);                         // calls my_uart_isr(3)
ivt_remove_handler(3);                   // → 0 (back to default)
ivt_dispatch(3);                         // calls default handler
ivt_install_handler(8, some_handler);    // → -1 (out of range)
ivt_dispatch(-1);                        // → -1 (invalid)
```

## Constraints

- `NUM_IRQS` is 8 (IRQ numbers 0 through 7)
- Out-of-range IRQ numbers must return -1
- Installing a NULL handler must return -1
- After `ivt_remove_handler()`, the default handler is restored
- The default handler must be a real function (not NULL) that logs a default value

## Notes

- In ARM Cortex-M, the vector table lives at address 0x00000000 (or VTOR offset).
- In Linux, `request_irq()` / `free_irq()` install/remove interrupt handlers.
- The "default handler" pattern catches spurious/unhandled interrupts without crashing.
- No typedef is used — practice declaring arrays of raw function pointers.
