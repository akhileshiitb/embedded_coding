#include <stdio.h>
#include <string.h>

/* ---- functions under test (from solution.c) ---- */
extern void sw_timer_init(void);
extern void sw_timer_start(unsigned int expiry, void (*cb)(void *arg), void *arg);
extern void sw_timer_on_expire(void);

/* ============================================================
 * Simulated single hardware timer
 *
 * - sim_now: the current absolute time (ticks), controlled by the test.
 * - sim_armed / sim_expiry: whether the one hardware timer is armed, and for when.
 *
 * hw_get_time() and hw_timer_set() are the primitives the solution calls.
 * sim_advance_to(t) moves time forward; whenever time reaches the armed
 * expiry, it calls sw_timer_on_expire() (simulating the hardware interrupt),
 * exactly as real hardware would.
 * ============================================================ */

static unsigned int sim_now;
static int          sim_armed;
static unsigned int sim_expiry;

/* Provided to the solution */
unsigned int hw_get_time(void) {
    return sim_now;
}

void hw_timer_set(unsigned int expiry) {
    sim_armed = 1;
    sim_expiry = expiry;
}

/* Reset the simulated hardware + module for a fresh test. */
static void sim_reset(void) {
    sim_now = 0;
    sim_armed = 0;
    sim_expiry = 0;
    sw_timer_init();
}

/*
 * Advance simulated time up to absolute time `target`, dispatching the
 * hardware timer whenever its armed expiry is reached. Because a dispatch may
 * re-arm the timer for a still-earlier-or-equal time (past-due) or a later
 * time, we loop until no armed expiry is <= current time.
 */
static void sim_advance_to(unsigned int target) {
    for (;;) {
        if (sim_armed && sim_expiry <= target) {
            /* jump time to the expiry (not past it) and fire */
            if (sim_expiry > sim_now) sim_now = sim_expiry;
            sim_armed = 0;              /* consume this arming; solution re-arms */
            sw_timer_on_expire();
            /* loop again: solution may have re-armed for another due time */
        } else {
            break;
        }
    }
    if (target > sim_now) sim_now = target;
    /* After settling at target, also fire anything armed exactly for <= now
     * (covers timers started in the past-due region). */
    while (sim_armed && sim_expiry <= sim_now) {
        sim_armed = 0;
        sw_timer_on_expire();
    }
}

/* ============================================================
 * Callback instrumentation — record firing order by id
 * ============================================================ */
static int  fire_log[64];
static int  fire_count;

static void record_fire(void *arg) {
    int id = (int)(long)arg;
    if (fire_count < 64) fire_log[fire_count++] = id;
}

static void clear_log(void) {
    fire_count = 0;
    memset(fire_log, 0, sizeof(fire_log));
}

/* helper: start a timer whose callback records `id` */
static void start_id(unsigned int expiry, int id) {
    sw_timer_start(expiry, record_fire, (void *)(long)id);
}

static int log_equals(const int *expect, int n) {
    if (fire_count != n) return 0;
    for (int i = 0; i < n; i++) if (fire_log[i] != expect[i]) return 0;
    return 1;
}

/* For a "set" comparison where order among equal-expiry timers is unspecified */
static int log_is_permutation(const int *expect, int n) {
    if (fire_count != n) return 0;
    int used[64] = {0};
    for (int i = 0; i < n; i++) {
        int found = 0;
        for (int j = 0; j < n; j++) {
            if (!used[j] && fire_log[i] == expect[j]) { used[j] = 1; found = 1; break; }
        }
        if (!found) return 0;
    }
    return 1;
}

/* ============================================================
 * Tests
 * ============================================================ */
