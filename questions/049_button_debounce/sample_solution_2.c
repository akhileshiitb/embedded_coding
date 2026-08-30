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

#define DEBOUNCE_N 4
#define STATE_REL   0
#define STATE_PRESS 1

static char data;
static char priv_state;
static char curr_state;
static char event_press;

/* Reset: debounced state = released (0); clear counters/edge state. */
void debounce_init(void) {
    /* TODO: Implement */
    data = 0;
    priv_state = STATE_REL;
    curr_state = STATE_REL;
    event_press = 0;
}

/* Feed one raw sample (0/1). Return the current debounced state (0/1). */
int debounce_update(int raw_sample) {
    /* TODO: Implement */
    // update data
    char mask = (1UL << DEBOUNCE_N) - 1;

    data = (data << 1U) | (raw_sample & 0x1);
    
    if ((data & mask) == mask) {
        // Transition to PRESS state if not already
        if (curr_state != STATE_PRESS) {
            priv_state = curr_state;
            curr_state = STATE_PRESS;
            event_press = 1;
        }
    } else if ((data & mask) == 0){
        // Transition to REL state 
        if (curr_state != STATE_REL) {
            priv_state = curr_state;
            curr_state = STATE_REL;
        }
    } else {
        // no state change : debounding logic in play
    }

    return curr_state;
}

/* Return 1 exactly once per committed released->pressed transition, else 0. */
int debounce_pressed_event(void) {
    /* TODO: Implement */

    if (event_press) {
        event_press = 0;
        return 1; 
    }

    return 0;
}
