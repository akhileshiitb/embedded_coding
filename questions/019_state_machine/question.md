# 019. State Machine with Function Pointers

## Difficulty: Medium

## Problem Statement

Implement a simple state machine using function pointers to represent state handlers. Each state is a function that processes an event and returns a status code. This is a common firmware pattern for protocol parsers, motor controllers, and UI logic.

The state machine has these states:
- **IDLE** — waiting for a START event
- **RUNNING** — processing data, can STOP or receive DATA
- **ERROR** — recoverable via RESET event

Events are integers:
- `EVENT_START` (0) — transitions from IDLE to RUNNING
- `EVENT_STOP` (1) — transitions from RUNNING to IDLE
- `EVENT_DATA` (2) — processed while in RUNNING (stays in RUNNING, accumulates value)
- `EVENT_ERROR` (3) — any state transitions to ERROR
- `EVENT_RESET` (4) — transitions from ERROR to IDLE

You must implement:
1. `sm_init()` — initialize the state machine to IDLE state with counter = 0
2. `sm_process_event()` — process an event, transition state, return a status code
3. `sm_get_state()` — return the current state as an integer (0=IDLE, 1=RUNNING, 2=ERROR)
4. `sm_get_counter()` — return the accumulated data counter

## Function Signatures

```c
void sm_init(void);
int sm_process_event(int event, int data);
int sm_get_state(void);
int sm_get_counter(void);
```

## Event/State Constants

```c
#define EVENT_START  0
#define EVENT_STOP   1
#define EVENT_DATA   2
#define EVENT_ERROR  3
#define EVENT_RESET  4

#define STATE_IDLE    0
#define STATE_RUNNING 1
#define STATE_ERROR   2
```

## Return Values for sm_process_event

- `0` — event processed successfully (state transitioned)
- `1` — event ignored (not valid for current state)
- `-1` — invalid event ID

## Examples

```
sm_init();
sm_get_state();                    → 0 (IDLE)
sm_process_event(EVENT_START, 0);  → 0 (now RUNNING)
sm_process_event(EVENT_DATA, 5);   → 0 (still RUNNING, counter = 5)
sm_process_event(EVENT_DATA, 3);   → 0 (still RUNNING, counter = 8)
sm_get_counter();                  → 8
sm_process_event(EVENT_STOP, 0);   → 0 (now IDLE)
sm_process_event(EVENT_STOP, 0);   → 1 (ignored, already IDLE)
```

## Constraints

- Only the transitions described above are valid
- Invalid events (< 0 or > 4) return -1
- Events that don't apply to the current state return 1 (ignored)
- `EVENT_ERROR` is valid from any state and always transitions to ERROR
- DATA events accumulate (add) the data parameter to the counter
- RESET clears the counter back to 0

## Notes

- Internally, represent each state as a handler function with signature: `int (*)(int event, int data)`
- The current state is stored as a function pointer that gets reassigned on transitions.
- This avoids large switch-case blocks and makes adding states trivial.
- No typedef is used — practice declaring and assigning raw function pointers.
