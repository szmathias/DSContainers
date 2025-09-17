//
// Created by zack on 9/16/25.
//
// Comprehensive test suite for chain iterator functionality.
// Tests cover basic iteration, edge cases, multiple iterator chaining,
// error handling, and composition with other iterators.

#include "Iterator.h"
#include "TestAssert.h"
#include "TestHelpers.h"

#include <stdio.h>
#include <stdlib.h>

//==============================================================================
// Helper Functions for Chain Iterator Tests
//==============================================================================

/**
 * Helper function to collect all values from an iterator into an array.
 * Returns the number of values collected.
 */
static int collect_values(const DSCIterator* it, int* values, const int max_count)
{
    int count = 0;
    while (it->has_next(it) && count < max_count)
    {
        const int* value = it->get(it);
        if (value)
        {
            values[count++] = *value;
        }
        it->next(it);
    }
    return count;
}

/**
 * Helper function to verify an array matches expected values.
 */
static int verify_values(const int* actual, const int* expected, const int count, const char* test_name)
{
    for (int i = 0; i < count; i++)
    {
        if (actual[i] != expected[i])
        {
            printf("FAIL: %s - Expected %d at index %d, got %d\n",
                   test_name, expected[i], i, actual[i]);
            return 0;
        }
    }
    return 1;
}

//==============================================================================
// Basic Chain Iterator Tests
//==============================================================================

static int test_chain_basic_functionality(void)
{
    const DSCAllocator alloc = create_int_allocator();

    // Create two range iterators to chain
    DSCIterator range1 = dsc_iterator_range(1, 4, 1, &alloc);   // [1,2,3]
    DSCIterator range2 = dsc_iterator_range(10, 13, 1, &alloc); // [10,11,12]

    // Create array of iterators to chain
    DSCIterator iterators[] = {range1, range2};
    DSCIterator chain_it = dsc_iterator_chain(iterators, 2, &alloc);
    ASSERT_TRUE(chain_it.is_valid(&chain_it));

    int values[10];
    const int count = collect_values(&chain_it, values, 10);

    ASSERT_EQ(count, 6);

    // Should get all elements from first iterator, then all from second
    const int expected[] = {1, 2, 3, 10, 11, 12};
    ASSERT_TRUE(verify_values(values, expected, 6, "chain_basic"));

    chain_it.destroy(&chain_it);
    return TEST_SUCCESS;
}

static int test_chain_single_iterator(void)
{
    const DSCAllocator alloc = create_int_allocator();

    // Chain a single iterator
    DSCIterator range1 = dsc_iterator_range(5, 8, 1, &alloc); // [5,6,7]

    DSCIterator iterators[] = {range1};
    DSCIterator chain_it = dsc_iterator_chain(iterators, 1, &alloc);
    ASSERT_TRUE(chain_it.is_valid(&chain_it));

    int values[10];
    const int count = collect_values(&chain_it, values, 10);

    ASSERT_EQ(count, 3);

    const int expected[] = {5, 6, 7};
    ASSERT_TRUE(verify_values(values, expected, 3, "chain_single"));

    chain_it.destroy(&chain_it);
    return TEST_SUCCESS;
}

static int test_chain_empty_iterators(void)
{
    const DSCAllocator alloc = create_int_allocator();

    // Create empty range iterators (start == end)
    DSCIterator range1 = dsc_iterator_range(5, 5, 1, &alloc); // empty
    DSCIterator range2 = dsc_iterator_range(10, 10, 1, &alloc); // empty

    DSCIterator iterators[] = {range1, range2};
    DSCIterator chain_it = dsc_iterator_chain(iterators, 2, &alloc);
    ASSERT_TRUE(chain_it.is_valid(&chain_it));

    ASSERT_FALSE(chain_it.has_next(&chain_it));
    ASSERT_NULL(chain_it.get(&chain_it));

    chain_it.destroy(&chain_it);
    return TEST_SUCCESS;
}

