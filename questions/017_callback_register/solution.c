#include <stddef.h>

/**
 * Callback function type: takes an int, returns an int.
 */
typedef int (*callback_fn)(int data);

/**
 * @brief Register a callback function pointer.
 *
 * Stores the given function pointer for later invocation.
 * Passing NULL deregisters any existing callback.
 *
 * @param cb  Function pointer to register, or NULL to deregister.
 */
void register_callback(callback_fn cb) {
    /* TODO: Implement your solution here */
    (void)cb;
}

/**
 * @brief Invoke the registered callback with the given data.
 *
 * @param data  Integer value to pass to the callback.
 * @return      Return value of the callback, or -1 if none registered.
 */
int invoke_callback(int data) {
    /* TODO: Implement your solution here */
    (void)data;
    return -1;
}
