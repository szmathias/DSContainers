//
// Created by zack on 9/2/25.
//

#include "ArrayList.h"
#include "TestAssert.h"
#include "TestHelpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int test_sort(void)
{
    DSCAlloc* alloc    = create_std_allocator();
    DSCArrayList* list = dsc_arraylist_create(alloc, 0);

    // Add unsorted elements

    for (int i = 0; i < 6; i++)
    {
        const int values[] = {5, 2, 8, 1, 9, 3};
        int* val           = malloc(sizeof(int));
        *val               = values[i];
        dsc_arraylist_push_back(list, val);
    }

    ASSERT_EQ(dsc_arraylist_sort(list, int_cmp), 0);

    // Verify sorted order
    for (int i = 0; i < 6; i++)
    {
        const int expected[] = {1, 2, 3, 5, 8, 9};
        ASSERT_EQ(*(int*)dsc_arraylist_get(list, i), expected[i]);
    }

    dsc_arraylist_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_sort_empty(void)
{
    DSCAlloc* alloc    = create_std_allocator();
    DSCArrayList* list = dsc_arraylist_create(alloc, 0);

    // Sort empty list should succeed
    ASSERT_EQ(dsc_arraylist_sort(list, int_cmp), 0);
    ASSERT_EQ(dsc_arraylist_size(list), 0);

    dsc_arraylist_destroy(list, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_sort_single_element(void)
{
    DSCAlloc* alloc    = create_std_allocator();
    DSCArrayList* list = dsc_arraylist_create(alloc, 0);

    int* val = malloc(sizeof(int));
    *val     = 42;
    dsc_arraylist_push_back(list, val);

    ASSERT_EQ(dsc_arraylist_sort(list, int_cmp), 0);
    ASSERT_EQ(*(int*)dsc_arraylist_get(list, 0), 42);

    dsc_arraylist_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_sort_already_sorted(void)
{
    DSCAlloc* alloc    = create_std_allocator();
    DSCArrayList* list = dsc_arraylist_create(alloc, 0);

    // Add already sorted elements
    for (int i = 1; i <= 10; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_arraylist_push_back(list, val);
    }

    ASSERT_EQ(dsc_arraylist_sort(list, int_cmp), 0);

    // Verify still sorted
    for (int i = 0; i < 10; i++)
    {
        ASSERT_EQ(*(int*)dsc_arraylist_get(list, i), i + 1);
    }

    dsc_arraylist_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_sort_reverse_sorted(void)
{
    DSCAlloc* alloc    = create_std_allocator();
    DSCArrayList* list = dsc_arraylist_create(alloc, 0);

    // Add reverse sorted elements
    for (int i = 10; i >= 1; i--)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_arraylist_push_back(list, val);
    }

    ASSERT_EQ(dsc_arraylist_sort(list, int_cmp), 0);

    // Verify now sorted
    for (int i = 0; i < 10; i++)
    {
        ASSERT_EQ(*(int*)dsc_arraylist_get(list, i), i + 1);
    }

    dsc_arraylist_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_reverse(void)
{
    DSCAlloc* alloc    = create_std_allocator();
    DSCArrayList* list = dsc_arraylist_create(alloc, 0);

    // Add elements 1, 2, 3, 4, 5
    for (int i = 1; i <= 5; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_arraylist_push_back(list, val);
    }

    ASSERT_EQ(dsc_arraylist_reverse(list), 0);

    // Verify reversed order: 5, 4, 3, 2, 1
    for (int i = 0; i < 5; i++)
    {
        ASSERT_EQ(*(int*)dsc_arraylist_get(list, i), 5 - i);
    }

    dsc_arraylist_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_reverse_empty(void)
{
    DSCAlloc* alloc    = create_std_allocator();
    DSCArrayList* list = dsc_arraylist_create(alloc, 0);

    // Reverse empty list should succeed
    ASSERT_EQ(dsc_arraylist_reverse(list), 0);
    ASSERT_EQ(dsc_arraylist_size(list), 0);

    dsc_arraylist_destroy(list, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_reverse_single_element(void)
{
    DSCAlloc* alloc    = create_std_allocator();
    DSCArrayList* list = dsc_arraylist_create(alloc, 0);

    int* val = malloc(sizeof(int));
    *val     = 42;
    dsc_arraylist_push_back(list, val);

    ASSERT_EQ(dsc_arraylist_reverse(list), 0);
    ASSERT_EQ(*(int*)dsc_arraylist_get(list, 0), 42);

    dsc_arraylist_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_filter(void)
{
    DSCAlloc* alloc    = create_std_allocator();
    DSCArrayList* list = dsc_arraylist_create(alloc, 0);

    // Add numbers 1-10
    for (int i = 1; i <= 10; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_arraylist_push_back(list, val);
    }

    // Filter even numbers
    DSCArrayList* filtered = dsc_arraylist_filter(list, is_even);
    ASSERT_NOT_NULL(filtered);
    ASSERT_EQ(dsc_arraylist_size(filtered), 5);

    // Verify even numbers: 2, 4, 6, 8, 10
    for (int i = 0; i < 5; i++)
    {
        const int expected_evens[] = {2, 4, 6, 8, 10};
        ASSERT_EQ(*(int*)dsc_arraylist_get(filtered, i), expected_evens[i]);
    }

    dsc_arraylist_destroy(filtered, false);
    dsc_arraylist_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// New test: deep filter should produce copies of matching elements
int test_filter_deep(void)
{
    DSCAlloc* alloc    = create_std_allocator();
    DSCArrayList* list = dsc_arraylist_create(alloc, 0);

    // Add numbers 1-10
    for (int i = 1; i <= 10; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_arraylist_push_back(list, val);
    }

    DSCArrayList* filtered = dsc_arraylist_filter_deep(list, is_even);
    ASSERT_NOT_NULL(filtered);
    ASSERT_EQ(dsc_arraylist_size(filtered), 5);

    // Verify values are correct and that pointers are different (deep copied)
    for (size_t i = 0; i < dsc_arraylist_size(filtered); i++)
    {
        int* filtered_val = dsc_arraylist_get(filtered, i);
        int* original_val = dsc_arraylist_get(list, i * 2 + 1); // original even positions: 1,3,5,...
        ASSERT_NOT_NULL(filtered_val);
        ASSERT_NOT_NULL(original_val);
        ASSERT_EQ(*(int*)filtered_val, *(int*)original_val);
        ASSERT_NOT_EQ_PTR(filtered_val, original_val);
    }

    // Free deep-copied data in filtered list
    dsc_arraylist_destroy(filtered, true);
    dsc_arraylist_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// New test: deep filter on empty list should return an empty list
int test_filter_deep_empty(void)
{
    DSCAlloc* alloc    = create_std_allocator();
    DSCArrayList* list = dsc_arraylist_create(alloc, 0);

    DSCArrayList* filtered = dsc_arraylist_filter_deep(list, is_even);
    ASSERT_NOT_NULL(filtered);
    ASSERT_EQ(dsc_arraylist_size(filtered), 0);

    dsc_arraylist_destroy(filtered, false);
    dsc_arraylist_destroy(list, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_transform(void)
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

    // Transform by doubling
    DSCArrayList* transformed = dsc_arraylist_transform(list, double_value, false);
    ASSERT_NOT_NULL(transformed);
    ASSERT_EQ(dsc_arraylist_size(transformed), 5);

    // Verify doubled values: 2, 4, 6, 8, 10
    for (int i = 0; i < 5; i++)
    {
        ASSERT_EQ(*(int*)dsc_arraylist_get(transformed, i), (i + 1) * 2);
    }

    dsc_arraylist_destroy(transformed, true); // Free transformed data
    dsc_arraylist_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Helper for for_each test
static int for_each_sum = 0;

static void add_to_sum(void* data)
{
    for_each_sum += *(int*)data;
}

int test_for_each(void)
{
    DSCAlloc* alloc    = create_std_allocator();
    DSCArrayList* list = dsc_arraylist_create(alloc, 0);

    for_each_sum = 0;

    // Add numbers 1-5
    for (int i = 1; i <= 5; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_arraylist_push_back(list, val);
    }

    dsc_arraylist_for_each(list, add_to_sum);

    // Sum should be 1+2+3+4+5 = 15
    ASSERT_EQ(for_each_sum, 15);

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
    const TestCase tests[] = {
        {test_sort, "test_sort"},
        {test_sort_empty, "test_sort_empty"},
        {test_sort_single_element, "test_sort_single_element"},
        {test_sort_already_sorted, "test_sort_already_sorted"},
        {test_sort_reverse_sorted, "test_sort_reverse_sorted"},
        {test_reverse, "test_reverse"},
        {test_reverse_empty, "test_reverse_empty"},
        {test_reverse_single_element, "test_reverse_single_element"},
        {test_filter, "test_filter"},
        {test_filter_deep, "test_filter_deep"},
        {test_filter_deep_empty, "test_filter_deep_empty"},
        {test_transform, "test_transform"},
        {test_for_each, "test_for_each"},
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
        printf("All ArrayList algorithms tests passed.\n");
        return 0;
    }

    printf("%d ArrayList algorithms tests failed.\n", failed);
    return 1;
}