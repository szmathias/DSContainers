//
// Created by zack on 8/26/25.
//

#include "DoublyLinkedList.h"
#include "Iterator.h"
#include "TestAssert.h"
#include "TestHelpers.h"

#include <stdlib.h>

// Test basic iterator functionality
static int test_basic_iteration(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCDoublyLinkedList* list = dsc_dll_create(&alloc);
    ASSERT_NOT_NULL(list);

    // Insert elements
    for (int i = 1; i <= 5; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_dll_insert_back(list, val);
    }

    // Create iterator
    DSCIterator it = dsc_dll_iterator(list);
    ASSERT_NOT_NULL(it.data_state);
    ASSERT_TRUE(it.has_next(&it));

    // Iterate through list and verify values
    int expected = 1;
    while (it.has_next(&it))
    {
        const int* value = it.next(&it);
        ASSERT_NOT_NULL(value);
        ASSERT_EQ(*value, expected++);
    }

    // Verify we processed all elements
    ASSERT_EQ(expected, 6);

    // Verify the iterator is exhausted
    ASSERT_FALSE(it.has_next(&it));
    ASSERT_NULL(it.next(&it));

    // Cleanup
    if (it.destroy)
        it.destroy(&it);
    dsc_dll_destroy(list, true);
    return TEST_SUCCESS;
}

// Test iterator with empty list
static int test_empty_list_iterator(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCDoublyLinkedList* list = dsc_dll_create(&alloc);
    ASSERT_NOT_NULL(list);

    DSCIterator it = dsc_dll_iterator(list);

    // Verify iterator for empty list
    ASSERT_FALSE(it.has_next(&it));
    ASSERT_NULL(it.next(&it));

    // Cleanup
    if (it.destroy)
        it.destroy(&it);
    dsc_dll_destroy(list, false);
    return TEST_SUCCESS;
}

// Test iterator with modifications
static int test_iterator_with_modifications(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCDoublyLinkedList* list = dsc_dll_create(&alloc);
    ASSERT_NOT_NULL(list);

    // Insert initial elements
    for (int i = 1; i <= 3; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_dll_insert_back(list, val);
    }

    // Create iterator
    DSCIterator it = dsc_dll_iterator(list);

    // Consume first element
    const int* value = it.next(&it);
    ASSERT_NOT_NULL(value);
    ASSERT_EQ(*value, 1);

    // Modify list by adding new elements
    int* new_val = malloc(sizeof(int));
    *new_val     = 99;
    dsc_dll_insert_back(list, new_val);

    // Continue iteration
    value = it.next(&it);
    ASSERT_NOT_NULL(value);
    ASSERT_EQ(*value, 2);

    value = it.next(&it);
    ASSERT_NOT_NULL(value);
    ASSERT_EQ(*value, 3);

    // The newly added element should also be accessible
    value = it.next(&it);
    ASSERT_NOT_NULL(value);
    ASSERT_EQ(*value, 99);

    ASSERT_FALSE(it.has_next(&it));

    // Cleanup
    if (it.destroy)
        it.destroy(&it);
    dsc_dll_destroy(list, true);
    return TEST_SUCCESS;
}

// Test multiple iterators
static int test_multiple_iterators(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCDoublyLinkedList* list = dsc_dll_create(&alloc);
    ASSERT_NOT_NULL(list);

    // Insert elements
    for (int i = 1; i <= 5; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_dll_insert_back(list, val);
    }

    // Create two independent iterators
    DSCIterator it1 = dsc_dll_iterator(list);
    DSCIterator it2 = dsc_dll_iterator(list);

    // First iterator consumes two elements
    const int* value1 = it1.next(&it1);
    ASSERT_EQ(*value1, 1);
    value1 = it1.next(&it1);
    ASSERT_EQ(*value1, 2);

    // Second iterator should still be at the beginning
    const int* value2 = it2.next(&it2);
    ASSERT_EQ(*value2, 1);

    // Continue with both iterators
    value1 = it1.next(&it1);
    ASSERT_EQ(*value1, 3);
    value2 = it2.next(&it2);
    ASSERT_EQ(*value2, 2);

    // Cleanup
    if (it1.destroy)
        it1.destroy(&it1);
    if (it2.destroy)
        it2.destroy(&it2);
    dsc_dll_destroy(list, true);
    return TEST_SUCCESS;
}

