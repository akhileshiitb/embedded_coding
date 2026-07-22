# 018. Command Dispatch Table

## Difficulty: Medium

## Problem Statement

Implement a command dispatch system using a function pointer table. In firmware and kernel drivers, commands received over UART, SPI, or other interfaces are dispatched to handler functions via a lookup table indexed by command ID.

You must implement:
1. `init_dispatch_table()` — initialize the dispatch table with NULL entries
2. `register_handler()` — register a handler function for a given command ID
3. `dispatch_command()` — look up and execute the handler for a given command ID

The command handler has the signature: `int (*)(int arg)` — a pointer to a function that takes an `int` and returns an `int`.

The system supports command IDs from 0 to `MAX_COMMANDS - 1` (where `MAX_COMMANDS` is 8).

## Function Signatures

```c
#define MAX_COMMANDS 8

void init_dispatch_table(void);
int register_handler(int cmd_id, int (*handler)(int arg));
int dispatch_command(int cmd_id, int arg);
```

## Parameters

### register_handler
| Parameter | Type                  | Description                                  |
|-----------|-----------------------|----------------------------------------------|
| `cmd_id`  | `int`                 | Command ID (0 to MAX_COMMANDS-1)             |
| `handler` | `int (*)(int arg)`    | Function pointer to register for this command |

### dispatch_command
| Parameter | Type  | Description                        |
|-----------|-------|------------------------------------|
| `cmd_id`  | `int` | Command ID to dispatch             |
| `arg`     | `int` | Argument to pass to the handler    |

## Return Values

- `init_dispatch_table`: void
- `register_handler`: 0 on success, -1 if cmd_id is out of range
- `dispatch_command`: Return value from the handler, or -1 if cmd_id is out of range or no handler registered

## Examples

```
init_dispatch_table();
register_handler(0, my_read_handler);    → returns 0 (success)
register_handler(3, my_write_handler);   → returns 0 (success)
register_handler(8, some_handler);       → returns -1 (out of range)
dispatch_command(0, 42);                 → calls my_read_handler(42)
dispatch_command(3, 10);                 → calls my_write_handler(10)
dispatch_command(5, 0);                  → returns -1 (no handler)
```

## Constraints

- `MAX_COMMANDS` is 8 (command IDs 0 through 7)
- Out-of-range command IDs must return -1
- Unregistered commands must return -1 on dispatch
- Registering a handler for an already-registered slot replaces the old handler

## Notes

- This pattern is used in UART command parsers, I2C slave handlers, and USB device class drivers.
- The table is essentially an array of function pointers indexed by command ID.
- This is more efficient than a switch-case for large numbers of commands and allows runtime registration.
- No typedef is used — practice reading raw function pointer syntax.
