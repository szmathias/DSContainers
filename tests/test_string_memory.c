//
// Created by zack on 9/3/25.
//

#include "DynamicString.h"
#include "TestAssert.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int test_reserve_and_shrink(void) {
    String str = str_create_empty(4);
    const size_t old_capacity = str_capacity(&str);
    ASSERT_TRUE(str_reserve(&str, 128));
    ASSERT_GT(str_capacity(&str), old_capacity);
    str_assign_cstring(&str, "abc");
    ASSERT_TRUE(str_shrink_to_fit(&str));
    ASSERT_GTE(str_capacity(&str), str_size(&str) + 1);
    str_destroy(&str);
    return TEST_SUCCESS;
}

int test_buffer_growth(void) {
    String str = str_create_empty(4);
    const size_t initial_capacity = str_capacity(&str);
    // Append enough characters to force buffer growth
    for (int i = 0; i < 100; ++i)
    {
        str_push_back(&str, 'x');
    }
    ASSERT_GT(str_capacity(&str), initial_capacity);
    ASSERT_EQ(str_size(&str), 100);
    str_destroy(&str);
    return TEST_SUCCESS;
}

int test_large_string(void) {
    String str = str_create_empty(1024);
    for (int i = 0; i < 1000; ++i)
    {
        str_push_back(&str, 'x');
    }
    ASSERT_EQ(str_size(&str), 1000);
    ASSERT_GT(str_capacity(&str), 1000);
    str_destroy(&str);
    return TEST_SUCCESS;
}

int test_reserve_and_shrink_optimal(void) {
    String str = str_create_from_cstring("abc");
    const size_t cap = str_capacity(&str);
    ASSERT_FALSE(str_reserve(&str, cap));
    ASSERT_TRUE(str_shrink_to_fit(&str));
    str_destroy(&str);
    return TEST_SUCCESS;
}

typedef struct {
    int (*func)(void);
    const char *name;
} TestCase;

TestCase tests[] = {
    {test_reserve_and_shrink, "test_reserve_and_shrink"},
    {test_buffer_growth, "test_buffer_growth"},
    {test_large_string, "test_large_string"},
    {test_reserve_and_shrink_optimal, "test_reserve_and_shrink_optimal"},
};

int main(void) {
    int failed = 0;
    const int num_tests = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < num_tests; i++) {
        if (tests[i].func() != TEST_SUCCESS) {
            printf("%s failed\n", tests[i].name);
            failed++;
        }
    }

    if (failed == 0) {
        printf("All DString Memory tests passed.\n");
        return 0;
    }

    printf("%d DString Memory tests failed.\n", failed);
    return 1;
}