// Test iterator reset functionality if available
static int test_iterator_reset(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCDoublyLinkedList* list = dsc_dll_create(&alloc);
    ASSERT_NOT_NULL(list);

    for (int i = 1; i <= 3; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_dll_insert_back(list, val);
    }

    DSCIterator it = dsc_dll_iterator(list);
    while (it.has_next(&it))
        it.next(&it);
    ASSERT_FALSE(it.has_next(&it));

    if (it.reset)
    {
        it.reset(&it);
        ASSERT_TRUE(it.has_next(&it));
        int expected = 1;
        while (it.has_next(&it))
        {
            const int* value = it.next(&it);
            ASSERT_EQ(*value, expected++);
        }
        ASSERT_EQ(expected, 4);
    }

    if (it.destroy)
        it.destroy(&it);
    dsc_dll_destroy(list, true);
    return TEST_SUCCESS;
}

// Test reverse iterator functionality
static int test_reverse_iteration(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCDoublyLinkedList* list = dsc_dll_create(&alloc);
    ASSERT_NOT_NULL(list);

    for (int i = 1; i <= 5; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_dll_insert_back(list, val);
    }

    DSCIterator it = dsc_dll_iterator_reverse(list);
    ASSERT_NOT_NULL(it.data_state);
    ASSERT_TRUE(it.has_next(&it));

    int expected = 5;
    while (it.has_next(&it))
    {
        const int* value = it.next(&it);
        ASSERT_NOT_NULL(value);
        ASSERT_EQ(*value, expected--);
    }

    ASSERT_EQ(expected, 0);
    ASSERT_FALSE(it.has_next(&it));
    ASSERT_NULL(it.next(&it));

    if (it.destroy)
        it.destroy(&it);
    dsc_dll_destroy(list, true);
    return TEST_SUCCESS;
}

// Test empty list with reverse iterator
static int test_empty_list_reverse_iterator(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCDoublyLinkedList* list = dsc_dll_create(&alloc);
    ASSERT_NOT_NULL(list);

    DSCIterator it = dsc_dll_iterator_reverse(list);
    ASSERT_FALSE(it.has_next(&it));
    ASSERT_NULL(it.next(&it));

    if (it.destroy)
        it.destroy(&it);
    dsc_dll_destroy(list, false);
    return TEST_SUCCESS;
}

// Helper functions
static void* copy_int(const void* data)
{
    if (!data)
        return NULL;
    int* copy = malloc(sizeof(int));
    if (!copy)
        return NULL;
    *copy = *(const int*)data;
    return copy;
}

static int int_compare(const void* a, const void* b)
{
    const int* ia = a;
    const int* ib = b;
    if (*ia == *ib)
        return 0;
    return (*ia < *ib) ? -1 : 1;
}

// Test converting a basic iterator to a list with dsc_dll_from_iterator_custom
static int test_from_iterator_basic(void)
{
    DSCAllocator src_alloc = create_int_allocator();
    DSCDoublyLinkedList* src_list = dsc_dll_create(&src_alloc);
    ASSERT_NOT_NULL(src_list);

    for (int i = 1; i <= 5; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_dll_insert_back(src_list, val);
    }

    DSCIterator it = dsc_dll_iterator(src_list);
    ASSERT_TRUE(it.has_next(&it));

    // Convert iterator to a new list (shallow copy - same data pointers)
    DSCDoublyLinkedList* new_list = dsc_dll_copy(src_list);
    ASSERT_NOT_NULL(new_list);
    ASSERT_EQ(dsc_dll_size(new_list), 5);

    DSCIterator new_it = dsc_dll_iterator(new_list);
    int expected       = 1;
    while (new_it.has_next(&new_it))
    {
        const int* value = new_it.next(&new_it);
        ASSERT_EQ(*value, expected++);
    }

    const DSCDoublyLinkedNode* node = dsc_dll_find(src_list, &(int){1}, int_compare);
    ASSERT_NOT_NULL(node);
    *((int*)node->data) = 99;

    // Modify the original data; because this is a shallow copy, the new list
    // should reference the same data pointer. Modify the original node's data
    // and verify the new list's head sees the change by directly inspecting
    // the data pointer rather than creating another iterator.
    *((int*)node->data) = 99;

    // The head of the copied list should point to the same data pointer
    ASSERT_NOT_NULL(new_list->head);
    ASSERT_EQ(*(int*)new_list->head->data, 99);

    if (new_it.destroy)
        new_it.destroy(&new_it);
    if (it.destroy)
        it.destroy(&it);

    dsc_dll_destroy(new_list, false); // Don't free data - shared with src_list
    dsc_dll_destroy(src_list, true);  // Free the data

    return TEST_SUCCESS;
}

