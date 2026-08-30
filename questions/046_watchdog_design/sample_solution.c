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

    regs->LOAD = timeout_ticks;
    regs->COUNT = timeout_ticks;

    regs->CTRL |= WDT_CTRL_EN;
}

/**
 * @brief Feed (pet) the watchdog by writing the magic value to KICK.
 */
void wdt_pet(struct wdt_regs *regs) {

    regs->KICK = WDT_KICK_MAGIC;
}

/**
 * @brief Disable the watchdog (clear the enable bit only).
 */
void wdt_disable(struct wdt_regs *regs) {

    regs->CTRL &= ~WDT_CTRL_EN;

}

/* ============================================================
 * Part B: Task check-in layer
 * ============================================================ */
struct wdt_task_state {
    uint32_t reg_task_mask;
    uint32_t check_task_mask;
};

static struct wdt_task_state state;
/**
 * @brief Reset both bitmasks to zero.
 */
void wdt_task_init(void) {

    state = (struct wdt_task_state){0};
}

/**
 * @brief Register a task for monitoring (set its bit).
 */
void wdt_register_task(int task_id) {
    // bound check is not needed as task_id is [0,31] given in spec.
    state.reg_task_mask |= (1U << task_id);
}

/**
 * @brief Unregister a task (clear its bit in both masks).
 */
void wdt_unregister_task(int task_id) {
    state.reg_task_mask &= ~(1U << task_id);
    state.check_task_mask &= ~(1U << task_id);
}

/**
 * @brief Record that a task made forward progress (set its check-in bit).
 */
void wdt_task_checkin(int task_id) {
    state.check_task_mask |= 1U << task_id;
}

/**
 * @brief Watchdog task check. Feed the HW watchdog only if all registered
 *        tasks have checked in; if fed, clear the check-in mask.
 * @return 1 if the watchdog was fed, 0 if withheld.
 */
int wdt_task_check(struct wdt_regs *regs) {

    if (state.reg_task_mask == (state.reg_task_mask & state.check_task_mask)) {
        wdt_pet(regs); 
        state.check_task_mask = 0; // start next cycle. 
        return 1;
    }

    return 0;
}
