/*
 * Button debouncer.
 *
 * Turn a noisy raw button signal into a clean debounced state, and report a
 * press event once per real press. You DESIGN the internal state yourself.
 *
 * Rule: the debounced state changes only after the raw input holds the new
 * level for DEBOUNCE_N = 4 consecutive samples.
 *   raw sample: 0 = released, 1 = pressed
 *   initial debounced state: 0 (released)
 *
 * See debounce_design.md for the counter and shift-register methods.
 */

/* Reset: debounced state = released (0); clear counters/edge state. */
void debounce_init(void) {
    /* TODO: Implement */
}

/* Feed one raw sample (0/1). Return the current debounced state (0/1). */
int debounce_update(int raw_sample) {
    /* TODO: Implement */
    (void)raw_sample;
    return 0;
}

/* Return 1 exactly once per committed released->pressed transition, else 0. */
int debounce_pressed_event(void) {
    /* TODO: Implement */
    return 0;
}