static int test_chain_mixed_empty_and_non_empty(void)
{
    const DSCAllocator alloc = create_int_allocator();

    // Mix empty and non-empty iterators
    DSCIterator range1 = dsc_iterator_range(1, 1, 1, &alloc);   // empty
    DSCIterator range2 = dsc_iterator_range(5, 7, 1, &alloc);   // [5,6]
    DSCIterator range3 = dsc_iterator_range(10, 10, 1, &alloc); // empty
    DSCIterator range4 = dsc_iterator_range(20, 22, 1, &alloc); // [20,21]

    DSCIterator iterators[] = {range1, range2, range3, range4};
    DSCIterator chain_it = dsc_iterator_chain(iterators, 4, &alloc);
    ASSERT_TRUE(chain_it.is_valid(&chain_it));

    int values[10];
    const int count = collect_values(&chain_it, values, 10);

    ASSERT_EQ(count, 4);

    const int expected[] = {5, 6, 20, 21};
    ASSERT_TRUE(verify_values(values, expected, 4, "chain_mixed"));

    chain_it.destroy(&chain_it);
    return TEST_SUCCESS;
}

static int test_chain_multiple_iterators(void)
{
    const DSCAllocator alloc = create_int_allocator();

    // Chain multiple range iterators
    DSCIterator range1 = dsc_iterator_range(1, 3, 1, &alloc);   // [1,2]
    DSCIterator range2 = dsc_iterator_range(10, 12, 1, &alloc); // [10,11]
    DSCIterator range3 = dsc_iterator_range(20, 22, 1, &alloc); // [20,21]
    DSCIterator range4 = dsc_iterator_range(30, 32, 1, &alloc); // [30,31]

    DSCIterator iterators[] = {range1, range2, range3, range4};
    DSCIterator chain_it = dsc_iterator_chain(iterators, 4, &alloc);
    ASSERT_TRUE(chain_it.is_valid(&chain_it));

    int values[20];
    const int count = collect_values(&chain_it, values, 20);

    ASSERT_EQ(count, 8);

    const int expected[] = {1, 2, 10, 11, 20, 21, 30, 31};
    ASSERT_TRUE(verify_values(values, expected, 8, "chain_multiple"));

    chain_it.destroy(&chain_it);
    return TEST_SUCCESS;
}

//==============================================================================
// Chain Iterator with Different Iterator Types
//==============================================================================

static int test_chain_with_repeat_iterators(void)
{
    const DSCAllocator alloc = create_int_allocator();

    const int value1 = 42;
    const int value2 = 99;

    // Chain repeat iterators with different values
    DSCIterator repeat1 = dsc_iterator_repeat(&value1, &alloc, 3); // [42,42,42]
    DSCIterator repeat2 = dsc_iterator_repeat(&value2, &alloc, 2); // [99,99]

    DSCIterator iterators[] = {repeat1, repeat2};
    DSCIterator chain_it = dsc_iterator_chain(iterators, 2, &alloc);
    ASSERT_TRUE(chain_it.is_valid(&chain_it));

    int values[10];
    const int count = collect_values(&chain_it, values, 10);

    ASSERT_EQ(count, 5);

    const int expected[] = {42, 42, 42, 99, 99};
    ASSERT_TRUE(verify_values(values, expected, 5, "chain_repeat"));

    chain_it.destroy(&chain_it);
    return TEST_SUCCESS;
}

