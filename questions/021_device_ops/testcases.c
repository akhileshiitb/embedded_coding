#include <stdio.h>
#include <stddef.h>
#include <string.h>

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

/* Declaration of solution functions */
extern void device_init(struct device *dev, const char *name);
extern int device_register_ops(struct device *dev, const struct device_ops *ops);
extern int device_open(struct device *dev, int flags);
extern int device_read(struct device *dev, char *buf, int len);
extern int device_write(struct device *dev, const char *buf, int len);
extern int device_close(struct device *dev);

/* --- Mock device implementations --- */

static int uart_open(int flags) {
    (void)flags;
    return 0;  /* success */
}

static int uart_read(char *buf, int len) {
    /* Simulate reading: fill with 'U' */
    for (int i = 0; i < len; i++) buf[i] = 'U';
    return len;
}

static int uart_write(const char *buf, int len) {
    (void)buf;
    return len;  /* pretend we wrote all bytes */
}

static int uart_close(void) {
    return 0;
}

static const struct device_ops uart_ops = {
    .open  = uart_open,
    .read  = uart_read,
    .write = uart_write,
    .close = uart_close,
};

/* Read-only device (no write, no close) */
static int sensor_open(int flags) {
    (void)flags;
    return 0;
}

static int sensor_read(char *buf, int len) {
    for (int i = 0; i < len; i++) buf[i] = 'S';
    return len;
}

static const struct device_ops sensor_ops = {
    .open  = sensor_open,
    .read  = sensor_read,
    .write = NULL,
    .close = NULL,
};

/* Device that rejects open with non-zero flags */
static int strict_open(int flags) {
    if (flags != 0) return -1;
    return 0;
}

static const struct device_ops strict_ops = {
    .open  = strict_open,
    .read  = NULL,
    .write = NULL,
    .close = NULL,
};

int main(void) {
    int num_tests = 16;
    int passed = 0;
    int result;
    int test_num = 0;
    char buf[32];
    struct device dev;

    /* Test 1: Init and read before open returns -1 */
    test_num++;
    device_init(&dev, "uart0");
    device_register_ops(&dev, &uart_ops);
    result = device_read(&dev, buf, 5);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: Read before open returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: Read before open returns -1 | Got: %d\n", test_num, result);
    }

    /* Test 2: Open succeeds */
    test_num++;
    result = device_open(&dev, 0);
    if (result == 0) {
        passed++;
        printf("[PASS] Test %d: Open uart0 succeeds\n", test_num);
    } else {
        printf("[FAIL] Test %d: Open uart0 succeeds | Expected: 0, Got: %d\n", test_num, result);
    }

    /* Test 3: Double open fails */
    test_num++;
    result = device_open(&dev, 0);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: Double open fails (-1)\n", test_num);
    } else {
        printf("[FAIL] Test %d: Double open fails (-1) | Got: %d\n", test_num, result);
    }

    /* Test 4: Read returns correct length */
    test_num++;
    memset(buf, 0, sizeof(buf));
    result = device_read(&dev, buf, 4);
    if (result == 4 && buf[0] == 'U' && buf[3] == 'U') {
        passed++;
        printf("[PASS] Test %d: Read 4 bytes from uart0\n", test_num);
    } else {
        printf("[FAIL] Test %d: Read 4 bytes from uart0 | result=%d, buf[0]=%c\n", test_num, result, buf[0]);
    }

    /* Test 5: Write returns bytes written */
    test_num++;
    result = device_write(&dev, "hello", 5);
    if (result == 5) {
        passed++;
        printf("[PASS] Test %d: Write 5 bytes to uart0\n", test_num);
    } else {
        printf("[FAIL] Test %d: Write 5 bytes to uart0 | Expected: 5, Got: %d\n", test_num, result);
    }

    /* Test 6: Close succeeds */
    test_num++;
    result = device_close(&dev);
    if (result == 0) {
        passed++;
        printf("[PASS] Test %d: Close uart0 succeeds\n", test_num);
    } else {
        printf("[FAIL] Test %d: Close uart0 succeeds | Expected: 0, Got: %d\n", test_num, result);
    }

    /* Test 7: Read after close fails */
    test_num++;
    result = device_read(&dev, buf, 4);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: Read after close returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: Read after close returns -1 | Got: %d\n", test_num, result);
    }

    /* Test 8: Write after close fails */
    test_num++;
    result = device_write(&dev, "x", 1);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: Write after close returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: Write after close returns -1 | Got: %d\n", test_num, result);
    }

    /* Test 9: Close when not open fails */
    test_num++;
    result = device_close(&dev);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: Close when not open returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: Close when not open returns -1 | Got: %d\n", test_num, result);
    }

    /* Test 10: Sensor device (read-only, no close op) */
    test_num++;
    struct device sensor;
    device_init(&sensor, "temp_sensor");
    device_register_ops(&sensor, &sensor_ops);
    device_open(&sensor, 0);
    memset(buf, 0, sizeof(buf));
    result = device_read(&sensor, buf, 3);
    if (result == 3 && buf[0] == 'S') {
        passed++;
        printf("[PASS] Test %d: Sensor read 3 bytes\n", test_num);
    } else {
        printf("[FAIL] Test %d: Sensor read 3 bytes | result=%d, buf[0]=%c\n", test_num, result, buf[0]);
    }

    /* Test 11: Sensor write fails (no write op) */
    test_num++;
    result = device_write(&sensor, "x", 1);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: Sensor write returns -1 (no write op)\n", test_num);
    } else {
        printf("[FAIL] Test %d: Sensor write returns -1 (no write op) | Got: %d\n", test_num, result);
    }

    /* Test 12: Sensor close succeeds (no close op → returns 0) */
    test_num++;
    result = device_close(&sensor);
    if (result == 0) {
        passed++;
        printf("[PASS] Test %d: Sensor close returns 0 (no close op, still succeeds)\n", test_num);
    } else {
        printf("[FAIL] Test %d: Sensor close returns 0 (no close op, still succeeds) | Got: %d\n", test_num, result);
    }

    /* Test 13: NULL device pointer */
    test_num++;
    result = device_open(NULL, 0);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: NULL device pointer returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: NULL device pointer returns -1 | Got: %d\n", test_num, result);
    }

    /* Test 14: Register NULL ops returns -1 */
    test_num++;
    struct device bad_dev;
    device_init(&bad_dev, "bad");
    result = device_register_ops(&bad_dev, NULL);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: Register NULL ops returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: Register NULL ops returns -1 | Got: %d\n", test_num, result);
    }

    /* Test 15: Open without ops registered fails */
    test_num++;
    result = device_open(&bad_dev, 0);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: Open without ops returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: Open without ops returns -1 | Got: %d\n", test_num, result);
    }

    /* Test 16: Strict device rejects non-zero flags */
    test_num++;
    struct device strict_dev;
    device_init(&strict_dev, "strict");
    device_register_ops(&strict_dev, &strict_ops);
    result = device_open(&strict_dev, 1);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: Strict device rejects non-zero flags\n", test_num);
    } else {
        printf("[FAIL] Test %d: Strict device rejects non-zero flags | Got: %d\n", test_num, result);
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);

    return (passed == num_tests) ? 0 : 1;
}
