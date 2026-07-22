#include <stddef.h>

#define EVENT_START  0
#define EVENT_STOP   1
#define EVENT_DATA   2
#define EVENT_ERROR  3
#define EVENT_RESET  4

#define STATE_IDLE    0
#define STATE_RUNNING 1
#define STATE_ERROR   2

/**
 * State handler function signature: int (*)(int event, int data)
 * Takes an event ID and data, returns:
 *   0 = event processed (transition occurred)
 *   1 = event ignored (not valid for this state)
 */

/**
 * @brief Initialize the state machine to IDLE with counter = 0.
 */
void sm_init(void) {
    /* TODO: Implement your solution here */
}

/**
 * @brief Process an event in the current state.
 *
 * @param event  Event ID (EVENT_START, EVENT_STOP, EVENT_DATA, EVENT_ERROR, EVENT_RESET).
 * @param data   Data associated with the event (used by EVENT_DATA).
 * @return       0 if processed, 1 if ignored, -1 if invalid event.
 */
int sm_process_event(int event, int data) {
    /* TODO: Implement your solution here */
    (void)event;
    (void)data;
    return -1;
}

/**
 * @brief Get the current state as an integer.
 * @return STATE_IDLE (0), STATE_RUNNING (1), or STATE_ERROR (2).
 */
int sm_get_state(void) {
    /* TODO: Implement your solution here */
    return 0;
}

/**
 * @brief Get the accumulated data counter.
 * @return Current counter value.
 */
int sm_get_counter(void) {
    /* TODO: Implement your solution here */
    return 0;
}
