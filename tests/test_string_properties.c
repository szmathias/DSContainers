//
// Created by zack on 9/4/25.
//

#include "DynamicString.h"
#include "TestAssert.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>


// Property: The size of a string should never exceed its capacity.
int test_string_size_and_capacity(void)
{
    DSCString str = dsc_str_create_empty(0);
    ASSERT_GTE(dsc_str_capacity(&str), dsc_str_size(&str));

    dsc_str_assign_cstring(&str, "hello");
    ASSERT_GTE(dsc_str_capacity(&str), dsc_str_size(&str));

    for (int i = 0; i < 100; ++i)
    {
        dsc_str_push_back(&str, 'a');
        ASSERT_GTE(dsc_str_capacity(&str), dsc_str_size(&str));
    }

    dsc_str_shrink_to_fit(&str);
    ASSERT_GTE(dsc_str_capacity(&str), dsc_str_size(&str));

    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

// Property: Trimming an already-trimmed string should not change it.
int test_string_idempotent_trim(void)
{
    DSCString str  = dsc_str_create_from_cstring("no whitespace");
    DSCString copy = dsc_str_create_from_string(&str);

    dsc_str_trim_front(&str);
    dsc_str_trim_back(&str);

    ASSERT_EQ_DSTRING(&str, &copy);

    dsc_str_destroy(&str);
    dsc_str_destroy(&copy);
    return TEST_SUCCESS;
}

// Property: Converting to lower than upper case should be the same as just converting to upper case.
int test_string_case_conversion_reversibility(void)
{
    DSCString str1 = dsc_str_create_from_cstring("MiXeD cAsE 123!");
    DSCString str2 = dsc_str_create_from_string(&str1);

    dsc_str_to_lower(&str1);
    dsc_str_to_upper(&str1);

    dsc_str_to_upper(&str2);

    ASSERT_EQ_DSTRING(&str1, &str2);

    dsc_str_destroy(&str1);
    dsc_str_destroy(&str2);
    return TEST_SUCCESS;
}

typedef struct
{
    int (*func)(void);
    const char* name;
} TestCase;

TestCase tests[] = {
    {test_string_size_and_capacity, "test_string_size_and_capacity"},
    {test_string_idempotent_trim, "test_string_idempotent_trim"},
    {test_string_case_conversion_reversibility, "test_string_case_conversion_reversibility"},
};

int main(void)
{
    int failed          = 0;
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
        printf("All DString Property tests passed.\n");
        return 0;
    }

    printf("%d DString Property tests failed.\n", failed);
    return 1;
}