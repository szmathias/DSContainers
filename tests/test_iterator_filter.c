//
// Created by zack on 9/3/25.
//

#include "Iterator.h"
#include "DoublyLinkedList.h"
#include "TestAssert.h"
#include "TestHelpers.h"
#include <stdlib.h>

// Test filter iterator functionality
static int test_filter_iterator(void)
{
    DSCAlloc alloc           = create_int_allocator();
    DSCDoublyLinkedList* list = dsc_dll_create(&alloc);
    ASSERT_NOT_NULL(list);

    // Insert elements 1-10
    for (int i = 1; i <= 10; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_dll_insert_back(list, val);
    }

    // Create base iterator
    DSCIterator base_it = dsc_dll_iterator(list);

    // Create filter iterator for even numbers
    DSCIterator filter_it = dsc_iterator_filter(&base_it, is_even, &alloc);
    ASSERT_NOT_NULL(filter_it.data_state);

    // Verify filter correctly returns only even numbers
    int idx = 0;

    while (filter_it.has_next(&filter_it))
    {
        const int expected_values[] = {2, 4, 6, 8, 10};
        const int* value            = filter_it.next(&filter_it);
        ASSERT_NOT_NULL(value);
        ASSERT_EQ(*value, expected_values[idx++]);
    }

    // Verify we found all 5 even numbers
    ASSERT_EQ(idx, 5);

    // Cleanup
    if (filter_it.destroy)
    {
        filter_it.destroy(&filter_it);
    }
    dsc_dll_destroy(list, true);

    return TEST_SUCCESS;
}

// Test filter iterator with no matches
static int test_filter_no_matches(void)
{
    DSCAlloc alloc           = create_int_allocator();
    DSCDoublyLinkedList* list = dsc_dll_create(&alloc);
    ASSERT_NOT_NULL(list);

    // Insert odd numbers only
    for (int i = 1; i <= 5; i += 2)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_dll_insert_back(list, val);
    }

    // Create base iterator
    DSCIterator base_it = dsc_dll_iterator(list);

    // Create filter iterator for even numbers (should find none)
    DSCIterator filter_it = dsc_iterator_filter(&base_it, is_even, &alloc);

    // Verify filter correctly returns no results
    ASSERT_FALSE(filter_it.has_next(&filter_it));
    ASSERT_NULL(filter_it.next(&filter_it));

    // Cleanup
    if (filter_it.destroy)
    {
        filter_it.destroy(&filter_it);
    }
    dsc_dll_destroy(list, true);

    return TEST_SUCCESS;
}

// Test chaining multiple filters
static int test_multiple_filters(void)
{
    DSCAlloc alloc = create_int_allocator();
    // Create range from 1 to 30, then keep only even numbers, then only those divisible by 3
    DSCIterator range_it = dsc_iterator_range(1, 30, 1, &alloc);
    ASSERT_TRUE(range_it.is_valid(&range_it));

    DSCIterator even_it = dsc_iterator_filter(&range_it, is_even, &alloc);
    ASSERT_TRUE(even_it.is_valid(&even_it));

    DSCIterator div3_it = dsc_iterator_filter(&even_it, is_divisible_by_3, &alloc);
    ASSERT_TRUE(div3_it.is_valid(&div3_it));

    // Expected: [6,12,18,24] (even numbers from 1-29 that are also divisible by 3)
    int idx = 0;
    while (div3_it.has_next(&div3_it))
    {
        const int expected[] = {6, 12, 18, 24};
        int* value           = div3_it.next(&div3_it);
        ASSERT_NOT_NULL(value);
        ASSERT_EQ(*value, expected[idx]);
        idx++;
        free(value);
    }

    ASSERT_EQ(idx, 4); // Should have 4 elements

    // Cleanup
    div3_it.destroy(&div3_it); // This should destroy all chained iterators
    return TEST_SUCCESS;
}

typedef struct
{
    int (*func)(void);
    const char* name;
} TestCase;

TestCase tests[] = {
    {test_filter_iterator, "test_filter_iterator"},
    {test_filter_no_matches, "test_filter_no_matches"},
    {test_multiple_filters, "test_multiple_filters"},
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
        printf("All filter iterator tests passed!\n");
        return 0;
    }
}