# 017. Callback Registration

## Difficulty: Easy

## Problem Statement

Implement a simple callback registration system. In embedded systems and kernel drivers, modules often register callback functions that are invoked when specific events occur (e.g., data received, timer expired, interrupt fired).

You must implement two functions:
1. `register_callback()` — stores a function pointer for later use
2. `invoke_callback()` — calls the registered callback with provided data

The callback has the signature: `int (*callback_fn)(int data)`

If no callback is registered (NULL), `invoke_callback()` should return -1.

## Function Signatures

```c
void register_callback(callback_fn cb);
int invoke_callback(int data);
```

## Type Definitions

```c
typedef int (*callback_fn)(int data);
```

## Parameters

### register_callback
| Parameter | Type          | Description                    |
|-----------|---------------|--------------------------------|
| `cb`      | `callback_fn` | Function pointer to register (may be NULL) |

### invoke_callback
| Parameter | Type  | Description                          |
|-----------|-------|--------------------------------------|
| `data`    | `int` | Data to pass to the registered callback |

## Return Value

- `register_callback`: void (no return)
- `invoke_callback`: The return value of the callback, or -1 if no callback is registered

## Examples

### Example 1
```
// Given: int double_it(int x) { return x * 2; }
register_callback(double_it);
invoke_callback(5);  → returns 10
```

### Example 2
```
// No callback registered
invoke_callback(5);  → returns -1
```

### Example 3
```
// Given: int negate(int x) { return -x; }
register_callback(negate);
invoke_callback(7);  → returns -7
```

## Constraints

- Only one callback is active at a time (new registration replaces old)
- Registering NULL effectively deregisters the callback
- The system must handle being called before any registration

## Notes

- This pattern is the foundation of event-driven firmware design.
- In Linux kernel, `request_irq()` registers interrupt handler callbacks.
- Use a static/global variable to store the function pointer.
