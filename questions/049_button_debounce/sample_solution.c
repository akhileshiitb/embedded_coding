/*
 * Sample solution — button debouncer (counter method).
 *
 * Track the committed debounced state, plus a run-length counter of how many
 * consecutive samples have disagreed with it. When that reaches DEBOUNCE_N,
 * commit the new level. A committed 0->1 transition raises a one-shot press
 * event.
 */

#define DEBOUNCE_N 4

static int s_state;        /* committed debounced state (0/1) */
static int s_counter;      /* consecutive samples equal to the opposite level */
static int s_press_event;  /* set on a committed 0->1 transition, cleared on read */

void debounce_init(void) {
    s_state = 0;
    s_counter = 0;
    s_press_event = 0;
}

int debounce_update(int raw_sample) {
    raw_sample = raw_sample ? 1 : 0;

    if (raw_sample != s_state) {
        /* input disagrees with committed state: count toward a change */
        s_counter++;
        if (s_counter >= DEBOUNCE_N) {
            int prev = s_state;
            s_state = raw_sample;      /* commit the new level */
            s_counter = 0;
            if (prev == 0 && s_state == 1)
                s_press_event = 1;     /* released -> pressed */
        }
    } else {
        /* input agrees with committed state: discard any pending bounce */
        s_counter = 0;
    }
    return s_state;
}

int debounce_pressed_event(void) {
    int e = s_press_event;
    s_press_event = 0;                 /* one-shot: consume the event */
    return e;
}
