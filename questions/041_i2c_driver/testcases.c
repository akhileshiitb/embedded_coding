#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

/* ============================================================
 * Replicate types from solution.c
 * ============================================================ */

struct i2c_regs {
    volatile uint32_t CR;
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t CCR;
};

#define I2C_CR_EN     (1U << 0)
#define I2C_CR_START  (1U << 1)
#define I2C_CR_STOP   (1U << 2)
#define I2C_CR_ACK    (1U << 3)

#define I2C_SR_BUSY   (1U << 0)
#define I2C_SR_TXE    (1U << 1)
#define I2C_SR_RXNE   (1U << 2)
#define I2C_SR_ADDR   (1U << 3)
#define I2C_SR_NACK   (1U << 4)
#define I2C_SR_TC     (1U << 5)

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

struct i2c_config {
    uint32_t clock_hz;
    uint8_t  slave_addr;
};

struct i2c_device {
    struct i2c_regs *regs;
    const struct driver_ops *ops;
    struct i2c_config config;
    uint8_t initialized;
    uint8_t opened;
};

extern const struct driver_ops i2c_ops;

/* ============================================================
 * Simulated Hardware
 *
 * The sim auto-responds to driver actions:
 * - When CR.START is set → SR.BUSY = 1
 * - When address is written to DR → SR.ADDR = 1 (slave ACK)
 * - SR.TXE is always 1 (instant TX)
 * - SR.RXNE is pre-set for read tests
 * - SR.TC is set after write completes
 * - CR.START and CR.STOP auto-clear (simulating HW behavior)
 * ============================================================ */

static struct i2c_regs sim_regs;

static void sim_reset(void) {
    memset(&sim_regs, 0, sizeof(sim_regs));
    /* Simulate: TXE=1 (ready), BUSY=1 (START already sent), ADDR=1 (slave ACK'd) */
    sim_regs.SR = I2C_SR_TXE | I2C_SR_BUSY | I2C_SR_ADDR | I2C_SR_TC;
}

static struct i2c_device test_dev;

static void setup_device(void) {
    sim_reset();
    memset(&test_dev, 0, sizeof(test_dev));
    test_dev.regs = &sim_regs;
    test_dev.ops = &i2c_ops;
}

/* For NACK simulation */
static void sim_reset_nack(void) {
    memset(&sim_regs, 0, sizeof(sim_regs));
    /* Slave NACKs: set NACK flag, BUSY (START was sent) */
    sim_regs.SR = I2C_SR_TXE | I2C_SR_BUSY | I2C_SR_NACK;
}

/* ============================================================
 * Test Cases
 * ============================================================ */

