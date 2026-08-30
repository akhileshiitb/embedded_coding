#include <stdint.h>

/* ============================================================
 * Simulated WDT hardware register interface
 * ============================================================ */
struct wdt_regs {
    volatile uint32_t CTRL;    /* Control: bit0 = EN */
    volatile uint32_t LOAD;    /* Reload value (timeout in ticks) */
    volatile uint32_t COUNT;   /* Current down-counter */
    volatile uint32_t KICK;    /* Feed register (write magic to reload) */
    volatile uint32_t RESET;   /* Sticky reset flag: bit0 set on watchdog reset */
};

#define WDT_CTRL_EN     (1U << 0)
#define WDT_KICK_MAGIC  0x5A5A5A5AU
#define WDT_RESET_FLAG  (1U << 0)

/* ============================================================
 * Part A: Hardware watchdog driver
 * ============================================================ */

/**
 * @brief Enable the watchdog with the given timeout (in ticks).
 *        Set LOAD and COUNT to timeout_ticks, then set the enable bit.
 */
void wdt_enable(struct wdt_regs *regs, uint32_t timeout_ticks) {
    /* TODO: Implement */
    (void)regs;
    (void)timeout_ticks;
}

/**
 * @brief Feed (pet) the watchdog by writing the magic value to KICK.
 */
void wdt_pet(struct wdt_regs *regs) {
    /* TODO: Implement */
    (void)regs;
}

/**
 * @brief Disable the watchdog (clear the enable bit only).
 */
void wdt_disable(struct wdt_regs *regs) {
    /* TODO: Implement */
    (void)regs;
}

/* ============================================================
 * Part B: Task check-in layer
 * ============================================================ */

/**
 * @brief Reset both bitmasks to zero.
 */
void wdt_task_init(void) {
    /* TODO: Implement */
}

/**
 * @brief Register a task for monitoring (set its bit).
 */
void wdt_register_task(int task_id) {
    /* TODO: Implement */
    (void)task_id;
}

/**
 * @brief Unregister a task (clear its bit in both masks).
 */
void wdt_unregister_task(int task_id) {
    /* TODO: Implement */
    (void)task_id;
}

/**
 * @brief Record that a task made forward progress (set its check-in bit).
 */
void wdt_task_checkin(int task_id) {
    /* TODO: Implement */
    (void)task_id;
}

/**
 * @brief Watchdog task check. Feed the HW watchdog only if all registered
 *        tasks have checked in; if fed, clear the check-in mask.
 * @return 1 if the watchdog was fed, 0 if withheld.
 */
int wdt_task_check(struct wdt_regs *regs) {
    /* TODO: Implement */
    (void)regs;
    return 0;
}
