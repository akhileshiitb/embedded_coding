#include <stdio.h>
#include <stddef.h>

#define MAX_COMMANDS 8

/* Declaration of solution functions (raw function pointer syntax) */
extern void init_dispatch_table(void);
extern int register_handler(int cmd_id, int (*handler)(int arg));
extern int dispatch_command(int cmd_id, int arg);

/* --- Test handler functions --- */

static int cmd_read(int arg) {
    return arg;  /* echo back */
}

static int cmd_write(int arg) {
    return arg * 2;
}

static int cmd_reset(int arg) {
    (void)arg;
    return 0;
}

static int cmd_status(int arg) {
    return arg + 100;
}

static int cmd_config(int arg) {
    return arg - 1;
}

static int cmd_negate(int arg) {
    return -arg;
}

int main(void) {
    int num_tests = 16;
    int passed = 0;
    int result;
    int test_num = 0;

    /* Initialize the table */
    init_dispatch_table();

    /* Test 1: Dispatch without any handlers returns -1 */
    test_num++;
    result = dispatch_command(0, 10);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: Dispatch empty table returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: Dispatch empty table returns -1 | Input: cmd=0, arg=10 | Expected: -1, Got: %d\n", test_num, result);
    }

    /* Test 2: Register handler for cmd 0 */
    test_num++;
    result = register_handler(0, cmd_read);
    if (result == 0) {
        passed++;
        printf("[PASS] Test %d: Register cmd 0 succeeds\n", test_num);
    } else {
        printf("[FAIL] Test %d: Register cmd 0 succeeds | Expected: 0, Got: %d\n", test_num, result);
    }

    /* Test 3: Dispatch cmd 0 */
    test_num++;
    result = dispatch_command(0, 42);
    if (result == 42) {
        passed++;
        printf("[PASS] Test %d: Dispatch cmd 0 (read) with arg=42\n", test_num);
    } else {
        printf("[FAIL] Test %d: Dispatch cmd 0 (read) with arg=42 | Expected: 42, Got: %d\n", test_num, result);
    }

    /* Test 4: Register handler for cmd 3 */
    test_num++;
    register_handler(3, cmd_write);
    result = dispatch_command(3, 5);
    if (result == 10) {
        passed++;
        printf("[PASS] Test %d: Dispatch cmd 3 (write) with arg=5\n", test_num);
    } else {
        printf("[FAIL] Test %d: Dispatch cmd 3 (write) with arg=5 | Expected: 10, Got: %d\n", test_num, result);
    }

    /* Test 5: Dispatch unregistered cmd returns -1 */
    test_num++;
    result = dispatch_command(5, 99);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: Dispatch unregistered cmd 5 returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: Dispatch unregistered cmd 5 returns -1 | Expected: -1, Got: %d\n", test_num, result);
    }

    /* Test 6: Out-of-range register returns -1 */
    test_num++;
    result = register_handler(8, cmd_status);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: Register out-of-range cmd 8 returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: Register out-of-range cmd 8 returns -1 | Expected: -1, Got: %d\n", test_num, result);
    }

    /* Test 7: Negative cmd_id register returns -1 */
    test_num++;
    result = register_handler(-1, cmd_status);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: Register negative cmd -1 returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: Register negative cmd -1 returns -1 | Expected: -1, Got: %d\n", test_num, result);
    }

    /* Test 8: Dispatch out-of-range returns -1 */
    test_num++;
    result = dispatch_command(10, 0);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: Dispatch out-of-range cmd 10 returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: Dispatch out-of-range cmd 10 returns -1 | Expected: -1, Got: %d\n", test_num, result);
    }

    /* Test 9: Dispatch negative cmd_id returns -1 */
    test_num++;
    result = dispatch_command(-2, 0);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: Dispatch negative cmd -2 returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: Dispatch negative cmd -2 returns -1 | Expected: -1, Got: %d\n", test_num, result);
    }

    /* Test 10: Register all slots */
    test_num++;
    register_handler(1, cmd_status);
    register_handler(2, cmd_reset);
    register_handler(4, cmd_config);
    register_handler(5, cmd_negate);
    register_handler(6, cmd_read);
    register_handler(7, cmd_write);
    result = dispatch_command(1, 5);
    if (result == 105) {
        passed++;
        printf("[PASS] Test %d: Dispatch cmd 1 (status) arg=5 → 105\n", test_num);
    } else {
        printf("[FAIL] Test %d: Dispatch cmd 1 (status) arg=5 → 105 | Expected: 105, Got: %d\n", test_num, result);
    }

    /* Test 11: Dispatch cmd 2 (reset) */
    test_num++;
    result = dispatch_command(2, 999);
    if (result == 0) {
        passed++;
        printf("[PASS] Test %d: Dispatch cmd 2 (reset) always returns 0\n", test_num);
    } else {
        printf("[FAIL] Test %d: Dispatch cmd 2 (reset) always returns 0 | Expected: 0, Got: %d\n", test_num, result);
    }

    /* Test 12: Dispatch cmd 5 (negate) */
    test_num++;
    result = dispatch_command(5, 7);
    if (result == -7) {
        passed++;
        printf("[PASS] Test %d: Dispatch cmd 5 (negate) arg=7 → -7\n", test_num);
    } else {
        printf("[FAIL] Test %d: Dispatch cmd 5 (negate) arg=7 → -7 | Expected: -7, Got: %d\n", test_num, result);
    }

    /* Test 13: Replace handler for cmd 0 */
    test_num++;
    register_handler(0, cmd_negate);
    result = dispatch_command(0, 3);
    if (result == -3) {
        passed++;
        printf("[PASS] Test %d: Replace cmd 0 handler, negate(3) = -3\n", test_num);
    } else {
        printf("[FAIL] Test %d: Replace cmd 0 handler, negate(3) = -3 | Expected: -3, Got: %d\n", test_num, result);
    }

    /* Test 14: Dispatch last valid cmd (7) */
    test_num++;
    result = dispatch_command(7, 4);
    if (result == 8) {
        passed++;
        printf("[PASS] Test %d: Dispatch cmd 7 (write) arg=4 → 8\n", test_num);
    } else {
        printf("[FAIL] Test %d: Dispatch cmd 7 (write) arg=4 → 8 | Expected: 8, Got: %d\n", test_num, result);
    }

    /* Test 15: Re-init clears all handlers */
    test_num++;
    init_dispatch_table();
    result = dispatch_command(0, 10);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: After re-init, dispatch returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: After re-init, dispatch returns -1 | Expected: -1, Got: %d\n", test_num, result);
    }

    /* Test 16: Verify another slot also cleared */
    test_num++;
    result = dispatch_command(3, 10);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: After re-init, cmd 3 also returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: After re-init, cmd 3 also returns -1 | Expected: -1, Got: %d\n", test_num, result);
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);

    return (passed == num_tests) ? 0 : 1;
}
