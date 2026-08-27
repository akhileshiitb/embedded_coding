#include <stdio.h>
#include <stdint.h>
#include <string.h>

extern void rotate_left(int32_t *arr, int n, int k);
extern void rotate_right(int32_t *arr, int n, int k);

static int arrays_equal(const int32_t *a, const int32_t *b, int n) {
    for (int i = 0; i < n; i++) {
        if (a[i] != b[i]) return 0;
    }
    return 1;
}

static void print_array(const int32_t *arr, int n) {
    printf("{");
    for (int i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i < n - 1) printf(", ");
    }
    printf("}");
}

enum dir { LEFT, RIGHT };

struct test_case {
    int32_t input[20];
    int32_t expected[20];
    int n;
    int k;
    enum dir direction;
    const char *description;
};

int main(void) {
    struct test_case tests[] = {
        /* === Rotate Left === */
        {{1,2,3,4,5}, {3,4,5,1,2}, 5, 2, LEFT, "Left: rotate {1..5} by 2"},
        {{1,2,3,4,5}, {2,3,4,5,1}, 5, 1, LEFT, "Left: rotate {1..5} by 1"},
        {{1,2,3,4,5}, {5,1,2,3,4}, 5, 4, LEFT, "Left: rotate {1..5} by 4"},
        {{10,20,30}, {20,30,10}, 3, 1, LEFT, "Left: rotate {10,20,30} by 1"},
        {{1,2,3,4,5,6}, {4,5,6,1,2,3}, 6, 3, LEFT, "Left: rotate {1..6} by 3 (half)"},

        /* === Rotate Right === */
        {{1,2,3,4,5}, {4,5,1,2,3}, 5, 2, RIGHT, "Right: rotate {1..5} by 2"},
        {{1,2,3,4,5}, {5,1,2,3,4}, 5, 1, RIGHT, "Right: rotate {1..5} by 1"},
        {{1,2,3,4,5}, {2,3,4,5,1}, 5, 4, RIGHT, "Right: rotate {1..5} by 4"},
        {{10,20,30}, {30,10,20}, 3, 1, RIGHT, "Right: rotate {10,20,30} by 1"},
        {{1,2,3,4,5,6}, {4,5,6,1,2,3}, 6, 3, RIGHT, "Right: rotate {1..6} by 3 (half)"},

        /* === k = 0 (no-op) === */
        {{1,2,3,4}, {1,2,3,4}, 4, 0, LEFT, "Left: k=0 is no-op"},
        {{1,2,3,4}, {1,2,3,4}, 4, 0, RIGHT, "Right: k=0 is no-op"},

        /* === k = n (full rotation, no-op) === */
        {{1,2,3,4}, {1,2,3,4}, 4, 4, LEFT, "Left: k=n is no-op"},
        {{1,2,3,4}, {1,2,3,4}, 4, 4, RIGHT, "Right: k=n is no-op"},

        /* === k > n (needs normalization) === */
        {{1,2,3}, {3,1,2}, 3, 5, LEFT, "Left: k=5>n=3 → effective k=2"},
        {{1,2,3}, {2,3,1}, 3, 5, RIGHT, "Right: k=5>n=3 → effective k=2"},
        {{1,2,3,4}, {1,2,3,4}, 4, 8, LEFT, "Left: k=8=2n → no-op"},

        /* === Single element === */
        {{42}, {42}, 1, 3, LEFT, "Left: single element (any k)"},
        {{42}, {42}, 1, 1, RIGHT, "Right: single element"},

        /* === Negative values === */
        {{-1,-2,-3,-4}, {-3,-4,-1,-2}, 4, 2, LEFT, "Left: negatives {-1..-4} by 2"},
        {{-1,-2,-3,-4}, {-3,-4,-1,-2}, 4, 2, RIGHT, "Right: negatives {-1..-4} by 2"},

        /* === Two elements === */
        {{7,8}, {8,7}, 2, 1, LEFT, "Left: two elements by 1"},
        {{7,8}, {8,7}, 2, 1, RIGHT, "Right: two elements by 1"},
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    for (int i = 0; i < num_tests; i++) {
        int32_t work[20];
        memcpy(work, tests[i].input, tests[i].n * sizeof(int32_t));

        if (tests[i].direction == LEFT) {
            rotate_left(work, tests[i].n, tests[i].k);
        } else {
            rotate_right(work, tests[i].n, tests[i].k);
        }

        if (arrays_equal(work, tests[i].expected, tests[i].n)) {
            passed++;
            printf("[PASS] Test %d: %s\n", i + 1, tests[i].description);
        } else {
            printf("[FAIL] Test %d: %s | Got: ", i + 1, tests[i].description);
            print_array(work, tests[i].n);
            printf(" | Expected: ");
            print_array(tests[i].expected, tests[i].n);
            printf("\n");
        }
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);

    return (passed == num_tests) ? 0 : 1;
}
