#include <stdio.h>
#include <stddef.h>

#define EVENT_START  0
#define EVENT_STOP   1
#define EVENT_DATA   2
#define EVENT_ERROR  3
#define EVENT_RESET  4

#define STATE_IDLE    0
#define STATE_RUNNING 1
#define STATE_ERROR   2

/* Declaration of solution functions */
extern void sm_init(void);
extern int sm_process_event(int event, int data);
extern int sm_get_state(void);
extern int sm_get_counter(void);

int main(void) {
    int num_tests = 16;
    int passed = 0;
    int result;
    int test_num = 0;

    /* Initialize state machine */
    sm_init();

    /* Test 1: Initial state is IDLE */
    test_num++;
    result = sm_get_state();
    if (result == STATE_IDLE) {
        passed++;
        printf("[PASS] Test %d: Initial state is IDLE\n", test_num);
    } else {
        printf("[FAIL] Test %d: Initial state is IDLE | Expected: %d, Got: %d\n", test_num, STATE_IDLE, result);
    }

    /* Test 2: STOP while IDLE is ignored */
    test_num++;
    result = sm_process_event(EVENT_STOP, 0);
    if (result == 1) {
        passed++;
        printf("[PASS] Test %d: STOP while IDLE is ignored (returns 1)\n", test_num);
    } else {
        printf("[FAIL] Test %d: STOP while IDLE is ignored (returns 1) | Expected: 1, Got: %d\n", test_num, result);
    }

    /* Test 3: START transitions to RUNNING */
    test_num++;
    result = sm_process_event(EVENT_START, 0);
    if (result == 0 && sm_get_state() == STATE_RUNNING) {
        passed++;
        printf("[PASS] Test %d: START transitions IDLE → RUNNING\n", test_num);
    } else {
        printf("[FAIL] Test %d: START transitions IDLE → RUNNING | event_result=%d, state=%d\n", test_num, result, sm_get_state());
    }

    /* Test 4: DATA while RUNNING accumulates */
    test_num++;
    result = sm_process_event(EVENT_DATA, 5);
    if (result == 0 && sm_get_counter() == 5) {
        passed++;
        printf("[PASS] Test %d: DATA(5) while RUNNING, counter=5\n", test_num);
    } else {
        printf("[FAIL] Test %d: DATA(5) while RUNNING, counter=5 | result=%d, counter=%d\n", test_num, result, sm_get_counter());
    }

    /* Test 5: More DATA accumulates */
    test_num++;
    sm_process_event(EVENT_DATA, 3);
    if (sm_get_counter() == 8) {
        passed++;
        printf("[PASS] Test %d: DATA(3) accumulates, counter=8\n", test_num);
    } else {
        printf("[FAIL] Test %d: DATA(3) accumulates, counter=8 | Got counter=%d\n", test_num, sm_get_counter());
    }

    /* Test 6: START while RUNNING is ignored */
    test_num++;
    result = sm_process_event(EVENT_START, 0);
    if (result == 1) {
        passed++;
        printf("[PASS] Test %d: START while RUNNING is ignored\n", test_num);
    } else {
        printf("[FAIL] Test %d: START while RUNNING is ignored | Expected: 1, Got: %d\n", test_num, result);
    }

    /* Test 7: STOP transitions back to IDLE */
    test_num++;
    result = sm_process_event(EVENT_STOP, 0);
    if (result == 0 && sm_get_state() == STATE_IDLE) {
        passed++;
        printf("[PASS] Test %d: STOP transitions RUNNING → IDLE\n", test_num);
    } else {
        printf("[FAIL] Test %d: STOP transitions RUNNING → IDLE | result=%d, state=%d\n", test_num, result, sm_get_state());
    }

    /* Test 8: Counter preserved after STOP */
    test_num++;
    if (sm_get_counter() == 8) {
        passed++;
        printf("[PASS] Test %d: Counter preserved after STOP (=8)\n", test_num);
    } else {
        printf("[FAIL] Test %d: Counter preserved after STOP (=8) | Got: %d\n", test_num, sm_get_counter());
    }

    /* Test 9: ERROR from IDLE */
    test_num++;
    result = sm_process_event(EVENT_ERROR, 0);
    if (result == 0 && sm_get_state() == STATE_ERROR) {
        passed++;
        printf("[PASS] Test %d: ERROR transitions IDLE → ERROR\n", test_num);
    } else {
        printf("[FAIL] Test %d: ERROR transitions IDLE → ERROR | result=%d, state=%d\n", test_num, result, sm_get_state());
    }

    /* Test 10: DATA while ERROR is ignored */
    test_num++;
    result = sm_process_event(EVENT_DATA, 10);
    if (result == 1) {
        passed++;
        printf("[PASS] Test %d: DATA while ERROR is ignored\n", test_num);
    } else {
        printf("[FAIL] Test %d: DATA while ERROR is ignored | Expected: 1, Got: %d\n", test_num, result);
    }

    /* Test 11: RESET from ERROR goes to IDLE and clears counter */
    test_num++;
    result = sm_process_event(EVENT_RESET, 0);
    if (result == 0 && sm_get_state() == STATE_IDLE && sm_get_counter() == 0) {
        passed++;
        printf("[PASS] Test %d: RESET transitions ERROR → IDLE, counter cleared\n", test_num);
    } else {
        printf("[FAIL] Test %d: RESET transitions ERROR → IDLE, counter cleared | result=%d, state=%d, counter=%d\n", test_num, result, sm_get_state(), sm_get_counter());
    }

    /* Test 12: RESET while IDLE is ignored */
    test_num++;
    result = sm_process_event(EVENT_RESET, 0);
    if (result == 1) {
        passed++;
        printf("[PASS] Test %d: RESET while IDLE is ignored\n", test_num);
    } else {
        printf("[FAIL] Test %d: RESET while IDLE is ignored | Expected: 1, Got: %d\n", test_num, result);
    }

    /* Test 13: Invalid event returns -1 */
    test_num++;
    result = sm_process_event(99, 0);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: Invalid event 99 returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: Invalid event 99 returns -1 | Expected: -1, Got: %d\n", test_num, result);
    }

    /* Test 14: Negative event returns -1 */
    test_num++;
    result = sm_process_event(-1, 0);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: Negative event -1 returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: Negative event -1 returns -1 | Expected: -1, Got: %d\n", test_num, result);
    }

    /* Test 15: ERROR from RUNNING */
    test_num++;
    sm_process_event(EVENT_START, 0);
    sm_process_event(EVENT_DATA, 7);
    result = sm_process_event(EVENT_ERROR, 0);
    if (result == 0 && sm_get_state() == STATE_ERROR) {
        passed++;
        printf("[PASS] Test %d: ERROR transitions RUNNING → ERROR\n", test_num);
    } else {
        printf("[FAIL] Test %d: ERROR transitions RUNNING → ERROR | result=%d, state=%d\n", test_num, result, sm_get_state());
    }

    /* Test 16: Full init resets everything */
    test_num++;
    sm_init();
    if (sm_get_state() == STATE_IDLE && sm_get_counter() == 0) {
        passed++;
        printf("[PASS] Test %d: sm_init resets state to IDLE and counter to 0\n", test_num);
    } else {
        printf("[FAIL] Test %d: sm_init resets state to IDLE and counter to 0 | state=%d, counter=%d\n", test_num, sm_get_state(), sm_get_counter());
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);

    return (passed == num_tests) ? 0 : 1;
}
