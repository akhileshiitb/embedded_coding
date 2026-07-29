#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#define PPBUF_MAX_SIZE 128
#define PPBUF_STATE_IDLE     0
#define PPBUF_STATE_READY    1
#define PPBUF_STATE_OVERRUN  2

/* Declaration of solution functions */
extern int      ppbuf_init(int buf_size);
extern int32_t *ppbuf_get_write_buffer(void);
extern int32_t *ppbuf_get_read_buffer(void);
extern int      ppbuf_swap(void);
extern int      ppbuf_consume(void);
extern int      ppbuf_get_state(void);
extern int      ppbuf_get_overrun_count(void);
extern int      ppbuf_write_sample(int index, int32_t value);
extern int32_t  ppbuf_read_sample(int index);

int main(void) {
    int num_tests = 16;
    int passed = 0;
    int result;
    int test_num = 0;

    /* Test 1: Init succeeds */
    test_num++;
    result = ppbuf_init(64);
    if (result == 0) {
        passed++;
        printf("[PASS] Test %d: ppbuf_init(64) succeeds\n", test_num);
    } else {
        printf("[FAIL] Test %d: ppbuf_init(64) | Expected: 0, Got: %d\n", test_num, result);
    }

    /* Test 2: Init with invalid size fails */
    test_num++;
    result = ppbuf_init(0);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: ppbuf_init(0) fails\n", test_num);
    } else {
        printf("[FAIL] Test %d: ppbuf_init(0) | Expected: -1, Got: %d\n", test_num, result);
    }

    /* Re-init for further tests */
    ppbuf_init(4);

    /* Test 3: Initial state is IDLE */
    test_num++;
    result = ppbuf_get_state();
    if (result == PPBUF_STATE_IDLE) {
        passed++;
        printf("[PASS] Test %d: Initial state is IDLE\n", test_num);
    } else {
        printf("[FAIL] Test %d: Initial state | Expected: %d, Got: %d\n",
               test_num, PPBUF_STATE_IDLE, result);
    }

    /* Test 4: No read buffer available initially */
    test_num++;
    int32_t *rd = ppbuf_get_read_buffer();
    if (rd == NULL) {
        passed++;
        printf("[PASS] Test %d: No read buffer initially (NULL)\n", test_num);
    } else {
        printf("[FAIL] Test %d: get_read_buffer | Expected: NULL, Got: %p\n",
               test_num, (void*)rd);
    }

    /* Test 5: Write buffer is valid */
    test_num++;
    int32_t *wr = ppbuf_get_write_buffer();
    if (wr != NULL) {
        passed++;
        printf("[PASS] Test %d: Write buffer is non-NULL\n", test_num);
    } else {
        printf("[FAIL] Test %d: get_write_buffer returned NULL\n", test_num);
    }

    /* Test 6: Write samples to buffer */
    test_num++;
    ppbuf_write_sample(0, 1000);
    ppbuf_write_sample(1, 2000);
    ppbuf_write_sample(2, 3000);
    ppbuf_write_sample(3, 4000);
    wr = ppbuf_get_write_buffer();
    if (wr != NULL && wr[0] == 1000 && wr[3] == 4000) {
        passed++;
        printf("[PASS] Test %d: Write samples to buffer\n", test_num);
    } else {
        printf("[FAIL] Test %d: Write samples | wr=%p\n", test_num, (void*)wr);
    }

    /* Test 7: Swap makes buffer ready */
    test_num++;
    int32_t *wr_before = ppbuf_get_write_buffer();
    result = ppbuf_swap();
    int32_t *wr_after = ppbuf_get_write_buffer();
    if (result == 0 && ppbuf_get_state() == PPBUF_STATE_READY
        && wr_before != wr_after) {
        passed++;
        printf("[PASS] Test %d: Swap succeeds, state=READY, write buffer changed\n",
               test_num);
    } else {
        printf("[FAIL] Test %d: Swap | result=%d, state=%d\n",
               test_num, result, ppbuf_get_state());
    }

    /* Test 8: Read buffer returns the old write buffer data */
    test_num++;
    int32_t sample = ppbuf_read_sample(0);
    if (sample == 1000) {
        passed++;
        printf("[PASS] Test %d: Read sample[0] = 1000 from ready buffer\n", test_num);
    } else {
        printf("[FAIL] Test %d: read_sample(0) | Expected: 1000, Got: %d\n",
               test_num, sample);
    }

    /* Test 9: Read sample at other indices */
    test_num++;
    sample = ppbuf_read_sample(3);
    if (sample == 4000) {
        passed++;
        printf("[PASS] Test %d: Read sample[3] = 4000 from ready buffer\n", test_num);
    } else {
        printf("[FAIL] Test %d: read_sample(3) | Expected: 4000, Got: %d\n",
               test_num, sample);
    }

    /* Test 10: Consume marks buffer as processed */
    test_num++;
    result = ppbuf_consume();
    if (result == 0 && ppbuf_get_state() == PPBUF_STATE_IDLE
        && ppbuf_get_read_buffer() == NULL) {
        passed++;
        printf("[PASS] Test %d: Consume succeeds, state=IDLE, read=NULL\n", test_num);
    } else {
        printf("[FAIL] Test %d: Consume | result=%d, state=%d\n",
               test_num, result, ppbuf_get_state());
    }

    /* Test 11: Consume when no buffer ready returns -1 */
    test_num++;
    result = ppbuf_consume();
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: Consume with no ready buffer returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: Consume empty | Expected: -1, Got: %d\n",
               test_num, result);
    }

    /* Test 12: Overrun detection — swap twice without consume */
    test_num++;
    ppbuf_swap();  /* First swap: buffer becomes ready */
    result = ppbuf_swap();  /* Second swap without consume: overrun! */
    if (result == -1 && ppbuf_get_overrun_count() == 1) {
        passed++;
        printf("[PASS] Test %d: Overrun detected on double swap\n", test_num);
    } else {
        printf("[FAIL] Test %d: Overrun | result=%d, count=%d\n",
               test_num, result, ppbuf_get_overrun_count());
    }

    /* Test 13: State is OVERRUN after overrun */
    test_num++;
    result = ppbuf_get_state();
    if (result == PPBUF_STATE_OVERRUN) {
        passed++;
        printf("[PASS] Test %d: State is OVERRUN after overrun\n", test_num);
    } else {
        printf("[FAIL] Test %d: State after overrun | Expected: %d, Got: %d\n",
               test_num, PPBUF_STATE_OVERRUN, result);
    }

    /* Test 14: System recovers after consume */
    test_num++;
    ppbuf_consume();
    ppbuf_write_sample(0, 9999);
    ppbuf_swap();
    sample = ppbuf_read_sample(0);
    if (sample == 9999) {
        passed++;
        printf("[PASS] Test %d: System recovers after overrun\n", test_num);
    } else {
        printf("[FAIL] Test %d: Recovery | Expected: 9999, Got: %d\n",
               test_num, sample);
    }

    /* Test 15: Write sample out of range returns -1 */
    test_num++;
    ppbuf_consume();
    result = ppbuf_write_sample(4, 100);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: write_sample(4) out of range returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: write_sample OOB | Expected: -1, Got: %d\n",
               test_num, result);
    }

    /* Test 16: Init with size > PPBUF_MAX_SIZE fails */
    test_num++;
    result = ppbuf_init(PPBUF_MAX_SIZE + 1);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: ppbuf_init(%d) fails (too large)\n",
               test_num, PPBUF_MAX_SIZE + 1);
    } else {
        printf("[FAIL] Test %d: ppbuf_init(%d) | Expected: -1, Got: %d\n",
               test_num, PPBUF_MAX_SIZE + 1, result);
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);
    return (passed == num_tests) ? 0 : 1;
}
