//
// Created by zack on 9/2/25.
//

#include "ArrayList.h"
#include "TestAssert.h"
#include "TestHelpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int test_equals(void)
{
    DSCAlloc* alloc     = create_std_allocator();
    DSCArrayList* list1 = dsc_arraylist_create(alloc, 0);
    DSCArrayList* list2 = dsc_arraylist_create(alloc, 0);

    // Test empty lists
    ASSERT_EQ(dsc_arraylist_equals(list1, list2, int_cmp), 1);

    // Add same elements to both
    for (int i = 1; i <= 3; i++)
    {
        int* val1 = malloc(sizeof(int));
        int* val2 = malloc(sizeof(int));
        *val1     = i;
        *val2     = i;
        dsc_arraylist_push_back(list1, val1);
        dsc_arraylist_push_back(list2, val2);
    }

    ASSERT_EQ(dsc_arraylist_equals(list1, list2, int_cmp), 1);

    // Add different element to list2
    int* val = malloc(sizeof(int));
    *val     = 99;
    dsc_arraylist_push_back(list2, val);

    ASSERT_EQ(dsc_arraylist_equals(list1, list2, int_cmp), 0);

    dsc_arraylist_destroy(list1, true);
    dsc_arraylist_destroy(list2, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_equals_different_sizes(void)
{
    DSCAlloc* alloc     = create_std_allocator();
    DSCArrayList* list1 = dsc_arraylist_create(alloc, 0);
    DSCArrayList* list2 = dsc_arraylist_create(alloc, 0);

    int* val1 = malloc(sizeof(int));
    *val1     = 1;
    int* val2 = malloc(sizeof(int));
    *val2     = 1;
    int* val3 = malloc(sizeof(int));
    *val3     = 2;

    dsc_arraylist_push_back(list1, val1);
    dsc_arraylist_push_back(list2, val2);
    dsc_arraylist_push_back(list2, val3);

    ASSERT_EQ(dsc_arraylist_equals(list1, list2, int_cmp), 0);

    dsc_arraylist_destroy(list1, true);
    dsc_arraylist_destroy(list2, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_copy_shallow(void)
{
    DSCAlloc* alloc        = create_std_allocator();
    DSCArrayList* original = dsc_arraylist_create(alloc, 0);

    // Add numbers 1-3
    for (int i = 1; i <= 3; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_arraylist_push_back(original, val);
    }

    DSCArrayList* copy = dsc_arraylist_copy(original);
    ASSERT_NOT_NULL(copy);
    ASSERT_EQ(dsc_arraylist_size(copy), 3);
    ASSERT_EQ(dsc_arraylist_equals(original, copy, int_cmp), 1);

    // Verify shallow copy (same data pointers)
    for (int i = 0; i < 3; i++)
    {
        ASSERT_EQ_PTR(dsc_arraylist_get(original, i), dsc_arraylist_get(copy, i));
    }

    dsc_arraylist_destroy(copy, false); // Don't free shared data
    dsc_arraylist_destroy(original, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_copy_deep(void)
{
    DSCAlloc* alloc        = create_std_allocator();
    DSCArrayList* original = dsc_arraylist_create(alloc, 0);

    // Add numbers 1-3
    for (int i = 1; i <= 3; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_arraylist_push_back(original, val);
    }

    DSCArrayList* copy = dsc_arraylist_copy_deep(original, false);
    ASSERT_NOT_NULL(copy);
    ASSERT_EQ(dsc_arraylist_size(copy), 3);
    ASSERT_EQ(dsc_arraylist_equals(original, copy, int_cmp), 1);

    // Verify deep copy (different data pointers, same values)
    for (int i = 0; i < 3; i++)
    {
        ASSERT_NOT_EQ_PTR(dsc_arraylist_get(original, i), dsc_arraylist_get(copy, i));
        ASSERT_EQ(*(int*)dsc_arraylist_get(original, i), *(int*)dsc_arraylist_get(copy, i));
    }

    dsc_arraylist_destroy(copy, true); // Free copied data
    dsc_arraylist_destroy(original, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_boundary_conditions(void)
{
    DSCAlloc* alloc    = create_std_allocator();
    DSCArrayList* list = dsc_arraylist_create(alloc, 0);

    // Test operations on empty list
    ASSERT_NULL(dsc_arraylist_get(list, 0));
    ASSERT_NULL(dsc_arraylist_front(list));
    ASSERT_NULL(dsc_arraylist_back(list));
    ASSERT_EQ(dsc_arraylist_remove_at(list, 0, false), -1);
    ASSERT_EQ(dsc_arraylist_pop_back(list, false), -1);
    ASSERT_EQ(dsc_arraylist_pop_front(list, false), -1);

    // Test invalid indices
    int* val = malloc(sizeof(int));
    *val     = 42;
    dsc_arraylist_push_back(list, val);

    ASSERT_NULL(dsc_arraylist_get(list, 1));
    ASSERT_EQ(dsc_arraylist_set(list, 1, val, false), -1);
    ASSERT_EQ(dsc_arraylist_remove_at(list, 1, false), -1);
    ASSERT_EQ(dsc_arraylist_insert(list, 2, val), -1); // Can insert at size, but not size+1

    dsc_arraylist_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_null_parameters(void)
{
    DSCAlloc* alloc    = create_std_allocator();
    DSCArrayList* list = dsc_arraylist_create(alloc, 0);

    // Test NULL list parameter
    ASSERT_EQ(dsc_arraylist_size(NULL), 0);
    ASSERT_EQ(dsc_arraylist_capacity(NULL), 0);
    ASSERT(dsc_arraylist_is_empty(NULL));
    ASSERT_NULL(dsc_arraylist_get(NULL, 0));
    ASSERT_EQ(dsc_arraylist_push_back(NULL, &list), -1);

    // Test NULL allocator
    DSCArrayList* null_alloc_list = dsc_arraylist_create(NULL, 0);
    ASSERT_NULL(null_alloc_list);

    dsc_arraylist_destroy(list, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_size_consistency(void)
{
    DSCAlloc* alloc    = create_std_allocator();
    DSCArrayList* list = dsc_arraylist_create(alloc, 0);

    ASSERT_EQ(dsc_arraylist_size(list), 0);
    ASSERT(dsc_arraylist_is_empty(list));

    // Add elements and verify size
    for (int i = 0; i < 10; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_arraylist_push_back(list, val);
        ASSERT_EQ(dsc_arraylist_size(list), (size_t)i + 1);
        ASSERT(!dsc_arraylist_is_empty(list));
    }

    // Remove elements and verify size
    for (int i = 9; i >= 0; i--)
    {
        dsc_arraylist_pop_back(list, true);
        ASSERT_EQ(dsc_arraylist_size(list), (size_t)i);
        if (i == 0)
        {
            ASSERT(dsc_arraylist_is_empty(list));
        }
        else
        {
            ASSERT(!dsc_arraylist_is_empty(list));
        }
    }

    dsc_arraylist_destroy(list, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_data_integrity_after_operations(void)
{
    DSCAlloc* alloc    = create_std_allocator();
    DSCArrayList* list = dsc_arraylist_create(alloc, 0);

    // Add initial data
    for (int i = 0; i < 10; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i * 10; // 0, 10, 20, ..., 90
        dsc_arraylist_push_back(list, val);
    }

    // Insert in middle
    int* val = malloc(sizeof(int));
    *val     = 99;
    dsc_arraylist_insert(list, 5, val);

    // Verify data integrity
    for (int i = 0; i < 5; i++)
    {
        ASSERT_EQ(*(int*)dsc_arraylist_get(list, i), i * 10);
    }
    ASSERT_EQ(*(int*)dsc_arraylist_get(list, 5), 99);
    for (int i = 6; i < 11; i++)
    {
        ASSERT_EQ(*(int*)dsc_arraylist_get(list, i), (i - 1) * 10);
    }

    // Remove from middle
    dsc_arraylist_remove_at(list, 5, true);

    // Verify data integrity restored
    for (int i = 0; i < 10; i++)
    {
        ASSERT_EQ(*(int*)dsc_arraylist_get(list, i), i * 10);
    }

    dsc_arraylist_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_large_data_set(void)
{
    DSCAlloc* alloc    = create_std_allocator();
    DSCArrayList* list = dsc_arraylist_create(alloc, 0);

    const int NUM_ELEMENTS = 10000;

    // Add large number of elements
    for (int i = 0; i < NUM_ELEMENTS; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        ASSERT_EQ(dsc_arraylist_push_back(list, val), 0);
    }

    ASSERT_EQ(dsc_arraylist_size(list), (size_t)NUM_ELEMENTS);

    // Spot check values
    ASSERT_EQ(*(int*)dsc_arraylist_get(list, 0), 0);
    ASSERT_EQ(*(int*)dsc_arraylist_get(list, NUM_ELEMENTS / 2), NUM_ELEMENTS / 2);
    ASSERT_EQ(*(int*)dsc_arraylist_get(list, NUM_ELEMENTS - 1), NUM_ELEMENTS - 1);

    // Remove half the elements
    for (int i = 0; i < NUM_ELEMENTS / 2; i++)
    {
        dsc_arraylist_pop_back(list, true);
    }

    ASSERT_EQ(dsc_arraylist_size(list), (size_t)NUM_ELEMENTS / 2);
    ASSERT_EQ(*(int*)dsc_arraylist_back(list), NUM_ELEMENTS / 2 - 1);

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
        {test_equals, "test_equals"},
        {test_equals_different_sizes, "test_equals_different_sizes"},
        {test_copy_shallow, "test_copy_shallow"},
        {test_copy_deep, "test_copy_deep"},
        {test_boundary_conditions, "test_boundary_conditions"},
        {test_null_parameters, "test_null_parameters"},
        {test_size_consistency, "test_size_consistency"},
        {test_data_integrity_after_operations, "test_data_integrity_after_operations"},
        {test_large_data_set, "test_large_data_set"},
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
        printf("All ArrayList properties tests passed.\n");
        return 0;
    }

    printf("%d ArrayList properties tests failed.\n", failed);
    return 1;
}