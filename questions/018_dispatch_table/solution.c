#include <stddef.h>

#define MAX_COMMANDS 8

/**
 * Command handler function type: takes an int argument, returns an int result.
 */
typedef int (*cmd_handler_fn)(int arg);

/**
 * @brief Initialize the dispatch table, clearing all entries to NULL.
 */
void init_dispatch_table(void) {
    /* TODO: Implement your solution here */
}

/**
 * @brief Register a command handler for the given command ID.
 *
 * @param cmd_id   Command ID (0 to MAX_COMMANDS-1).
 * @param handler  Function pointer to register.
 * @return         0 on success, -1 if cmd_id is out of range.
 */
int register_handler(int cmd_id, cmd_handler_fn handler) {
    /* TODO: Implement your solution here */
    (void)cmd_id;
    (void)handler;
    return -1;
}

/**
 * @brief Dispatch a command by looking up and calling the registered handler.
 *
 * @param cmd_id  Command ID to dispatch.
 * @param arg     Argument to pass to the handler.
 * @return        Handler return value, or -1 if no handler or invalid cmd_id.
 */
int dispatch_command(int cmd_id, int arg) {
    /* TODO: Implement your solution here */
    (void)cmd_id;
    (void)arg;
    return -1;
}
