//
// Created by zack on 9/15/25.
//
// Comprehensive test suite for zip iterator functionality.
// Tests cover basic iteration, edge cases, pair handling,
// error handling, and composition with other iterators.

#include "ArrayList.h"
#include "DoublyLinkedList.h"
#include "Iterator.h"
#include "Pair.h"
#include "Queue.h"
#include "Stack.h"
#include "TestAssert.h"
#include "TestHelpers.h"

#include <stdio.h>
#include <stdlib.h>

//==============================================================================
// Helper Functions for Zip Iterator Tests
//==============================================================================

/**
 * Helper function to collect all pairs from a zip iterator into arrays.
 * Returns the number of pairs collected.
 */
static int collect_pairs(const DSCIterator* it, int* first_values, int* second_values, const int max_count)
{
    int count = 0;
    while (it->has_next(it) && count < max_count)
    {
        const DSCPair* pair = it->get(it);
        if (pair && pair->first && pair->second)
        {
            first_values[count] = *(const int*)pair->first;
            second_values[count] = *(const int*)pair->second;
            count++;
        }
        it->next(it);
    }
    return count;
}

/**
 * Helper function to verify pairs match expected values.
 */
static int verify_pairs(const int* actual_first, const int* actual_second,
                       const int* expected_first, const int* expected_second,
                       const int count, const char* test_name)
{
    for (int i = 0; i < count; i++)
    {
        if (actual_first[i] != expected_first[i] || actual_second[i] != expected_second[i])
        {
            printf("FAIL: %s - Expected (%d,%d) at index %d, got (%d,%d)\n",
                   test_name, expected_first[i], expected_second[i], i,
                   actual_first[i], actual_second[i]);
            return 0;
        }
    }
    return 1;
}

//==============================================================================
// Basic Zip Iterator Tests
//==============================================================================

int test_zip_basic_functionality(void)
{
    const DSCAllocator alloc = create_int_allocator();

    // Create two range iterators
    DSCIterator range1 = dsc_iterator_range(1, 4, 1, &alloc);   // [1,2,3]
    DSCIterator range2 = dsc_iterator_range(10, 13, 1, &alloc); // [10,11,12]

    // Zip them together
    DSCIterator zip_it = dsc_iterator_zip(&range1, &range2, &alloc);
    ASSERT_TRUE(zip_it.is_valid(&zip_it));

    int first_values[10], second_values[10];
    const int count = collect_pairs(&zip_it, first_values, second_values, 10);

    ASSERT_EQ(count, 3);

    const int expected_first[] = {1, 2, 3};
    const int expected_second[] = {10, 11, 12};
    ASSERT_TRUE(verify_pairs(first_values, second_values, expected_first, expected_second, 3, "zip_basic"));

    zip_it.destroy(&zip_it);
    return TEST_SUCCESS;
}

int test_zip_different_lengths_first_shorter(void)
{
    const DSCAllocator alloc = create_int_allocator();

    // First iterator shorter: [1,2] vs [10,11,12,13]
    DSCIterator range1 = dsc_iterator_range(1, 3, 1, &alloc);   // [1,2]
    DSCIterator range2 = dsc_iterator_range(10, 14, 1, &alloc); // [10,11,12,13]

    DSCIterator zip_it = dsc_iterator_zip(&range1, &range2, &alloc);
    ASSERT_TRUE(zip_it.is_valid(&zip_it));

    int first_values[10], second_values[10];
    const int count = collect_pairs(&zip_it, first_values, second_values, 10);

    ASSERT_EQ(count, 2); // Should stop when first iterator exhausted

    const int expected_first[] = {1, 2};
    const int expected_second[] = {10, 11};
    ASSERT_TRUE(verify_pairs(first_values, second_values, expected_first, expected_second, 2, "zip_first_shorter"));

    zip_it.destroy(&zip_it);
    return TEST_SUCCESS;
}

