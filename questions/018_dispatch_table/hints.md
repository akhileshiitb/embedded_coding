# Hints

## Hint 1

You need an array of function pointers, where each index corresponds to a command ID. The array type is `cmd_handler_fn dispatch_table[MAX_COMMANDS]`. Initialize all entries to NULL in `init_dispatch_table()`.

## Hint 2

In `register_handler()`, validate that `cmd_id` is within bounds (0 to MAX_COMMANDS-1), then store the handler at `dispatch_table[cmd_id]`. In `dispatch_command()`, validate bounds and check for NULL before calling the function pointer.

## Hint 3

```c
static cmd_handler_fn dispatch_table[MAX_COMMANDS];

void init_dispatch_table(void) {
    for (int i = 0; i < MAX_COMMANDS; i++)
        dispatch_table[i] = NULL;
}

int register_handler(int cmd_id, cmd_handler_fn handler) {
    if (cmd_id < 0 || cmd_id >= MAX_COMMANDS) return -1;
    dispatch_table[cmd_id] = handler;
    return 0;
}

int dispatch_command(int cmd_id, int arg) {
    if (cmd_id < 0 || cmd_id >= MAX_COMMANDS) return -1;
    if (dispatch_table[cmd_id] == NULL) return -1;
    return dispatch_table[cmd_id](arg);
}
```
