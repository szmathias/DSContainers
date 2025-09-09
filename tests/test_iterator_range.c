//
// Created by zack on 8/28/25.
//

#include "Iterator.h"
#include "TestAssert.h"
#include "TestHelpers.h"

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

// Test basic range iteration with positive step
static int test_range_positive_step(void)
{
    // Create range iterator from 0 to 5 with step 1
    DSCIterator it = dsc_iterator_range(0, 5, 1);
    ASSERT_TRUE(it.is_valid(&it));

    int expected = 0;
    while (it.has_next(&it))
    {
        int* value = it.next(&it);
        ASSERT_NOT_NULL(value);
        ASSERT_EQ(*value, expected);
        expected++;
        free(value);
    }

    // Should have iterated over [0,1,2,3,4]
    ASSERT_EQ(expected, 5);

    // Cleanup
    it.destroy(&it);
    return TEST_SUCCESS;
}

// Test range iterator with negative step
static int test_range_negative_step(void)
{
    // Create range iterator from 10 down to 5 with step -1
    DSCIterator it = dsc_iterator_range(10, 5, -1);
    ASSERT_TRUE(it.is_valid(&it));

    int expected = 10;
    while (it.has_next(&it))
    {
        int* value = it.next(&it);
        ASSERT_NOT_NULL(value);
        ASSERT_EQ(*value, expected);
        expected--;
        free(value);
    }

    // Should have iterated over [10,9,8,7,6]
    ASSERT_EQ(expected, 5);

    // Cleanup
    it.destroy(&it);
    return TEST_SUCCESS;
}

// Test range iterator with larger step
static int test_range_larger_step(void)
{
    // Create range iterator from 1 to 20 with step 3
    DSCIterator it = dsc_iterator_range(1, 20, 3);
    ASSERT_TRUE(it.is_valid(&it));

    int count = 0;

    while (it.has_next(&it))
    {
        const int expected_values[] = {1, 4, 7, 10, 13, 16, 19};
        int* value                  = it.next(&it);
        ASSERT_NOT_NULL(value);
        ASSERT_EQ(*value, expected_values[count]);
        count++;
        free(value);
    }

    // Should have iterated over [1,4,7,10,13,16,19]
    ASSERT_EQ(count, 7);

    // Cleanup
    it.destroy(&it);
    return TEST_SUCCESS;
}

// Test range iterator with invalid steps
static int test_range_invalid_step(void)
{
    // Create range iterator with step 0 (invalid)
    DSCIterator it1 = dsc_iterator_range(0, 5, 0);
    ASSERT_FALSE(it1.is_valid(&it1));
    ASSERT_FALSE(it1.has_next(&it1));
    ASSERT_NULL(it1.next(&it1));
    it1.destroy(&it1);

    // Create range iterator with conflicting direction
    // (start < end but step negative)
    DSCIterator it2 = dsc_iterator_range(0, 5, -1);
    ASSERT_FALSE(it2.is_valid(&it2));
    ASSERT_FALSE(it2.has_next(&it2));
    ASSERT_NULL(it2.next(&it2));
    it2.destroy(&it2);

    // Create range iterator with conflicting direction
    // (start > end but step positive)
    DSCIterator it3 = dsc_iterator_range(10, 5, 1);
    ASSERT_FALSE(it3.is_valid(&it3));
    ASSERT_FALSE(it3.has_next(&it3));
    ASSERT_NULL(it3.next(&it3));
    it3.destroy(&it3);

    return TEST_SUCCESS;
}

// Test range iterator empty ranges
static int test_range_empty(void)
{
    // Create range iterator where start equals end
    DSCIterator it = dsc_iterator_range(5, 5, 1);
    ASSERT_TRUE(it.is_valid(&it));
    ASSERT_FALSE(it.has_next(&it));
    ASSERT_NULL(it.next(&it));
    it.destroy(&it);

    return TEST_SUCCESS;
}

// Test range reset functionality
static int test_range_reset(void)
{
    // Create range iterator from 0 to 10 with step 2
    DSCIterator it = dsc_iterator_range(0, 10, 2);
    ASSERT_TRUE(it.is_valid(&it));

    // Iterate partially through the range
    int* value1 = it.next(&it);
    ASSERT_EQ(*value1, 0);
    free(value1);

    int* value2 = it.next(&it);
    ASSERT_EQ(*value2, 2);
    free(value2);

    int* value3 = it.next(&it);
    ASSERT_EQ(*value3, 4);
    free(value3);

    // Reset the iterator
    it.reset(&it);

    // Should be back at the beginning
    int expected = 0;
    while (it.has_next(&it))
    {
        int* value = it.next(&it);
        ASSERT_NOT_NULL(value);
        ASSERT_EQ(*value, expected);
        expected += 2;
        free(value);
    }

    // Should have iterated over [0,2,4,6,8]
    ASSERT_EQ(expected, 10);

    // Cleanup
    it.destroy(&it);
    return TEST_SUCCESS;
}

