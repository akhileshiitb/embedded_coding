#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

/* ============================================================
 * Replicate types from solution.c for linking
 * ============================================================ */

struct uart_regs {
    volatile uint32_t CR;
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t BRR;
};

#define UART_CR_EN      (1U << 0)
#define UART_CR_TXEN    (1U << 1)
#define UART_CR_RXEN    (1U << 2)

#define UART_SR_TXE     (1U << 0)
#define UART_SR_RXNE    (1U << 1)
#define UART_SR_BUSY    (1U << 2)
#define UART_SR_ORE     (1U << 3)

#define DRV_OK              0
#define DRV_ERR_INVALID    -1
#define DRV_ERR_NOT_OPEN   -2
#define DRV_ERR_ALREADY    -3
#define DRV_ERR_HW         -4

struct driver_ops {
    int (*init)(void *handle, const void *config);
    int (*open)(void *handle);
    int (*close)(void *handle);
    int (*read)(void *handle, uint8_t *data, size_t len);
    int (*write)(void *handle, const uint8_t *data, size_t len);
    int (*deinit)(void *handle);
};

enum uart_parity {
    UART_PARITY_NONE = 0,
    UART_PARITY_ODD  = 1,
    UART_PARITY_EVEN = 2
};

struct uart_config {
    uint32_t baud_rate;
    uint8_t  data_bits;
    uint8_t  stop_bits;
    uint8_t  parity;
};

struct uart_device {
    struct uart_regs *regs;
    const struct driver_ops *ops;
    struct uart_config config;
    uint8_t initialized;
    uint8_t opened;
};

/* ============================================================
 * Extern: driver vtable from solution.c
 * ============================================================ */
extern const struct driver_ops uart_ops;

/* ============================================================
 * Simulated Hardware
 *
 * The "hardware" is just a struct. The test harness manipulates
 * SR flags to simulate TX ready and RX data arrival.
 * ============================================================ */

/* Simulated register bank */
static struct uart_regs sim_regs;

/* Hook: called when DR is written (captures TX data) */
static void sim_reset(void) {
    memset(&sim_regs, 0, sizeof(sim_regs));
    sim_regs.SR = UART_SR_TXE;  /* TX empty at reset */
}



/*
 * Simulated HW behavior:
 * - TXE is always 1 (instant TX, no wait needed)
 * - RXNE is set manually before read tests (simulates incoming data)
 * - DR captures written bytes (last byte visible after write)
 * - DR is preloaded with data before read tests
 */

/* ============================================================
 * Helper: set up device for testing
 * ============================================================ */
static struct uart_device test_dev;

static void setup_device(void) {
    sim_reset();
    memset(&test_dev, 0, sizeof(test_dev));
    test_dev.regs = &sim_regs;
    test_dev.ops = &uart_ops;
}

/* ============================================================
 * Test Cases
 * ============================================================ */