static int test_chain_with_take_skip_iterators(void)
{
    const DSCAllocator alloc = create_int_allocator();

    // Create base range and apply take/skip to different copies
    DSCIterator range1 = dsc_iterator_range(1, 10, 1, &alloc); // [1,2,3,4,5,6,7,8,9]
    DSCIterator range2 = dsc_iterator_range(1, 10, 1, &alloc); // [1,2,3,4,5,6,7,8,9]

    DSCIterator take_it = dsc_iterator_take(&range1, &alloc, 3);    // [1,2,3]
    DSCIterator skip_it = dsc_iterator_skip(&range2, &alloc, 6);    // [7,8,9]

    DSCIterator iterators[] = {take_it, skip_it};
    DSCIterator chain_it = dsc_iterator_chain(iterators, 2, &alloc);
    ASSERT_TRUE(chain_it.is_valid(&chain_it));

    int values[10];
    const int count = collect_values(&chain_it, values, 10);

    ASSERT_EQ(count, 6);

    const int expected[] = {1, 2, 3, 7, 8, 9};
    ASSERT_TRUE(verify_values(values, expected, 6, "chain_take_skip"));

    chain_it.destroy(&chain_it);
    return TEST_SUCCESS;
}

//==============================================================================
// Edge Cases and Error Handling
//==============================================================================

static int test_chain_invalid_parameters(void)
{
    const DSCAllocator alloc = create_int_allocator();

    // Test with NULL iterators array
    DSCIterator chain_it1 = dsc_iterator_chain(NULL, 2, &alloc);
    ASSERT_FALSE(chain_it1.is_valid(&chain_it1));

    // Test with zero count
    DSCIterator range = dsc_iterator_range(1, 3, 1, &alloc);
    DSCIterator iterators[] = {range};
    DSCIterator chain_it2 = dsc_iterator_chain(iterators, 0, &alloc);
    ASSERT_FALSE(chain_it2.is_valid(&chain_it2));

    // Test with NULL allocator
    DSCIterator chain_it3 = dsc_iterator_chain(iterators, 1, NULL);
    ASSERT_FALSE(chain_it3.is_valid(&chain_it3));

    // Clean up the range iterator that wasn't consumed
    range.destroy(&range);

    return TEST_SUCCESS;
}

static int test_chain_iterator_operations(void)
{
    const DSCAllocator alloc = create_int_allocator();

    DSCIterator range1 = dsc_iterator_range(1, 3, 1, &alloc); // [1,2]
    DSCIterator range2 = dsc_iterator_range(10, 12, 1, &alloc); // [10,11]

    DSCIterator iterators[] = {range1, range2};
    DSCIterator chain_it = dsc_iterator_chain(iterators, 2, &alloc);

    // Test initial state
    ASSERT_TRUE(chain_it.has_next(&chain_it));
    const int* value = chain_it.get(&chain_it);
    ASSERT_NOT_NULL(value);
    ASSERT_EQ(*value, 1);

    // Test advancement
    ASSERT_EQ(chain_it.next(&chain_it), 0);
    ASSERT_TRUE(chain_it.has_next(&chain_it));
    value = chain_it.get(&chain_it);
    ASSERT_NOT_NULL(value);
    ASSERT_EQ(*value, 2);

    // Test transition to second iterator
    ASSERT_EQ(chain_it.next(&chain_it), 0);
    ASSERT_TRUE(chain_it.has_next(&chain_it));
    value = chain_it.get(&chain_it);
    ASSERT_NOT_NULL(value);
    ASSERT_EQ(*value, 10);

    // Continue through second iterator
    ASSERT_EQ(chain_it.next(&chain_it), 0);
    ASSERT_TRUE(chain_it.has_next(&chain_it));
    value = chain_it.get(&chain_it);
    ASSERT_NOT_NULL(value);
    ASSERT_EQ(*value, 11);

    // Test end of iteration
    ASSERT_EQ(chain_it.next(&chain_it), 0);
    ASSERT_FALSE(chain_it.has_next(&chain_it));
    ASSERT_NULL(chain_it.get(&chain_it));

    // Test operations not supported
    ASSERT_FALSE(chain_it.has_prev(&chain_it));
    ASSERT_EQ(chain_it.prev(&chain_it), -1);

    chain_it.destroy(&chain_it);
    return TEST_SUCCESS;
}

