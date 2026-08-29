/*
 * Sample solution — N software timers on one hardware timer.
 *
 * Data structure: unsorted fixed array (pragmatic for small N).
 *   - start:  O(1) insert + O(N) re-arm scan
 *   - dispatch: O(N) scan for due timers + O(N) re-arm scan
 * For thousands of timers, swap the array for a min-heap or timer wheel;
 * the three-function shape stays identical.
 */

extern unsigned int hw_get_time(void);
extern void         hw_timer_set(unsigned int expiry);

#define MAX_TIMERS 16

struct sw_timer {
    unsigned int expiry;
    void (*cb)(void *arg);
    void *arg;
    int  active;
};

static struct sw_timer s_timers[MAX_TIMERS];

/* Arm the hardware for the soonest active timer (if any). */
static void arm_next(void) {
    int found = 0;
    unsigned int soonest = 0;
    for (int i = 0; i < MAX_TIMERS; i++) {
        if (s_timers[i].active) {
            if (!found || s_timers[i].expiry < soonest) {
                soonest = s_timers[i].expiry;
                found = 1;
            }
        }
    }
    if (found) hw_timer_set(soonest);
    /* if none active, leave the hardware idle */
}

void sw_timer_init(void) {
    for (int i = 0; i < MAX_TIMERS; i++) s_timers[i].active = 0;
}

void sw_timer_start(unsigned int expiry, void (*cb)(void *arg), void *arg) {
    for (int i = 0; i < MAX_TIMERS; i++) {
        if (!s_timers[i].active) {
            s_timers[i].expiry = expiry;
            s_timers[i].cb = cb;
            s_timers[i].arg = arg;
            s_timers[i].active = 1;
            break;
        }
    }
    arm_next();
}

void sw_timer_on_expire(void) {
    unsigned int now = hw_get_time();
    for (int i = 0; i < MAX_TIMERS; i++) {
        if (s_timers[i].active && s_timers[i].expiry <= now) {
            void (*cb)(void *) = s_timers[i].cb;
            void *arg = s_timers[i].arg;
            s_timers[i].active = 0;   /* one-shot: remove before firing */
            cb(arg);
        }
    }
    arm_next();
}