int test_zip_different_lengths_second_shorter(void)
{
    const DSCAllocator alloc = create_int_allocator();

    // Second iterator shorter: [1,2,3,4] vs [10,11]
    DSCIterator range1 = dsc_iterator_range(1, 5, 1, &alloc);   // [1,2,3,4]
    DSCIterator range2 = dsc_iterator_range(10, 12, 1, &alloc); // [10,11]

    DSCIterator zip_it = dsc_iterator_zip(&range1, &range2, &alloc);
    ASSERT_TRUE(zip_it.is_valid(&zip_it));

    int first_values[10], second_values[10];
    const int count = collect_pairs(&zip_it, first_values, second_values, 10);

    ASSERT_EQ(count, 2); // Should stop when second iterator exhausted

    const int expected_first[] = {1, 2};
    const int expected_second[] = {10, 11};
    ASSERT_TRUE(verify_pairs(first_values, second_values, expected_first, expected_second, 2, "zip_second_shorter"));

    zip_it.destroy(&zip_it);
    return TEST_SUCCESS;
}

int test_zip_equal_length_single_elements(void)
{
    const DSCAllocator alloc = create_int_allocator();

    // Both iterators have single element
    DSCIterator range1 = dsc_iterator_range(42, 43, 1, &alloc); // [42]
    DSCIterator range2 = dsc_iterator_range(99, 100, 1, &alloc); // [99]

    DSCIterator zip_it = dsc_iterator_zip(&range1, &range2, &alloc);
    ASSERT_TRUE(zip_it.is_valid(&zip_it));

    ASSERT_TRUE(zip_it.has_next(&zip_it));
    const DSCPair* pair = zip_it.get(&zip_it);
    ASSERT_NOT_NULL(pair);
    ASSERT_NOT_NULL(pair->first);
    ASSERT_NOT_NULL(pair->second);
    ASSERT_EQ(*(const int*)pair->first, 42);
    ASSERT_EQ(*(const int*)pair->second, 99);

    ASSERT_EQ(zip_it.next(&zip_it), 0);
    ASSERT_FALSE(zip_it.has_next(&zip_it));
    ASSERT_NULL(zip_it.get(&zip_it));

    zip_it.destroy(&zip_it);
    return TEST_SUCCESS;
}

//==============================================================================
// Edge Cases and Error Handling
//==============================================================================

int test_zip_both_empty(void)
{
    const DSCAllocator alloc = create_int_allocator();

    // Both iterators empty
    DSCIterator range1 = dsc_iterator_range(1, 1, 1, &alloc); // Empty
    DSCIterator range2 = dsc_iterator_range(1, 1, 1, &alloc); // Empty

    DSCIterator zip_it = dsc_iterator_zip(&range1, &range2, &alloc);
    ASSERT_TRUE(zip_it.is_valid(&zip_it));

    // Should have no elements
    ASSERT_FALSE(zip_it.has_next(&zip_it));
    ASSERT_NULL(zip_it.get(&zip_it));

    zip_it.destroy(&zip_it);
    return TEST_SUCCESS;
}

int test_zip_one_empty_first(void)
{
    const DSCAllocator alloc = create_int_allocator();

    // First iterator empty, second has elements
    DSCIterator range1 = dsc_iterator_range(1, 1, 1, &alloc);   // Empty
    DSCIterator range2 = dsc_iterator_range(10, 13, 1, &alloc); // [10,11,12]

    DSCIterator zip_it = dsc_iterator_zip(&range1, &range2, &alloc);
    ASSERT_TRUE(zip_it.is_valid(&zip_it));

    // Should have no elements
    ASSERT_FALSE(zip_it.has_next(&zip_it));
    ASSERT_NULL(zip_it.get(&zip_it));

    zip_it.destroy(&zip_it);
    return TEST_SUCCESS;
}

int test_zip_one_empty_second(void)
{
    const DSCAllocator alloc = create_int_allocator();

    // First iterator has elements, second empty
    DSCIterator range1 = dsc_iterator_range(1, 4, 1, &alloc);   // [1,2,3]
    DSCIterator range2 = dsc_iterator_range(1, 1, 1, &alloc);   // Empty

    DSCIterator zip_it = dsc_iterator_zip(&range1, &range2, &alloc);
    ASSERT_TRUE(zip_it.is_valid(&zip_it));

    // Should have no elements
    ASSERT_FALSE(zip_it.has_next(&zip_it));
    ASSERT_NULL(zip_it.get(&zip_it));

    zip_it.destroy(&zip_it);
    return TEST_SUCCESS;
}

