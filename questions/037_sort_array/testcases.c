#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Declaration of the solution function */
extern void sort_array(int32_t *arr, int n);

/* Helper: check if array matches expected */
static int arrays_equal(const int32_t *a, const int32_t *b, int n) {
    for (int i = 0; i < n; i++) {
        if (a[i] != b[i]) return 0;
    }
    return 1;
}

/* Helper: print array */
static void print_array(const int32_t *arr, int n) {
    printf("{");
    for (int i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i < n - 1) printf(", ");
    }
    printf("}");
}

struct test_case {
    int32_t input[20];
    int32_t expected[20];
    int n;
    const char *description;
};

int main(void) {
    struct test_case tests[] = {
        /* Basic cases */
        {{5, 2, 9, 1, 5, 6}, {1, 2, 5, 5, 6, 9}, 6,
         "Basic: mixed with duplicate"},
        {{3, 1, 4, 1, 5, 9, 2, 6}, {1, 1, 2, 3, 4, 5, 6, 9}, 8,
         "Basic: digits of pi"},
        {{10, 20, 30, 40, 50}, {10, 20, 30, 40, 50}, 5,
         "Already sorted"},
        {{5, 4, 3, 2, 1}, {1, 2, 3, 4, 5}, 5,
         "Reverse sorted (worst case for insertion sort)"},

        /* Single and two elements */
        {{42}, {42}, 1,
         "Single element"},
        {{7, 3}, {3, 7}, 2,
         "Two elements: needs swap"},
        {{3, 7}, {3, 7}, 2,
         "Two elements: already sorted"},

        /* Negative numbers */
        {{3, -1, 0, -7, 4}, {-7, -1, 0, 3, 4}, 5,
         "Negative: mixed positive and negative"},
        {{-5, -3, -8, -1, -4}, {-8, -5, -4, -3, -1}, 5,
         "Negative: all negative"},
        {{-100, 100, -50, 50, 0}, {-100, -50, 0, 50, 100}, 5,
         "Negative: symmetric around zero"},

        /* Duplicates */
        {{7, 7, 7, 7, 7}, {7, 7, 7, 7, 7}, 5,
         "Duplicates: all same value"},
        {{1, 3, 1, 3, 1, 3}, {1, 1, 1, 3, 3, 3}, 6,
         "Duplicates: alternating two values"},
        {{5, 5, 5, 1, 1, 1, 9, 9}, {1, 1, 1, 5, 5, 5, 9, 9}, 8,
         "Duplicates: groups"},

        /* Boundary values */
        {{2147483647, -2147483647 - 1, 0}, {-2147483647 - 1, 0, 2147483647}, 3,
         "Boundary: INT_MAX, INT_MIN, 0"},
        {{2147483647, 2147483646, 2147483647}, {2147483646, 2147483647, 2147483647}, 3,
         "Boundary: near INT_MAX with duplicate"},
        {{-2147483647 - 1, -2147483647 - 1, -2147483647}, {-2147483647 - 1, -2147483647 - 1, -2147483647}, 3,
         "Boundary: near INT_MIN with duplicate"},

        /* Nearly sorted (best case for insertion sort) */
        {{1, 2, 3, 5, 4, 6, 7, 8}, {1, 2, 3, 4, 5, 6, 7, 8}, 8,
         "Nearly sorted: one swap needed"},
        {{2, 1, 4, 3, 6, 5, 8, 7}, {1, 2, 3, 4, 5, 6, 7, 8}, 8,
         "Nearly sorted: adjacent pairs swapped"},

        /* Larger array */
        {{19, 7, 15, 3, 11, 1, 17, 5, 13, 9, 20, 8, 16, 4, 12, 2, 18, 6, 14, 10},
         {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20}, 20,
         "Large: 20 elements shuffled"},
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    for (int i = 0; i < num_tests; i++) {
        /* Copy input so we don't modify the test data */
        int32_t work[20];
        memcpy(work, tests[i].input, tests[i].n * sizeof(int32_t));

        sort_array(work, tests[i].n);

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
