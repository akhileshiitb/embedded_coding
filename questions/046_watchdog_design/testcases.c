#include <stdio.h>
#include <stdint.h>

/* ============================================================
 * Replicate the register interface from solution.c
 * ============================================================ */
struct wdt_regs {
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t COUNT;
    volatile uint32_t KICK;
    volatile uint32_t RESET;
};

#define WDT_CTRL_EN     (1U << 0)
#define WDT_KICK_MAGIC  0x5A5A5A5AU
#define WDT_RESET_FLAG  (1U << 0)

/* ---- functions under test ---- */
extern void wdt_enable(volatile struct wdt_regs *regs, uint32_t timeout_ticks);
extern void wdt_pet(volatile struct wdt_regs *regs);
extern void wdt_disable(volatile struct wdt_regs *regs);
extern void wdt_task_init(void);
extern void wdt_register_task(int task_id);
extern void wdt_unregister_task(int task_id);
extern void wdt_task_checkin(int task_id);
extern int  wdt_task_check(volatile struct wdt_regs *regs);

/* ============================================================
 * Simulated WDT hardware "tick engine"
 *
 * The candidate's wdt_pet() writes WDT_KICK_MAGIC to regs->KICK.
 * Since we can't hook a volatile write in plain C, we model the
 * feed inside sim_tick(): before decrementing, we check if KICK
 * holds the magic value; if so, we reload COUNT and clear KICK.
 * This faithfully simulates hardware that reloads on a magic write.
 * ============================================================ */

/* Process a pending magic-kick write (reload COUNT), if present. */
static void sim_process_kick(struct wdt_regs *r) {
    if (r->KICK == WDT_KICK_MAGIC) {
        r->COUNT = r->LOAD;   /* magic feed reloads the counter */
    }
    r->KICK = 0;              /* consume the write (magic or not) */
}

/* Advance the simulated hardware by one tick. */
static void sim_tick(struct wdt_regs *r) {
    /* First, honor any pending feed from the driver. */
    sim_process_kick(r);

    if (!(r->CTRL & WDT_CTRL_EN)) {
        return;                       /* disabled: no counting */
    }
    if (r->RESET & WDT_RESET_FLAG) {
        return;                       /* already reset: stop */
    }
    if (r->COUNT > 0) {
        r->COUNT--;
    }
    if (r->COUNT == 0) {
        r->RESET |= WDT_RESET_FLAG;   /* simulated SoC reset */
    }
}

/* Run n ticks. */
static void sim_run(struct wdt_regs *r, int n) {
    for (int i = 0; i < n; i++) {
        sim_tick(r);
    }
}

static int sim_did_reset(const struct wdt_regs *r) {
    return (r->RESET & WDT_RESET_FLAG) ? 1 : 0;
}

static void regs_reset(struct wdt_regs *r) {
    r->CTRL = 0;
    r->LOAD = 0;
    r->COUNT = 0;
    r->KICK = 0;
    r->RESET = 0;
}

/* ============================================================
 * Tests
 * ============================================================ */