int test_zip_invalid_parameters(void)
{
    const DSCAllocator alloc = create_int_allocator();

    // Test with NULL first iterator
    DSCIterator range2 = dsc_iterator_range(1, 4, 1, &alloc);
    const DSCIterator zip_it1 = dsc_iterator_zip(NULL, &range2, &alloc);
    ASSERT_FALSE(zip_it1.is_valid(&zip_it1));
    range2.destroy(&range2); // Clean up since zip failed

    // Test with NULL second iterator
    DSCIterator range1 = dsc_iterator_range(1, 4, 1, &alloc);
    const DSCIterator zip_it2 = dsc_iterator_zip(&range1, NULL, &alloc);
    ASSERT_FALSE(zip_it2.is_valid(&zip_it2));
    range1.destroy(&range1); // Clean up since zip failed

    // Test with NULL allocator
    DSCIterator range3 = dsc_iterator_range(1, 4, 1, &alloc);
    DSCIterator range4 = dsc_iterator_range(1, 4, 1, &alloc);
    const DSCIterator zip_it3 = dsc_iterator_zip(&range3, &range4, NULL);
    ASSERT_FALSE(zip_it3.is_valid(&zip_it3));
    range3.destroy(&range3); // Clean up since zip failed
    range4.destroy(&range4); // Clean up since zip failed

    return TEST_SUCCESS;
}

//==============================================================================
// Iterator Composition Tests
//==============================================================================

int test_zip_with_filter(void)
{
    const DSCAllocator alloc = create_int_allocator();

    // Create range 1-6, filter evens from first, zip with 10-15
    DSCIterator range1 = dsc_iterator_range(1, 7, 1, &alloc);   // [1,2,3,4,5,6]
    DSCIterator filter_it = dsc_iterator_filter(&range1, &alloc, is_even); // [2,4,6]
    DSCIterator range2 = dsc_iterator_range(10, 16, 1, &alloc); // [10,11,12,13,14,15]

    DSCIterator zip_it = dsc_iterator_zip(&filter_it, &range2, &alloc);
    ASSERT_TRUE(zip_it.is_valid(&zip_it));

    int first_values[10], second_values[10];
    const int count = collect_pairs(&zip_it, first_values, second_values, 10);

    ASSERT_EQ(count, 3);

    const int expected_first[] = {2, 4, 6};
    const int expected_second[] = {10, 11, 12};
    ASSERT_TRUE(verify_pairs(first_values, second_values, expected_first, expected_second, 3, "zip_with_filter"));

    zip_it.destroy(&zip_it);
    return TEST_SUCCESS;
}

int test_zip_with_take(void)
{
    const DSCAllocator alloc = create_int_allocator();

    // Create ranges, take first 2 from each, then zip
    DSCIterator range1 = dsc_iterator_range(1, 10, 1, &alloc);  // [1,2,3,4,5,6,7,8,9]
    DSCIterator take_it1 = dsc_iterator_take(&range1, &alloc, 2); // [1,2]

    DSCIterator range2 = dsc_iterator_range(20, 30, 1, &alloc); // [20,21,22,23,24,25,26,27,28,29]
    DSCIterator take_it2 = dsc_iterator_take(&range2, &alloc, 2); // [20,21]

    DSCIterator zip_it = dsc_iterator_zip(&take_it1, &take_it2, &alloc);
    ASSERT_TRUE(zip_it.is_valid(&zip_it));

    int first_values[10], second_values[10];
    const int count = collect_pairs(&zip_it, first_values, second_values, 10);

    ASSERT_EQ(count, 2);

    const int expected_first[] = {1, 2};
    const int expected_second[] = {20, 21};
    ASSERT_TRUE(verify_pairs(first_values, second_values, expected_first, expected_second, 2, "zip_with_take"));

    zip_it.destroy(&zip_it);
    return TEST_SUCCESS;
}

