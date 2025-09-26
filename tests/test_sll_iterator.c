//
// Created by zack on 9/2/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SinglyLinkedList.h"
#include "TestAssert.h"
#include "TestHelpers.h"

int test_forward_iterator(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);

    // Add numbers 1-5
    for (int i = 1; i <= 5; i++)
    {
        int* val = malloc(sizeof(int));
        *val = i;
        dsc_sll_push_back(list, val);
    }

    DSCIterator iter = dsc_sll_iterator(list);
    ASSERT(iter.is_valid(&iter));

    // Test forward iteration
    int expected = 1;
    while (iter.has_next(&iter))
    {
        const int* val = (int*)iter.get(&iter);
        ASSERT_NOT_NULL(val);
        ASSERT_EQ(*val, expected);
        expected++;
        iter.next(&iter);
    }

    ASSERT_EQ(expected, 6); // Should have iterated through all 5 elements
    ASSERT(!iter.has_next(&iter));

    iter.destroy(&iter);
    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

int test_iterator_get(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);

    // Add numbers 1-3
    for (int i = 1; i <= 3; i++)
    {
        int* val = malloc(sizeof(int));
        *val = i;
        dsc_sll_push_back(list, val);
    }

    DSCIterator iter = dsc_sll_iterator(list);

    // Test get without advancing
    const int* val = iter.get(&iter);
    ASSERT_NOT_NULL(val);
    ASSERT_EQ(*val, 1);

    // Get again - should return same value
    val = iter.get(&iter);
    ASSERT_EQ(*val, 1);

    // Now advance and test get
    iter.next(&iter);
    val = iter.get(&iter);
    ASSERT_EQ(*val, 2);

    iter.destroy(&iter);
    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

int test_iterator_reset(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);

    // Add numbers 1-3
    for (int i = 1; i <= 3; i++)
    {
        int* val = malloc(sizeof(int));
        *val = i;
        dsc_sll_push_back(list, val);
    }

    DSCIterator iter = dsc_sll_iterator(list);

    // Advance iterator
    iter.next(&iter);
    iter.next(&iter);

    // Reset and verify back at beginning
    iter.reset(&iter);
    const int* val = iter.get(&iter);
    ASSERT_EQ(*val, 1);
    ASSERT(iter.has_next(&iter));

    iter.destroy(&iter);
    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

int test_iterator_empty_list(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);

    DSCIterator iter = dsc_sll_iterator(list);
    ASSERT(iter.is_valid(&iter));
    ASSERT(!iter.has_next(&iter));
    ASSERT(!iter.has_prev(&iter));   // SLL doesn't support prev
    ASSERT_EQ(iter.next(&iter), -1); // Should return error code
    ASSERT_NULL(iter.get(&iter));

    iter.destroy(&iter);
    dsc_sll_destroy(list, false);
    return TEST_SUCCESS;
}

int test_iterator_single_element(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);

    int* val = malloc(sizeof(int));
    *val = 42;
    dsc_sll_push_back(list, val);

    DSCIterator iter = dsc_sll_iterator(list);

    ASSERT(iter.has_next(&iter));
    ASSERT(!iter.has_prev(&iter)); // SLL doesn't support prev

    const int* retrieved = iter.get(&iter);
    ASSERT_EQ(*retrieved, 42);

    iter.next(&iter);
    ASSERT(!iter.has_next(&iter));
    ASSERT(!iter.has_prev(&iter)); // SLL doesn't support prev

    iter.destroy(&iter);
    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