//==============================================================================
// Composition Tests
//==============================================================================

static int test_chain_with_nested_chains(void)
{
    const DSCAllocator alloc = create_int_allocator();

    // Create first chain: [1,2] + [10,11]
    DSCIterator range1 = dsc_iterator_range(1, 3, 1, &alloc);
    DSCIterator range2 = dsc_iterator_range(10, 12, 1, &alloc);
    DSCIterator iterators1[] = {range1, range2};
    DSCIterator chain1 = dsc_iterator_chain(iterators1, 2, &alloc);

    // Create second chain: [20,21] + [30,31]
    DSCIterator range3 = dsc_iterator_range(20, 22, 1, &alloc);
    DSCIterator range4 = dsc_iterator_range(30, 32, 1, &alloc);
    DSCIterator iterators2[] = {range3, range4};
    DSCIterator chain2 = dsc_iterator_chain(iterators2, 2, &alloc);

    // Chain the two chain iterators
    DSCIterator master_iterators[] = {chain1, chain2};
    DSCIterator master_chain = dsc_iterator_chain(master_iterators, 2, &alloc);
    ASSERT_TRUE(master_chain.is_valid(&master_chain));

    int values[20];
    const int count = collect_values(&master_chain, values, 20);

    ASSERT_EQ(count, 8);

    // Should get: [1,2,10,11,20,21,30,31]
    const int expected[] = {1, 2, 10, 11, 20, 21, 30, 31};
    ASSERT_TRUE(verify_values(values, expected, 8, "chain_nested"));

    master_chain.destroy(&master_chain);
    return TEST_SUCCESS;
}

//==============================================================================
// Mixed Data Structure Tests
//==============================================================================

#include "ArrayList.h"
#include "DoublyLinkedList.h"
#include "HashSet.h"
#include "Pair.h"
#include "Queue.h"
#include "SinglyLinkedList.h"
#include "Stack.h"

static int test_chain_with_arraylist_and_dll(void)
{
    DSCAllocator alloc = create_int_allocator();

    // Create ArrayList with values [1, 2, 3, 4, 5]
    DSCArrayList* arraylist = dsc_arraylist_create(&alloc, 5);
    for (int i = 1; i <= 5; i++)
    {
        int* val = alloc.alloc_func(sizeof(int));
        *val = i;
        dsc_arraylist_push_back(arraylist, val);
    }

    // Create DoublyLinkedList with values [10, 20, 30]
    DSCDoublyLinkedList* dll = dsc_dll_create(&alloc);
    for (int i = 1; i <= 3; i++)
    {
        int* val = alloc.alloc_func(sizeof(int));
        *val = i * 10;
        dsc_dll_insert_back(dll, val);
    }

    // Get iterators from data structures
    DSCIterator arraylist_it = dsc_arraylist_iterator(arraylist);
    DSCIterator dll_it = dsc_dll_iterator(dll);

    // Apply transformations - filter even numbers from arraylist, take first 2 from dll
    const DSCIterator filtered_arraylist = dsc_iterator_filter(&arraylist_it, &alloc, is_even);
    const DSCIterator taken_dll = dsc_iterator_take(&dll_it, &alloc, 2);

    // Chain the transformed iterators
    DSCIterator iterators[] = {filtered_arraylist, taken_dll};
    DSCIterator chain_it = dsc_iterator_chain(iterators, 2, &alloc);
    ASSERT_TRUE(chain_it.is_valid(&chain_it));

    int values[10];
    const int count = collect_values(&chain_it, values, 10);

    ASSERT_EQ(count, 4); // [2, 4] from filtered arraylist + [10, 20] from taken dll

    const int expected[] = {2, 4, 10, 20};
    ASSERT_TRUE(verify_values(values, expected, 4, "chain_arraylist_dll"));

    chain_it.destroy(&chain_it);
    dsc_arraylist_destroy(arraylist, true);
    dsc_dll_destroy(dll, true);
    return TEST_SUCCESS;
}

