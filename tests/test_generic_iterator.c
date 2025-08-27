//
// Created by zack on 8/26/25.
//

#include "DLinkedList.h"
#include "Iterator.h"
#include "TestAssert.h"

#include <stdlib.h>

// Helper functions for transform tests

// Helper function to double an integer value
static void* double_value(const void* data) {
    const int* original = data;
    int* result = malloc(sizeof(int));
    *result = (*original) * 2;
    return result;
}

// Second transformation: add 1 to the doubled value
static void* add_one(const void* data) {
    const int* original = data;
    int* result = malloc(sizeof(int));
    *result = (*original) + 1;
    return result;
}

// Helper function: multiply by 3
static void* multiply_by_three(const void* data) {
    const int* original = data;
    int* result = malloc(sizeof(int));
    *result = (*original) * 3;
    return result;
}

// Helper function: add 5 to a value
static void* add_five(const void* data) {
    const int* original = data;
    int* result = malloc(sizeof(int));
    *result = (*original) + 5;
    return result;
}

// Helper functions for filter tests

// Predicate function: return true for even numbers
static int is_even(const void* data) {
    const int* value = data;
    return (*value % 2 == 0);
}

// Predicate: is greater than 5
static int is_greater_than_five(const void* data) {
    const int* value = data;
    return (*value > 5);
}

// Predicate: is divisible by 6
static int is_divisible_by_six(const void* data) {
    const int* value = data;
    return (*value % 6 == 0);
}

// Predicate: is odd
static int is_odd(const void* data) {
    const int* value = data;
    return (*value % 2 == 1);
}

// TRANSFORM ITERATOR TESTS

// Test basic transform iterator functionality
static int test_transform_iterator() {
    // Create a list with integers
    DLinkedList* list = dll_create();
    ASSERT_NOT_NULLPTR(list);

    // Add some elements
    for (int i = 1; i <= 5; i++) {
        int* val = malloc(sizeof(int));
        *val = i;
        dll_insert_back(list, val);
    }

    // Create a basic iterator
    Iterator base_it = dll_iterator(list);
    ASSERT_TRUE(base_it.has_next(&base_it));

    // Create a transform iterator that doubles each value
    Iterator transform_it = iterator_transform(&base_it, double_value);
    ASSERT_TRUE(transform_it.has_next(&transform_it));

    // Test that transform works correctly
    int expected = 2; // First element (1) doubled
    while (transform_it.has_next(&transform_it)) {
        int* value = transform_it.next(&transform_it);
        ASSERT_NOT_NULLPTR(value);
        ASSERT_EQ(*value, expected);
        expected += 2; // Next expected value
        free(value); // Free the transformed value
    }

    // Verify we processed all elements
    ASSERT_EQ(expected, 12); // After 1,2,3,4,5 -> 2,4,6,8,10

    // Cleanup
    transform_it.destroy(&transform_it);
    dll_destroy(list, free);

    return TEST_SUCCESS;
}

// Test transform iterator with edge cases
static int test_transform_edge_cases() {
    // Test with empty list
    DLinkedList* empty_list = dll_create();
    ASSERT_NOT_NULLPTR(empty_list);

    Iterator empty_it = dll_iterator(empty_list);
    ASSERT_FALSE(empty_it.has_next(&empty_it));

    Iterator transform_empty = iterator_transform(&empty_it, double_value);
    ASSERT_FALSE(transform_empty.has_next(&transform_empty));
    ASSERT_NULLPTR(transform_empty.next(&transform_empty));

    transform_empty.destroy(&transform_empty);
    dll_destroy(empty_list, nullptr);

    // Test with NULL base iterator (should handle gracefully)
    Iterator null_transform = iterator_transform(nullptr, double_value);
    ASSERT_FALSE(null_transform.has_next(&null_transform));
    ASSERT_NULLPTR(null_transform.next(&null_transform));

    null_transform.destroy(&null_transform);

    return TEST_SUCCESS;
}

// Test transform iterator chaining
static int test_transform_chaining() {
    DLinkedList* list = dll_create();
    ASSERT_NOT_NULLPTR(list);

    // Add elements 1-3
    for (int i = 1; i <= 3; i++) {
        int* val = malloc(sizeof(int));
        *val = i;
        dll_insert_back(list, val);
    }

    // First transformation: double the value
    Iterator base_it = dll_iterator(list);
    Iterator double_it = iterator_transform(&base_it, double_value);

    Iterator chain_it = iterator_transform(&double_it, add_one);

    // Test the chained transformations (1->2->3, 2->4->6, 3->5->7)
    int idx = 0;

    while (chain_it.has_next(&chain_it)) {
        constexpr int expected_values[] = {3, 5, 7};
        int* value                  = chain_it.next(&chain_it);
        ASSERT_NOT_NULLPTR(value);
        ASSERT_EQ(*value, expected_values[idx++]);
        free(value);
    }

    ASSERT_EQ(idx, 3); // Verify we processed all elements

    // Cleanup - note that destroying the outermost iterator should
    // recursively destroy all inner iterators
    chain_it.destroy(&chain_it);
    dll_destroy(list, free);

    return TEST_SUCCESS;
}