// Test creating singly linked list from iterator
int test_from_iterator(void)
{
    DSCAllocator alloc = create_int_allocator();

    // Create a range iterator (0, 1, 2, 3, 4)
    DSCIterator range_it = dsc_iterator_range(0, 5, 1, &alloc);

    // Create singly linked list from iterator
    DSCSinglyLinkedList* list = dsc_sll_from_iterator(&range_it, &alloc, true);
    ASSERT_NOT_NULL(list);
    ASSERT_EQ(dsc_sll_size(list), 5);

    // Clean up the iterator immediately after use
    range_it.destroy(&range_it);

    // Verify singly linked list has correct values in sequential order
    // Iterator gives 0,1,2,3,4 and singly linked list should have them as 0,1,2,3,4 (head to tail)
    const DSCSinglyLinkedNode* node = list->head;
    for (int expected = 0; expected < 5; expected++)
    {
        ASSERT_NOT_NULL(node);
        ASSERT_EQ(*(int*)node->data, expected);
        node = node->next;
    }

    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

// Test iterator with invalid singly linked list
int test_iterator_invalid(void)
{
    const DSCIterator iter = dsc_sll_iterator(NULL);
    ASSERT(!iter.is_valid(&iter));
    return TEST_SUCCESS;
}

// Test iterator state after singly linked list modifications
int test_iterator_modification(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);

    // Add initial data
    for (int i = 0; i < 3; i++)
    {
        int* data = malloc(sizeof(int));
        *data = i * 10;
        ASSERT_EQ(dsc_sll_push_back(list, data), 0);
    }

    DSCIterator iter = dsc_sll_iterator(list);
    ASSERT(iter.is_valid(&iter));

    // Get first element
    void* first = iter.get(&iter);
    ASSERT_EQ(*(int*)first, 0); // Should be first element (0*10)
    iter.next(&iter);

    // Modify singly linked list while iterator exists (implementation detail: iterator may become invalid)
    int* new_data = malloc(sizeof(int));
    *new_data = 999;
    ASSERT_EQ(dsc_sll_push_back(list, new_data), 0);

    // Iterator should still be valid but may not reflect new state
    ASSERT(iter.is_valid(&iter));

    iter.destroy(&iter);
    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

// Test copy isolation - verify that copied elements are independent
int test_sll_copy_isolation(void)
{
    DSCAllocator alloc = create_int_allocator();

    // Create original data that we can modify
    const int original_values[] = {10, 20, 30};
    int* data_ptrs[3];

    // Create a source singly linked list
    DSCSinglyLinkedList* source_list = dsc_sll_create(&alloc);
    ASSERT_NOT_NULL(source_list);

    for (int i = 0; i < 3; i++)
    {
        data_ptrs[i] = malloc(sizeof(int));
        *data_ptrs[i] = original_values[i];
        ASSERT_EQ(dsc_sll_push_back(source_list, data_ptrs[i]), 0);
    }

    DSCIterator list_it = dsc_sll_iterator(source_list);
    ASSERT(list_it.is_valid(&list_it));

    // Create singly linked list with copying enabled
    DSCSinglyLinkedList* new_list = dsc_sll_from_iterator(&list_it, &alloc, true);
    ASSERT_NOT_NULL(new_list);
    ASSERT_EQ(dsc_sll_size(new_list), 3);

    // Modify original data
    *data_ptrs[0] = 999;
    *data_ptrs[1] = 888;
    *data_ptrs[2] = 777;

    // SinglyLinkedList should still have original values (proving data was copied)
    // Sequential order: 10, 20, 30
    const DSCSinglyLinkedNode* node = new_list->head;
    ASSERT_NOT_NULL(node);
    ASSERT_EQ(*(int*)node->data, 10); // Should be unchanged

    node = node->next;
    ASSERT_NOT_NULL(node);
    ASSERT_EQ(*(int*)node->data, 20); // Should be unchanged

    node = node->next;
    ASSERT_NOT_NULL(node);
    ASSERT_EQ(*(int*)node->data, 30); // Should be unchanged

    // Cleanup
    list_it.destroy(&list_it);
    dsc_sll_destroy(new_list, true);
    dsc_sll_destroy(source_list, true);

    return TEST_SUCCESS;
}

// Test that should_copy=true fails when allocator has no copy function
int test_sll_copy_function_required(void)
{
    DSCAllocator alloc = dsc_alloc_default();
    alloc.copy_func = NULL;

    DSCIterator range_it = dsc_iterator_range(0, 3, 1, &alloc);
    ASSERT(range_it.is_valid(&range_it));

    // Should return NULL because should_copy=true but no copy function available
    DSCSinglyLinkedList* list = dsc_sll_from_iterator(&range_it, &alloc, true);
    ASSERT_NULL(list);

    range_it.destroy(&range_it);
    return TEST_SUCCESS;
}

// Test that should_copy=false uses elements directly without copying
int test_sll_from_iterator_no_copy(void)
{
    DSCAllocator alloc = create_int_allocator();

    // Create a range iterator and then a copy iterator to get actual owned data
    DSCIterator range_it = dsc_iterator_range(0, 3, 1, &alloc);
    ASSERT(range_it.is_valid(&range_it));

    // Use copy iterator to create actual data elements that we own
    DSCIterator copy_it = dsc_iterator_copy(&range_it, &alloc, int_copy);
    ASSERT(copy_it.is_valid(&copy_it));

    // Create singly linked list without copying (should_copy = false)
    // This will use the copied elements directly from the copy iterator
    DSCSinglyLinkedList* list = dsc_sll_from_iterator(&copy_it, &alloc, false);
    ASSERT_NOT_NULL(list);
    ASSERT_EQ(dsc_sll_size(list), 3);

    // Verify values are correct (sequential order: 0, 1, 2)
    const DSCSinglyLinkedNode* node = list->head;
    ASSERT_NOT_NULL(node);
    ASSERT_EQ(*(int*)node->data, 0);

    node = node->next;
    ASSERT_NOT_NULL(node);
    ASSERT_EQ(*(int*)node->data, 1);

    node = node->next;
    ASSERT_NOT_NULL(node);
    ASSERT_EQ(*(int*)node->data, 2);

    range_it.destroy(&range_it);
    copy_it.destroy(&copy_it);
    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

// Test that iterator is exhausted after being consumed by dsc_sll_from_iterator
int test_iterator_exhaustion_after_sll_creation(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCIterator range_it = dsc_iterator_range(0, 5, 1, &alloc);
    ASSERT(range_it.is_valid(&range_it));

    // Verify iterator starts with elements
    ASSERT(range_it.has_next(&range_it));

    // Create singly linked list from iterator (consumes all elements)
    DSCSinglyLinkedList* list = dsc_sll_from_iterator(&range_it, &alloc, true);
    ASSERT_NOT_NULL(list);
    ASSERT_EQ(dsc_sll_size(list), 5);

    // Iterator should now be exhausted
    ASSERT(!range_it.has_next(&range_it));
    ASSERT_NULL(range_it.get(&range_it));
    ASSERT_EQ(range_it.next(&range_it), -1); // Should fail to advance

    // But iterator should still be valid
    ASSERT(range_it.is_valid(&range_it));

    range_it.destroy(&range_it);
    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

// Test next() return values for proper error handling
int test_sll_iterator_next_return_values(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);
    ASSERT_NOT_NULL(list);

    // Add single element
    int* data = malloc(sizeof(int));
    *data = 42;
    ASSERT_EQ(dsc_sll_push_back(list, data), 0);

    DSCIterator iter = dsc_sll_iterator(list);
    ASSERT(iter.is_valid(&iter));

    // Should successfully advance once
    ASSERT(iter.has_next(&iter));
    ASSERT_EQ(iter.next(&iter), 0); // Success

    // Should fail to advance when exhausted
    ASSERT(!iter.has_next(&iter));
    ASSERT_EQ(iter.next(&iter), -1); // Failure

    // Additional calls should continue to fail
    ASSERT_EQ(iter.next(&iter), -1); // Still failure
    ASSERT(!iter.has_next(&iter));   // Still no elements

    iter.destroy(&iter);
    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

// Test various combinations of get/next/has_next calls for consistency
int test_sll_iterator_mixed_operations(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);
    ASSERT_NOT_NULL(list);

    // Add test data (will be in sequential order: 0, 10, 20)
    for (int i = 0; i < 3; i++)
    {
        int* data = malloc(sizeof(int));
        *data = i * 10;
        ASSERT_EQ(dsc_sll_push_back(list, data), 0);
    }

    DSCIterator iter = dsc_sll_iterator(list);
    ASSERT(iter.is_valid(&iter));

    // Multiple get() calls should return same value
    void* data1 = iter.get(&iter);
    void* data2 = iter.get(&iter);
    ASSERT_NOT_NULL(data1);
    ASSERT_NOT_NULL(data2);
    ASSERT_EQ(data1, data2);               // Same pointer
    ASSERT_EQ(*(int*)data1, *(int*)data2); // Same value
    ASSERT_EQ(*(int*)data1, 0);            // First element should be 0

    // has_next should be consistent
    ASSERT(iter.has_next(&iter));
    ASSERT(iter.has_next(&iter)); // Multiple calls should be safe

    // Advance and verify new position
    ASSERT_EQ(iter.next(&iter), 0);
    void* data3 = iter.get(&iter);
    ASSERT_NOT_NULL(data3);
    // Note: data1 and data3 point to different singly linked list elements
    ASSERT_NOT_EQ(*(int*)data1, *(int*)data3); // Different values
    ASSERT_EQ(*(int*)data3, 10);               // Next element should be 10

    // Verify we can still advance
    ASSERT(iter.has_next(&iter));
    ASSERT_EQ(iter.next(&iter), 0);

    void* data4 = iter.get(&iter);
    ASSERT_NOT_NULL(data4);
    ASSERT_EQ(*(int*)data4, 20); // Last element should be 20

    // Now should be at end
    ASSERT_EQ(iter.next(&iter), 0); // Advance past last element
    ASSERT(!iter.has_next(&iter));
    ASSERT_NULL(iter.get(&iter));

    iter.destroy(&iter);
    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

// Test unsupported prev operations (SLL only supports forward iteration)
int test_sll_iterator_unsupported_operations(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);

    // Add test data
    for (int i = 1; i <= 3; i++)
    {
        int* data = malloc(sizeof(int));
        *data = i;
        ASSERT_EQ(dsc_sll_push_back(list, data), 0);
    }

    DSCIterator iter = dsc_sll_iterator(list);
    ASSERT(iter.is_valid(&iter));

    // SLL iterator should not support bidirectional operations
    ASSERT(!iter.has_prev(&iter));
    ASSERT_EQ(iter.prev(&iter), -1); // Returns -1 for unsupported

    // Advance and test again
    iter.next(&iter);
    ASSERT(!iter.has_prev(&iter));
    ASSERT_EQ(iter.prev(&iter), -1); // Still unsupported

    // Reset should work
    iter.reset(&iter);

    // Should still be valid after unsupported operations
    ASSERT(iter.is_valid(&iter));

    iter.destroy(&iter);
    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

// Test iterator traversal order
int test_sll_iterator_order(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);

    // Add elements in specific order
    const int values[] = {100, 200, 300, 400, 500};
    for (int i = 0; i < 5; i++)
    {
        int* data = malloc(sizeof(int));
        *data = values[i];
        ASSERT_EQ(dsc_sll_push_back(list, data), 0);
    }

    // Create iterator and verify order
    DSCIterator iter = dsc_sll_iterator(list);

    for (int i = 0; i < 5; i++)
    {
        ASSERT(iter.has_next(&iter));
        void* data = iter.get(&iter);
        ASSERT_NOT_NULL(data);
        ASSERT_EQ(*(int*)data, values[i]);
        iter.next(&iter);
    }

    ASSERT(!iter.has_next(&iter));

    iter.destroy(&iter);
    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

// Test multiple iterators on the same list
int test_multiple_iterators(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);

    for (int i = 0; i < 5; i++)
    {
        int* val = malloc(sizeof(int));
        *val = i;
        dsc_sll_push_back(list, val);
    }

    // Create two iterators on the same list
    DSCIterator iter1 = dsc_sll_iterator(list);
    DSCIterator iter2 = dsc_sll_iterator(list);

    // Advance first iterator by 2
    iter1.next(&iter1);
    iter1.next(&iter1);

    // Second iterator should still be at the beginning
    ASSERT_EQ(*(int*)iter2.get(&iter2), 0);

    // Both iterators should be independent
    ASSERT_EQ(*(int*)iter1.get(&iter1), 2);
    iter2.next(&iter2);
    ASSERT_EQ(*(int*)iter2.get(&iter2), 1);

    // Advance both and check
    iter1.next(&iter1);
    ASSERT_EQ(*(int*)iter1.get(&iter1), 3);
    iter2.next(&iter2);
    ASSERT_EQ(*(int*)iter2.get(&iter2), 2);

    iter1.destroy(&iter1);
    iter2.destroy(&iter2);
    dsc_sll_destroy(list, true);
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
        {test_forward_iterator, "test_forward_iterator"},
        {test_iterator_get, "test_iterator_get"},
        {test_iterator_reset, "test_iterator_reset"},
        {test_iterator_empty_list, "test_iterator_empty_list"},
        {test_iterator_single_element, "test_iterator_single_element"},
        {test_from_iterator, "test_from_iterator"},
        {test_iterator_invalid, "test_iterator_invalid"},
        {test_iterator_modification, "test_iterator_modification"},
        {test_sll_copy_isolation, "test_sll_copy_isolation"},
        {test_sll_copy_function_required, "test_sll_copy_function_required"},
        {test_sll_from_iterator_no_copy, "test_sll_from_iterator_no_copy"},
        {test_iterator_exhaustion_after_sll_creation, "test_iterator_exhaustion_after_sll_creation"},
        {test_sll_iterator_next_return_values, "test_sll_iterator_next_return_values"},
        {test_sll_iterator_mixed_operations, "test_sll_iterator_mixed_operations"},
        {test_sll_iterator_unsupported_operations, "test_sll_iterator_unsupported_operations"},
        {test_sll_iterator_order, "test_sll_iterator_order"},
        {test_multiple_iterators, "test_multiple_iterators"},
    };

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

    if (failed)
    {
        printf("%d test(s) failed.\n", failed);
        return 1;
    }

    printf("All singly linked list iterator tests passed!\n");
    return 0;
}