int test_zip_with_skip(void)
{
    const DSCAllocator alloc = create_int_allocator();

    // Create ranges, skip first 2 from each, then zip
    DSCIterator range1 = dsc_iterator_range(1, 6, 1, &alloc);   // [1,2,3,4,5]
    DSCIterator skip_it1 = dsc_iterator_skip(&range1, &alloc, 2); // [3,4,5]

    DSCIterator range2 = dsc_iterator_range(10, 15, 1, &alloc); // [10,11,12,13,14]
    DSCIterator skip_it2 = dsc_iterator_skip(&range2, &alloc, 2); // [12,13,14]

    DSCIterator zip_it = dsc_iterator_zip(&skip_it1, &skip_it2, &alloc);
    ASSERT_TRUE(zip_it.is_valid(&zip_it));

    int first_values[10], second_values[10];
    const int count = collect_pairs(&zip_it, first_values, second_values, 10);

    ASSERT_EQ(count, 3);

    const int expected_first[] = {3, 4, 5};
    const int expected_second[] = {12, 13, 14};
    ASSERT_TRUE(verify_pairs(first_values, second_values, expected_first, expected_second, 3, "zip_with_skip"));

    zip_it.destroy(&zip_it);
    return TEST_SUCCESS;
}

int test_zip_nested(void)
{
    const DSCAllocator alloc = create_int_allocator();

    // Create nested zip: zip(1-3, 10-12) then zip that with 100-102
    DSCIterator range1 = dsc_iterator_range(1, 4, 1, &alloc);     // [1,2,3]
    DSCIterator range2 = dsc_iterator_range(10, 13, 1, &alloc);   // [10,11,12]
    DSCIterator zip_it1 = dsc_iterator_zip(&range1, &range2, &alloc); // [(1,10),(2,11),(3,12)]

    DSCIterator range3 = dsc_iterator_range(100, 103, 1, &alloc); // [100,101,102]
    DSCIterator zip_it2 = dsc_iterator_zip(&zip_it1, &range3, &alloc);

    ASSERT_TRUE(zip_it2.is_valid(&zip_it2));

    // Iterate and verify structure
    int count = 0;
    while (zip_it2.has_next(&zip_it2) && count < 3)
    {
        const DSCPair* outer_pair = zip_it2.get(&zip_it2);
        ASSERT_NOT_NULL(outer_pair);
        ASSERT_NOT_NULL(outer_pair->first);  // Should be a DSCPair*
        ASSERT_NOT_NULL(outer_pair->second); // Should be an int*

        const DSCPair* inner_pair = outer_pair->first;
        const int* third_value = outer_pair->second;

        ASSERT_NOT_NULL(inner_pair->first);
        ASSERT_NOT_NULL(inner_pair->second);

        const int first_val = *(const int*)inner_pair->first;
        const int second_val = *(const int*)inner_pair->second;
        const int third_val = *third_value;

        // Verify expected values
        ASSERT_EQ(first_val, 1 + count);
        ASSERT_EQ(second_val, 10 + count);
        ASSERT_EQ(third_val, 100 + count);

        zip_it2.next(&zip_it2);
        count++;
    }

    ASSERT_EQ(count, 3);
    ASSERT_FALSE(zip_it2.has_next(&zip_it2));

    zip_it2.destroy(&zip_it2);
    return TEST_SUCCESS;
}

//==============================================================================
// Behavior and State Tests
//==============================================================================

int test_zip_iteration_state(void)
{
    const DSCAllocator alloc = create_int_allocator();

    // Create zip iterator and test step-by-step iteration
    DSCIterator range1 = dsc_iterator_range(100, 103, 1, &alloc); // [100,101,102]
    DSCIterator range2 = dsc_iterator_range(200, 203, 1, &alloc); // [200,201,202]

    DSCIterator zip_it = dsc_iterator_zip(&range1, &range2, &alloc);

    // Test step-by-step iteration
    ASSERT_TRUE(zip_it.has_next(&zip_it));
    const DSCPair* pair1 = zip_it.get(&zip_it);
    ASSERT_NOT_NULL(pair1);
    ASSERT_EQ(*(const int*)pair1->first, 100);
    ASSERT_EQ(*(const int*)pair1->second, 200);

    ASSERT_EQ(zip_it.next(&zip_it), 0);
    ASSERT_TRUE(zip_it.has_next(&zip_it));

    const DSCPair* pair2 = zip_it.get(&zip_it);
    ASSERT_NOT_NULL(pair2);
    ASSERT_EQ(*(const int*)pair2->first, 101);
    ASSERT_EQ(*(const int*)pair2->second, 201);

    ASSERT_EQ(zip_it.next(&zip_it), 0);
    ASSERT_TRUE(zip_it.has_next(&zip_it));

    const DSCPair* pair3 = zip_it.get(&zip_it);
    ASSERT_NOT_NULL(pair3);
    ASSERT_EQ(*(const int*)pair3->first, 102);
    ASSERT_EQ(*(const int*)pair3->second, 202);

    ASSERT_EQ(zip_it.next(&zip_it), 0);
    ASSERT_FALSE(zip_it.has_next(&zip_it));
    ASSERT_NULL(zip_it.get(&zip_it));
    ASSERT_EQ(zip_it.next(&zip_it), -1);

    zip_it.destroy(&zip_it);
    return TEST_SUCCESS;
}

