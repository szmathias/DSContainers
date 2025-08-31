//
// Created by zack on 8/26/25.
//

#include "DoublyLinkedList.h"
#include "Iterator.h"
#include "TestAssert.h"
#include "TestHelpers.h"

#include <stdlib.h>

// Test basic iterator functionality
static int test_basic_iteration(void) {
    DoublyLinkedList* list = dll_create();
    ASSERT_NOT_NULL(list);

    // Insert elements
    for (int i = 1; i <= 5; i++) {
        int* val = malloc(sizeof(int));
        *val = i;
        dll_insert_back(list, val);
    }

    // Create iterator
    Iterator it = dll_iterator(list);
    ASSERT_NOT_NULL(it.data_state);
    ASSERT_TRUE(it.has_next(&it));

    // Iterate through list and verify values
    int expected = 1;
    while (it.has_next(&it)) {
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
    if (it.destroy) {
        it.destroy(&it);
    }
    dll_destroy(list, free);

    return TEST_SUCCESS;
}

// Test iterator with empty list
static int test_empty_list_iterator(void) {
    DoublyLinkedList* list = dll_create();
    ASSERT_NOT_NULL(list);

    Iterator it = dll_iterator(list);

    // Verify iterator for empty list
    ASSERT_FALSE(it.has_next(&it));
    ASSERT_NULL(it.next(&it));

    // Cleanup
    if (it.destroy) {
        it.destroy(&it);
    }
    dll_destroy(list, NULL);

    return TEST_SUCCESS;
}

// Test iterator with modifications
static int test_iterator_with_modifications(void) {
    DoublyLinkedList* list = dll_create();
    ASSERT_NOT_NULL(list);

    // Insert initial elements
    for (int i = 1; i <= 3; i++) {
        int* val = malloc(sizeof(int));
        *val = i;
        dll_insert_back(list, val);
    }

    // Create iterator
    Iterator it = dll_iterator(list);

    // Consume first element
    const int* value = it.next(&it);
    ASSERT_NOT_NULL(value);
    ASSERT_EQ(*value, 1);

    // Modify list by adding new elements
    int* new_val = malloc(sizeof(int));
    *new_val = 99;
    dll_insert_back(list, new_val);

    // Continue iteration - should work with unmodified behavior
    value = (int*)it.next(&it);
    ASSERT_NOT_NULL(value);
    ASSERT_EQ(*value, 2);

    value = (int*)it.next(&it);
    ASSERT_NOT_NULL(value);
    ASSERT_EQ(*value, 3);

    // The newly added element should also be accessible
    value = (int*)it.next(&it);
    ASSERT_NOT_NULL(value);
    ASSERT_EQ(*value, 99);

    // Iterator should now be exhausted
    ASSERT_FALSE(it.has_next(&it));

    // Cleanup
    if (it.destroy) {
        it.destroy(&it);
    }
    dll_destroy(list, free);

    return TEST_SUCCESS;
}

// Test multiple iterators
static int test_multiple_iterators(void) {
    DoublyLinkedList* list = dll_create();
    ASSERT_NOT_NULL(list);

    // Insert elements
    for (int i = 1; i <= 5; i++) {
        int* val = malloc(sizeof(int));
        *val = i;
        dll_insert_back(list, val);
    }

    // Create two independent iterators
    Iterator it1 = dll_iterator(list);
    Iterator it2 = dll_iterator(list);

    // First iterator consumes two elements
    const int* value1 = it1.next(&it1);
    ASSERT_EQ(*value1, 1);

    value1 = (int*)it1.next(&it1);
    ASSERT_EQ(*value1, 2);

    // Second iterator should still be at the beginning
    const int* value2 = it2.next(&it2);
    ASSERT_EQ(*value2, 1);

    // Continue with both iterators
    value1 = (int*)it1.next(&it1);
    ASSERT_EQ(*value1, 3);

    value2 = (int*)it2.next(&it2);
    ASSERT_EQ(*value2, 2);

    // Cleanup
    if (it1.destroy) {
        it1.destroy(&it1);
    }
    if (it2.destroy) {
        it2.destroy(&it2);
    }
    dll_destroy(list, free);

    return TEST_SUCCESS;
}

// Test iterator reset functionality if available
static int test_iterator_reset(void) {
    DoublyLinkedList* list = dll_create();
    ASSERT_NOT_NULL(list);

    // Insert elements
    for (int i = 1; i <= 3; i++) {
        int* val = malloc(sizeof(int));
        *val = i;
        dll_insert_back(list, val);
    }

    // Create iterator
    Iterator it = dll_iterator(list);

    // Consume all elements
    while (it.has_next(&it)) {
        it.next(&it);
    }

    // Iterator should be exhausted
    ASSERT_FALSE(it.has_next(&it));

    // Reset iterator if supported
    if (it.reset) {
        it.reset(&it);

        // Should be able to iterate again
        ASSERT_TRUE(it.has_next(&it));

        int expected = 1;
        while (it.has_next(&it)) {
            const int* value = it.next(&it);
            ASSERT_EQ(*value, expected++);
        }

        ASSERT_EQ(expected, 4); // Confirm we iterated through all elements again
    }

    // Cleanup
    if (it.destroy) {
        it.destroy(&it);
    }
    dll_destroy(list, free);

    return TEST_SUCCESS;
}

// Test reverse iterator functionality
static int test_reverse_iteration(void) {
    DoublyLinkedList* list = dll_create();
    ASSERT_NOT_NULL(list);

    // Insert elements
    for (int i = 1; i <= 5; i++) {
        int* val = malloc(sizeof(int));
        *val = i;
        dll_insert_back(list, val);
    }

    // Create reverse iterator
    Iterator it = dll_iterator_reverse(list);
    ASSERT_NOT_NULL(it.data_state);
    ASSERT_TRUE(it.has_next(&it));

    // Iterate through list in reverse order and verify values
    int expected = 5;
    while (it.has_next(&it)) {
        const int* value = it.next(&it);
        ASSERT_NOT_NULL(value);
        ASSERT_EQ(*value, expected--);
    }

    // Verify we processed all elements
    ASSERT_EQ(expected, 0);

    // Verify the iterator is exhausted
    ASSERT_FALSE(it.has_next(&it));
    ASSERT_NULL(it.next(&it));

    // Cleanup
    if (it.destroy) {
        it.destroy(&it);
    }
    dll_destroy(list, free);

    return TEST_SUCCESS;
}

// Test empty list with reverse iterator
static int test_empty_list_reverse_iterator(void) {
    DoublyLinkedList* list = dll_create();
    ASSERT_NOT_NULL(list);

    Iterator it = dll_iterator_reverse(list);

    // Verify iterator for empty list
    ASSERT_FALSE(it.has_next(&it));
    ASSERT_NULL(it.next(&it));

    // Cleanup
    if (it.destroy) {
        it.destroy(&it);
    }
    dll_destroy(list, NULL);

    return TEST_SUCCESS;
}

// Helper function to create a copy of an integer
static void* copy_int(const void* data) {
    if (!data) return NULL;

    int* copy = malloc(sizeof(int));
    if (!copy) return NULL;

    *copy = *(const int*)data;
    return copy;
}

// Helper function to compare two integers for equality
static int int_compare(const void* a, const void* b) {
    const int* int_a = a;
    const int* int_b = b;

    if (*int_a == *int_b) return 0;
    return (*int_a < *int_b) ? -1 : 1;
}

// Test converting a basic iterator to a list with dll_from_iterator
static int test_from_iterator_basic(void) {
    // Create a source list
    DoublyLinkedList* src_list = dll_create();
    ASSERT_NOT_NULL(src_list);

    // Add some elements
    for (int i = 1; i <= 5; i++) {
        int* val = malloc(sizeof(int));
        *val = i;
        dll_insert_back(src_list, val);
    }

    // Create an iterator
    Iterator it = dll_iterator(src_list);
    ASSERT_TRUE(it.has_next(&it));

    // Convert iterator to a new list (shallow copy - same data pointers)
    DoublyLinkedList* new_list = dll_from_iterator(&it, NULL, NULL);
    ASSERT_NOT_NULL(new_list);

    // Verify the new list has the same values
    ASSERT_EQ(dll_size(new_list), 5);

    Iterator new_it = dll_iterator(new_list);
    int expected = 1;
    while (new_it.has_next(&new_it)) {
        const int* value = new_it.next(&new_it);
        ASSERT_EQ(*value, expected++);
    }

    // Since we did a shallow copy, modifying source list data should affect new list
    const DoublyLinkedNode* node = dll_find(src_list, &(int){1}, int_compare);
    ASSERT_NOT_NULL(node);

    // FIXED: Directly modify the value instead of freeing and replacing
    // This avoids the use-after-free error because both lists share the same data
    *((int*)node->data) = 99;

    // The first element in the new list should now be 99 as well
    Iterator check_it = dll_iterator(new_list);
    const int* first = check_it.next(&check_it);
    ASSERT_EQ(*first, 99);

    // Clean up - only free data in src_list to avoid double free
    if (new_it.destroy) {
        new_it.destroy(&new_it);
    }
    if (check_it.destroy) {
        check_it.destroy(&check_it);
    }
    if (it.destroy)
    {
        it.destroy(&it);
    }
    dll_destroy(new_list, NULL); // Don't free data - shared with src_list
    dll_destroy(src_list, free); // Free the data

    return TEST_SUCCESS;
}

// Test deep copying with dll_from_iterator using a copy function
static int test_from_iterator_with_copy(void) {
    // Create a source list
    DoublyLinkedList* src_list = dll_create();
    ASSERT_NOT_NULL(src_list);

    // Add some elements
    for (int i = 1; i <= 5; i++) {
        int* val = malloc(sizeof(int));
        *val = i;
        dll_insert_back(src_list, val);
    }

    // Create an iterator
    Iterator it = dll_iterator(src_list);
    ASSERT_TRUE(it.has_next(&it));

    // Convert iterator to a new list with deep copy
    DoublyLinkedList* new_list = dll_from_iterator(&it, copy_int, free);
    ASSERT_NOT_NULL(new_list);

    // Verify the new list has the same values
    ASSERT_EQ(dll_size(new_list), 5);

    // Modify the first element in the source list
    const DoublyLinkedNode* node = dll_find(src_list, &(int){1}, int_compare);
    ASSERT_NOT_NULL(node);
    *((int*)node->data) = 99;

    // The new list should still have the original values
    Iterator new_it = dll_iterator(new_list);
    int expected = 1;
    while (new_it.has_next(&new_it)) {
        const int* value = new_it.next(&new_it);
        ASSERT_EQ(*value, expected++);
    }

    // Clean up - we can free data in both lists
    if (new_it.destroy) {
        new_it.destroy(&new_it);
    }

    if (it.destroy)
    {
        it.destroy(&it);
    }

    dll_destroy(new_list, free);
    dll_destroy(src_list, free);

    return TEST_SUCCESS;
}

// Test dll_from_iterator with an empty list
static int test_from_iterator_empty(void) {
    // Create an empty source list
    DoublyLinkedList* src_list = dll_create();
    ASSERT_NOT_NULL(src_list);

    // Create an iterator
    Iterator it = dll_iterator(src_list);
    ASSERT_FALSE(it.has_next(&it));

    // Convert iterator to a new list
    DoublyLinkedList* new_list = dll_from_iterator(&it, NULL, NULL);
    ASSERT_NOT_NULL(new_list);

    // Verify the new list is empty
    ASSERT_EQ(dll_size(new_list), 0);
    ASSERT_TRUE(dll_is_empty(new_list));

    it.destroy(&it);

    // Clean up
    dll_destroy(new_list, NULL);
    dll_destroy(src_list, NULL);

    return TEST_SUCCESS;
}

// Test dll_from_iterator with invalid iterator
static int test_from_iterator_invalid(void) {
    // Test with NULL iterator
    DoublyLinkedList* new_list = dll_from_iterator(NULL, NULL, NULL);
    ASSERT_NULL(new_list);

    // Create an iterator and destroy it to make it invalid
    DoublyLinkedList* list = dll_create();
    Iterator it = dll_iterator(list);
    if (it.destroy) {
        it.destroy(&it);
    }

    // Try to create a list from the destroyed iterator
    new_list = dll_from_iterator(&it, NULL, NULL);
    ASSERT_NULL(new_list);

    // Clean up
    dll_destroy(list, NULL);

    return TEST_SUCCESS;
}

typedef struct {
    int (*func)(void);
    const char *name;
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

int main(void) {
    int failed = 0;
    const int num_tests = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < num_tests; i++) {
        if (tests[i].func() != TEST_SUCCESS) {
            printf("%s failed\n", tests[i].name);
            failed++;
        }
    }

    if (failed == 0) {
        printf("All DoublyLinkedList Iterator tests passed.\n");
        return 0;
    }

    printf("%d DoublyLinkedList Iterator tests failed.\n", failed);
    return 1;
}
