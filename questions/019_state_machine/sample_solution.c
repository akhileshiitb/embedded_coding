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
static struct drv_data_t {
    int (*curr_state_handler)(int event, int data);
    int counter;
    int curr_state;
} drv_data;

// function prototypes

int idle_handler(int event, int data);
int running_handler(int event, int data);
int error_handler(int event, int data);

int idle_handler(int event, int data)
{
    switch(event) {
    case EVENT_START: 
        drv_data.curr_state_handler = running_handler;
        drv_data.curr_state = STATE_RUNNING;
        break;

    case EVENT_ERROR:
        drv_data.curr_state_handler = error_handler;
        drv_data.curr_state = STATE_ERROR;
        break;
    default:
            return 1;
    }

    (void)data;
    return 0;
}

int running_handler(int event, int data)
{

    switch(event) {
    case EVENT_STOP:
        drv_data.curr_state_handler = idle_handler;
        drv_data.curr_state = STATE_IDLE;
        break;

    case EVENT_DATA:
        // no state change
        drv_data.counter += data;
        break;

    case EVENT_ERROR:
        drv_data.curr_state_handler = error_handler;
        drv_data.curr_state = STATE_ERROR;
        break;

    default:
        return 1;
    }

    return 0;

}

int error_handler(int event, int data)
{
    switch(event) {
        case EVENT_RESET:
            drv_data.curr_state_handler = idle_handler;
            drv_data.counter = 0;
            drv_data.curr_state = STATE_IDLE;
            break;
        case EVENT_ERROR:
            // no state change
            break;
        default:
            return 1;
    }
    (void)data;
    return 0;
}

/**
 * @brief Initialize the state machine to IDLE with counter = 0.
 */
void sm_init(void) {
    /* TODO: Implement your solution here */
    drv_data.curr_state_handler = idle_handler;
    drv_data.counter = 0;
    drv_data.curr_state = STATE_IDLE;
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
    // error handling on event. 
    if ((event < 0) || (event > 4))
        return -1;

    return (*drv_data.curr_state_handler)(event, data);
}

/**
 * @brief Get the current state as an integer.
 * @return STATE_IDLE (0), STATE_RUNNING (1), or STATE_ERROR (2).
 */
int sm_get_state(void) {
    /* TODO: Implement your solution here */
    return drv_data.curr_state;
}

/**
 * @brief Get the accumulated data counter.
 * @return Current counter value.
 */
int sm_get_counter(void) {
    /* TODO: Implement your solution here */
    return drv_data.counter;
}