int test_zip_unsupported_operations(void)
{
    const DSCAllocator alloc = create_int_allocator();

    // Create zip iterator
    DSCIterator range1 = dsc_iterator_range(1, 4, 1, &alloc);
    DSCIterator range2 = dsc_iterator_range(10, 13, 1, &alloc);
    DSCIterator zip_it = dsc_iterator_zip(&range1, &range2, &alloc);

    // Test unsupported operations
    ASSERT_FALSE(zip_it.has_prev(&zip_it));
    ASSERT_EQ(zip_it.prev(&zip_it), -1);

    // Reset should be no-op (doesn't crash)
    zip_it.reset(&zip_it);

    zip_it.destroy(&zip_it);
    return TEST_SUCCESS;
}

int test_zip_pair_consistency(void)
{
    const DSCAllocator alloc = create_int_allocator();

    // Test that the same pair pointer is returned for multiple get() calls
    DSCIterator range1 = dsc_iterator_range(1, 3, 1, &alloc); // [1,2]
    DSCIterator range2 = dsc_iterator_range(10, 12, 1, &alloc); // [10,11]

    DSCIterator zip_it = dsc_iterator_zip(&range1, &range2, &alloc);

    ASSERT_TRUE(zip_it.has_next(&zip_it));

    const DSCPair* pair1 = zip_it.get(&zip_it);
    const DSCPair* pair2 = zip_it.get(&zip_it);

    // Should return same pointer (cached pair)
    ASSERT_EQ(pair1, pair2);
    ASSERT_EQ(*(const int*)pair1->first, 1);
    ASSERT_EQ(*(const int*)pair1->second, 10);

    // After next(), should get different values but could be same pointer
    zip_it.next(&zip_it);
    const DSCPair* pair3 = zip_it.get(&zip_it);
    ASSERT_EQ(*(const int*)pair3->first, 2);
    ASSERT_EQ(*(const int*)pair3->second, 11);

    zip_it.destroy(&zip_it);
    return TEST_SUCCESS;
}

//==============================================================================
// Data Structure Iterator Composition Tests
//==============================================================================

int test_zip_arraylist_iterators(void)
{
    DSCAllocator alloc = create_int_allocator();

    // Create two ArrayLists with different data
    DSCArrayList* list1 = dsc_arraylist_create(&alloc, 0);
    DSCArrayList* list2 = dsc_arraylist_create(&alloc, 0);

    // Populate list1 with 1, 2, 3
    for (int i = 1; i <= 3; i++)
    {
        int* val = malloc(sizeof(int));
        *val = i;
        dsc_arraylist_push_back(list1, val);
    }

    // Populate list2 with 10, 20, 30
    for (int i = 1; i <= 3; i++)
    {
        int* val = malloc(sizeof(int));
        *val = i * 10;
        dsc_arraylist_push_back(list2, val);
    }

    // Create iterators
    DSCIterator iter1 = dsc_arraylist_iterator(list1);
    DSCIterator iter2 = dsc_arraylist_iterator(list2);

    // Zip the iterators
    DSCIterator zip_it = dsc_iterator_zip(&iter1, &iter2, &alloc);
    ASSERT_TRUE(zip_it.is_valid(&zip_it));

    int first_values[10], second_values[10];
    const int count = collect_pairs(&zip_it, first_values, second_values, 10);

    ASSERT_EQ(count, 3);

    const int expected_first[] = {1, 2, 3};
    const int expected_second[] = {10, 20, 30};
    ASSERT_TRUE(verify_pairs(first_values, second_values, expected_first, expected_second, 3, "zip_arraylist"));

    zip_it.destroy(&zip_it);
    dsc_arraylist_destroy(list1, true);
    dsc_arraylist_destroy(list2, true);
    return TEST_SUCCESS;
}

