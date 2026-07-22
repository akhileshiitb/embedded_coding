# Hints

## Hint 1

You need a way to persist the function pointer between calls to `register_callback()` and `invoke_callback()`. In C, a `static` variable inside a file (or inside a function) retains its value across function calls. Declare a static variable of type `callback_fn`.

## Hint 2

Store the callback in a file-scope static variable: `static callback_fn registered_cb = NULL;`. In `register_callback()`, assign `cb` to this variable. In `invoke_callback()`, check if the stored pointer is NULL before calling it.

## Hint 3

```c
static callback_fn registered_cb = NULL;

void register_callback(callback_fn cb) {
    registered_cb = cb;
}

int invoke_callback(int data) {
    if (registered_cb != NULL) {
        return registered_cb(data);
    }
    return -1;
}
```
The static variable persists across calls. The NULL check prevents calling through an invalid pointer.