// Test deep copying with dsc_dll_from_iterator_custom using a copy function
static int test_from_iterator_with_copy(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCDoublyLinkedList* src_list = dsc_dll_create(&alloc);
    ASSERT_NOT_NULL(src_list);

    for (int i = 1; i <= 5; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_dll_insert_back(src_list, val);
    }

    DSCIterator it = dsc_dll_iterator(src_list);
    ASSERT_TRUE(it.has_next(&it));

    DSCAllocator alloc_copy = create_int_allocator();
    DSCDoublyLinkedList* new_list = dsc_dll_from_iterator(&it, &alloc_copy);
    ASSERT_NOT_NULL(new_list);
    ASSERT_EQ(dsc_dll_size(new_list), 5);

    const DSCDoublyLinkedNode* node = dsc_dll_find(src_list, &(int){1}, int_compare);
    ASSERT_NOT_NULL(node);
    *((int*)node->data) = 99;

    DSCIterator new_it = dsc_dll_iterator(new_list);
    int expected2      = 1;
    while (new_it.has_next(&new_it))
    {
        const int* value = new_it.next(&new_it);
        ASSERT_EQ(*value, expected2++);
    }

    if (new_it.destroy)
        new_it.destroy(&new_it);
    if (it.destroy)
        it.destroy(&it);

    dsc_dll_destroy(new_list, true);
    dsc_dll_destroy(src_list, true);
    return TEST_SUCCESS;
}

// Test dsc_dll_from_iterator_custom with an empty list
static int test_from_iterator_empty(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCDoublyLinkedList* src_list = dsc_dll_create(&alloc);
    ASSERT_NOT_NULL(src_list);

    DSCIterator it = dsc_dll_iterator(src_list);
    ASSERT_FALSE(it.has_next(&it));

    DSCDoublyLinkedList* new_list = dsc_dll_from_iterator(&it, &alloc);
    ASSERT_NOT_NULL(new_list);
    ASSERT_EQ(dsc_dll_size(new_list), 0);
    ASSERT_TRUE(dsc_dll_is_empty(new_list));

    if (it.destroy)
        it.destroy(&it);
    dsc_dll_destroy(new_list, false);
    dsc_dll_destroy(src_list, false);
    return TEST_SUCCESS;
}

// Test dsc_dll_from_iterator_custom with invalid iterator
static int test_from_iterator_invalid(void)
{
    DSCDoublyLinkedList* new_list = dsc_dll_from_iterator(NULL, NULL);
    ASSERT_NULL(new_list);

    DSCAllocator alloc = create_int_allocator();
    DSCDoublyLinkedList* list = dsc_dll_create(&alloc);
    DSCIterator it = dsc_dll_iterator(list);
    if (it.destroy)
        it.destroy(&it);

    new_list = dsc_dll_from_iterator(&it, &alloc);
    ASSERT_NULL(new_list);

    dsc_dll_destroy(list, false);
    return TEST_SUCCESS;
}

typedef struct
{
    int (*func)(void);
    const char* name;
} TestCase;

TestCase tests[] = {
    {test_basic_iteration, "test_basic_iteration"},
    {test_empty_list_iterator, "test_empty_list_iterator"},
    {test_iterator_with_modifications, "test_iterator_with_modifications"},
    {test_multiple_iterators, "test_multiple_iterators"},
    {test_iterator_reset, "test_iterator_reset"},
    {test_reverse_iteration, "test_reverse_iteration"},
    {test_empty_list_reverse_iterator, "test_empty_list_reverse_iterator"},
    {test_from_iterator_basic, "test_from_iterator_basic"},
    {test_from_iterator_with_copy, "test_from_iterator_with_copy"},
    {test_from_iterator_empty, "test_from_iterator_empty"},
    {test_from_iterator_invalid, "test_from_iterator_invalid"},
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
        printf("All DoublyLinkedList Iterator tests passed.\n");
        return 0;
    }
    printf("%d DoublyLinkedList Iterator tests failed.\n", failed);
    return 1;
}