//
// Created by zack on 9/3/25.
//

#include "DynamicString.h"
#include "TestAssert.h"

#include <stdio.h>
#include <stdlib.h>

int test_reserve_and_shrink(void)
{
    DSCString str = dsc_str_create_empty(4);
    const size_t old_capacity = dsc_str_capacity(&str);
    ASSERT_TRUE(dsc_str_reserve(&str, 128));
    ASSERT_GT(dsc_str_capacity(&str), old_capacity);
    dsc_str_assign_cstring(&str, "abc");
    ASSERT_TRUE(dsc_str_shrink_to_fit(&str));
    ASSERT_GTE(dsc_str_capacity(&str), dsc_str_size(&str) + 1);
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_buffer_growth(void)
{
    DSCString str = dsc_str_create_empty(4);
    const size_t initial_capacity = dsc_str_capacity(&str);
    // Append enough characters to force buffer growth
    for (int i = 0; i < 100; ++i)
    {
        dsc_str_push_back(&str, 'x');
    }
    ASSERT_GT(dsc_str_capacity(&str), initial_capacity);
    ASSERT_EQ(dsc_str_size(&str), 100);
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_large_string(void)
{
    DSCString str = dsc_str_create_empty(1024);
    for (int i = 0; i < 1000; ++i)
    {
        dsc_str_push_back(&str, 'x');
    }
    ASSERT_EQ(dsc_str_size(&str), 1000);
    ASSERT_GT(dsc_str_capacity(&str), 1000);
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_reserve_and_shrink_optimal(void)
{
    DSCString str = dsc_str_create_from_cstring("abc");
    const size_t cap = dsc_str_capacity(&str);
    ASSERT_FALSE(dsc_str_reserve(&str, cap));
    ASSERT_TRUE(dsc_str_shrink_to_fit(&str));
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

typedef struct
{
    int (*func)(void);
    const char* name;
} TestCase;

TestCase tests[] = {
    {test_reserve_and_shrink, "test_reserve_and_shrink"},
    {test_buffer_growth, "test_buffer_growth"},
    {test_large_string, "test_large_string"},
    {test_reserve_and_shrink_optimal, "test_reserve_and_shrink_optimal"},
};

int main(void)
{
    int failed = 0;
    const int num_tests = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < num_tests; i++)
    {
        if (tests[i].func() != TEST_SUCCESS)
        {
            printf("%s failed\n", tests[i].name);
            failed++;
        }
    }

    if (failed == 0)
    {
        printf("All DString Memory tests passed.\n");
        return 0;
    }

    printf("%d DString Memory tests failed.\n", failed);
    return 1;
}