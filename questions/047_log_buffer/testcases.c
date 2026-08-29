#include <stdio.h>
#include <string.h>

/* Functions under test (defined in solution.c). No internal types are shared. */
extern void log_init(void);
extern void log_set_level(int threshold);
extern void log_write(int level, unsigned int timestamp, const char *msg);
extern int  dmesg_read(int *out_level, unsigned int *out_timestamp,
                       char *out_msg, int out_msg_size);

/* Level constants (contract) */
#define LVL_ERR   0
#define LVL_WARN  1
#define LVL_INFO  2
#define LVL_DEBUG 3

int main(void) {
    int passed = 0;
    int num_tests = 12;
    int test_num = 0;

    int lvl;
    unsigned int ts;
    char buf[64];

    /* === Test 1: empty read returns 0 === */
    {
        test_num++;
        log_init();
        int r = dmesg_read(&lvl, &ts, buf, sizeof buf);
        if (r == 0) {
            passed++;
            printf("[PASS] Test %d: read on empty log returns 0\n", test_num);
        } else {
            printf("[FAIL] Test %d: empty read returned %d (expected 0)\n", test_num, r);
        }
    }

    /* === Test 2: single write then read (fields correct) === */
    {
        test_num++;
        log_init();
        log_write(LVL_ERR, 100, "boot fail");
        int r = dmesg_read(&lvl, &ts, buf, sizeof buf);
        if (r == 1 && lvl == LVL_ERR && ts == 100 && strcmp(buf, "boot fail") == 0) {
            passed++;
            printf("[PASS] Test %d: single write/read returns correct fields\n", test_num);
        } else {
            printf("[FAIL] Test %d: r=%d, lvl=%d, ts=%u, buf=\"%s\"\n",
                   test_num, r, lvl, ts, buf);
        }
    }

    /* === Test 3: FIFO order of two records === */
    {
        test_num++;
        log_init();
        log_write(LVL_INFO, 1, "first");
        log_write(LVL_INFO, 2, "second");
        int r1 = dmesg_read(&lvl, &ts, buf, sizeof buf);
        int ok1 = (r1 == 1 && ts == 1 && strcmp(buf, "first") == 0);
        int r2 = dmesg_read(&lvl, &ts, buf, sizeof buf);
        int ok2 = (r2 == 1 && ts == 2 && strcmp(buf, "second") == 0);
        int r3 = dmesg_read(&lvl, &ts, buf, sizeof buf);
        if (ok1 && ok2 && r3 == 0) {
            passed++;
            printf("[PASS] Test %d: FIFO order preserved\n", test_num);
        } else {
            printf("[FAIL] Test %d: ok1=%d ok2=%d r3=%d\n", test_num, ok1, ok2, r3);
        }
    }

    /* === Test 4: default threshold is DEBUG (keeps everything) === */
    {
        test_num++;
        log_init();  /* no set_level: default should be DEBUG */
        log_write(LVL_DEBUG, 7, "dbg");
        int r = dmesg_read(&lvl, &ts, buf, sizeof buf);
        if (r == 1 && lvl == LVL_DEBUG && strcmp(buf, "dbg") == 0) {
            passed++;
            printf("[PASS] Test %d: default threshold keeps DEBUG\n", test_num);
        } else {
            printf("[FAIL] Test %d: r=%d, lvl=%d, buf=\"%s\"\n", test_num, r, lvl, buf);
        }
    }

    /* === Test 5: filtering drops below threshold === */
    {
        test_num++;
        log_init();
        log_set_level(LVL_INFO);           /* keep ERR/WARN/INFO, drop DEBUG */
        log_write(LVL_DEBUG, 1, "drop me");
        int r = dmesg_read(&lvl, &ts, buf, sizeof buf);
        if (r == 0) {
            passed++;
            printf("[PASS] Test %d: DEBUG dropped when threshold=INFO\n", test_num);
        } else {
            printf("[FAIL] Test %d: expected drop, got r=%d buf=\"%s\"\n", test_num, r, buf);
        }
    }

    /* === Test 6: filtering keeps at/above severity === */
    {
        test_num++;
        log_init();
        log_set_level(LVL_WARN);           /* keep ERR, WARN; drop INFO, DEBUG */
        log_write(LVL_ERR,   1, "err");
        log_write(LVL_WARN,  2, "warn");
        log_write(LVL_INFO,  3, "info");   /* dropped */
        log_write(LVL_DEBUG, 4, "debug");  /* dropped */
        int c = 0;
        while (dmesg_read(&lvl, &ts, buf, sizeof buf)) c++;
        if (c == 2) {
            passed++;
            printf("[PASS] Test %d: threshold=WARN keeps exactly 2 records\n", test_num);
        } else {
            printf("[FAIL] Test %d: kept %d records (expected 2)\n", test_num, c);
        }
    }

    /* === Test 7: fill to capacity (7) then read all in order === */
    {
        test_num++;
        log_init();
        for (unsigned int i = 1; i <= 7; i++) {
            char m[2] = { (char)('A' + (i - 1)), '\0' };
            log_write(LVL_INFO, i, m);
        }
        int ok = 1;
        for (int i = 0; i < 7; i++) {
            int r = dmesg_read(&lvl, &ts, buf, sizeof buf);
            if (r != 1 || buf[0] != (char)('A' + i) || ts != (unsigned)(i + 1)) ok = 0;
        }
        int r_end = dmesg_read(&lvl, &ts, buf, sizeof buf);
        if (ok && r_end == 0) {
            passed++;
            printf("[PASS] Test %d: capacity 7 fills and reads in order\n", test_num);
        } else {
            printf("[FAIL] Test %d: ok=%d r_end=%d\n", test_num, ok, r_end);
        }
    }

    /* === Test 8: overwrite oldest when full (1 over capacity) === */
    {
        test_num++;
        log_init();
        for (unsigned int i = 1; i <= 7; i++) {          /* A..G fills ring */
            char m[2] = { (char)('A' + (i - 1)), '\0' };
            log_write(LVL_INFO, i, m);
        }
        log_write(LVL_INFO, 8, "H");   /* overwrites A -> [B C D E F G H] */
        /* First read must be B (ts=2), not A */
        int r = dmesg_read(&lvl, &ts, buf, sizeof buf);
        if (r == 1 && ts == 2 && strcmp(buf, "B") == 0) {
            passed++;
            printf("[PASS] Test %d: overwrite oldest — first read is B, A gone\n", test_num);
        } else {
            printf("[FAIL] Test %d: r=%d ts=%u buf=\"%s\" (expected B ts=2)\n",
                   test_num, r, ts, buf);
        }
    }

    /* === Test 9: overwrite — full drain after 2 overflows yields newest 7 === */
    {
        test_num++;
        log_init();
        for (unsigned int i = 1; i <= 9; i++) {          /* A..I; ring holds newest 7: C..I */
            char m[2] = { (char)('A' + (i - 1)), '\0' };
            log_write(LVL_INFO, i, m);
        }
        char expect[7] = {'C','D','E','F','G','H','I'};
        int ok = 1, c = 0;
        while (dmesg_read(&lvl, &ts, buf, sizeof buf)) {
            if (c < 7 && buf[0] != expect[c]) ok = 0;
            c++;
        }
        if (ok && c == 7) {
            passed++;
            printf("[PASS] Test %d: after 2 overflows, ring holds newest 7 (C..I)\n", test_num);
        } else {
            printf("[FAIL] Test %d: ok=%d count=%d\n", test_num, ok, c);
        }
    }

    /* === Test 10: long message is truncated safely === */
    {
        test_num++;
        log_init();
        /* 40-char message; max is 31 chars + NUL */
        const char *long_msg = "0123456789012345678901234567890123456789";
        log_write(LVL_INFO, 1, long_msg);
        int r = dmesg_read(&lvl, &ts, buf, sizeof buf);
        size_t len = strlen(buf);
        /* Must be truncated to at most 31 chars and be a prefix of the original */
        if (r == 1 && len <= 31 && strncmp(buf, long_msg, len) == 0) {
            passed++;
            printf("[PASS] Test %d: long message truncated to %zu chars\n", test_num, len);
        } else {
            printf("[FAIL] Test %d: r=%d len=%zu buf=\"%s\"\n", test_num, r, len, buf);
        }
    }

    /* === Test 11: dmesg_read respects small out_msg_size === */
    {
        test_num++;
        log_init();
        log_write(LVL_INFO, 1, "hello world");
        char small[4];   /* room for 3 chars + NUL */
        int r = dmesg_read(&lvl, &ts, small, (int)sizeof small);
        if (r == 1 && strlen(small) <= 3 && strncmp(small, "hello world", strlen(small)) == 0) {
            passed++;
            printf("[PASS] Test %d: read respects small out_msg_size (\"%s\")\n", test_num, small);
        } else {
            printf("[FAIL] Test %d: r=%d small=\"%s\"\n", test_num, r, small);
        }
    }

    /* === Test 12: log_init clears previous contents === */
    {
        test_num++;
        log_init();
        log_write(LVL_INFO, 1, "stale");
        log_init();                        /* should wipe */
        int r = dmesg_read(&lvl, &ts, buf, sizeof buf);
        if (r == 0) {
            passed++;
            printf("[PASS] Test %d: log_init clears previous records\n", test_num);
        } else {
            printf("[FAIL] Test %d: stale record survived init (r=%d buf=\"%s\")\n",
                   test_num, r, buf);
        }
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);
    return (passed == num_tests) ? 0 : 1;
}
