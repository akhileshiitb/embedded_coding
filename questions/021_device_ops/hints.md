# Hints

## Hint 1

The key insight: `struct device_ops` is a vtable (virtual function table) — a struct of function pointers that provides polymorphism in C. Each device has a pointer to its own ops struct. The `device_*` functions check the ops pointer and the specific function pointer before calling.

## Hint 2

In `device_init()`, set `dev->name = name`, `dev->is_open = 0`, `dev->ops = NULL`. In each operation function, guard with: (1) check `dev` is not NULL, (2) check `dev->ops` is not NULL, (3) check the specific function pointer is not NULL, (4) check open state as required. Only then call through the function pointer.

## Hint 3

```c
void device_init(struct device *dev, const char *name) {
    dev->name = name;
    dev->is_open = 0;
    dev->ops = NULL;
}

int device_register_ops(struct device *dev, const struct device_ops *ops) {
    if (!dev || !ops) return -1;
    dev->ops = ops;
    return 0;
}

int device_open(struct device *dev, int flags) {
    if (!dev || !dev->ops || !dev->ops->open) return -1;
    if (dev->is_open) return -1;
    int ret = dev->ops->open(flags);
    if (ret == 0) dev->is_open = 1;
    return ret;
}

int device_read(struct device *dev, char *buf, int len) {
    if (!dev || !dev->is_open || !dev->ops || !dev->ops->read) return -1;
    return dev->ops->read(buf, len);
}

int device_write(struct device *dev, const char *buf, int len) {
    if (!dev || !dev->is_open || !dev->ops || !dev->ops->write) return -1;
    return dev->ops->write(buf, len);
}

int device_close(struct device *dev) {
    if (!dev || !dev->is_open) return -1;
    int ret = 0;
    if (dev->ops && dev->ops->close) ret = dev->ops->close();
    dev->is_open = 0;
    return ret;
}
```