static int test_chain_with_stack_queue_and_hashset(void)
{
    DSCAllocator alloc = create_int_allocator();

    // Create Stack with values [100, 200, 300] (pushed in order, so top is 300)
    DSCStack* stack = dsc_stack_create(&alloc);
    for (int i = 1; i <= 3; i++)
    {
        int* val = alloc.alloc_func(sizeof(int));
        *val = i * 100;
        dsc_stack_push(stack, val);
    }

    // Create Queue with values [5, 10, 15, 20, 25]
    DSCQueue* queue = dsc_queue_create(&alloc);
    for (int i = 1; i <= 5; i++)
    {
        int* val = alloc.alloc_func(sizeof(int));
        *val = i * 5;
        dsc_queue_enqueue(queue, val);
    }

    // Create HashSet with values [7, 14, 21, 28, 35] (multiples of 7)
    DSCHashSet* hashset = dsc_hashset_create(&alloc, dsc_hash_int, int_cmp, 0);
    for (int i = 1; i <= 5; i++)
    {
        int* val = alloc.alloc_func(sizeof(int));
        *val = i * 7;
        dsc_hashset_add(hashset, val);
    }

    // Get iterators from data structures
    DSCIterator stack_it = dsc_stack_iterator(stack);
    DSCIterator queue_it = dsc_queue_iterator(queue);
    DSCIterator hashset_it = dsc_hashset_iterator(hashset);

    // Apply various transformations
    DSCIterator taken_stack = dsc_iterator_take(&stack_it, &alloc, 2);       // First 2 from stack
    DSCIterator skipped_queue = dsc_iterator_skip(&queue_it, &alloc, 2);     // Skip first 2 from queue
    DSCIterator filtered_hashset = dsc_iterator_filter(&hashset_it, &alloc, is_greater_than_20); // > 20 from hashset

    // Chain all three transformed iterators
    DSCIterator iterators[] = {taken_stack, skipped_queue, filtered_hashset};
    DSCIterator chain_it = dsc_iterator_chain(iterators, 3, &alloc);
    ASSERT_TRUE(chain_it.is_valid(&chain_it));

    int values[20];
    const int count = collect_values(&chain_it, values, 20);

    // Note: HashSet order is not guaranteed, so we'll just verify the count and that values are present
    ASSERT_TRUE(count >= 6); // At least 2 from stack + 3 from queue + 1+ from hashset

    // Verify stack values are first (300, 200 - stack iteration order)
    ASSERT_EQ(values[0], 300);
    ASSERT_EQ(values[1], 200);

    // Verify queue values come next (15, 20, 25 - after skipping 5, 10)
    ASSERT_EQ(values[2], 15);
    ASSERT_EQ(values[3], 20);
    ASSERT_EQ(values[4], 25);

    // Remaining values should be from hashset (> 20): could be 21, 28, 35 in any order
    for (int i = 5; i < count; i++)
    {
        ASSERT_TRUE(values[i] > 20 && values[i] % 7 == 0);
    }

    chain_it.destroy(&chain_it);
    dsc_stack_destroy(stack, true);
    dsc_queue_destroy(queue, true);
    dsc_hashset_destroy(hashset, true);
    return TEST_SUCCESS;
}