// Test chaining range with transform
static int test_range_transform_chain(void)
{
    // Create range iterator from 1 to 6 with step 1
    DSCIterator range_it = dsc_iterator_range(1, 6, 1);
    ASSERT_TRUE(range_it.is_valid(&range_it));

    // Chain with transform iterator that squares each value
    DSCIterator transform_it = dsc_iterator_transform(&range_it, square_func);

    // Expected: [1,4,9,16,25] (squares of 1,2,3,4,5)
    int idx = 0;

    while (transform_it.has_next(&transform_it))
    {
        const int expected_values[] = {1, 4, 9, 16, 25};
        int* value                  = transform_it.next(&transform_it);
        ASSERT_NOT_NULL(value);
        ASSERT_EQ(*value, expected_values[idx]);
        idx++;
        free(value);
    }

    ASSERT_EQ(idx, 5); // Verify we got all expected values

    // Cleanup
    transform_it.destroy(&transform_it);
    return TEST_SUCCESS;
}

// Test chaining range with filter
static int test_range_filter_chain(void)
{
    // Create range iterator from 1 to 11 with step 1
    DSCIterator range_it = dsc_iterator_range(1, 11, 1);
    ASSERT_TRUE(range_it.is_valid(&range_it));

    // Chain with filter iterator that only keeps odd values
    DSCIterator filter_it = dsc_iterator_filter(&range_it, is_odd);

    // Expected: [1,3,5,7,9] (odd numbers from 1-10)
    int idx = 0;

    while (filter_it.has_next(&filter_it))
    {
        const int expected_values[] = {1, 3, 5, 7, 9};
        int* value                  = filter_it.next(&filter_it);
        ASSERT_NOT_NULL(value);
        ASSERT_EQ(*value, expected_values[idx]);
        idx++;
        free(value);
    }

    ASSERT_EQ(idx, 5); // Verify we got all expected values

    // Cleanup
    filter_it.destroy(&filter_it);
    return TEST_SUCCESS;
}

// Test range iterator with extreme values
static int test_range_extreme_values(void)
{
    // Test with very large positive values
    DSCIterator it1 = dsc_iterator_range(INT_MAX - 10, INT_MAX, 1);
    ASSERT_TRUE(it1.is_valid(&it1));

    int expected = INT_MAX - 10;
    while (it1.has_next(&it1))
    {
        int* value = it1.next(&it1);
        ASSERT_NOT_NULL(value);
        ASSERT_EQ(*value, expected);
        expected++;
        free(value);
    }

    // Should have iterated to INT_MAX-1
    ASSERT_EQ(expected, INT_MAX);
    it1.destroy(&it1);

    // Test with very large negative values
    DSCIterator it2 = dsc_iterator_range(INT_MIN + 10, INT_MIN, -1);
    ASSERT_TRUE(it2.is_valid(&it2));

    expected = INT_MIN + 10;
    while (it2.has_next(&it2))
    {
        int* value = it2.next(&it2);
        ASSERT_NOT_NULL(value);
        ASSERT_EQ(*value, expected);
        expected--;
        free(value);
    }

    // Should have iterated to INT_MIN+1
    ASSERT_EQ(expected, INT_MIN);
    it2.destroy(&it2);

    return TEST_SUCCESS;
}

// Test complex iteration patterns with range iterator
static int test_complex_iteration_patterns(void)
{
    // Create range iterator from 0 to 10 with step 1
    DSCIterator it = dsc_iterator_range(0, 10, 1);
    ASSERT_TRUE(it.is_valid(&it));

    // Forward 3, backward 1, forward 2, reset, forward 4
    int* v1 = it.next(&it);
    ASSERT_EQ(*v1, 0);
    free(v1);
    int* v2 = it.next(&it);
    ASSERT_EQ(*v2, 1);
    free(v2);
    int* v3 = it.next(&it);
    ASSERT_EQ(*v3, 2);
    free(v3);

    ASSERT_TRUE(it.has_prev(&it));
    int* p1 = it.prev(&it);
    ASSERT_EQ(*p1, 1);
    free(p1);

    int* v4 = it.next(&it);
    ASSERT_EQ(*v4, 2);
    free(v4);
    int* v5 = it.next(&it);
    ASSERT_EQ(*v5, 3);
    free(v5);

    // Reset to beginning
    it.reset(&it);

    // Now iterate forward 4 elements
    int* r1 = it.next(&it);
    ASSERT_EQ(*r1, 0);
    free(r1);
    int* r2 = it.next(&it);
    ASSERT_EQ(*r2, 1);
    free(r2);
    int* r3 = it.next(&it);
    ASSERT_EQ(*r3, 2);
    free(r3);
    int* r4 = it.next(&it);
    ASSERT_EQ(*r4, 3);
    free(r4);

    // Cleanup
    it.destroy(&it);
    return TEST_SUCCESS;
}

