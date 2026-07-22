# Hints

## Hint 1

Define a function for each state (e.g., `state_idle`, `state_running`, `state_error`). Each function takes the event and data, and handles transitions for that state. Store the current state as a function pointer variable. When a transition occurs, reassign the pointer to the new state's function.

## Hint 2

Use a `typedef int (*state_handler_fn)(int event, int data);` for the function pointer type. Keep a static variable `static state_handler_fn current_state;` and a static counter. Each state handler checks the event: if valid for that state, perform the transition (change `current_state`) and return 0. If invalid, return 1. Check for EVENT_ERROR in every state since it's always valid.

## Hint 3

```c
static state_handler_fn current_state;
static int counter;

static int state_idle(int event, int data);
static int state_running(int event, int data);
static int state_error(int event, int data);

static int state_idle(int event, int data) {
    (void)data;
    if (event == EVENT_START) { current_state = state_running; return 0; }
    if (event == EVENT_ERROR) { current_state = state_error; return 0; }
    return 1;
}
static int state_running(int event, int data) {
    if (event == EVENT_STOP) { current_state = state_idle; return 0; }
    if (event == EVENT_DATA) { counter += data; return 0; }
    if (event == EVENT_ERROR) { current_state = state_error; return 0; }
    return 1;
}
static int state_error(int event, int data) {
    (void)data;
    if (event == EVENT_RESET) { current_state = state_idle; counter = 0; return 0; }
    if (event == EVENT_ERROR) { return 0; }
    return 1;
}
void sm_init(void) { current_state = state_idle; counter = 0; }
int sm_process_event(int event, int data) {
    if (event < 0 || event > 4) return -1;
    return current_state(event, data);
}
int sm_get_state(void) {
    if (current_state == state_idle) return STATE_IDLE;
    if (current_state == state_running) return STATE_RUNNING;
    return STATE_ERROR;
}
int sm_get_counter(void) { return counter; }
```