int main(void) {
    int passed = 0;
    int num_tests = 12;
    int test_num = 0;

    /* === Test 1: single timer fires === */
    {
        test_num++;
        sim_reset(); clear_log();
        start_id(10, 1);
        sim_advance_to(10);
        int e[] = {1};
        if (log_equals(e, 1)) { passed++; printf("[PASS] Test %d: single timer fires\n", test_num); }
        else printf("[FAIL] Test %d: count=%d\n", test_num, fire_count);
    }

    /* === Test 2: timer does NOT fire early === */
    {
        test_num++;
        sim_reset(); clear_log();
        start_id(100, 1);
        sim_advance_to(50);      /* not yet due */
        if (fire_count == 0) { passed++; printf("[PASS] Test %d: no early fire\n", test_num); }
        else printf("[FAIL] Test %d: fired early count=%d\n", test_num, fire_count);
    }

    /* === Test 3: two timers fire in expiry order (in-order insert) === */
    {
        test_num++;
        sim_reset(); clear_log();
        start_id(10, 1);
        start_id(20, 2);
        sim_advance_to(100);
        int e[] = {1, 2};
        if (log_equals(e, 2)) { passed++; printf("[PASS] Test %d: two timers in order\n", test_num); }
        else printf("[FAIL] Test %d: got count=%d [%d,%d]\n", test_num, fire_count, fire_log[0], fire_log[1]);
    }

    /* === Test 4: timers fire in expiry order (reverse insert) === */
    {
        test_num++;
        sim_reset(); clear_log();
        start_id(30, 3);
        start_id(10, 1);
        start_id(20, 2);
        sim_advance_to(100);
        int e[] = {1, 2, 3};
        if (log_equals(e, 3)) { passed++; printf("[PASS] Test %d: reverse insert fires in expiry order\n", test_num); }
        else printf("[FAIL] Test %d: got [%d,%d,%d]\n", test_num, fire_log[0], fire_log[1], fire_log[2]);
    }

    /* === Test 5: soonest re-arm honored step-by-step === */
    {
        test_num++;
        sim_reset(); clear_log();
        start_id(30, 3);
        start_id(10, 1);
        start_id(20, 2);
        sim_advance_to(10);
        int e1[] = {1};
        int ok = log_equals(e1, 1);
        sim_advance_to(20);
        int e2[] = {1, 2};
        ok = ok && log_equals(e2, 2);
        sim_advance_to(30);
        int e3[] = {1, 2, 3};
        ok = ok && log_equals(e3, 3);
        if (ok) { passed++; printf("[PASS] Test %d: step-by-step re-arm to next soonest\n", test_num); }
        else printf("[FAIL] Test %d: count=%d\n", test_num, fire_count);
    }

    /* === Test 6: multiple timers with SAME expiry all fire === */
    {
        test_num++;
        sim_reset(); clear_log();
        start_id(50, 1);
        start_id(50, 2);
        start_id(50, 3);
        sim_advance_to(50);
        int e[] = {1, 2, 3};
        if (log_is_permutation(e, 3)) { passed++; printf("[PASS] Test %d: same-expiry timers all fire\n", test_num); }
        else printf("[FAIL] Test %d: count=%d\n", test_num, fire_count);
    }

    /* === Test 7: past-due timer fires on next dispatch === */
    {
        test_num++;
        sim_reset(); clear_log();
        sim_now = 100;              /* jump time forward before starting */
        start_id(80, 7);            /* expiry < now */
        sim_advance_to(100);
        int e[] = {7};
        if (log_equals(e, 1)) { passed++; printf("[PASS] Test %d: past-due timer fires\n", test_num); }
        else printf("[FAIL] Test %d: count=%d\n", test_num, fire_count);
    }

    /* === Test 8: interleaved starts and dispatches === */
    {
        test_num++;
        sim_reset(); clear_log();
        start_id(20, 2);
        sim_advance_to(10);         /* nothing due yet */
        start_id(15, 9);            /* 15 < 20, becomes soonest (now=10) */
        sim_advance_to(15);
        int e1[] = {9};
        int ok = log_equals(e1, 1);
        sim_advance_to(20);
        int e2[] = {9, 2};
        ok = ok && log_equals(e2, 2);
        if (ok) { passed++; printf("[PASS] Test %d: interleaved start/dispatch\n", test_num); }
        else printf("[FAIL] Test %d: count=%d\n", test_num, fire_count);
    }

    /* === Test 9: mixed expiries fire in correct global order === */
    {
        test_num++;
        sim_reset(); clear_log();
        start_id(40, 4);
        start_id(10, 1);
        start_id(30, 3);
        start_id(20, 2);
        start_id(50, 5);
        sim_advance_to(1000);
        int e[] = {1, 2, 3, 4, 5};
        if (log_equals(e, 5)) { passed++; printf("[PASS] Test %d: five timers global order\n", test_num); }
        else printf("[FAIL] Test %d: count=%d\n", test_num, fire_count);
    }

    /* === Test 10: many timers (16) all fire in order === */
    {
        test_num++;
        sim_reset(); clear_log();
        /* start in shuffled order */
        int order[16] = {8,3,15,1,12,6,10,4,14,2,9,7,16,5,13,11};
        for (int i = 0; i < 16; i++) {
            start_id((unsigned)order[i] * 10, order[i]);
        }
        sim_advance_to(1000);
        int e[16];
        for (int i = 0; i < 16; i++) e[i] = i + 1;
        if (log_equals(e, 16)) { passed++; printf("[PASS] Test %d: 16 timers all fire in order\n", test_num); }
        else printf("[FAIL] Test %d: count=%d\n", test_num, fire_count);
    }

    /* === Test 11: dispatch fires only DUE timers, not future ones === */
    {
        test_num++;
        sim_reset(); clear_log();
        start_id(10, 1);
        start_id(1000, 99);         /* far future */
        sim_advance_to(10);
        int e[] = {1};
        /* only timer 1 should have fired; 99 still pending */
        if (log_equals(e, 1)) { passed++; printf("[PASS] Test %d: only due timer fires, future stays pending\n", test_num); }
        else printf("[FAIL] Test %d: count=%d\n", test_num, fire_count);
    }

    /* === Test 12: two due at once after a late dispatch === */
    {
        test_num++;
        sim_reset(); clear_log();
        start_id(10, 1);
        start_id(12, 2);
        /* Advance straight to 20: both 10 and 12 are overdue and must both fire. */
        sim_advance_to(20);
        int e[] = {1, 2};
        if (log_equals(e, 2)) { passed++; printf("[PASS] Test %d: late dispatch fires all overdue\n", test_num); }
        else printf("[FAIL] Test %d: count=%d [%d,%d]\n", test_num, fire_count, fire_log[0], fire_log[1]);
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);
    return (passed == num_tests) ? 0 : 1;
}