// FILTER ITERATOR TESTS

// Test filter iterator functionality
static int test_filter_iterator() {
    DLinkedList* list = dll_create();
    ASSERT_NOT_NULLPTR(list);

    // Insert elements 1-10
    for (int i = 1; i <= 10; i++) {
        int* val = malloc(sizeof(int));
        *val = i;
        dll_insert_back(list, val);
    }

    // Create base iterator
    Iterator base_it = dll_iterator(list);

    // Create filter iterator for even numbers
    Iterator filter_it = iterator_filter(&base_it, is_even);
    ASSERT_NOT_NULLPTR(filter_it.data_state);

    // Verify filter correctly returns only even numbers
    int idx = 0;

    while (filter_it.has_next(&filter_it)) {
        constexpr int expected_values[] = {2, 4, 6, 8, 10};
        const int* value            = filter_it.next(&filter_it);
        ASSERT_NOT_NULLPTR(value);
        ASSERT_EQ(*value, expected_values[idx++]);
    }

    // Verify we found all 5 even numbers
    ASSERT_EQ(idx, 5);

    // Cleanup
    if (filter_it.destroy) {
        filter_it.destroy(&filter_it);
    }
    dll_destroy(list, free);

    return TEST_SUCCESS;
}

// Test filter iterator with no matches
static int test_filter_no_matches() {
    DLinkedList* list = dll_create();
    ASSERT_NOT_NULLPTR(list);

    // Insert odd numbers only
    for (int i = 1; i <= 5; i += 2) {
        int* val = malloc(sizeof(int));
        *val = i;
        dll_insert_back(list, val);
    }

    // Create base iterator
    Iterator base_it = dll_iterator(list);

    // Create filter iterator for even numbers (should find none)
    Iterator filter_it = iterator_filter(&base_it, is_even);

    // Verify filter correctly returns no results
    ASSERT_FALSE(filter_it.has_next(&filter_it));
    ASSERT_NULLPTR(filter_it.next(&filter_it));

    // Cleanup
    if (filter_it.destroy) {
        filter_it.destroy(&filter_it);
    }
    dll_destroy(list, free);

    return TEST_SUCCESS;
}

// Test chaining filter and transform iterators
static int test_filter_transform_chain() {
    DLinkedList* list = dll_create();
    ASSERT_NOT_NULLPTR(list);

    // Insert elements 1-10
    for (int i = 1; i <= 10; i++) {
        int* val = malloc(sizeof(int));
        *val = i;
        dll_insert_back(list, val);
    }

    // Create base iterator
    Iterator base_it = dll_iterator(list);

    // First filter for even numbers
    Iterator filter_it = iterator_filter(&base_it, is_even);

    // Then transform to double the values
    Iterator transform_it = iterator_transform(&filter_it, double_value);

    // Expected: even numbers doubled (2->4, 4->8, 6->12, 8->16, 10->20)
    int idx = 0;

    while (transform_it.has_next(&transform_it)) {
        constexpr int expected_values[] = {4, 8, 12, 16, 20};
        int* value                  = transform_it.next(&transform_it);
        ASSERT_NOT_NULLPTR(value);
        ASSERT_EQ(*value, expected_values[idx++]);
        free(value); // Free the transformed value
    }

    ASSERT_EQ(idx, 5); // Verify we got all 5 expected values

    // Cleanup
    transform_it.destroy(&transform_it);
    dll_destroy(list, free);

    return TEST_SUCCESS;
}

// Test chaining transform and filter iterators (opposite order)
static int test_transform_filter_chain() {
    DLinkedList* list = dll_create();
    ASSERT_NOT_NULLPTR(list);

    // Insert elements 1-10
    for (int i = 1; i <= 10; i++) {
        int* val = malloc(sizeof(int));
        *val = i;
        dll_insert_back(list, val);
    }

    // Create base iterator
    Iterator base_it = dll_iterator(list);

    // First transform to multiply by 3
    Iterator transform_it = iterator_transform(&base_it, multiply_by_three);

    // Then filter for values divisible by 6
    Iterator filter_it = iterator_filter(&transform_it, is_divisible_by_six);

    // Expected: multiples of 3 that are divisible by 6 (2->6, 4->12, 6->18, 8->24, 10->30)
    int idx = 0;

    while (filter_it.has_next(&filter_it)) {
        constexpr int expected_values[] = {6, 12, 18, 24, 30};
        int* value                  = filter_it.next(&filter_it);
        ASSERT_NOT_NULLPTR(value);
        ASSERT_EQ(*value, expected_values[idx]);
        idx++;
        free(value); // Free the transformed value
    }

    ASSERT_EQ(idx, 5); // Verify we got all 5 expected values

    // Cleanup
    filter_it.destroy(&filter_it);
    dll_destroy(list, free);

    return TEST_SUCCESS;
}