int test_zip_dll_iterators(void)
{
    DSCAllocator alloc = create_int_allocator();

    // Create two DoublyLinkedLists
    DSCDoublyLinkedList* list1 = dsc_dll_create(&alloc);
    DSCDoublyLinkedList* list2 = dsc_dll_create(&alloc);

    // Populate list1 with 5, 6, 7, 8
    for (int i = 5; i <= 8; i++)
    {
        int* val = malloc(sizeof(int));
        *val = i;
        dsc_dll_insert_back(list1, val);
    }

    // Populate list2 with 50, 60 (shorter list)
    for (int i = 5; i <= 6; i++)
    {
        int* val = malloc(sizeof(int));
        *val = i * 10;
        dsc_dll_insert_back(list2, val);
    }

    // Create iterators
    DSCIterator iter1 = dsc_dll_iterator(list1);
    DSCIterator iter2 = dsc_dll_iterator(list2);

    // Zip the iterators
    DSCIterator zip_it = dsc_iterator_zip(&iter1, &iter2, &alloc);
    ASSERT_TRUE(zip_it.is_valid(&zip_it));

    int first_values[10], second_values[10];
    const int count = collect_pairs(&zip_it, first_values, second_values, 10);

    ASSERT_EQ(count, 2); // Limited by shorter list

    const int expected_first[] = {5, 6};
    const int expected_second[] = {50, 60};
    ASSERT_TRUE(verify_pairs(first_values, second_values, expected_first, expected_second, 2, "zip_dll"));

    zip_it.destroy(&zip_it);
    dsc_dll_destroy(list1, true);
    dsc_dll_destroy(list2, true);
    return TEST_SUCCESS;
}

int test_zip_arraylist_with_dll(void)
{
    DSCAllocator alloc = create_int_allocator();

    // Create ArrayList and DoublyLinkedList
    DSCArrayList* arraylist = dsc_arraylist_create(&alloc, 0);
    DSCDoublyLinkedList* dll = dsc_dll_create(&alloc);

    // Populate ArrayList with 100, 200, 300, 400
    for (int i = 1; i <= 4; i++)
    {
        int* val = malloc(sizeof(int));
        *val = i * 100;
        dsc_arraylist_push_back(arraylist, val);
    }

    // Populate DLL with 1, 2, 3
    for (int i = 1; i <= 3; i++)
    {
        int* val = malloc(sizeof(int));
        *val = i;
        dsc_dll_insert_back(dll, val);
    }

    // Create iterators
    DSCIterator array_iter = dsc_arraylist_iterator(arraylist);
    DSCIterator dll_iter = dsc_dll_iterator(dll);

    // Zip different data structure iterators
    DSCIterator zip_it = dsc_iterator_zip(&array_iter, &dll_iter, &alloc);
    ASSERT_TRUE(zip_it.is_valid(&zip_it));

    int first_values[10], second_values[10];
    const int count = collect_pairs(&zip_it, first_values, second_values, 10);

    ASSERT_EQ(count, 3); // Limited by DLL

    const int expected_first[] = {100, 200, 300};
    const int expected_second[] = {1, 2, 3};
    ASSERT_TRUE(verify_pairs(first_values, second_values, expected_first, expected_second, 3, "zip_arraylist_dll"));

    zip_it.destroy(&zip_it);
    dsc_arraylist_destroy(arraylist, true);
    dsc_dll_destroy(dll, true);
    return TEST_SUCCESS;
}

