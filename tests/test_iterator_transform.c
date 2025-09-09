//
// Created by zack on 9/3/25.
//

#include "Iterator.h"
#include "DoublyLinkedList.h"
#include "TestAssert.h"
#include "TestHelpers.h"
#include <stdlib.h>

// Test basic transform iterator functionality
static int test_transform_iterator(void)
{
    // Create a list with integers
    DSCAlloc* alloc           = create_std_allocator();
    DSCDoublyLinkedList* list = dsc_dll_create(alloc);
    ASSERT_NOT_NULL(list);

    // Add some elements
    for (int i = 1; i <= 5; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_dll_insert_back(list, val);
    }

    // Create a basic iterator
    DSCIterator base_it = dsc_dll_iterator(list);
    ASSERT_TRUE(base_it.has_next(&base_it));

    // Create a transform iterator that doubles each value
    DSCIterator transform_it = dsc_iterator_transform(&base_it, double_value);
    ASSERT_TRUE(transform_it.has_next(&transform_it));

    // Test that transform works correctly
    int expected = 2; // First element (1) doubled
    while (transform_it.has_next(&transform_it))
    {
        int* value = transform_it.next(&transform_it);
        ASSERT_NOT_NULL(value);
        ASSERT_EQ(*value, expected);
        expected += 2; // Next expected value
        free(value);   // Free the transformed value
    }

    // Verify we processed all elements
    ASSERT_EQ(expected, 12); // After 1,2,3,4,5 -> 2,4,6,8,10

    // Cleanup
    transform_it.destroy(&transform_it);
    dsc_dll_destroy(list, true);
    destroy_allocator(alloc);

    return TEST_SUCCESS;
}

// Test transform iterator with edge cases
static int test_transform_edge_cases(void)
{
    // Test with empty list
    DSCAlloc* alloc                 = create_std_allocator();
    DSCDoublyLinkedList* empty_list = dsc_dll_create(alloc);
    ASSERT_NOT_NULL(empty_list);

    DSCIterator empty_it = dsc_dll_iterator(empty_list);
    ASSERT_FALSE(empty_it.has_next(&empty_it));

    DSCIterator transform_empty = dsc_iterator_transform(&empty_it, double_value);
    ASSERT_FALSE(transform_empty.has_next(&transform_empty));
    ASSERT_NULL(transform_empty.next(&transform_empty));

    transform_empty.destroy(&transform_empty);
    dsc_dll_destroy(empty_list, false);
    destroy_allocator(alloc);

    // Test with NULL base iterator (should handle gracefully)
    DSCIterator null_transform = dsc_iterator_transform(NULL, double_value);
    ASSERT_FALSE(null_transform.has_next(&null_transform));
    ASSERT_NULL(null_transform.next(&null_transform));

    null_transform.destroy(&null_transform);

    return TEST_SUCCESS;
}

// Test transform iterator chaining
static int test_transform_chaining(void)
{
    DSCAlloc* alloc           = create_std_allocator();
    DSCDoublyLinkedList* list = dsc_dll_create(alloc);
    ASSERT_NOT_NULL(list);

    // Add elements 1-3
    for (int i = 1; i <= 3; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_dll_insert_back(list, val);
    }

    // First transformation: double the value
    DSCIterator base_it   = dsc_dll_iterator(list);
    DSCIterator double_it = dsc_iterator_transform(&base_it, double_value);

    DSCIterator chain_it = dsc_iterator_transform(&double_it, add_one);

    // Test the chained transformations (1->2->3, 2->4->5, 3->6->7)
    int idx = 0;

    while (chain_it.has_next(&chain_it))
    {
        const int expected_values[] = {3, 5, 7};
        int* value                  = chain_it.next(&chain_it);
        ASSERT_NOT_NULL(value);
        ASSERT_EQ(*value, expected_values[idx++]);
        free(value);
    }

    ASSERT_EQ(idx, 3); // Verify we processed all elements

    // Cleanup - note that destroying the outermost iterator should
    // recursively destroy all inner iterators
    chain_it.destroy(&chain_it);
    dsc_dll_destroy(list, true);
    destroy_allocator(alloc);

    return TEST_SUCCESS;
}

// Test multiple transform chaining (range -> transform -> transform)
static int test_multiple_transforms(void)
{
    // Create range from 1 to 6, then double, then add 10
    DSCIterator range_it = dsc_iterator_range(1, 6, 1);
    ASSERT_TRUE(range_it.is_valid(&range_it));

    DSCIterator double_it = dsc_iterator_transform(&range_it, double_value);
    ASSERT_TRUE(double_it.is_valid(&double_it));

    DSCIterator add_ten_it = dsc_iterator_transform(&double_it, add_ten_func);
    ASSERT_TRUE(add_ten_it.is_valid(&add_ten_it));

    // Expected: [12,14,16,18,20] (double each element in range [1-5], then add 10)
    int idx = 0;

    while (add_ten_it.has_next(&add_ten_it))
    {
        const int expected[] = {12, 14, 16, 18, 20};
        int* value           = add_ten_it.next(&add_ten_it);
        ASSERT_NOT_NULL(value);
        ASSERT_EQ(*value, expected[idx]);
        idx++;
        free(value);
    }

    ASSERT_EQ(idx, 5); // Should have 5 elements

    // Cleanup
    add_ten_it.destroy(&add_ten_it); // This should destroy all chained iterators
    return TEST_SUCCESS;
}


typedef struct
{
    int (*func)(void);
    const char* name;
} TestCase;

TestCase tests[] = {
    {test_transform_iterator, "test_transform_iterator"},
    {test_transform_edge_cases, "test_transform_edge_cases"},
    {test_transform_chaining, "test_transform_chaining"},
    {test_multiple_transforms, "test_multiple_transforms"},
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
        printf("All transform iterator tests passed!\n");
        return 0;
    }
}