int main(void) {
    int passed = 0;
    int num_tests = 18;
    int test_num = 0;

    /* === Test 1: Init with valid config === */
    {
        test_num++;
        setup_device();
        struct uart_config cfg = {115200, 8, 1, UART_PARITY_NONE};
        int ret = uart_ops.init(&test_dev, &cfg);
        /* BRR should be 16000000/115200 = 138 (integer division) */
        if (ret == DRV_OK && sim_regs.BRR == 138 && test_dev.initialized == 1) {
            passed++;
            printf("[PASS] Test %d: Init with 115200 baud sets BRR=138\n", test_num);
        } else {
            printf("[FAIL] Test %d: Init with 115200 baud | ret=%d, BRR=%u, init=%d\n",
                   test_num, ret, sim_regs.BRR, test_dev.initialized);
        }
    }

    /* === Test 2: Init with 9600 baud === */
    {
        test_num++;
        setup_device();
        struct uart_config cfg = {9600, 8, 1, UART_PARITY_NONE};
        int ret = uart_ops.init(&test_dev, &cfg);
        /* BRR = 16000000/9600 = 1666 */
        if (ret == DRV_OK && sim_regs.BRR == 1666) {
            passed++;
            printf("[PASS] Test %d: Init with 9600 baud sets BRR=1666\n", test_num);
        } else {
            printf("[FAIL] Test %d: Init with 9600 baud | ret=%d, BRR=%u\n",
                   test_num, ret, sim_regs.BRR);
        }
    }

    /* === Test 3: Init with NULL handle === */
    {
        test_num++;
        struct uart_config cfg = {115200, 8, 1, UART_PARITY_NONE};
        int ret = uart_ops.init(NULL, &cfg);
        if (ret == DRV_ERR_INVALID) {
            passed++;
            printf("[PASS] Test %d: Init with NULL handle returns error\n", test_num);
        } else {
            printf("[FAIL] Test %d: Init with NULL handle | ret=%d (expected %d)\n",
                   test_num, ret, DRV_ERR_INVALID);
        }
    }

    /* === Test 4: Init with invalid baud (0) === */
    {
        test_num++;
        setup_device();
        struct uart_config cfg = {0, 8, 1, UART_PARITY_NONE};
        int ret = uart_ops.init(&test_dev, &cfg);
        if (ret == DRV_ERR_INVALID) {
            passed++;
            printf("[PASS] Test %d: Init with baud=0 returns error\n", test_num);
        } else {
            printf("[FAIL] Test %d: Init with baud=0 | ret=%d (expected %d)\n",
                   test_num, ret, DRV_ERR_INVALID);
        }
    }

    /* === Test 5: Init with invalid data_bits (9) === */
    {
        test_num++;
        setup_device();
        struct uart_config cfg = {115200, 9, 1, UART_PARITY_NONE};
        int ret = uart_ops.init(&test_dev, &cfg);
        if (ret == DRV_ERR_INVALID) {
            passed++;
            printf("[PASS] Test %d: Init with data_bits=9 returns error\n", test_num);
        } else {
            printf("[FAIL] Test %d: Init with data_bits=9 | ret=%d (expected %d)\n",
                   test_num, ret, DRV_ERR_INVALID);
        }
    }

    /* === Test 6: Open after init sets CR correctly === */
    {
        test_num++;
        setup_device();
        struct uart_config cfg = {115200, 8, 1, UART_PARITY_NONE};
        uart_ops.init(&test_dev, &cfg);
        int ret = uart_ops.open(&test_dev);
        uint32_t expected_cr = UART_CR_EN | UART_CR_TXEN | UART_CR_RXEN;
        if (ret == DRV_OK && sim_regs.CR == expected_cr && test_dev.opened == 1) {
            passed++;
            printf("[PASS] Test %d: Open sets CR=0x%02X (EN|TXEN|RXEN)\n", test_num, expected_cr);
        } else {
            printf("[FAIL] Test %d: Open | ret=%d, CR=0x%08X (expected 0x%08X)\n",
                   test_num, ret, sim_regs.CR, expected_cr);
        }
    }

    /* === Test 7: Open without init returns error === */
    {
        test_num++;
        setup_device();
        int ret = uart_ops.open(&test_dev);
        if (ret == DRV_ERR_INVALID || ret == DRV_ERR_NOT_OPEN) {
            passed++;
            printf("[PASS] Test %d: Open without init returns error (%d)\n", test_num, ret);
        } else {
            printf("[FAIL] Test %d: Open without init | ret=%d (expected error)\n",
                   test_num, ret);
        }
    }

    /* === Test 8: Write single byte === */
    {
        test_num++;
        setup_device();
        struct uart_config cfg = {115200, 8, 1, UART_PARITY_NONE};
        uart_ops.init(&test_dev, &cfg);
        uart_ops.open(&test_dev);
        /* TXE is already set (sim_reset sets SR = TXE) */
        uint8_t data = 0x42;
        int ret = uart_ops.write(&test_dev, &data, 1);
        if (ret == DRV_OK && (sim_regs.DR & 0xFF) == 0x42) {
            passed++;
            printf("[PASS] Test %d: Write 0x42 — DR=0x%02X\n", test_num, sim_regs.DR & 0xFF);
        } else {
            printf("[FAIL] Test %d: Write 0x42 | ret=%d, DR=0x%02X\n",
                   test_num, ret, sim_regs.DR & 0xFF);
        }
    }

    /* === Test 9: Write multiple bytes (verify last byte in DR) === */
    {
        test_num++;
        setup_device();
        struct uart_config cfg = {115200, 8, 1, UART_PARITY_NONE};
        uart_ops.init(&test_dev, &cfg);
        uart_ops.open(&test_dev);
        uint8_t data[] = {'H', 'e', 'l', 'l', 'o'};
        int ret = uart_ops.write(&test_dev, data, 5);
        /* Last byte written should be 'o' = 0x6F */
        if (ret == DRV_OK && (sim_regs.DR & 0xFF) == 'o') {
            passed++;
            printf("[PASS] Test %d: Write \"Hello\" — last DR='o' (0x%02X)\n", test_num, sim_regs.DR & 0xFF);
        } else {
            printf("[FAIL] Test %d: Write \"Hello\" | ret=%d, DR=0x%02X (expected 0x6F)\n",
                   test_num, ret, sim_regs.DR & 0xFF);
        }
    }

    /* === Test 10: Write without open returns error === */
    {
        test_num++;
        setup_device();
        struct uart_config cfg = {115200, 8, 1, UART_PARITY_NONE};
        uart_ops.init(&test_dev, &cfg);
        /* NOT opening */
        uint8_t data = 0x55;
        int ret = uart_ops.write(&test_dev, &data, 1);
        if (ret == DRV_ERR_NOT_OPEN) {
            passed++;
            printf("[PASS] Test %d: Write without open returns -2\n", test_num);
        } else {
            printf("[FAIL] Test %d: Write without open | ret=%d (expected %d)\n",
                   test_num, ret, DRV_ERR_NOT_OPEN);
        }
    }

    /* === Test 11: Read single byte === */
    {
        test_num++;
        setup_device();
        struct uart_config cfg = {115200, 8, 1, UART_PARITY_NONE};
        uart_ops.init(&test_dev, &cfg);
        uart_ops.open(&test_dev);
        /* Simulate incoming data: put byte in DR, set RXNE */
        sim_regs.DR = 0xAB;
        sim_regs.SR |= UART_SR_RXNE;
        uint8_t buf = 0;
        int ret = uart_ops.read(&test_dev, &buf, 1);
        if (ret == DRV_OK && buf == 0xAB) {
            passed++;
            printf("[PASS] Test %d: Read byte — got 0x%02X\n", test_num, buf);
        } else {
            printf("[FAIL] Test %d: Read byte | ret=%d, buf=0x%02X (expected 0xAB)\n",
                   test_num, ret, buf);
        }
    }

    /* === Test 12: Read without open returns error === */
    {
        test_num++;
        setup_device();
        struct uart_config cfg = {115200, 8, 1, UART_PARITY_NONE};
        uart_ops.init(&test_dev, &cfg);
        uint8_t buf = 0;
        int ret = uart_ops.read(&test_dev, &buf, 1);
        if (ret == DRV_ERR_NOT_OPEN) {
            passed++;
            printf("[PASS] Test %d: Read without open returns -2\n", test_num);
        } else {
            printf("[FAIL] Test %d: Read without open | ret=%d (expected %d)\n",
                   test_num, ret, DRV_ERR_NOT_OPEN);
        }
    }

    /* === Test 13: Close clears CR === */
    {
        test_num++;
        setup_device();
        struct uart_config cfg = {115200, 8, 1, UART_PARITY_NONE};
        uart_ops.init(&test_dev, &cfg);
        uart_ops.open(&test_dev);
        int ret = uart_ops.close(&test_dev);
        if (ret == DRV_OK && sim_regs.CR == 0 && test_dev.opened == 0) {
            passed++;
            printf("[PASS] Test %d: Close clears CR and marks closed\n", test_num);
        } else {
            printf("[FAIL] Test %d: Close | ret=%d, CR=0x%08X, opened=%d\n",
                   test_num, ret, sim_regs.CR, test_dev.opened);
        }
    }

    /* === Test 14: Close without open returns error === */
    {
        test_num++;
        setup_device();
        struct uart_config cfg = {115200, 8, 1, UART_PARITY_NONE};
        uart_ops.init(&test_dev, &cfg);
        int ret = uart_ops.close(&test_dev);
        if (ret == DRV_ERR_NOT_OPEN || ret == DRV_ERR_INVALID) {
            passed++;
            printf("[PASS] Test %d: Close without open returns error (%d)\n", test_num, ret);
        } else {
            printf("[FAIL] Test %d: Close without open | ret=%d (expected error)\n",
                   test_num, ret);
        }
    }

    /* === Test 15: Deinit resets everything === */
    {
        test_num++;
        setup_device();
        struct uart_config cfg = {115200, 8, 1, UART_PARITY_NONE};
        uart_ops.init(&test_dev, &cfg);
        uart_ops.open(&test_dev);
        int ret = uart_ops.deinit(&test_dev);
        if (ret == DRV_OK && sim_regs.CR == 0 && sim_regs.BRR == 0 &&
            test_dev.initialized == 0 && test_dev.opened == 0) {
            passed++;
            printf("[PASS] Test %d: Deinit resets CR, BRR, and state\n", test_num);
        } else {
            printf("[FAIL] Test %d: Deinit | ret=%d, CR=0x%X, BRR=%u, init=%d, open=%d\n",
                   test_num, ret, sim_regs.CR, sim_regs.BRR,
                   test_dev.initialized, test_dev.opened);
        }
    }

    /* === Test 16: Write with NULL data returns error === */
    {
        test_num++;
        setup_device();
        struct uart_config cfg = {115200, 8, 1, UART_PARITY_NONE};
        uart_ops.init(&test_dev, &cfg);
        uart_ops.open(&test_dev);
        int ret = uart_ops.write(&test_dev, NULL, 5);
        if (ret == DRV_ERR_INVALID) {
            passed++;
            printf("[PASS] Test %d: Write with NULL data returns error\n", test_num);
        } else {
            printf("[FAIL] Test %d: Write with NULL data | ret=%d (expected %d)\n",
                   test_num, ret, DRV_ERR_INVALID);
        }
    }

    /* === Test 17: Write with len=0 returns error === */
    {
        test_num++;
        setup_device();
        struct uart_config cfg = {115200, 8, 1, UART_PARITY_NONE};
        uart_ops.init(&test_dev, &cfg);
        uart_ops.open(&test_dev);
        uint8_t data = 0x01;
        int ret = uart_ops.write(&test_dev, &data, 0);
        if (ret == DRV_ERR_INVALID) {
            passed++;
            printf("[PASS] Test %d: Write with len=0 returns error\n", test_num);
        } else {
            printf("[FAIL] Test %d: Write with len=0 | ret=%d (expected %d)\n",
                   test_num, ret, DRV_ERR_INVALID);
        }
    }

    /* === Test 18: Full lifecycle — init, open, write, read, close, deinit === */
    {
        test_num++;
        setup_device();
        struct uart_config cfg = {57600, 8, 1, UART_PARITY_EVEN};
        int ok = 1;

        /* Init */
        int ret = uart_ops.init(&test_dev, &cfg);
        if (ret != DRV_OK || sim_regs.BRR != (16000000 / 57600)) { ok = 0; }

        /* Open */
        ret = uart_ops.open(&test_dev);
        if (ret != DRV_OK || sim_regs.CR != (UART_CR_EN | UART_CR_TXEN | UART_CR_RXEN)) { ok = 0; }

        /* Write */
        uint8_t tx_data[] = {0xDE, 0xAD};
        ret = uart_ops.write(&test_dev, tx_data, 2);
        if (ret != DRV_OK) { ok = 0; }

        /* Read */
        sim_regs.DR = 0xBE;
        sim_regs.SR |= UART_SR_RXNE;
        uint8_t rx_buf = 0;
        ret = uart_ops.read(&test_dev, &rx_buf, 1);
        if (ret != DRV_OK || rx_buf != 0xBE) { ok = 0; }

        /* Close */
        ret = uart_ops.close(&test_dev);
        if (ret != DRV_OK || sim_regs.CR != 0) { ok = 0; }

        /* Deinit */
        ret = uart_ops.deinit(&test_dev);
        if (ret != DRV_OK || sim_regs.BRR != 0) { ok = 0; }

        if (ok) {
            passed++;
            printf("[PASS] Test %d: Full lifecycle (init→open→write→read→close→deinit)\n", test_num);
        } else {
            printf("[FAIL] Test %d: Full lifecycle failed at some step\n", test_num);
        }
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);

    return (passed == num_tests) ? 0 : 1;
}
