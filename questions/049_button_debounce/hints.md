# Hints

## Hint 1

Track two things as file-scope `static`: the **committed debounced state** (0/1) and a **counter**
of how many recent samples disagree with it. Each `debounce_update`, compare the raw sample to the
committed state:
- disagrees → increment the counter,
- agrees → reset the counter (any bounce toward the other level is discarded).
When the counter hits `DEBOUNCE_N` (4), commit the new level and reset the counter.

## Hint 2

For the **press event**: when you commit a change, check whether it was a `0 -> 1` transition
(released → pressed). If so, set a `press_event` flag. `debounce_pressed_event()` returns that flag
and clears it (one-shot — consumed on read), so each real press yields exactly one event.

Alternative to the counter: the **shift-register method** —
`history = (history << 1) | raw; if ((history & 0xF) == 0xF) state = 1; else if ((history & 0xF) == 0) state = 0;`
(low 4 bits all-1 = stable pressed, all-0 = stable released). Both need 4 stable samples.

## Hint 3

```c
#define DEBOUNCE_N 4

static int s_state;        /* committed debounced state */
static int s_counter;      /* consecutive samples disagreeing with s_state */
static int s_press_event;  /* one-shot flag for 0->1 commit */

void debounce_init(void) {
    s_state = 0; s_counter = 0; s_press_event = 0;
}

int debounce_update(int raw_sample) {
    raw_sample = raw_sample ? 1 : 0;
    if (raw_sample != s_state) {
        if (++s_counter >= DEBOUNCE_N) {
            int prev = s_state;
            s_state = raw_sample;          /* commit */
            s_counter = 0;
            if (prev == 0 && s_state == 1) s_press_event = 1;
        }
    } else {
        s_counter = 0;                     /* agrees: discard pending bounce */
    }
    return s_state;
}

int debounce_pressed_event(void) {
    int e = s_press_event;
    s_press_event = 0;                     /* consume */
    return e;
}
```

**Key reasoning:** the counter only advances while the input *disagrees* with the committed state,
and any single agreeing sample resets it — so bounce (which flips back and forth) never
accumulates 4 in a row. Only a genuinely stable new level reaches the threshold and commits. The
event is just an edge check on the committed level.
