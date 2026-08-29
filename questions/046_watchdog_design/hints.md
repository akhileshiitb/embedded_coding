# Hints

## Hint 1 — Part A (hardware watchdog)

These three functions are direct register writes. `wdt_enable` must set the reload value, load
the current counter, and turn on the enable bit — all three. `wdt_pet` writes the **magic
value** (`WDT_KICK_MAGIC`) to `KICK` (a plain value write, not a read-modify-write). `wdt_disable`
must clear *only* the enable bit — use a read-modify-write with `&= ~WDT_CTRL_EN` so you don't
stomp other CTRL bits.

## Hint 2 — Part B (task check-in bitmasks)

Each task owns one bit: `1U << task_id`. `register`/`unregister` set/clear that bit in
`s_registered_tasks`; `unregister` must also clear it from `s_checked_in_tasks` (a task that
left shouldn't count as "checked in"). `checkin` sets the bit in `s_checked_in_tasks`.

The heart is `wdt_task_check`: all registered tasks have checked in exactly when
`(s_checked_in_tasks & s_registered_tasks) == s_registered_tasks`. If so, feed and clear the
check-in mask; otherwise withhold. Note this condition is trivially true when nothing is
registered (0 == 0).

## Hint 3 — Full solution

```c
void wdt_enable(volatile struct wdt_regs *regs, uint32_t timeout_ticks) {
    regs->LOAD  = timeout_ticks;
    regs->COUNT = timeout_ticks;
    regs->CTRL |= WDT_CTRL_EN;
}

void wdt_pet(volatile struct wdt_regs *regs) {
    regs->KICK = WDT_KICK_MAGIC;
}

void wdt_disable(volatile struct wdt_regs *regs) {
    regs->CTRL &= ~WDT_CTRL_EN;
}

static uint32_t s_registered_tasks = 0;
static uint32_t s_checked_in_tasks = 0;

void wdt_task_init(void) {
    s_registered_tasks = 0;
    s_checked_in_tasks = 0;
}

void wdt_register_task(int task_id) {
    s_registered_tasks |= (1U << task_id);
}

void wdt_unregister_task(int task_id) {
    s_registered_tasks &= ~(1U << task_id);
    s_checked_in_tasks &= ~(1U << task_id);
}

void wdt_task_checkin(int task_id) {
    s_checked_in_tasks |= (1U << task_id);
}

int wdt_task_check(volatile struct wdt_regs *regs) {
    if ((s_checked_in_tasks & s_registered_tasks) == s_registered_tasks) {
        wdt_pet(regs);
        s_checked_in_tasks = 0;   /* start a fresh round */
        return 1;
    }
    return 0;
}
```

**Key reasoning:** the check-in layer never talks to the counter directly — it decides
*whether* to call `wdt_pet()`. That single indirection is what upgrades a naive watchdog (which
can't see deadlocks) into one that requires *every* registered task to prove forward progress
before the hardware is fed.
