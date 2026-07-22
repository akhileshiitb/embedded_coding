#include <stdio.h>
#include <stddef.h>

#define NUM_IRQS 8

typedef void (*isr_handler_fn)(int irq_num);

/* Declaration of solution functions */
extern void ivt_init(void);
extern int ivt_install_handler(int irq, isr_handler_fn handler);
extern int ivt_remove_handler(int irq);
extern int ivt_dispatch(int irq);

/* Shared log array declared in solution.c */
extern int irq_log[NUM_IRQS];

/* --- Test ISR handlers --- */

static void uart_isr(int irq) {
    irq_log[irq] = 100 + irq;
}

static void timer_isr(int irq) {
    irq_log[irq] = 200 + irq;
}

static void gpio_isr(int irq) {
    irq_log[irq] = 300 + irq;
}

static void spi_isr(int irq) {
    irq_log[irq] = 400 + irq;
}

static void dma_isr(int irq) {
    irq_log[irq] = 500 + irq;
}

int main(void) {
    int num_tests = 16;
    int passed = 0;
    int result;
    int test_num = 0;

    /* Initialize */
    ivt_init();

    /* Test 1: Dispatch IRQ 0 with default handler (should write -1) */
    test_num++;
    ivt_dispatch(0);
    if (irq_log[0] == -1) {
        passed++;
        printf("[PASS] Test %d: Default handler writes -1 to irq_log[0]\n", test_num);
    } else {
        printf("[FAIL] Test %d: Default handler writes -1 to irq_log[0] | Got: %d\n", test_num, irq_log[0]);
    }

    /* Test 2: Dispatch IRQ 7 with default handler */
    test_num++;
    ivt_dispatch(7);
    if (irq_log[7] == -1) {
        passed++;
        printf("[PASS] Test %d: Default handler writes -1 to irq_log[7]\n", test_num);
    } else {
        printf("[FAIL] Test %d: Default handler writes -1 to irq_log[7] | Got: %d\n", test_num, irq_log[7]);
    }

    /* Test 3: Install uart_isr at IRQ 0 */
    test_num++;
    result = ivt_install_handler(0, uart_isr);
    if (result == 0) {
        passed++;
        printf("[PASS] Test %d: Install uart_isr at IRQ 0 succeeds\n", test_num);
    } else {
        printf("[FAIL] Test %d: Install uart_isr at IRQ 0 succeeds | Expected: 0, Got: %d\n", test_num, result);
    }

    /* Test 4: Dispatch IRQ 0 calls uart_isr */
    test_num++;
    irq_log[0] = 0;
    ivt_dispatch(0);
    if (irq_log[0] == 100) {
        passed++;
        printf("[PASS] Test %d: uart_isr writes 100 to irq_log[0]\n", test_num);
    } else {
        printf("[FAIL] Test %d: uart_isr writes 100 to irq_log[0] | Got: %d\n", test_num, irq_log[0]);
    }

    /* Test 5: Install timer_isr at IRQ 3 */
    test_num++;
    ivt_install_handler(3, timer_isr);
    irq_log[3] = 0;
    ivt_dispatch(3);
    if (irq_log[3] == 203) {
        passed++;
        printf("[PASS] Test %d: timer_isr writes 203 to irq_log[3]\n", test_num);
    } else {
        printf("[FAIL] Test %d: timer_isr writes 203 to irq_log[3] | Got: %d\n", test_num, irq_log[3]);
    }

    /* Test 6: Install NULL handler returns -1 */
    test_num++;
    result = ivt_install_handler(2, NULL);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: Install NULL handler returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: Install NULL handler returns -1 | Expected: -1, Got: %d\n", test_num, result);
    }

    /* Test 7: Install out-of-range IRQ returns -1 */
    test_num++;
    result = ivt_install_handler(8, gpio_isr);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: Install out-of-range IRQ 8 returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: Install out-of-range IRQ 8 returns -1 | Expected: -1, Got: %d\n", test_num, result);
    }

    /* Test 8: Install negative IRQ returns -1 */
    test_num++;
    result = ivt_install_handler(-1, gpio_isr);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: Install negative IRQ -1 returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: Install negative IRQ -1 returns -1 | Expected: -1, Got: %d\n", test_num, result);
    }

    /* Test 9: Dispatch out-of-range returns -1 */
    test_num++;
    result = ivt_dispatch(8);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: Dispatch out-of-range IRQ 8 returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: Dispatch out-of-range IRQ 8 returns -1 | Expected: -1, Got: %d\n", test_num, result);
    }

    /* Test 10: Dispatch negative IRQ returns -1 */
    test_num++;
    result = ivt_dispatch(-2);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: Dispatch negative IRQ -2 returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: Dispatch negative IRQ -2 returns -1 | Expected: -1, Got: %d\n", test_num, result);
    }

    /* Test 11: Remove handler restores default */
    test_num++;
    ivt_remove_handler(0);
    irq_log[0] = 0;
    ivt_dispatch(0);
    if (irq_log[0] == -1) {
        passed++;
        printf("[PASS] Test %d: After remove, default handler restored for IRQ 0\n", test_num);
    } else {
        printf("[FAIL] Test %d: After remove, default handler restored for IRQ 0 | Got: %d\n", test_num, irq_log[0]);
    }

    /* Test 12: Remove out-of-range returns -1 */
    test_num++;
    result = ivt_remove_handler(10);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: Remove out-of-range IRQ 10 returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: Remove out-of-range IRQ 10 returns -1 | Expected: -1, Got: %d\n", test_num, result);
    }

    /* Test 13: Multiple ISRs coexist */
    test_num++;
    ivt_install_handler(4, gpio_isr);
    ivt_install_handler(5, spi_isr);
    ivt_install_handler(6, dma_isr);
    irq_log[4] = 0; irq_log[5] = 0; irq_log[6] = 0;
    ivt_dispatch(4);
    ivt_dispatch(5);
    ivt_dispatch(6);
    if (irq_log[4] == 304 && irq_log[5] == 405 && irq_log[6] == 506) {
        passed++;
        printf("[PASS] Test %d: Multiple ISRs coexist correctly\n", test_num);
    } else {
        printf("[FAIL] Test %d: Multiple ISRs coexist correctly | Got: [4]=%d, [5]=%d, [6]=%d\n", test_num, irq_log[4], irq_log[5], irq_log[6]);
    }

    /* Test 14: Replace existing handler */
    test_num++;
    ivt_install_handler(3, dma_isr);
    irq_log[3] = 0;
    ivt_dispatch(3);
    if (irq_log[3] == 503) {
        passed++;
        printf("[PASS] Test %d: Replace timer_isr with dma_isr on IRQ 3\n", test_num);
    } else {
        printf("[FAIL] Test %d: Replace timer_isr with dma_isr on IRQ 3 | Got: %d\n", test_num, irq_log[3]);
    }

    /* Test 15: Dispatch returns 0 on success */
    test_num++;
    result = ivt_dispatch(5);
    if (result == 0) {
        passed++;
        printf("[PASS] Test %d: Dispatch returns 0 on success\n", test_num);
    } else {
        printf("[FAIL] Test %d: Dispatch returns 0 on success | Expected: 0, Got: %d\n", test_num, result);
    }

    /* Test 16: Re-init clears all custom handlers */
    test_num++;
    ivt_init();
    irq_log[3] = 0;
    ivt_dispatch(3);
    if (irq_log[3] == -1) {
        passed++;
        printf("[PASS] Test %d: After re-init, IRQ 3 back to default handler\n", test_num);
    } else {
        printf("[FAIL] Test %d: After re-init, IRQ 3 back to default handler | Got: %d\n", test_num, irq_log[3]);
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);

    return (passed == num_tests) ? 0 : 1;
}
