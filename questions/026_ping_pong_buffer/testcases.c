#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#define PPBUF_MAX_SIZE 128
#define PPBUF_STATE_IDLE     0
#define PPBUF_STATE_READY    1

/* Declaration of solution functions */
extern int      ppbuf_init(int buf_size);
extern int32_t *ppbuf_get_write_buffer(void);
extern int32_t *ppbuf_get_read_buffer(void);
extern int      ppbuf_swap(void);
extern int      ppbuf_consume(void);
extern int      ppbuf_get_state(void);
extern int      ppbuf_write_sample(int index, int32_t value);
extern int32_t  ppbuf_read_sample(int index);

int main(void) {
    int num_tests = 15;
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

    /* Test 12: Repeated swap/consume cycle (consumer keeps up) */
    test_num++;
    ppbuf_write_sample(0, 5555);
    ppbuf_swap();
    int ok = (ppbuf_read_sample(0) == 5555) && (ppbuf_consume() == 0);
    ppbuf_write_sample(0, 6666);
    ppbuf_swap();
    ok = ok && (ppbuf_read_sample(0) == 6666) && (ppbuf_consume() == 0);
    if (ok && ppbuf_get_state() == PPBUF_STATE_IDLE) {
        passed++;
        printf("[PASS] Test %d: Repeated swap/consume cycle works\n", test_num);
    } else {
        printf("[FAIL] Test %d: swap/consume cycle | state=%d\n",
               test_num, ppbuf_get_state());
    }

    /* Test 13: Write sample out of range returns -1 */
    test_num++;
    result = ppbuf_write_sample(4, 100);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: write_sample(4) out of range returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: write_sample OOB | Expected: -1, Got: %d\n",
               test_num, result);
    }

    /* Test 14: Init with size > PPBUF_MAX_SIZE fails */
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

    /* Test 15: read_sample out of range returns 0 (not a valid sample) */
    test_num++;
    ppbuf_init(4);
    ppbuf_write_sample(0, 7777);
    ppbuf_swap();                 /* buffer is now READY */
    sample = ppbuf_read_sample(4);  /* index 4 is out of range for size 4 */
    if (sample == 0) {
        passed++;
        printf("[PASS] Test %d: read_sample(4) out of range returns 0\n", test_num);
    } else {
        printf("[FAIL] Test %d: read_sample OOB | Expected: 0, Got: %d\n",
               test_num, sample);
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);
    return (passed == num_tests) ? 0 : 1;
}
