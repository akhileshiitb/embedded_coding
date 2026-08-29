/*
 * Software timer module — multiplex N one-shot software timers onto ONE
 * hardware timer.
 *
 * Implement the three functions below. You DESIGN the internal data structure
 * yourself (how you store pending timers, find the soonest, and dispatch).
 * See hints.md for a comparison of data-structure choices.
 *
 * The hardware layer (provided by the test harness) offers:
 *     unsigned int hw_get_time(void);          // current absolute time (ticks)
 *     void         hw_timer_set(unsigned int expiry);  // arm the ONE hw timer
 * When the armed time is reached, the harness calls sw_timer_on_expire().
 *
 * Contract (see question.md for full spec):
 *   - One-shot timers only; support >= 16 concurrent pending timers.
 *   - sw_timer_start: record timer, arm hw for the soonest pending expiry.
 *   - sw_timer_on_expire: fire ALL due timers (expiry <= now), then re-arm
 *     the hw for the next-soonest remaining timer (or leave idle if none).
 *   - Callbacks are void (*)(void *arg), invoked as cb(arg).
 */

/* Provided by the test harness. */
extern unsigned int hw_get_time(void);
extern void         hw_timer_set(unsigned int expiry);

/* Reset the module to empty. */
void sw_timer_init(void) {
    /* TODO: Implement */
}

/* Register a one-shot software timer that fires cb(arg) at absolute `expiry`. */
void sw_timer_start(unsigned int expiry, void (*cb)(void *arg), void *arg) {
    /* TODO: Implement */
    (void)expiry;
    (void)cb;
    (void)arg;
}

/* Called by the hardware layer when the armed hardware timer fires. */
void sw_timer_on_expire(void) {
    /* TODO: Implement */
}
