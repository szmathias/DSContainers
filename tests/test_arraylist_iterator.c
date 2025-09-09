//
// Created by zack on 9/2/25.
//

#include "ArrayList.h"
#include "TestAssert.h"
#include "TestHelpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int test_forward_iterator(void)
{
    DSCAlloc* alloc    = create_std_allocator();
    DSCArrayList* list = dsc_arraylist_create(alloc, 0);

    // Add numbers 1-5
    for (int i = 1; i <= 5; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_arraylist_push_back(list, val);
    }

    DSCIterator iter = dsc_arraylist_iterator(list);
    ASSERT(iter.is_valid(&iter));

    // Test forward iteration
    int expected = 1;
    while (iter.has_next(&iter))
    {
        const int* val = (int*)iter.next(&iter);
        ASSERT_NOT_NULL(val);
        ASSERT_EQ(*val, expected);
        expected++;
    }

    ASSERT_EQ(expected, 6); // Should have iterated through all 5 elements
    ASSERT(!iter.has_next(&iter));

    iter.destroy(&iter);
    dsc_arraylist_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_reverse_iterator(void)
{
    DSCAlloc* alloc    = create_std_allocator();
    DSCArrayList* list = dsc_arraylist_create(alloc, 0);

    // Add numbers 1-5
    for (int i = 1; i <= 5; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_arraylist_push_back(list, val);
    }

    DSCIterator iter = dsc_arraylist_iterator_reverse(list);
    ASSERT(iter.is_valid(&iter));

    // Test reverse iteration (should get 5, 4, 3, 2, 1)
    int expected = 5;
    while (iter.has_next(&iter))
    {
        const int* val = (int*)iter.next(&iter);
        ASSERT_NOT_NULL(val);
        ASSERT_EQ(*val, expected);
        expected--;
    }

    ASSERT_EQ(expected, 0); // Should have iterated through all 5 elements
    ASSERT(!iter.has_next(&iter));

    iter.destroy(&iter);
    dsc_arraylist_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_iterator_get(void)
{
    DSCAlloc* alloc    = create_std_allocator();
    DSCArrayList* list = dsc_arraylist_create(alloc, 0);

    // Add numbers 1-3
    for (int i = 1; i <= 3; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_arraylist_push_back(list, val);
    }

    DSCIterator iter = dsc_arraylist_iterator(list);

    // Test get without advancing
    int* val = (int*)iter.get(&iter);
    ASSERT_NOT_NULL(val);
    ASSERT_EQ(*val, 1);

    // Get again - should return same value
    val = (int*)iter.get(&iter);
    ASSERT_EQ(*val, 1);

    // Now advance and test get
    iter.next(&iter);
    val = (int*)iter.get(&iter);
    ASSERT_EQ(*val, 2);

    iter.destroy(&iter);
    dsc_arraylist_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_iterator_prev(void)
{
    DSCAlloc* alloc    = create_std_allocator();
    DSCArrayList* list = dsc_arraylist_create(alloc, 0);

    // Add numbers 1-3
    for (int i = 1; i <= 3; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_arraylist_push_back(list, val);
    }

    DSCIterator iter = dsc_arraylist_iterator(list);


    iter.next(&iter); // 2
    iter.next(&iter); // 3
    iter.next(&iter); // Null

    // Test has_prev and prev
    ASSERT(iter.has_prev(&iter));
    const int* val = (int*)iter.prev(&iter);
    ASSERT_EQ(*val, 3);

    ASSERT(iter.has_prev(&iter));
    val = (int*)iter.prev(&iter);
    ASSERT_EQ(*val, 2);

    ASSERT(iter.has_prev(&iter));
    val = (int*)iter.prev(&iter);
    ASSERT_EQ(*val, 1);

    ASSERT(!iter.has_prev(&iter));

    iter.destroy(&iter);
    dsc_arraylist_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_iterator_reset(void)
{
    DSCAlloc* alloc    = create_std_allocator();
    DSCArrayList* list = dsc_arraylist_create(alloc, 0);

    // Add numbers 1-3
    for (int i = 1; i <= 3; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_arraylist_push_back(list, val);
    }

    DSCIterator iter = dsc_arraylist_iterator(list);

    // Advance iterator
    iter.next(&iter);
    iter.next(&iter);

    // Reset and verify back at beginning
    iter.reset(&iter);
    int* val = (int*)iter.get(&iter);
    ASSERT_EQ(*val, 1);
    ASSERT(iter.has_next(&iter));

    iter.destroy(&iter);
    dsc_arraylist_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_iterator_empty_list(void)
{
    DSCAlloc* alloc    = create_std_allocator();
    DSCArrayList* list = dsc_arraylist_create(alloc, 0);

    DSCIterator iter = dsc_arraylist_iterator(list);
    ASSERT(iter.is_valid(&iter));
    ASSERT(!iter.has_next(&iter));
    ASSERT(!iter.has_prev(&iter));
    ASSERT_NULL(iter.next(&iter));
    ASSERT_NULL(iter.get(&iter));

    iter.destroy(&iter);
    dsc_arraylist_destroy(list, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_iterator_single_element(void)
{
    DSCAlloc* alloc    = create_std_allocator();
    DSCArrayList* list = dsc_arraylist_create(alloc, 0);

    int* val = malloc(sizeof(int));
    *val     = 42;
    dsc_arraylist_push_back(list, val);

    DSCIterator iter = dsc_arraylist_iterator(list);

    ASSERT(iter.has_next(&iter));
    ASSERT(!iter.has_prev(&iter));

    int* retrieved = (int*)iter.next(&iter);
    ASSERT_EQ(*retrieved, 42);

    ASSERT(!iter.has_next(&iter));
    ASSERT(iter.has_prev(&iter));

    iter.destroy(&iter);
    dsc_arraylist_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_from_iterator(void)
{
    DSCAlloc* alloc        = create_std_allocator();
    DSCArrayList* original = dsc_arraylist_create(alloc, 0);

    // Add numbers 1-5
    for (int i = 1; i <= 5; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_arraylist_push_back(original, val);
    }

    // Create iterator and new list from it
    DSCIterator iter       = dsc_arraylist_iterator(original);
    DSCArrayList* new_list = dsc_arraylist_from_iterator(&iter, alloc);

    ASSERT_NOT_NULL(new_list);
    ASSERT_EQ(dsc_arraylist_size(new_list), 5);

    // Verify contents (should share same data pointers)
    for (int i = 0; i < 5; i++)
    {
        ASSERT_EQ(*(int*)dsc_arraylist_get(new_list, i), i + 1);
    }

    iter.destroy(&iter);
    dsc_arraylist_destroy(new_list, false); // Don't free shared data
    dsc_arraylist_destroy(original, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_bidirectional_iteration(void)
{
    DSCAlloc* alloc    = create_std_allocator();
    DSCArrayList* list = dsc_arraylist_create(alloc, 0);

    // Add numbers 1-5
    for (int i = 1; i <= 5; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_arraylist_push_back(list, val);
    }

    DSCIterator iter = dsc_arraylist_iterator(list);

    // Move forward to middle
    iter.next(&iter); // 2
    iter.next(&iter); // 3
    iter.next(&iter); // 4

    const int* val = (int*)iter.get(&iter);
    ASSERT_EQ(*val, 4);

    // Move back
    val = (int*)iter.prev(&iter);
    ASSERT_EQ(*val, 3);

    // Move forward again
    val = (int*)iter.next(&iter);
    ASSERT_EQ(*val, 3);

    iter.destroy(&iter);
    dsc_arraylist_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

typedef struct
{
    int (*func)(void);
    const char* name;
} TestCase;

int main(void)
{
    TestCase tests[] = {
        {test_forward_iterator, "test_forward_iterator"},
        {test_reverse_iterator, "test_reverse_iterator"},
        {test_iterator_get, "test_iterator_get"},
        {test_iterator_prev, "test_iterator_prev"},
        {test_iterator_reset, "test_iterator_reset"},
        {test_iterator_empty_list, "test_iterator_empty_list"},
        {test_iterator_single_element, "test_iterator_single_element"},
        {test_from_iterator, "test_from_iterator"},
        {test_bidirectional_iteration, "test_bidirectional_iteration"},
    };

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
        printf("All ArrayList iterator tests passed.\n");
        return 0;
    }

    printf("%d ArrayList iterator tests failed.\n", failed);
    return 1;
}