static int test_chain_with_complex_transformations(void)
{
    DSCAllocator alloc = create_int_allocator();

    // Create ArrayList for source data
    DSCArrayList* list1 = dsc_arraylist_create(&alloc, 10);
    DSCArrayList* list2 = dsc_arraylist_create(&alloc, 10);

    // Fill first list with [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
    for (int i = 1; i <= 10; i++)
    {
        int* val = alloc.alloc_func(sizeof(int));
        *val = i;
        dsc_arraylist_push_back(list1, val);
    }

    // Fill second list with [11, 12, 13, 14, 15]
    for (int i = 11; i <= 15; i++)
    {
        int* val = alloc.alloc_func(sizeof(int));
        *val = i;
        dsc_arraylist_push_back(list2, val);
    }

    // Get base iterators
    DSCIterator it1 = dsc_arraylist_iterator(list1);
    DSCIterator it2 = dsc_arraylist_iterator(list2);

    // Create complex transformation chain 1: skip(2) -> filter(even) -> take(3) -> double
    DSCIterator skipped1 = dsc_iterator_skip(&it1, &alloc, 2);           // [3,4,5,6,7,8,9,10]
    DSCIterator filtered1 = dsc_iterator_filter(&skipped1, &alloc, is_even); // [4,6,8,10]
    DSCIterator taken1 = dsc_iterator_take(&filtered1, &alloc, 3);       // [4,6,8]
    DSCIterator doubled1 = dsc_iterator_transform(&taken1, &alloc, double_value, 1); // [8,12,16]

    // Create complex transformation chain 2: filter(odd) -> enumerate -> take(2)
    DSCIterator filtered2 = dsc_iterator_filter(&it2, &alloc, is_odd);   // [11,13,15]
    DSCIterator enumerated2 = dsc_iterator_enumerate(&filtered2, &alloc, 0); // [(0,11),(1,13),(2,15)]
    DSCIterator taken2 = dsc_iterator_take(&enumerated2, &alloc, 2);     // [(0,11),(1,13)]

    // Chain the two complex transformation results
    DSCIterator iterators[] = {doubled1, taken2};
    DSCIterator chain_it = dsc_iterator_chain(iterators, 2, &alloc);
    ASSERT_TRUE(chain_it.is_valid(&chain_it));

    // Collect and verify first part (doubled values)
    ASSERT_TRUE(chain_it.has_next(&chain_it));
    const int* val = chain_it.get(&chain_it);
    ASSERT_NOT_NULL(val);
    ASSERT_EQ(*val, 8);

    chain_it.next(&chain_it);
    ASSERT_TRUE(chain_it.has_next(&chain_it));
    val = chain_it.get(&chain_it);
    ASSERT_NOT_NULL(val);
    ASSERT_EQ(*val, 12);

    chain_it.next(&chain_it);
    ASSERT_TRUE(chain_it.has_next(&chain_it));
    val = chain_it.get(&chain_it);
    ASSERT_NOT_NULL(val);
    ASSERT_EQ(*val, 16);

    // Now we should get the enumerated elements
    chain_it.next(&chain_it);
    ASSERT_TRUE(chain_it.has_next(&chain_it));
    const DSCIndexedElement* indexed = chain_it.get(&chain_it);
    ASSERT_NOT_NULL(indexed);
    ASSERT_EQ(indexed->index, 0);
    ASSERT_EQ(*(int*)indexed->element, 11);

    chain_it.next(&chain_it);
    ASSERT_TRUE(chain_it.has_next(&chain_it));
    indexed = chain_it.get(&chain_it);
    ASSERT_NOT_NULL(indexed);
    ASSERT_EQ(indexed->index, 1);
    ASSERT_EQ(*(int*)indexed->element, 13);

    chain_it.next(&chain_it);
    ASSERT_FALSE(chain_it.has_next(&chain_it));

    chain_it.destroy(&chain_it);
    dsc_arraylist_destroy(list1, true);
    dsc_arraylist_destroy(list2, true);
    return TEST_SUCCESS;
}