int main(void) {
    int passed = 0;
    int num_tests = 18;
    int test_num = 0;

    /* === Test 1: Init 100kHz, slave 0x50 === */
    {
        test_num++;
        setup_device();
        struct i2c_config cfg = {100000, 0x50};
        int ret = i2c_ops.init(&test_dev, &cfg);
        /* CCR = 16000000 / (2 * 100000) = 80 */
        if (ret == DRV_OK && sim_regs.CCR == 80 && test_dev.initialized == 1) {
            passed++;
            printf("[PASS] Test %d: Init 100kHz — CCR=80\n", test_num);
        } else {
            printf("[FAIL] Test %d: Init 100kHz | ret=%d, CCR=%u\n",
                   test_num, ret, sim_regs.CCR);
        }
    }

    /* === Test 2: Init 400kHz, slave 0x68 === */
    {
        test_num++;
        setup_device();
        struct i2c_config cfg = {400000, 0x68};
        int ret = i2c_ops.init(&test_dev, &cfg);
        /* CCR = 16000000 / (2 * 400000) = 20 */
        if (ret == DRV_OK && sim_regs.CCR == 20) {
            passed++;
            printf("[PASS] Test %d: Init 400kHz — CCR=20\n", test_num);
        } else {
            printf("[FAIL] Test %d: Init 400kHz | ret=%d, CCR=%u\n",
                   test_num, ret, sim_regs.CCR);
        }
    }

    /* === Test 3: Init with invalid clock (50000) === */
    {
        test_num++;
        setup_device();
        struct i2c_config cfg = {50000, 0x50};
        int ret = i2c_ops.init(&test_dev, &cfg);
        if (ret == DRV_ERR_INVALID) {
            passed++;
            printf("[PASS] Test %d: Init with clock=50kHz returns error\n", test_num);
        } else {
            printf("[FAIL] Test %d: Init clock=50kHz | ret=%d (expected %d)\n",
                   test_num, ret, DRV_ERR_INVALID);
        }
    }

    /* === Test 4: Init with invalid slave addr (0x00) === */
    {
        test_num++;
        setup_device();
        struct i2c_config cfg = {100000, 0x00};
        int ret = i2c_ops.init(&test_dev, &cfg);
        if (ret == DRV_ERR_INVALID) {
            passed++;
            printf("[PASS] Test %d: Init with slave_addr=0x00 returns error\n", test_num);
        } else {
            printf("[FAIL] Test %d: Init addr=0x00 | ret=%d (expected %d)\n",
                   test_num, ret, DRV_ERR_INVALID);
        }
    }

    /* === Test 5: Init with invalid slave addr (0x78) === */
    {
        test_num++;
        setup_device();
        struct i2c_config cfg = {100000, 0x78};
        int ret = i2c_ops.init(&test_dev, &cfg);
        if (ret == DRV_ERR_INVALID) {
            passed++;
            printf("[PASS] Test %d: Init with slave_addr=0x78 returns error\n", test_num);
        } else {
            printf("[FAIL] Test %d: Init addr=0x78 | ret=%d (expected %d)\n",
                   test_num, ret, DRV_ERR_INVALID);
        }
    }

    /* === Test 6: Init with NULL handle === */
    {
        test_num++;
        struct i2c_config cfg = {100000, 0x50};
        int ret = i2c_ops.init(NULL, &cfg);
        if (ret == DRV_ERR_INVALID) {
            passed++;
            printf("[PASS] Test %d: Init with NULL handle returns error\n", test_num);
        } else {
            printf("[FAIL] Test %d: Init NULL handle | ret=%d (expected %d)\n",
                   test_num, ret, DRV_ERR_INVALID);
        }
    }

    /* === Test 7: Open sets EN bit === */
    {
        test_num++;
        setup_device();
        struct i2c_config cfg = {100000, 0x50};
        i2c_ops.init(&test_dev, &cfg);
        int ret = i2c_ops.open(&test_dev);
        if (ret == DRV_OK && (sim_regs.CR & I2C_CR_EN) && test_dev.opened == 1) {
            passed++;
            printf("[PASS] Test %d: Open sets CR.EN\n", test_num);
        } else {
            printf("[FAIL] Test %d: Open | ret=%d, CR=0x%02X\n",
                   test_num, ret, sim_regs.CR);
        }
    }

    /* === Test 8: Open without init returns error === */
    {
        test_num++;
        setup_device();
        int ret = i2c_ops.open(&test_dev);
        if (ret == DRV_ERR_INVALID) {
            passed++;
            printf("[PASS] Test %d: Open without init returns error\n", test_num);
        } else {
            printf("[FAIL] Test %d: Open without init | ret=%d (expected %d)\n",
                   test_num, ret, DRV_ERR_INVALID);
        }
    }

    /* === Test 9: Write — generates START, sends addr+W, data, STOP === */
    {
        test_num++;
        setup_device();
        struct i2c_config cfg = {100000, 0x50};
        i2c_ops.init(&test_dev, &cfg);
        i2c_ops.open(&test_dev);
        uint8_t data[] = {0xAB, 0xCD};
        int ret = i2c_ops.write(&test_dev, data, 2);
        /* Verify: START was set, STOP was set at end */
        if (ret == DRV_OK && (sim_regs.CR & I2C_CR_STOP)) {
            passed++;
            printf("[PASS] Test %d: Write 2 bytes — START+STOP generated\n", test_num);
        } else {
            printf("[FAIL] Test %d: Write | ret=%d, CR=0x%02X\n",
                   test_num, ret, sim_regs.CR);
        }
    }

    /* === Test 10: Write sends correct address byte (addr<<1 | 0) === */
    {
        test_num++;
        setup_device();
        struct i2c_config cfg = {100000, 0x50};
        i2c_ops.init(&test_dev, &cfg);
        i2c_ops.open(&test_dev);
        /* We check DR after the address phase.
         * Since sim processes instantly, DR will have the last data byte.
         * We verify the write succeeds (address was ACK'd per sim_reset). */
        uint8_t data[] = {0x42};
        int ret = i2c_ops.write(&test_dev, data, 1);
        /* Last thing written to DR should be data byte 0x42 */
        if (ret == DRV_OK && (sim_regs.DR & 0xFF) == 0x42) {
            passed++;
            printf("[PASS] Test %d: Write — last DR=0x42 (data byte)\n", test_num);
        } else {
            printf("[FAIL] Test %d: Write addr check | ret=%d, DR=0x%02X\n",
                   test_num, ret, sim_regs.DR & 0xFF);
        }
    }

    /* === Test 11: Write without open returns error === */
    {
        test_num++;
        setup_device();
        struct i2c_config cfg = {100000, 0x50};
        i2c_ops.init(&test_dev, &cfg);
        uint8_t data = 0x55;
        int ret = i2c_ops.write(&test_dev, &data, 1);
        if (ret == DRV_ERR_NOT_OPEN) {
            passed++;
            printf("[PASS] Test %d: Write without open returns -2\n", test_num);
        } else {
            printf("[FAIL] Test %d: Write without open | ret=%d (expected %d)\n",
                   test_num, ret, DRV_ERR_NOT_OPEN);
        }
    }

    /* === Test 12: Write with NACK returns HW error === */
    {
        test_num++;
        sim_reset_nack();  /* Slave will NACK */
        memset(&test_dev, 0, sizeof(test_dev));
        test_dev.regs = &sim_regs;
        test_dev.ops = &i2c_ops;
        struct i2c_config cfg = {100000, 0x50};
        i2c_ops.init(&test_dev, &cfg);
        i2c_ops.open(&test_dev);
        uint8_t data = 0x01;
        int ret = i2c_ops.write(&test_dev, &data, 1);
        if (ret == DRV_ERR_HW) {
            passed++;
            printf("[PASS] Test %d: Write with NACK returns -4 (HW error)\n", test_num);
        } else {
            printf("[FAIL] Test %d: Write NACK | ret=%d (expected %d)\n",
                   test_num, ret, DRV_ERR_HW);
        }
    }

    /* === Test 13: Read — generates START, sends addr+R, reads data, STOP === */
    {
        test_num++;
        setup_device();
        /* Set RXNE so driver can read data */
        sim_regs.SR |= I2C_SR_RXNE;
        sim_regs.DR = 0xEF;
        struct i2c_config cfg = {100000, 0x50};
        i2c_ops.init(&test_dev, &cfg);
        i2c_ops.open(&test_dev);
        uint8_t buf = 0;
        int ret = i2c_ops.read(&test_dev, &buf, 1);
        /* For single byte read, ACK should be cleared (NACK last byte) */
        if (ret == DRV_OK && (sim_regs.CR & I2C_CR_STOP)) {
            passed++;
            printf("[PASS] Test %d: Read 1 byte — STOP generated\n", test_num);
        } else {
            printf("[FAIL] Test %d: Read | ret=%d, CR=0x%02X\n",
                   test_num, ret, sim_regs.CR);
        }
    }

    /* === Test 14: Read — NACK on last byte (ACK cleared) === */
    {
        test_num++;
        setup_device();
        sim_regs.SR |= I2C_SR_RXNE;
        sim_regs.DR = 0x77;
        struct i2c_config cfg = {100000, 0x50};
        i2c_ops.init(&test_dev, &cfg);
        i2c_ops.open(&test_dev);
        uint8_t buf = 0;
        int ret = i2c_ops.read(&test_dev, &buf, 1);
        /* For single byte read, ACK should be cleared before reading */
        if (ret == DRV_OK && !(sim_regs.CR & I2C_CR_ACK)) {
            passed++;
            printf("[PASS] Test %d: Read 1 byte — ACK cleared (NACK sent)\n", test_num);
        } else {
            printf("[FAIL] Test %d: Read ACK check | ret=%d, CR=0x%02X\n",
                   test_num, ret, sim_regs.CR);
        }
    }

    /* === Test 15: Read without open returns error === */
    {
        test_num++;
        setup_device();
        struct i2c_config cfg = {100000, 0x50};
        i2c_ops.init(&test_dev, &cfg);
        uint8_t buf = 0;
        int ret = i2c_ops.read(&test_dev, &buf, 1);
        if (ret == DRV_ERR_NOT_OPEN) {
            passed++;
            printf("[PASS] Test %d: Read without open returns -2\n", test_num);
        } else {
            printf("[FAIL] Test %d: Read without open | ret=%d (expected %d)\n",
                   test_num, ret, DRV_ERR_NOT_OPEN);
        }
    }

    /* === Test 16: Close clears EN === */
    {
        test_num++;
        setup_device();
        struct i2c_config cfg = {100000, 0x50};
        i2c_ops.init(&test_dev, &cfg);
        i2c_ops.open(&test_dev);
        int ret = i2c_ops.close(&test_dev);
        if (ret == DRV_OK && !(sim_regs.CR & I2C_CR_EN) && test_dev.opened == 0) {
            passed++;
            printf("[PASS] Test %d: Close clears EN\n", test_num);
        } else {
            printf("[FAIL] Test %d: Close | ret=%d, CR=0x%02X\n",
                   test_num, ret, sim_regs.CR);
        }
    }

    /* === Test 17: Deinit resets everything === */
    {
        test_num++;
        setup_device();
        struct i2c_config cfg = {100000, 0x50};
        i2c_ops.init(&test_dev, &cfg);
        i2c_ops.open(&test_dev);
        int ret = i2c_ops.deinit(&test_dev);
        if (ret == DRV_OK && sim_regs.CR == 0 && sim_regs.CCR == 0 &&
            test_dev.initialized == 0 && test_dev.opened == 0) {
            passed++;
            printf("[PASS] Test %d: Deinit resets CR, CCR, and state\n", test_num);
        } else {
            printf("[FAIL] Test %d: Deinit | ret=%d, CR=0x%X, CCR=%u\n",
                   test_num, ret, sim_regs.CR, sim_regs.CCR);
        }
    }

    /* === Test 18: Full lifecycle === */
    {
        test_num++;
        setup_device();
        struct i2c_config cfg = {400000, 0x68};
        int ok = 1;

        int ret = i2c_ops.init(&test_dev, &cfg);
        if (ret != DRV_OK || sim_regs.CCR != 20) { ok = 0; }

        ret = i2c_ops.open(&test_dev);
        if (ret != DRV_OK || !(sim_regs.CR & I2C_CR_EN)) { ok = 0; }

        uint8_t tx[] = {0x01, 0x02};
        ret = i2c_ops.write(&test_dev, tx, 2);
        if (ret != DRV_OK) { ok = 0; }

        sim_regs.SR |= I2C_SR_RXNE;
        sim_regs.DR = 0xAA;
        uint8_t rx = 0;
        ret = i2c_ops.read(&test_dev, &rx, 1);
        if (ret != DRV_OK) { ok = 0; }

        ret = i2c_ops.close(&test_dev);
        if (ret != DRV_OK) { ok = 0; }

        ret = i2c_ops.deinit(&test_dev);
        if (ret != DRV_OK) { ok = 0; }

        if (ok) {
            passed++;
            printf("[PASS] Test %d: Full lifecycle (400kHz, slave 0x68)\n", test_num);
        } else {
            printf("[FAIL] Test %d: Full lifecycle failed\n", test_num);
        }
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);

    return (passed == num_tests) ? 0 : 1;
}
