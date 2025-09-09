//
// Created by zack on 9/3/25.
//

#include "Iterator.h"
#include "DoublyLinkedList.h"
#include "TestAssert.h"
#include "TestHelpers.h"
#include <stdlib.h>

// Test chaining filter and transform iterators
static int test_filter_then_transform(void)
{
    // Create range from 1 to 15, keep only even numbers, then square them
    DSCIterator range_it = dsc_iterator_range(1, 15, 1);
    ASSERT_TRUE(range_it.is_valid(&range_it));

    DSCIterator even_it = dsc_iterator_filter(&range_it, is_even);
    ASSERT_TRUE(even_it.is_valid(&even_it));

    DSCIterator square_it = dsc_iterator_transform(&even_it, square_func);
    ASSERT_TRUE(square_it.is_valid(&square_it));

    // Expected: [4,16,36,64,100,144,196] (squares of even numbers from 1-14: 2,4,6,8,10,12,14)
    int idx = 0;

    while (square_it.has_next(&square_it))
    {
        const int expected[] = {4, 16, 36, 64, 100, 144, 196};
        int* value           = square_it.next(&square_it);
        ASSERT_NOT_NULL(value);
        ASSERT_EQ(*value, expected[idx]);
        idx++;
        free(value);
    }

    ASSERT_EQ(idx, 7); // Should have 7 elements

    // Cleanup
    square_it.destroy(&square_it); // This should destroy all chained iterators
    return TEST_SUCCESS;
}

// Test chaining transform and filter iterators (opposite order)
static int test_transform_then_filter(void)
{
    // Create range from 1 to 10, add 10 to each, then keep only values divisible by 3
    DSCIterator range_it = dsc_iterator_range(1, 10, 1);
    ASSERT_TRUE(range_it.is_valid(&range_it));

    DSCIterator add_ten_it = dsc_iterator_transform(&range_it, add_ten_func);
    ASSERT_TRUE(add_ten_it.is_valid(&add_ten_it));

    DSCIterator div3_it = dsc_iterator_filter(&add_ten_it, is_divisible_by_3);
    ASSERT_TRUE(div3_it.is_valid(&div3_it));

    // Expected: [12,15,18] (values from [11-19] that are divisible by 3)
    int idx = 0;

    while (div3_it.has_next(&div3_it))
    {
        const int expected[] = {12, 15, 18};
        int* value           = div3_it.next(&div3_it);
        ASSERT_NOT_NULL(value);
        ASSERT_EQ(*value, expected[idx]);
        idx++;
        free(value);
    }

    ASSERT_EQ(idx, 3); // Should have 3 elements

    // Cleanup
    div3_it.destroy(&div3_it); // This should destroy all chained iterators
    return TEST_SUCCESS;
}

// Test complex chaining of filters and transforms
static int test_complex_chain(void)
{
    // Create range from 1 to 20, double each, keep values > 10, then square
    DSCIterator range_it = dsc_iterator_range(6, 20, 1);
    ASSERT_TRUE(range_it.is_valid(&range_it));

    DSCIterator double_it = dsc_iterator_transform(&range_it, double_value);
    ASSERT_TRUE(double_it.is_valid(&double_it));

    DSCIterator gt10_it = dsc_iterator_filter(&double_it, is_greater_than_10);
    ASSERT_TRUE(gt10_it.is_valid(&gt10_it));

    DSCIterator square_it = dsc_iterator_transform(&gt10_it, square_func);
    ASSERT_TRUE(square_it.is_valid(&square_it));

    // Expected: Values 12,14,16,...,38 (doubled from 6-19) squared
    int idx = 0;

    while (square_it.has_next(&square_it))
    {
        const int expected[] = {144, 196, 256, 324, 400, 484, 576, 676, 784, 900, 1024, 1156, 1296, 1444};
        int* value           = square_it.next(&square_it);
        ASSERT_NOT_NULL(value);
        ASSERT_EQ(*value, expected[idx]);
        idx++;
        free(value);
    }

    ASSERT_EQ(idx, 14); // Should have 14 elements

    // Cleanup
    square_it.destroy(&square_it); // This should destroy all chained iterators
    return TEST_SUCCESS;
}

// Test premature get/next operations
static int test_premature_operations(void)
{
    // Create a chain of transformations
    DSCIterator range_it  = dsc_iterator_range(0, 5, 1);
    DSCIterator double_it = dsc_iterator_transform(&range_it, double_value);

    // Call get() before starting iteration
    int* value = double_it.get(&double_it);
    ASSERT_NOT_NULL(value);
    ASSERT_EQ(*value, 0); // Should be 0 (transformed from first element)
    free(value);

    // Now properly iterate
    int idx = 0;

    while (double_it.has_next(&double_it))
    {
        const int expected[] = {0, 2, 4, 6, 8};
        value                = double_it.next(&double_it);
        ASSERT_NOT_NULL(value);
        ASSERT_EQ(*value, expected[idx]);
        idx++;
        free(value);
    }

    ASSERT_EQ(idx, 5); // Should have iterated all 5 elements

    // Cleanup
    double_it.destroy(&double_it);
    return TEST_SUCCESS;
}

// Test interleaved has_next/get/next calls
static int test_interleaved_operations(void)
{
    DSCIterator range_it  = dsc_iterator_range(0, 5, 1);
    DSCIterator square_it = dsc_iterator_transform(&range_it, square_func);

    // Pattern: has_next, get, next, get, has_next, next, ...
    ASSERT_TRUE(square_it.has_next(&square_it));

    int* v1 = square_it.get(&square_it);
    ASSERT_EQ(*v1, 0);
    free(v1);

    int* v2 = square_it.next(&square_it);
    ASSERT_EQ(*v2, 0);
    free(v2);

    int* v3 = square_it.get(&square_it);
    ASSERT_EQ(*v3, 1);
    free(v3);

    ASSERT_TRUE(square_it.has_next(&square_it));

    int* v4 = square_it.next(&square_it);
    ASSERT_EQ(*v4, 1);
    free(v4);

    ASSERT_TRUE(square_it.has_next(&square_it));

    int* v5 = square_it.next(&square_it);
    ASSERT_EQ(*v5, 4);
    free(v5);

    // Cleanup
    square_it.destroy(&square_it);
    return TEST_SUCCESS;
}

typedef struct
{
    int (*func)(void);
    const char* name;
} TestCase;

TestCase tests[] = {
    {test_filter_then_transform, "test_filter_then_transform"},
    {test_transform_then_filter, "test_transform_then_filter"},
    {test_complex_chain, "test_complex_chain"},
    {test_premature_operations, "test_premature_operations"},
    {test_interleaved_operations, "test_interleaved_operations"},
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

    if (failed)
    {
        printf("%d test(s) failed.\n", failed);
        return 1;
    }
    else
    {
        printf("All chained iterator tests passed!\n");
        return 0;
    }
}