int main(void) {
    int passed = 0;
    int num_tests = 17;
    int test_num = 0;
    struct wdt_regs regs;

    /* === Test 1: wdt_enable sets LOAD, COUNT, EN === */
    {
        test_num++;
        regs_reset(&regs);
        wdt_enable(&regs, 10);
        if ((regs.CTRL & WDT_CTRL_EN) && regs.LOAD == 10 && regs.COUNT == 10) {
            passed++;
            printf("[PASS] Test %d: enable sets LOAD=10, COUNT=10, EN=1\n", test_num);
        } else {
            printf("[FAIL] Test %d: enable | CTRL=0x%X, LOAD=%u, COUNT=%u\n",
                   test_num, regs.CTRL, regs.LOAD, regs.COUNT);
        }
    }

    /* === Test 2: wdt_pet writes the magic value === */
    {
        test_num++;
        regs_reset(&regs);
        wdt_enable(&regs, 10);
        wdt_pet(&regs);
        if (regs.KICK == WDT_KICK_MAGIC) {
            passed++;
            printf("[PASS] Test %d: pet writes magic value to KICK\n", test_num);
        } else {
            printf("[FAIL] Test %d: pet | KICK=0x%X (expected 0x%X)\n",
                   test_num, regs.KICK, WDT_KICK_MAGIC);
        }
    }

    /* === Test 3: wdt_disable clears EN === */
    {
        test_num++;
        regs_reset(&regs);
        wdt_enable(&regs, 10);
        wdt_disable(&regs);
        if (!(regs.CTRL & WDT_CTRL_EN)) {
            passed++;
            printf("[PASS] Test %d: disable clears EN\n", test_num);
        } else {
            printf("[FAIL] Test %d: disable | CTRL=0x%X\n", test_num, regs.CTRL);
        }
    }

    /* === Test 4: counter decrements on tick when enabled === */
    {
        test_num++;
        regs_reset(&regs);
        wdt_enable(&regs, 10);
        sim_run(&regs, 3);
        if (regs.COUNT == 7 && !sim_did_reset(&regs)) {
            passed++;
            printf("[PASS] Test %d: 3 ticks -> COUNT=7, no reset\n", test_num);
        } else {
            printf("[FAIL] Test %d: | COUNT=%u, reset=%d\n",
                   test_num, regs.COUNT, sim_did_reset(&regs));
        }
    }

    /* === Test 5: SAD PATH — no pet -> reset when COUNT hits 0 === */
    {
        test_num++;
        regs_reset(&regs);
        wdt_enable(&regs, 5);
        sim_run(&regs, 5);   /* never petted */
        if (sim_did_reset(&regs)) {
            passed++;
            printf("[PASS] Test %d: SAD PATH — no pet -> SoC reset\n", test_num);
        } else {
            printf("[FAIL] Test %d: expected reset | COUNT=%u, reset=%d\n",
                   test_num, regs.COUNT, sim_did_reset(&regs));
        }
    }

    /* === Test 6: HAPPY PATH — petting keeps it alive === */
    {
        test_num++;
        regs_reset(&regs);
        wdt_enable(&regs, 5);
        /* pet every 3 ticks over 20 ticks — should never reset */
        int reset_seen = 0;
        for (int i = 0; i < 20; i++) {
            if (i % 3 == 0) {
                wdt_pet(&regs);
            }
            sim_tick(&regs);
            if (sim_did_reset(&regs)) { reset_seen = 1; break; }
        }
        if (!reset_seen) {
            passed++;
            printf("[PASS] Test %d: HAPPY PATH — regular pet -> no reset\n", test_num);
        } else {
            printf("[FAIL] Test %d: unexpected reset during happy path\n", test_num);
        }
    }

    /* === Test 7: pet reloads COUNT to LOAD === */
    {
        test_num++;
        regs_reset(&regs);
        wdt_enable(&regs, 10);
        sim_run(&regs, 4);        /* COUNT = 6 */
        wdt_pet(&regs);
        sim_tick(&regs);          /* process kick (reload to 10), then decrement to 9 */
        if (regs.COUNT == 9) {
            passed++;
            printf("[PASS] Test %d: pet reloads COUNT to LOAD\n", test_num);
        } else {
            printf("[FAIL] Test %d: | COUNT=%u (expected 9)\n", test_num, regs.COUNT);
        }
    }

    /* === Test 8: non-magic write does NOT feed === */
    {
        test_num++;
        regs_reset(&regs);
        wdt_enable(&regs, 5);
        sim_run(&regs, 2);            /* COUNT = 3 */
        regs.KICK = 0x12345678U;      /* bogus write, not the magic value */
        sim_run(&regs, 3);            /* should NOT reload; counts 3->0 -> reset */
        if (sim_did_reset(&regs)) {
            passed++;
            printf("[PASS] Test %d: non-magic write does not feed -> reset\n", test_num);
        } else {
            printf("[FAIL] Test %d: bogus kick wrongly fed | COUNT=%u\n", test_num, regs.COUNT);
        }
    }

    /* === Test 9: disabled watchdog does not count/reset === */
    {
        test_num++;
        regs_reset(&regs);
        wdt_enable(&regs, 3);
        wdt_disable(&regs);
        sim_run(&regs, 10);
        if (!sim_did_reset(&regs)) {
            passed++;
            printf("[PASS] Test %d: disabled WDT never resets\n", test_num);
        } else {
            printf("[FAIL] Test %d: disabled WDT wrongly reset\n", test_num);
        }
    }

    /* === Test 10: task_check with no tasks registered feeds (vacuously true) === */
    {
        test_num++;
        regs_reset(&regs);
        wdt_task_init();
        wdt_enable(&regs, 5);
        int fed = wdt_task_check(&regs);
        if (fed == 1 && regs.KICK == WDT_KICK_MAGIC) {
            passed++;
            printf("[PASS] Test %d: task_check with no tasks feeds (returns 1)\n", test_num);
        } else {
            printf("[FAIL] Test %d: | fed=%d, KICK=0x%X\n", test_num, fed, regs.KICK);
        }
    }

    /* === Test 11: task_check withholds when a registered task hasn't checked in === */
    {
        test_num++;
        regs_reset(&regs);
        wdt_task_init();
        wdt_enable(&regs, 5);
        wdt_register_task(0);
        wdt_register_task(1);
        wdt_task_checkin(0);           /* only task 0 checks in */
        int fed = wdt_task_check(&regs);
        if (fed == 0 && regs.KICK != WDT_KICK_MAGIC) {
            passed++;
            printf("[PASS] Test %d: task_check withholds when task 1 missing (returns 0)\n", test_num);
        } else {
            printf("[FAIL] Test %d: | fed=%d, KICK=0x%X\n", test_num, fed, regs.KICK);
        }
    }

    /* === Test 12: task_check feeds when all registered tasks check in === */
    {
        test_num++;
        regs_reset(&regs);
        wdt_task_init();
        wdt_enable(&regs, 5);
        wdt_register_task(0);
        wdt_register_task(1);
        wdt_task_checkin(0);
        wdt_task_checkin(1);
        int fed = wdt_task_check(&regs);
        if (fed == 1 && regs.KICK == WDT_KICK_MAGIC) {
            passed++;
            printf("[PASS] Test %d: task_check feeds when all check in (returns 1)\n", test_num);
        } else {
            printf("[FAIL] Test %d: | fed=%d, KICK=0x%X\n", test_num, fed, regs.KICK);
        }
    }

    /* === Test 13: check-in mask cleared after a successful feed === */
    {
        test_num++;
        regs_reset(&regs);
        wdt_task_init();
        wdt_enable(&regs, 5);
        wdt_register_task(0);
        wdt_task_checkin(0);
        wdt_task_check(&regs);         /* feeds, clears check-in */
        /* next round: task 0 has NOT checked in again */
        regs.KICK = 0;                 /* clear observation */
        int fed = wdt_task_check(&regs);
        if (fed == 0) {
            passed++;
            printf("[PASS] Test %d: check-in mask cleared after feed (next round withholds)\n", test_num);
        } else {
            printf("[FAIL] Test %d: stale check-in wrongly fed again\n", test_num);
        }
    }

    /* === Test 14: unregister a stuck task allows feeding to resume === */
    {
        test_num++;
        regs_reset(&regs);
        wdt_task_init();
        wdt_enable(&regs, 5);
        wdt_register_task(0);
        wdt_register_task(1);
        wdt_task_checkin(0);
        int fed_before = wdt_task_check(&regs);   /* withholds (task 1 missing) */
        wdt_unregister_task(1);                    /* task 1 done / removed */
        wdt_task_checkin(0);
        int fed_after = wdt_task_check(&regs);     /* now only task 0 matters */
        if (fed_before == 0 && fed_after == 1) {
            passed++;
            printf("[PASS] Test %d: unregister stuck task lets feeding resume\n", test_num);
        } else {
            printf("[FAIL] Test %d: | before=%d, after=%d\n", test_num, fed_before, fed_after);
        }
    }

    /* === Test 15: DEADLOCK PATH — stuck task -> system resets over time === */
    {
        test_num++;
        regs_reset(&regs);
        wdt_task_init();
        wdt_enable(&regs, 4);
        wdt_register_task(0);
        wdt_register_task(1);   /* task 1 will be "deadlocked" — never checks in */

        int reset_seen = 0;
        for (int round = 0; round < 10; round++) {
            wdt_task_checkin(0);          /* task 0 healthy */
            /* task 1 deadlocked: no checkin */
            wdt_task_check(&regs);        /* will withhold every round */
            sim_tick(&regs);
            if (sim_did_reset(&regs)) { reset_seen = 1; break; }
        }
        if (reset_seen) {
            passed++;
            printf("[PASS] Test %d: DEADLOCK PATH — stuck task -> SoC reset\n", test_num);
        } else {
            printf("[FAIL] Test %d: deadlock not caught, no reset\n", test_num);
        }
    }

    /* === Test 16: healthy multi-task loop never resets === */
    {
        test_num++;
        regs_reset(&regs);
        wdt_task_init();
        wdt_enable(&regs, 4);
        wdt_register_task(0);
        wdt_register_task(2);
        wdt_register_task(5);

        int reset_seen = 0;
        for (int round = 0; round < 30; round++) {
            wdt_task_checkin(0);
            wdt_task_checkin(2);
            wdt_task_checkin(5);
            wdt_task_check(&regs);   /* all in -> feeds every round */
            sim_tick(&regs);
            if (sim_did_reset(&regs)) { reset_seen = 1; break; }
        }
        if (!reset_seen) {
            passed++;
            printf("[PASS] Test %d: healthy 3-task loop -> no reset\n", test_num);
        } else {
            printf("[FAIL] Test %d: healthy loop wrongly reset\n", test_num);
        }
    }

    /* === Test 17: unregister must clear the stale check-in bit ===
     *
     * Spec: wdt_unregister_task clears the task's bit in BOTH masks.
     * If it only clears registered_tasks, a stale check-in bit lingers.
     * Re-registering the task would then make it look "alive" on the very
     * first check WITHOUT a fresh check-in — a real watchdog safety hole.
     */
    {
        test_num++;
        regs_reset(&regs);
        wdt_task_init();
        wdt_enable(&regs, 5);

        wdt_register_task(1);
        wdt_task_checkin(1);           /* task 1 checks in (sets its check-in bit) */
        wdt_unregister_task(1);        /* must clear BOTH masks for task 1 */

        /* Task 1 comes back but has NOT checked in yet this life. */
        wdt_register_task(1);
        regs.KICK = 0;                 /* clear observation */
        int fed = wdt_task_check(&regs);   /* should WITHHOLD: no fresh check-in */
        if (fed == 0 && regs.KICK != WDT_KICK_MAGIC) {
            passed++;
            printf("[PASS] Test %d: unregister clears stale check-in (re-registered task must re-check-in)\n", test_num);
        } else {
            printf("[FAIL] Test %d: stale check-in bit survived unregister | fed=%d, KICK=0x%X\n",
                   test_num, fed, regs.KICK);
        }
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);
    return (passed == num_tests) ? 0 : 1;
}