// Test boundary iteration with bidirectional movement
static int test_bidirectional_iteration(void)
{
    DSCIterator range_it = dsc_iterator_range(0, 5, 1);
    ASSERT_TRUE(range_it.is_valid(&range_it));

    // Move forward to end, then back to beginning
    int* v1 = range_it.next(&range_it);
    ASSERT_EQ(*v1, 0);
    free(v1);

    int* v2 = range_it.next(&range_it);
    ASSERT_EQ(*v2, 1);
    free(v2);

    int* v3 = range_it.next(&range_it);
    ASSERT_EQ(*v3, 2);
    free(v3);

    int* v4 = range_it.next(&range_it);
    ASSERT_EQ(*v4, 3);
    free(v4);

    // Now we're at 3, let's go backward
    ASSERT_TRUE(range_it.has_prev(&range_it));

    int* p1 = range_it.prev(&range_it);
    ASSERT_EQ(*p1, 2);
    free(p1);

    int* p2 = range_it.prev(&range_it);
    ASSERT_EQ(*p2, 1);
    free(p2);

    int* p3 = range_it.prev(&range_it);
    ASSERT_EQ(*p3, 0);
    free(p3);

    // Now we're at the beginning, should not be able to go back further
    ASSERT_FALSE(range_it.has_prev(&range_it));
    ASSERT_NULL(range_it.prev(&range_it));

    // Let's go forward again
    int* v5 = range_it.next(&range_it);
    ASSERT_EQ(*v5, 1);
    free(v5);

    // Cleanup
    range_it.destroy(&range_it);
    return TEST_SUCCESS;
}

// Test zigzag traversal (alternating forward and backward)
static int test_zigzag_traversal(void)
{
    DSCIterator range_it = dsc_iterator_range(0, 10, 1);
    ASSERT_TRUE(range_it.is_valid(&range_it));

    // Pattern: forward 2, back 1, forward 3, back 2, forward 4
    int* v1 = range_it.next(&range_it);
    ASSERT_EQ(*v1, 0);
    free(v1);
    int* v2 = range_it.next(&range_it);
    ASSERT_EQ(*v2, 1);
    free(v2);
    int* p1 = range_it.prev(&range_it);
    ASSERT_EQ(*p1, 0);
    free(p1);
    int* v3 = range_it.next(&range_it);
    ASSERT_EQ(*v3, 1);
    free(v3);
    int* v4 = range_it.next(&range_it);
    ASSERT_EQ(*v4, 2);
    free(v4);
    int* v5 = range_it.next(&range_it);
    ASSERT_EQ(*v5, 3);
    free(v5);
    int* p2 = range_it.prev(&range_it);
    ASSERT_EQ(*p2, 2);
    free(p2);
    int* p3 = range_it.prev(&range_it);
    ASSERT_EQ(*p3, 1);
    free(p3);
    int* v6 = range_it.next(&range_it);
    ASSERT_EQ(*v6, 2);
    free(v6);
    int* v7 = range_it.next(&range_it);
    ASSERT_EQ(*v7, 3);
    free(v7);
    int* v8 = range_it.next(&range_it);
    ASSERT_EQ(*v8, 4);
    free(v8);
    int* v9 = range_it.next(&range_it);
    ASSERT_EQ(*v9, 5);
    free(v9);

    // Cleanup
    range_it.destroy(&range_it);
    return TEST_SUCCESS;
}

// Stress test with large number of iterations
static int test_range_stress(void)
{
    // Create range iterator with large number of elements
    const int SIZE = 100000;
    DSCIterator it = dsc_iterator_range(0, SIZE, 1);
    ASSERT_TRUE(it.is_valid(&it));

    int count = 0;
    while (it.has_next(&it))
    {
        int* value = it.next(&it);
        ASSERT_NOT_NULL(value);
        ASSERT_EQ(*value, count);
        count++;
        free(value);
    }

    // Should have iterated SIZE times
    ASSERT_EQ(count, SIZE);

    // Cleanup
    it.destroy(&it);
    return TEST_SUCCESS;
}

typedef struct
{
    int (*func)(void);
    const char* name;
} TestCase;

TestCase tests[] = {
    {test_range_positive_step, "test_range_positive_step"},
    {test_range_negative_step, "test_range_negative_step"},
    {test_range_larger_step, "test_range_larger_step"},
    {test_range_invalid_step, "test_range_invalid_step"},
    {test_range_empty, "test_range_empty"},
    {test_range_reset, "test_range_reset"},
    {test_range_transform_chain, "test_range_transform_chain"},
    {test_range_filter_chain, "test_range_filter_chain"},
    {test_range_extreme_values, "test_range_extreme_values"},
    {test_complex_iteration_patterns, "test_complex_iteration_patterns"},
    {test_bidirectional_iteration, "test_bidirectional_iteration"},
    {test_zigzag_traversal, "test_zigzag_traversal"},
    {test_range_stress, "test_range_stress"},
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
        printf("All range iterator tests passed!\n");
        return 0;
    }
}