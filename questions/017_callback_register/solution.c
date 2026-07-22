#include <stddef.h>

/**
 * @brief Register a callback function pointer.
 *
 * Stores the given function pointer for later invocation.
 * Passing NULL deregisters any existing callback.
 *
 * @param cb  Function pointer to register (int (*)(int)), or NULL to deregister.
 */
void register_callback(int (*cb)(int data)) {
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