int test_zip_queue_with_stack(void)
{
    DSCAllocator alloc = create_int_allocator();

    // Create Queue and Stack
    DSCQueue* queue = dsc_queue_create(&alloc);
    DSCStack* stack = dsc_stack_create(&alloc);

    // Populate queue with 1, 2, 3, 4 (FIFO: will iterate as 1, 2, 3, 4)
    for (int i = 1; i <= 4; i++)
    {
        int* val = malloc(sizeof(int));
        *val = i;
        dsc_queue_enqueue(queue, val);
    }

    // Populate stack with 10, 20, 30, 40 (LIFO: will iterate as 40, 30, 20, 10)
    for (int i = 1; i <= 4; i++)
    {
        int* val = malloc(sizeof(int));
        *val = i * 10;
        dsc_stack_push(stack, val);
    }

    // Create iterators
    DSCIterator queue_iter = dsc_queue_iterator(queue);
    DSCIterator stack_iter = dsc_stack_iterator(stack);

    // Zip queue and stack iterators
    DSCIterator zip_it = dsc_iterator_zip(&queue_iter, &stack_iter, &alloc);
    ASSERT_TRUE(zip_it.is_valid(&zip_it));

    int first_values[10], second_values[10];
    const int count = collect_pairs(&zip_it, first_values, second_values, 10);

    ASSERT_EQ(count, 4);

    // Queue: FIFO order (1, 2, 3, 4), Stack: LIFO order (40, 30, 20, 10)
    const int expected_first[] = {1, 2, 3, 4};
    const int expected_second[] = {40, 30, 20, 10};
    ASSERT_TRUE(verify_pairs(first_values, second_values, expected_first, expected_second, 4, "zip_queue_stack"));

    zip_it.destroy(&zip_it);
    dsc_queue_destroy(queue, true);
    dsc_stack_destroy(stack, true);
    return TEST_SUCCESS;
}

int test_zip_range_with_arraylist(void)
{
    DSCAllocator alloc = create_int_allocator();

    // Create range iterator and ArrayList
    DSCIterator range_iter = dsc_iterator_range(100, 105, 1, &alloc); // [100, 101, 102, 103, 104]

    DSCArrayList* list = dsc_arraylist_create(&alloc, 0);
    // Populate with characters as integers: 'a', 'b', 'c'
    for (int i = 0; i < 3; i++)
    {
        int* val = malloc(sizeof(int));
        *val = 'a' + i;
        dsc_arraylist_push_back(list, val);
    }

    DSCIterator array_iter = dsc_arraylist_iterator(list);

    // Zip range with ArrayList
    DSCIterator zip_it = dsc_iterator_zip(&range_iter, &array_iter, &alloc);
    ASSERT_TRUE(zip_it.is_valid(&zip_it));

    int first_values[10], second_values[10];
    const int count = collect_pairs(&zip_it, first_values, second_values, 10);

    ASSERT_EQ(count, 3); // Limited by ArrayList

    const int expected_first[] = {100, 101, 102};
    const int expected_second[] = {'a', 'b', 'c'};
    ASSERT_TRUE(verify_pairs(first_values, second_values, expected_first, expected_second, 3, "zip_range_arraylist"));

    zip_it.destroy(&zip_it);
    dsc_arraylist_destroy(list, true);
    return TEST_SUCCESS;
}

int test_zip_filtered_data_structures(void)
{
    DSCAllocator alloc = create_int_allocator();

    // Create ArrayList with numbers 1-10
    DSCArrayList* list1 = dsc_arraylist_create(&alloc, 0);
    for (int i = 1; i <= 10; i++)
    {
        int* val = malloc(sizeof(int));
        *val = i;
        dsc_arraylist_push_back(list1, val);
    }

    // Create DLL with numbers 11-20
    DSCDoublyLinkedList* list2 = dsc_dll_create(&alloc);
    for (int i = 11; i <= 20; i++)
    {
        int* val = malloc(sizeof(int));
        *val = i;
        dsc_dll_insert_back(list2, val);
    }

    // Create iterators and filter them
    DSCIterator array_iter = dsc_arraylist_iterator(list1);
    DSCIterator filtered_array = dsc_iterator_filter(&array_iter, &alloc, is_even); // [2, 4, 6, 8, 10]

    DSCIterator dll_iter = dsc_dll_iterator(list2);
    DSCIterator filtered_dll = dsc_iterator_filter(&dll_iter, &alloc, is_odd); // [11, 13, 15, 17, 19]

    // Zip the filtered iterators
    DSCIterator zip_it = dsc_iterator_zip(&filtered_array, &filtered_dll, &alloc);
    ASSERT_TRUE(zip_it.is_valid(&zip_it));

    int first_values[10], second_values[10];
    const int count = collect_pairs(&zip_it, first_values, second_values, 10);

    ASSERT_EQ(count, 5);

    const int expected_first[] = {2, 4, 6, 8, 10};
    const int expected_second[] = {11, 13, 15, 17, 19};
    ASSERT_TRUE(verify_pairs(first_values, second_values, expected_first, expected_second, 5, "zip_filtered_structures"));

    zip_it.destroy(&zip_it);
    dsc_arraylist_destroy(list1, true);
    dsc_dll_destroy(list2, true);
    return TEST_SUCCESS;
}

