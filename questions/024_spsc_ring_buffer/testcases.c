#include <stdio.h>
#include <stdint.h>
#include <stdatomic.h>

/* Declaration of solution functions */
extern int  spsc_init(int capacity);
extern int  spsc_push(int32_t value);
extern int  spsc_pop(int32_t *out);
extern int  spsc_is_empty(void);
extern int  spsc_is_full(void);
extern int  spsc_count(void);

int main(void) {
    int num_tests = 16;
    int passed = 0;
    int result;
    int test_num = 0;
    int32_t val;

    /* Test 1: Init with valid capacity */
    test_num++;
    result = spsc_init(8);
    if (result == 0) {
        passed++;
        printf("[PASS] Test %d: spsc_init(8) succeeds\n", test_num);
    } else {
        printf("[FAIL] Test %d: spsc_init(8) | Expected: 0, Got: %d\n", test_num, result);
    }

    /* Test 2: Init with non-power-of-2 fails */
    test_num++;
    result = spsc_init(5);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: spsc_init(5) fails (not power of 2)\n", test_num);
    } else {
        printf("[FAIL] Test %d: spsc_init(5) | Expected: -1, Got: %d\n", test_num, result);
    }

    /* Re-initialize for remaining tests */
    spsc_init(8);

    /* Test 3: Buffer starts empty */
    test_num++;
    result = spsc_is_empty();
    if (result == 1) {
        passed++;
        printf("[PASS] Test %d: Buffer starts empty\n", test_num);
    } else {
        printf("[FAIL] Test %d: is_empty after init | Expected: 1, Got: %d\n", test_num, result);
    }

    /* Test 4: Push one element */
    test_num++;
    result = spsc_push(42);
    if (result == 0 && spsc_count() == 1) {
        passed++;
        printf("[PASS] Test %d: Push 42 succeeds, count=1\n", test_num);
    } else {
        printf("[FAIL] Test %d: Push 42 | result=%d, count=%d\n", test_num, result, spsc_count());
    }

    /* Test 5: Buffer not empty after push */
    test_num++;
    result = spsc_is_empty();
    if (result == 0) {
        passed++;
        printf("[PASS] Test %d: Not empty after push\n", test_num);
    } else {
        printf("[FAIL] Test %d: is_empty after push | Expected: 0, Got: %d\n", test_num, result);
    }

    /* Test 6: Pop returns correct value (FIFO) */
    test_num++;
    val = 0;
    result = spsc_pop(&val);
    if (result == 0 && val == 42) {
        passed++;
        printf("[PASS] Test %d: Pop returns 42\n", test_num);
    } else {
        printf("[FAIL] Test %d: Pop | Expected: val=42, Got: val=%d, result=%d\n", test_num, val, result);
    }

    /* Test 7: Pop from empty buffer fails */
    test_num++;
    result = spsc_pop(&val);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: Pop from empty returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: Pop from empty | Expected: -1, Got: %d\n", test_num, result);
    }

    /* Test 8: Fill buffer to capacity-1 (7 elements for capacity 8) */
    test_num++;
    spsc_init(8);
    int fill_ok = 1;
    for (int i = 0; i < 7; i++) {
        if (spsc_push(i * 10) != 0) { fill_ok = 0; break; }
    }
    if (fill_ok && spsc_count() == 7) {
        passed++;
        printf("[PASS] Test %d: Fill 7 elements into capacity-8 buffer\n", test_num);
    } else {
        printf("[FAIL] Test %d: Fill buffer | fill_ok=%d, count=%d\n", test_num, fill_ok, spsc_count());
    }

    /* Test 9: Buffer is full */
    test_num++;
    result = spsc_is_full();
    if (result == 1) {
        passed++;
        printf("[PASS] Test %d: Buffer is full (7/7 usable slots)\n", test_num);
    } else {
        printf("[FAIL] Test %d: is_full | Expected: 1, Got: %d\n", test_num, result);
    }

    /* Test 10: Push to full buffer fails */
    test_num++;
    result = spsc_push(999);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: Push to full buffer returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: Push to full | Expected: -1, Got: %d\n", test_num, result);
    }

    /* Test 11: Pop all in FIFO order */
    test_num++;
    int fifo_ok = 1;
    for (int i = 0; i < 7; i++) {
        spsc_pop(&val);
        if (val != i * 10) { fifo_ok = 0; break; }
    }
    if (fifo_ok) {
        passed++;
        printf("[PASS] Test %d: FIFO order preserved on pop\n", test_num);
    } else {
        printf("[FAIL] Test %d: FIFO order broken | Got unexpected val=%d\n", test_num, val);
    }

    /* Test 12: Wrap-around test */
    test_num++;
    spsc_init(4);  /* Small buffer for easy wraparound */
    spsc_push(1); spsc_push(2); spsc_push(3);  /* Fill 3/3 usable */
    spsc_pop(&val);  /* pop 1, now head > tail conceptually and wrap happens */
    spsc_push(4);    /* This wraps around */
    spsc_pop(&val);  /* should get 2 */
    int wrap_ok = (val == 2);
    spsc_pop(&val);  /* should get 3 */
    wrap_ok = wrap_ok && (val == 3);
    spsc_pop(&val);  /* should get 4 */
    wrap_ok = wrap_ok && (val == 4);
    if (wrap_ok) {
        passed++;
        printf("[PASS] Test %d: Wrap-around works correctly\n", test_num);
    } else {
        printf("[FAIL] Test %d: Wrap-around | Got unexpected val=%d\n", test_num, val);
    }

    /* Test 13: Count after partial operations */
    test_num++;
    spsc_init(8);
    spsc_push(10); spsc_push(20); spsc_push(30);
    spsc_pop(&val);
    result = spsc_count();
    if (result == 2) {
        passed++;
        printf("[PASS] Test %d: Count = 2 after 3 push + 1 pop\n", test_num);
    } else {
        printf("[FAIL] Test %d: Count after ops | Expected: 2, Got: %d\n", test_num, result);
    }

    /* Test 14: Init with capacity 0 fails */
    test_num++;
    result = spsc_init(0);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: spsc_init(0) fails\n", test_num);
    } else {
        printf("[FAIL] Test %d: spsc_init(0) | Expected: -1, Got: %d\n", test_num, result);
    }

    /* Test 15: Init with capacity 1 fails (need at least 2) */
    test_num++;
    result = spsc_init(1);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: spsc_init(1) fails (need >= 2)\n", test_num);
    } else {
        printf("[FAIL] Test %d: spsc_init(1) | Expected: -1, Got: %d\n", test_num, result);
    }

    /* Test 16: Smallest valid buffer (capacity=2, 1 usable slot) */
    test_num++;
    spsc_init(2);
    result = spsc_push(77);
    int push2_fail = spsc_push(88);  /* should fail — only 1 usable slot */
    spsc_pop(&val);
    if (result == 0 && push2_fail == -1 && val == 77) {
        passed++;
        printf("[PASS] Test %d: Capacity-2 buffer works (1 usable slot)\n", test_num);
    } else {
        printf("[FAIL] Test %d: Capacity-2 | push1=%d, push2=%d, val=%d\n", test_num, result, push2_fail, val);
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);
    return (passed == num_tests) ? 0 : 1;
}