static int test_chain_with_zip_and_data_structures(void)
{
    DSCAllocator alloc = create_int_allocator();

    // Create data structures
    DSCArrayList* arraylist = dsc_arraylist_create(&alloc, 5);
    DSCSinglyLinkedList* sll = dsc_sll_create(&alloc);

    // Fill ArrayList with [1, 2, 3, 4, 5]
    for (int i = 1; i <= 5; i++)
    {
        int* val = alloc.alloc_func(sizeof(int));
        *val = i;
        dsc_arraylist_push_back(arraylist, val);
    }

    // Fill SLL with [10, 20, 30]
    for (int i = 1; i <= 3; i++)
    {
        int* val = alloc.alloc_func(sizeof(int));
        *val = i * 10;
        dsc_sll_insert_back(sll, val);
    }

    // Get iterators and apply transformations
    DSCIterator arraylist_it = dsc_arraylist_iterator(arraylist);
    DSCIterator sll_it = dsc_sll_iterator(sll);

    DSCIterator filtered_arraylist = dsc_iterator_filter(&arraylist_it, &alloc, is_odd); // [1,3,5]
    DSCIterator taken_sll = dsc_iterator_take(&sll_it, &alloc, 2);                       // [10,20]

    // Zip the transformed iterators
    const DSCIterator zipped = dsc_iterator_zip(&filtered_arraylist, &taken_sll, &alloc); // [(1,10),(3,20)]

    // Create a simple range for chaining
    const DSCIterator range_it = dsc_iterator_range(100, 103, 1, &alloc); // [100,101,102]

    // Chain zip result with range
    DSCIterator iterators[] = {zipped, range_it};
    DSCIterator chain_it = dsc_iterator_chain(iterators, 2, &alloc);
    ASSERT_TRUE(chain_it.is_valid(&chain_it));

    // Verify zipped pairs first
    ASSERT_TRUE(chain_it.has_next(&chain_it));
    const DSCPair* pair = chain_it.get(&chain_it);
    ASSERT_NOT_NULL(pair);
    ASSERT_EQ(*(int*)pair->first, 1);
    ASSERT_EQ(*(int*)pair->second, 10);

    chain_it.next(&chain_it);
    ASSERT_TRUE(chain_it.has_next(&chain_it));
    pair = chain_it.get(&chain_it);
    ASSERT_NOT_NULL(pair);
    ASSERT_EQ(*(int*)pair->first, 3);
    ASSERT_EQ(*(int*)pair->second, 20);

    // Then verify range values
    chain_it.next(&chain_it);
    ASSERT_TRUE(chain_it.has_next(&chain_it));
    const int* val = chain_it.get(&chain_it);
    ASSERT_NOT_NULL(val);
    ASSERT_EQ(*val, 100);

    chain_it.next(&chain_it);
    ASSERT_TRUE(chain_it.has_next(&chain_it));
    val = chain_it.get(&chain_it);
    ASSERT_NOT_NULL(val);
    ASSERT_EQ(*val, 101);

    chain_it.next(&chain_it);
    ASSERT_TRUE(chain_it.has_next(&chain_it));
    val = chain_it.get(&chain_it);
    ASSERT_NOT_NULL(val);
    ASSERT_EQ(*val, 102);

    chain_it.next(&chain_it);
    ASSERT_FALSE(chain_it.has_next(&chain_it));

    chain_it.destroy(&chain_it);
    dsc_arraylist_destroy(arraylist, true);
    dsc_sll_destroy(sll, true);
    return TEST_SUCCESS;
}

