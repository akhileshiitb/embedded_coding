#include <stddef.h>

/**
 * Device operations struct — function pointers for device I/O.
 * Any pointer may be NULL (operation not supported).
 */
struct device_ops {
    int (*open)(int flags);
    int (*read)(char *buf, int len);
    int (*write)(const char *buf, int len);
    int (*close)(void);
};

/**
 * Device context struct.
 */
struct device {
    const char *name;
    int is_open;
    const struct device_ops *ops;
};

/**
 * @brief Initialize a device context.
 *
 * @param dev   Pointer to device struct to initialize.
 * @param name  Device name string.
 */
void device_init(struct device *dev, const char *name) {
    /* TODO: Implement your solution here */
    (void)dev;
    (void)name;
}

/**
 * @brief Register an operations struct with the device.
 *
 * @param dev  Pointer to device struct.
 * @param ops  Pointer to operations struct.
 * @return     0 on success, -1 if dev or ops is NULL.
 */
int device_register_ops(struct device *dev, const struct device_ops *ops) {
    /* TODO: Implement your solution here */
    (void)dev;
    (void)ops;
    return -1;
}

/**
 * @brief Open the device.
 *
 * @param dev    Pointer to device struct.
 * @param flags  Flags to pass to the open operation.
 * @return       Return value from open op, or -1 on error.
 */
int device_open(struct device *dev, int flags) {
    /* TODO: Implement your solution here */
    (void)dev;
    (void)flags;
    return -1;
}

/**
 * @brief Read from the device.
 *
 * @param dev  Pointer to device struct.
 * @param buf  Buffer to read into.
 * @param len  Number of bytes to read.
 * @return     Return value from read op, or -1 on error.
 */
int device_read(struct device *dev, char *buf, int len) {
    /* TODO: Implement your solution here */
    (void)dev;
    (void)buf;
    (void)len;
    return -1;
}

/**
 * @brief Write to the device.
 *
 * @param dev  Pointer to device struct.
 * @param buf  Buffer to write from.
 * @param len  Number of bytes to write.
 * @return     Return value from write op, or -1 on error.
 */
int device_write(struct device *dev, const char *buf, int len) {
    /* TODO: Implement your solution here */
    (void)dev;
    (void)buf;
    (void)len;
    return -1;
}

/**
 * @brief Close the device.
 *
 * @param dev  Pointer to device struct.
 * @return     Return value from close op (or 0 if no close), -1 if not open.
 */
int device_close(struct device *dev) {
    /* TODO: Implement your solution here */
    (void)dev;
    return -1;
}