// Test chaining multiple filters
static int test_multiple_filters() {
    DLinkedList* list = dll_create();
    ASSERT_NOT_NULLPTR(list);

    // Insert elements 1-20
    for (int i = 1; i <= 20; i++) {
        int* val = malloc(sizeof(int));
        *val = i;
        dll_insert_back(list, val);
    }

    // Create base iterator
    Iterator base_it = dll_iterator(list);

    // Filter for even numbers
    Iterator filter_even = iterator_filter(&base_it, is_even);

    // Filter for numbers > 5
    Iterator filter_greater = iterator_filter(&filter_even, is_greater_than_five);

    // Expected: even numbers > 5 (6, 8, 10, 12, 14, 16, 18, 20)
    int idx = 0;

    while (filter_greater.has_next(&filter_greater))
    {
        constexpr int expected_values[] = {6, 8, 10, 12, 14, 16, 18, 20};
        const int* value            = filter_greater.next(&filter_greater);
        ASSERT_NOT_NULLPTR(value);
        ASSERT_EQ(*value, expected_values[idx]);
        idx++;
    }

    ASSERT_EQ(idx, 8); // Verify we got all 8 expected values

    // Cleanup
    filter_greater.destroy(&filter_greater);
    dll_destroy(list, free);

    return TEST_SUCCESS;
}

// Test complex chaining of filters and transforms
static int test_complex_iterator_chain() {
    DLinkedList* list = dll_create();
    ASSERT_NOT_NULLPTR(list);

    // Insert elements 1-10
    for (int i = 1; i <= 10; i++) {
        int* val = malloc(sizeof(int));
        *val = i;
        dll_insert_back(list, val);
    }

    // Create base iterator
    Iterator base_it = dll_iterator(list);

    // 1. Filter for even numbers
    Iterator filter_even = iterator_filter(&base_it, is_even);

    // 2. Transform: double the value
    Iterator transform_double = iterator_transform(&filter_even, double_value);

    // 3. Transform: add 5
    Iterator transform_add = iterator_transform(&transform_double, add_five);

    // 4. Filter for odd numbers (after transformations)
    Iterator filter_odd = iterator_filter(&transform_add, is_odd);

    // Expected: even numbers -> doubled -> add 5 -> keep odd results
    // 2 -> 4 -> 9 (odd)
    // 4 -> 8 -> 13 (odd)
    // 6 -> 12 -> 17 (odd)
    // 8 -> 16 -> 21 (odd)
    // 10 -> 20 -> 25 (odd)
    int idx = 0;

    while (filter_odd.has_next(&filter_odd)) {
        constexpr int expected_values[] = {9, 13, 17, 21, 25};
        int* value                  = filter_odd.next(&filter_odd);
        ASSERT_NOT_NULLPTR(value);
        ASSERT_EQ(*value, expected_values[idx]);
        idx++;
        free(value); // Free the transformed value
    }

    ASSERT_EQ(idx, 5); // Verify we got all 5 expected values

    // Cleanup
    filter_odd.destroy(&filter_odd);
    dll_destroy(list, free);

    return TEST_SUCCESS;
}

int main() {
    int failed = 0;

    // Transform iterator tests
    if (test_transform_iterator() != TEST_SUCCESS) {
        printf("test_transform_iterator failed\n");
        failed++;
    }
    if (test_transform_edge_cases() != TEST_SUCCESS) {
        printf("test_transform_edge_cases failed\n");
        failed++;
    }
    if (test_transform_chaining() != TEST_SUCCESS) {
        printf("test_transform_chaining failed\n");
        failed++;
    }

    // Filter iterator tests
    if (test_filter_iterator() != TEST_SUCCESS) {
        printf("test_filter_iterator failed\n");
        failed++;
    }
    if (test_filter_no_matches() != TEST_SUCCESS) {
        printf("test_filter_no_matches failed\n");
        failed++;
    }

    // Combination tests
    if (test_filter_transform_chain() != TEST_SUCCESS) {
        printf("test_filter_transform_chain failed\n");
        failed++;
    }
    if (test_transform_filter_chain() != TEST_SUCCESS) {
        printf("test_transform_filter_chain failed\n");
        failed++;
    }
    if (test_multiple_filters() != TEST_SUCCESS) {
        printf("test_multiple_filters failed\n");
        failed++;
    }
    if (test_complex_iterator_chain() != TEST_SUCCESS) {
        printf("test_complex_iterator_chain failed\n");
        failed++;
    }

    if (failed) {
        printf("%d test(s) failed.\n", failed);
    } else {
        printf("All tests passed!\n");
    }

    return failed;
}