static int test_chain_data_structure_round_trip(void)
{
    DSCAllocator alloc = create_int_allocator();

    // Create original ArrayList
    DSCArrayList* original_list = dsc_arraylist_create(&alloc, 5);
    for (int i = 1; i <= 5; i++)
    {
        int* val = alloc.alloc_func(sizeof(int));
        *val = i * i; // [1, 4, 9, 16, 25]
        dsc_arraylist_push_back(original_list, val);
    }

    // Create range iterator
    const DSCIterator range_it = dsc_iterator_range(100, 103, 1, &alloc); // [100, 101, 102]

    // Get iterator from ArrayList and apply transformation
    DSCIterator list_it = dsc_arraylist_iterator(original_list);
    const DSCIterator filtered_list = dsc_iterator_filter(&list_it, &alloc, is_greater_than_10); // [16, 25]

    // Chain filtered list with range
    DSCIterator iterators[] = {filtered_list, range_it};
    DSCIterator chained = dsc_iterator_chain(iterators, 2, &alloc);

    // Create new data structures from the chained iterator
    DSCDoublyLinkedList* result_dll = dsc_dll_from_iterator(&chained, &alloc, true);
    ASSERT_NOT_NULL(result_dll);
    ASSERT_EQ(dsc_dll_size(result_dll), 5); // [16, 25, 100, 101, 102]

    // Verify the round-trip worked correctly
    DSCIterator result_it = dsc_dll_iterator(result_dll);
    
    const int expected[] = {16, 25, 100, 101, 102};
    int i = 0;
    while (result_it.has_next(&result_it))
    {
        const int* val = result_it.get(&result_it);
        ASSERT_NOT_NULL(val);
        ASSERT_EQ(*val, expected[i]);
        i++;
        result_it.next(&result_it);
    }
    ASSERT_EQ(i, 5);

    chained.destroy(&chained);
    result_it.destroy(&result_it);
    dsc_dll_destroy(result_dll, true);
    dsc_arraylist_destroy(original_list, true);
    return TEST_SUCCESS;
}

//==============================================================================
// Test Suite Definition
//==============================================================================

typedef struct
{
    int (*func)(void);
    const char* name;
} TestCase;

static const TestCase tests[] = {
    // Basic functionality tests
    {test_chain_basic_functionality, "test_chain_basic_functionality"},
    {test_chain_single_iterator, "test_chain_single_iterator"},
    {test_chain_empty_iterators, "test_chain_empty_iterators"},
    {test_chain_mixed_empty_and_non_empty, "test_chain_mixed_empty_and_non_empty"},
    {test_chain_multiple_iterators, "test_chain_multiple_iterators"},

    // Different iterator types
    {test_chain_with_repeat_iterators, "test_chain_with_repeat_iterators"},
    {test_chain_with_take_skip_iterators, "test_chain_with_take_skip_iterators"},

    // Error handling tests
    {test_chain_invalid_parameters, "test_chain_invalid_parameters"},
    {test_chain_iterator_operations, "test_chain_iterator_operations"},

    // Composition tests
    {test_chain_with_nested_chains, "test_chain_with_nested_chains"},

    // Mixed data structure tests
    {test_chain_with_arraylist_and_dll, "test_chain_with_arraylist_and_dll"},
    {test_chain_with_stack_queue_and_hashset, "test_chain_with_stack_queue_and_hashset"},
    {test_chain_with_complex_transformations, "test_chain_with_complex_transformations"},
    {test_chain_with_zip_and_data_structures, "test_chain_with_zip_and_data_structures"},
    {test_chain_data_structure_round_trip, "test_chain_data_structure_round_trip"},
};

//==============================================================================
// Main Test Runner
//==============================================================================

int main(void)
{
    printf("Running Chain Iterator Tests...\n");

    int passed = 0;
    int failed = 0;
    const int total_tests = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < total_tests; i++)
    {
        printf("  Running %s... ", tests[i].name);
        fflush(stdout);

        const int result = tests[i].func();
        if (result == TEST_SUCCESS)
        {
            printf("PASSED\n");
            passed++;
        }
        else if (result == TEST_FAILURE)
        {
            printf("FAILED\n");
            failed++;
        }
        else
        {
            printf("SKIPPED\n");
        }
    }

    printf("\nChain Iterator Test Results:\n");
    printf("  Total tests: %d\n", total_tests);
    printf("  Passed: %d\n", passed);
    printf("  Failed: %d\n", failed);
    printf("  Skipped: %d\n", total_tests - passed - failed);

    if (failed > 0)
    {
        printf("\nSome tests failed!\n");
        return 1;
    }

    printf("\nAll Chain Iterator tests passed!\n");
    return 0;
}
