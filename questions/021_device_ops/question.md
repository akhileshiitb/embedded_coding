# 021. Device Operations Struct (Linux Kernel Style)

## Difficulty: Medium

## Problem Statement

Implement the Linux kernel's "operations struct" pattern — the most important use of function pointers in Linux device drivers. In the kernel, every device driver provides a struct full of function pointers (`file_operations`, `net_device_ops`, `block_device_operations`, etc.) that implement a standard interface.

You must implement:
1. `device_init()` — initialize a device context
2. `device_register_ops()` — attach an operations struct to the device
3. `device_open()` — call the device's open operation
4. `device_read()` — call the device's read operation
5. `device_write()` — call the device's write operation
6. `device_close()` — call the device's close operation

Each operation is optional (can be NULL). If an operation is not implemented, the function returns -1.

## Type Definitions

```c
struct device_ops {
    int (*open)(int flags);
    int (*read)(char *buf, int len);
    int (*write)(const char *buf, int len);
    int (*close)(void);
};

struct device {
    const char *name;
    int is_open;
    const struct device_ops *ops;
};
```

## Function Signatures

```c
void device_init(struct device *dev, const char *name);
int device_register_ops(struct device *dev, const struct device_ops *ops);
int device_open(struct device *dev, int flags);
int device_read(struct device *dev, char *buf, int len);
int device_write(struct device *dev, const char *buf, int len);
int device_close(struct device *dev);
```

## Return Values

- `device_init`: void
- `device_register_ops`: 0 on success, -1 if dev or ops is NULL
- `device_open`: return value from ops->open, or -1 if no ops/no open/already open
- `device_read`: return value from ops->read, or -1 if not open or no read op
- `device_write`: return value from ops->write, or -1 if not open or no write op
- `device_close`: return value from ops->close (or 0 if no close op), -1 if not open

## Rules

1. `device_open()` fails if the device is already open (return -1)
2. `device_read()`/`device_write()` fail if the device is not open (return -1)
3. `device_close()` marks the device as closed; if no `close` op is provided, it still succeeds (return 0)
4. After `device_close()`, read/write must fail until re-opened

## Examples

```
struct device dev;
device_init(&dev, "uart0");
device_register_ops(&dev, &uart_ops);   // → 0
device_open(&dev, 0);                    // → calls uart_ops.open(0)
device_read(&dev, buf, 10);              // → calls uart_ops.read(buf, 10)
device_close(&dev);                      // → calls uart_ops.close()
device_read(&dev, buf, 10);              // → -1 (device closed)
```

## Constraints

- NULL device pointer returns -1 from all functions
- Operations struct may have NULL function pointers (optional operations)
- A device can only be opened once at a time
- read/write require the device to be open

## Notes

- This is how `struct file_operations` works in Linux: `open`, `read`, `write`, `release`.
- The pattern decouples interface from implementation — multiple drivers implement the same ops struct.
- In the kernel, this enables polymorphism in C (different devices, same interface).
