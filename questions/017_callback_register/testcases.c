#include <stdio.h>
#include <stddef.h>

/* Type definition matching solution.c */
typedef int (*callback_fn)(int data);

/* Declaration of the solution functions */
extern void register_callback(callback_fn cb);
extern int invoke_callback(int data);

/* --- Test callback functions --- */

static int double_it(int x) {
    return x * 2;
}

static int negate(int x) {
    return -x;
}

static int add_ten(int x) {
    return x + 10;
}

static int square(int x) {
    return x * x;
}

static int identity(int x) {
    return x;
}

static int always_zero(int x) {
    (void)x;
    return 0;
}

/* Test case structure */
struct test_case {
    const char *description;
    int expected;
};

int main(void) {
    int num_tests = 16;
    int passed = 0;
    int result;
    int test_num = 0;

    /* Test 1: No callback registered, should return -1 */
    test_num++;
    result = invoke_callback(42);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: No callback registered returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: No callback registered returns -1 | Input: data=42 | Expected: -1, Got: %d\n", test_num, result);
    }

    /* Test 2: Register double_it, invoke with 5 */
    test_num++;
    register_callback(double_it);
    result = invoke_callback(5);
    if (result == 10) {
        passed++;
        printf("[PASS] Test %d: double_it(5) = 10\n", test_num);
    } else {
        printf("[FAIL] Test %d: double_it(5) = 10 | Input: data=5 | Expected: 10, Got: %d\n", test_num, result);
    }

    /* Test 3: Same callback, different data */
    test_num++;
    result = invoke_callback(0);
    if (result == 0) {
        passed++;
        printf("[PASS] Test %d: double_it(0) = 0\n", test_num);
    } else {
        printf("[FAIL] Test %d: double_it(0) = 0 | Input: data=0 | Expected: 0, Got: %d\n", test_num, result);
    }

    /* Test 4: double_it with negative */
    test_num++;
    result = invoke_callback(-3);
    if (result == -6) {
        passed++;
        printf("[PASS] Test %d: double_it(-3) = -6\n", test_num);
    } else {
        printf("[FAIL] Test %d: double_it(-3) = -6 | Input: data=-3 | Expected: -6, Got: %d\n", test_num, result);
    }

    /* Test 5: Replace callback with negate */
    test_num++;
    register_callback(negate);
    result = invoke_callback(7);
    if (result == -7) {
        passed++;
        printf("[PASS] Test %d: negate(7) = -7\n", test_num);
    } else {
        printf("[FAIL] Test %d: negate(7) = -7 | Input: data=7 | Expected: -7, Got: %d\n", test_num, result);
    }

    /* Test 6: negate with negative input (double negative) */
    test_num++;
    result = invoke_callback(-4);
    if (result == 4) {
        passed++;
        printf("[PASS] Test %d: negate(-4) = 4\n", test_num);
    } else {
        printf("[FAIL] Test %d: negate(-4) = 4 | Input: data=-4 | Expected: 4, Got: %d\n", test_num, result);
    }

    /* Test 7: Register add_ten */
    test_num++;
    register_callback(add_ten);
    result = invoke_callback(5);
    if (result == 15) {
        passed++;
        printf("[PASS] Test %d: add_ten(5) = 15\n", test_num);
    } else {
        printf("[FAIL] Test %d: add_ten(5) = 15 | Input: data=5 | Expected: 15, Got: %d\n", test_num, result);
    }

    /* Test 8: add_ten with zero */
    test_num++;
    result = invoke_callback(0);
    if (result == 10) {
        passed++;
        printf("[PASS] Test %d: add_ten(0) = 10\n", test_num);
    } else {
        printf("[FAIL] Test %d: add_ten(0) = 10 | Input: data=0 | Expected: 10, Got: %d\n", test_num, result);
    }

    /* Test 9: Register square */
    test_num++;
    register_callback(square);
    result = invoke_callback(4);
    if (result == 16) {
        passed++;
        printf("[PASS] Test %d: square(4) = 16\n", test_num);
    } else {
        printf("[FAIL] Test %d: square(4) = 16 | Input: data=4 | Expected: 16, Got: %d\n", test_num, result);
    }

    /* Test 10: square with negative */
    test_num++;
    result = invoke_callback(-3);
    if (result == 9) {
        passed++;
        printf("[PASS] Test %d: square(-3) = 9\n", test_num);
    } else {
        printf("[FAIL] Test %d: square(-3) = 9 | Input: data=-3 | Expected: 9, Got: %d\n", test_num, result);
    }

    /* Test 11: Register identity */
    test_num++;
    register_callback(identity);
    result = invoke_callback(99);
    if (result == 99) {
        passed++;
        printf("[PASS] Test %d: identity(99) = 99\n", test_num);
    } else {
        printf("[FAIL] Test %d: identity(99) = 99 | Input: data=99 | Expected: 99, Got: %d\n", test_num, result);
    }

    /* Test 12: Register always_zero */
    test_num++;
    register_callback(always_zero);
    result = invoke_callback(12345);
    if (result == 0) {
        passed++;
        printf("[PASS] Test %d: always_zero(12345) = 0\n", test_num);
    } else {
        printf("[FAIL] Test %d: always_zero(12345) = 0 | Input: data=12345 | Expected: 0, Got: %d\n", test_num, result);
    }

    /* Test 13: Deregister by passing NULL */
    test_num++;
    register_callback(NULL);
    result = invoke_callback(5);
    if (result == -1) {
        passed++;
        printf("[PASS] Test %d: After NULL registration, returns -1\n", test_num);
    } else {
        printf("[FAIL] Test %d: After NULL registration, returns -1 | Input: data=5 | Expected: -1, Got: %d\n", test_num, result);
    }

    /* Test 14: Re-register after NULL */
    test_num++;
    register_callback(double_it);
    result = invoke_callback(100);
    if (result == 200) {
        passed++;
        printf("[PASS] Test %d: Re-register double_it after NULL, double_it(100) = 200\n", test_num);
    } else {
        printf("[FAIL] Test %d: Re-register double_it after NULL, double_it(100) = 200 | Input: data=100 | Expected: 200, Got: %d\n", test_num, result);
    }

    /* Test 15: Invoke multiple times without re-register */
    test_num++;
    int sum = invoke_callback(1) + invoke_callback(2) + invoke_callback(3);
    if (sum == 12) {
        passed++;
        printf("[PASS] Test %d: Multiple invokes: double(1)+double(2)+double(3)=12\n", test_num);
    } else {
        printf("[FAIL] Test %d: Multiple invokes: double(1)+double(2)+double(3)=12 | Input: 1,2,3 | Expected: 12, Got: %d\n", test_num, sum);
    }

    /* Test 16: Register same function twice (should still work) */
    test_num++;
    register_callback(negate);
    register_callback(negate);
    result = invoke_callback(8);
    if (result == -8) {
        passed++;
        printf("[PASS] Test %d: Double registration of same function works\n", test_num);
    } else {
        printf("[FAIL] Test %d: Double registration of same function works | Input: data=8 | Expected: -8, Got: %d\n", test_num, result);
    }

    printf("\nResults: %d/%d passed\n", passed, num_tests);

    return (passed == num_tests) ? 0 : 1;
}
