#include <stdio.h>

extern void debounce_init(void);
extern int  debounce_update(int raw_sample);
extern int  debounce_pressed_event(void);

/* Run a raw-sample stream through the debouncer, capturing the debounced
 * output each tick and counting press events. */
static void run_stream(const int *raw, int n, int *out_debounced, int *out_press_events) {
    int events = 0;
    debounce_init();
    for (int i = 0; i < n; i++) {
        out_debounced[i] = debounce_update(raw[i]);
        if (debounce_pressed_event()) events++;
    }
    *out_press_events = events;
}

static int arr_eq(const int *a, const int *b, int n) {
    for (int i = 0; i < n; i++) if (a[i] != b[i]) return 0;
    return 1;
}

int main(void) {
    int passed = 0;
    int num_tests = 10;
    int test_num = 0;
    int deb[64];
    int events;

    /* === Test 1: clean press commits after 4 consecutive 1s === */
    {
        test_num++;
        int raw[] = {0,1,1,1,1,0,0,0,0};
        int exp[] = {0,0,0,0,1,1,1,1,0};
        run_stream(raw, 9, deb, &events);
        if (arr_eq(deb, exp, 9)) { passed++; printf("[PASS] Test %d: clean press debounced level\n", test_num); }
        else printf("[FAIL] Test %d: got %d%d%d%d%d%d%d%d%d\n", test_num,
                    deb[0],deb[1],deb[2],deb[3],deb[4],deb[5],deb[6],deb[7],deb[8]);
    }

    /* === Test 2: bounce before stable press is rejected === */
    {
        test_num++;
        int raw[] = {0,0,1,0,1,0,1,1,1,1,0,0,0,0};
        int exp[] = {0,0,0,0,0,0,0,0,0,1,1,1,1,0};
        run_stream(raw, 14, deb, &events);
        if (arr_eq(deb, exp, 14)) { passed++; printf("[PASS] Test %d: bounce rejected, commits only when stable\n", test_num); }
        else {
            printf("[FAIL] Test %d: got ", test_num);
            for (int i=0;i<14;i++) printf("%d", deb[i]);
            printf("\n");
        }
    }

    /* === Test 3: exactly one press event for one real press === */
    {
        test_num++;
        int raw[] = {0,0,1,0,1,0,1,1,1,1,0,0,0,0};
        run_stream(raw, 14, deb, &events);
        if (events == 1) { passed++; printf("[PASS] Test %d: one press -> one event\n", test_num); }
        else printf("[FAIL] Test %d: events=%d (expected 1)\n", test_num, events);
    }

    /* === Test 4: 3 consecutive 1s is NOT enough (needs 4) === */
    {
        test_num++;
        int raw[] = {0,1,1,1,0,0,0,0};  /* only 3 ones then back to 0 */
        run_stream(raw, 8, deb, &events);
        int any_pressed = 0;
        for (int i=0;i<8;i++) if (deb[i]==1) any_pressed = 1;
        if (!any_pressed && events == 0) { passed++; printf("[PASS] Test %d: 3 samples insufficient (no commit)\n", test_num); }
        else printf("[FAIL] Test %d: pressed=%d events=%d\n", test_num, any_pressed, events);
    }

    /* === Test 5: exactly 4 consecutive 1s commits === */
    {
        test_num++;
        int raw[] = {0,1,1,1,1};
        int exp[] = {0,0,0,0,1};
        run_stream(raw, 5, deb, &events);
        if (arr_eq(deb, exp, 5) && events == 1) { passed++; printf("[PASS] Test %d: exactly 4 commits + 1 event\n", test_num); }
        else printf("[FAIL] Test %d: deb[4]=%d events=%d\n", test_num, deb[4], events);
    }

    /* === Test 6: two separate presses -> two events === */
    {
        test_num++;
        int raw[] = {0,1,1,1,1, 0,0,0,0, 1,1,1,1, 0,0,0,0};
        run_stream(raw, 17, deb, &events);
        if (events == 2) { passed++; printf("[PASS] Test %d: two presses -> two events\n", test_num); }
        else printf("[FAIL] Test %d: events=%d (expected 2)\n", test_num, events);
    }

    /* === Test 7: interrupted count restarts (bounce mid-press) === */
    {
        test_num++;
        /* 1,1,1 then a 0 (resets), then need 4 more 1s */
        int raw[] = {0,1,1,1,0,1,1,1,1};
        int exp[] = {0,0,0,0,0,0,0,0,1};
        run_stream(raw, 9, deb, &events);
        if (arr_eq(deb, exp, 9) && events == 1) { passed++; printf("[PASS] Test %d: interrupted count restarts\n", test_num); }
        else {
            printf("[FAIL] Test %d: got ", test_num);
            for (int i=0;i<9;i++) printf("%d", deb[i]);
            printf(" events=%d\n", events);
        }
    }

    /* === Test 8: stays released with all-zero input, no events === */
    {
        test_num++;
        int raw[] = {0,0,0,0,0,0};
        int exp[] = {0,0,0,0,0,0};
        run_stream(raw, 6, deb, &events);
        if (arr_eq(deb, exp, 6) && events == 0) { passed++; printf("[PASS] Test %d: all-released, no events\n", test_num); }
        else printf("[FAIL] Test %d: events=%d\n", test_num, events);
    }

    /* === Test 9: release also requires 4 stable samples === */
    {
        test_num++;
        /* press stable, then bouncy release: 0s interrupted by 1s */
        int raw[] = {1,1,1,1, 0,1,0,0,0,0};
        /* commit press at idx3; release bounce: idx4=0(c1),idx5=1 resets,idx6..9=0 -> commit at idx9 */
        int exp[] = {0,0,0,1, 1,1,1,1,1,0};
        run_stream(raw, 10, deb, &events);
        if (arr_eq(deb, exp, 10)) { passed++; printf("[PASS] Test %d: release debounced symmetrically\n", test_num); }
        else {
            printf("[FAIL] Test %d: got ", test_num);
            for (int i=0;i<10;i++) printf("%d", deb[i]);
            printf("\n");
        }
    }

    /* === Test 10: press event is one-shot (consumed on read) === */
    {
        test_num++;
        debounce_init();
        int raw[] = {0,1,1,1,1};
        int e_first = 0, e_second = 0;
        for (int i=0;i<5;i++) {
            debounce_update(raw[i]);
            if (i == 4) {
                e_first = debounce_pressed_event();   /* should be 1 */
                e_second = debounce_pressed_event();  /* should be 0 (consumed) */
            }
        }
        if (e_first == 1 && e_second == 0) { passed++; printf("[PASS] Test %d: press event is one-shot\n", test_num); }
        else printf("[FAIL] Test %d: first=%d second=%d\n", test_num, e_first, e_second);
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);
    return (passed == num_tests) ? 0 : 1;
}