int test_zip_complex_composition(void)
{
    DSCAllocator alloc = create_int_allocator();

    // Create a complex composition: Range -> Skip -> Take vs ArrayList -> Filter
    DSCIterator range_iter = dsc_iterator_range(1, 20, 1, &alloc); // [1..19]
    DSCIterator skip_iter = dsc_iterator_skip(&range_iter, &alloc, 5); // [6..19]
    DSCIterator take_iter = dsc_iterator_take(&skip_iter, &alloc, 3); // [6, 7, 8]

    // Create ArrayList with multiples of 2
    DSCArrayList* list = dsc_arraylist_create(&alloc, 0);
    for (int i = 1; i <= 10; i++)
    {
        int* val = malloc(sizeof(int));
        *val = i * 2; // [2, 4, 6, 8, 10, 12, 14, 16, 18, 20]
        dsc_arraylist_push_back(list, val);
    }

    DSCIterator array_iter = dsc_arraylist_iterator(list);
    DSCIterator filtered_array = dsc_iterator_filter(&array_iter, &alloc, is_greater_than_10); // [12, 14, 16, 18, 20]

    // Zip the complex compositions
    DSCIterator zip_it = dsc_iterator_zip(&take_iter, &filtered_array, &alloc);
    ASSERT_TRUE(zip_it.is_valid(&zip_it));

    int first_values[10], second_values[10];
    const int count = collect_pairs(&zip_it, first_values, second_values, 10);

    ASSERT_EQ(count, 3); // Limited by take_iter

    const int expected_first[] = {6, 7, 8};
    const int expected_second[] = {12, 14, 16};
    ASSERT_TRUE(verify_pairs(first_values, second_values, expected_first, expected_second, 3, "zip_complex_composition"));

    zip_it.destroy(&zip_it);
    dsc_arraylist_destroy(list, true);
    return TEST_SUCCESS;
}

//==============================================================================
// Main Test Runner
//==============================================================================

int main(void)
{
    printf("Running zip iterator tests...\n");

    int (*tests[])(void) = {
        test_zip_basic_functionality,
        test_zip_different_lengths_first_shorter,
        test_zip_different_lengths_second_shorter,
        test_zip_equal_length_single_elements,
        test_zip_both_empty,
        test_zip_one_empty_first,
        test_zip_one_empty_second,
        test_zip_invalid_parameters,
        test_zip_with_filter,
        test_zip_with_take,
        test_zip_with_skip,
        test_zip_nested,
        test_zip_iteration_state,
        test_zip_unsupported_operations,
        test_zip_pair_consistency,
        test_zip_arraylist_iterators,
        test_zip_dll_iterators,
        test_zip_arraylist_with_dll,
        test_zip_queue_with_stack,
        test_zip_range_with_arraylist,
        test_zip_filtered_data_structures,
        test_zip_complex_composition,
    };

    const char* test_names[] = {
        "test_zip_basic_functionality",
        "test_zip_different_lengths_first_shorter",
        "test_zip_different_lengths_second_shorter",
        "test_zip_equal_length_single_elements",
        "test_zip_both_empty",
        "test_zip_one_empty_first",
        "test_zip_one_empty_second",
        "test_zip_invalid_parameters",
        "test_zip_with_filter",
        "test_zip_with_take",
        "test_zip_with_skip",
        "test_zip_nested",
        "test_zip_iteration_state",
        "test_zip_unsupported_operations",
        "test_zip_pair_consistency",
        "test_zip_arraylist_iterators",
        "test_zip_dll_iterators",
        "test_zip_arraylist_with_dll",
        "test_zip_queue_with_stack",
        "test_zip_range_with_arraylist",
        "test_zip_filtered_data_structures",
        "test_zip_complex_composition",
    };

    const int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    for (int i = 0; i < num_tests; i++) {
        printf("Running %s... ", test_names[i]);
        if (tests[i]() == TEST_SUCCESS) {
            printf("PASSED\n");
            passed++;
        } else {
            printf("FAILED\n");
        }
    }

    printf("Zip iterator tests: %d/%d passed\n", passed, num_tests);
    return (passed == num_tests) ? 0 : 1;
}
