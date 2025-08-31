//
// Created by zack on 9/2/25.
//

#include "SinglyLinkedList.h"
#include "TestAssert.h"
#include "TestHelpers.h"
#include <stdio.h>
#include <stdlib.h>

// Tests from test_sll.c that fit into iterator category
int test_iterator_basic(void){
    SinglyLinkedList *list = sll_create();
    int *a = malloc(sizeof(int)); *a = 10;
    int *b = malloc(sizeof(int)); *b = 20;
    int *c = malloc(sizeof(int)); *c = 30;
    sll_insert_back(list, a);
    sll_insert_back(list, b);
    sll_insert_back(list, c);

    Iterator it = sll_iterator(list);
    ASSERT_NOT_NULL(it.data_state);
    ASSERT_EQ(it.is_valid(&it), 1);
    ASSERT_EQ(it.has_next(&it), 1);

    // Test getting first element
    void *data = it.next(&it);
    ASSERT_NOT_NULL(data);
    ASSERT_EQ(*(int*)data, 10);

    // Test getting second element
    ASSERT_EQ(it.has_next(&it), 1);
    data = it.next(&it);
    ASSERT_NOT_NULL(data);
    ASSERT_EQ(*(int*)data, 20);

    // Test getting third element
    ASSERT_EQ(it.has_next(&it), 1);
    data = it.next(&it);
    ASSERT_NOT_NULL(data);
    ASSERT_EQ(*(int*)data, 30);

    // Should be at the end now
    ASSERT_EQ(it.has_next(&it), 0);

    // Reset and test again
    it.reset(&it);
    ASSERT_EQ(it.has_next(&it), 1);
    data = it.next(&it);
    ASSERT_EQ(*(int*)data, 10);

    it.destroy(&it);
    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_iterator_empty_list(void){
    SinglyLinkedList *list = sll_create();

    Iterator it = sll_iterator(list);
    ASSERT_NOT_NULL(it.data_state);
    ASSERT_EQ(it.is_valid(&it), 1);
    ASSERT_EQ(it.has_next(&it), 0);
    ASSERT_NULL(it.next(&it));

    it.destroy(&it);
    sll_destroy(list, NULL);
    return TEST_SUCCESS;
}

int test_iterator_null_list(void){
    const Iterator it = sll_iterator(NULL);
    ASSERT_NULL(it.data_state);

    // No need to destroy, it's invalid
    return TEST_SUCCESS;
}

int test_iterator_get(void){
    SinglyLinkedList *list = sll_create();
    int *a = malloc(sizeof(int)); *a = 10;
    int *b = malloc(sizeof(int)); *b = 20;
    sll_insert_back(list, a);
    sll_insert_back(list, b);

    Iterator it = sll_iterator(list);

    // Test get() without advancing
    void *data = it.get(&it);
    ASSERT_NOT_NULL(data);
    ASSERT_EQ(*(int*)data, 10);

    // Should still be at first element
    data = it.next(&it);
    ASSERT_EQ(*(int*)data, 10);

    // Now at second element
    data = it.get(&it);
    ASSERT_EQ(*(int*)data, 20);

    it.destroy(&it);
    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_iterator_unsupported_ops(void) {
    SinglyLinkedList *list = sll_create();
    int *a = malloc(sizeof(int)); *a = 10;
    sll_insert_back(list, a);

    Iterator it = sll_iterator(list);

    // has_prev and prev shouldn't work for SLL
    ASSERT_EQ(it.has_prev(&it), 0);
    ASSERT_NULL(it.prev(&it));

    it.destroy(&it);
    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_from_iterator_basic(void) {
    SinglyLinkedList *list = sll_create();
    int *a = malloc(sizeof(int)); *a = 10;
    int *b = malloc(sizeof(int)); *b = 20;
    int *c = malloc(sizeof(int)); *c = 30;
    sll_insert_back(list, a);
    sll_insert_back(list, b);
    sll_insert_back(list, c);

    Iterator it = sll_iterator(list);

    // Create new list from iterator (shallow copy)
    SinglyLinkedList *new_list = sll_from_iterator(&it, NULL, NULL);
    ASSERT_NOT_NULL(new_list);
    ASSERT_EQ(new_list->size, 3);

    // Check that values are the same
    const SinglyLinkedNode *node = new_list->head;
    ASSERT_EQ(*(int*)node->data, 10);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 20);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 30);

    // Check that pointers are the same (shallow copy)
    const SinglyLinkedNode *orig = list->head;
    node = new_list->head;
    while (orig && node) {
        ASSERT_EQ(orig->data, node->data);
        orig = orig->next;
        node = node->next;
    }

    it.destroy(&it);
    sll_destroy(list, int_free);
    sll_destroy(new_list, NULL); // Don't free shared data
    return TEST_SUCCESS;
}

int test_from_iterator_deep_copy(void) {
    SinglyLinkedList *list = sll_create();
    int *a = malloc(sizeof(int)); *a = 10;
    int *b = malloc(sizeof(int)); *b = 20;
    int *c = malloc(sizeof(int)); *c = 30;
    sll_insert_back(list, a);
    sll_insert_back(list, b);
    sll_insert_back(list, c);

    Iterator it = sll_iterator(list);

    // Create new list from iterator with deep copy
    SinglyLinkedList *new_list = sll_from_iterator(&it, int_copy, int_free);
    ASSERT_NOT_NULL(new_list);
    ASSERT_EQ(new_list->size, 3);

    // Check that values are the same
    const SinglyLinkedNode *node = new_list->head;
    ASSERT_EQ(*(int*)node->data, 10);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 20);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 30);

    // Check that pointers are different (deep copy)
    const SinglyLinkedNode *orig = list->head;
    node = new_list->head;
    while (orig && node) {
        ASSERT_NOT_EQ(orig->data, node->data);
        orig = orig->next;
        node = node->next;
    }

    // Modify original list data
    *(int*)list->head->data = 99;

    // Check that new list data is unchanged
    ASSERT_EQ(*(int*)new_list->head->data, 10);

    it.destroy(&it);
    sll_destroy(list, int_free);
    sll_destroy(new_list, int_free); // Free independent copies
    return TEST_SUCCESS;
}

int test_from_iterator_empty(void) {
    SinglyLinkedList *list = sll_create();

    Iterator it = sll_iterator(list);

    // Create new list from empty iterator
    SinglyLinkedList *new_list = sll_from_iterator(&it, int_copy, int_free);
    ASSERT_NOT_NULL(new_list);
    ASSERT_EQ(new_list->size, 0);
    ASSERT_NULL(new_list->head);

    it.destroy(&it);
    sll_destroy(list, NULL);
    sll_destroy(new_list, NULL);
    return TEST_SUCCESS;
}

int test_from_iterator_null(void) {
    ASSERT_NULL(sll_from_iterator(NULL, int_copy, int_free));

    SinglyLinkedList *list = sll_create();
    Iterator it = sll_iterator(list);

    // Iterator should be valid but copy function is optional
    SinglyLinkedList *new_list = sll_from_iterator(&it, NULL, NULL);
    ASSERT_NOT_NULL(new_list);

    it.destroy(&it);
    sll_destroy(list, NULL);
    sll_destroy(new_list, NULL);
    return TEST_SUCCESS;
}

int test_iterator_chaining(void) {
    SinglyLinkedList *list = sll_create();
    for (int i = 0; i < 10; i++) {
        int *val = malloc(sizeof(int));
        *val = i;
        sll_insert_back(list, val);
    }

    // Create an iterator
    Iterator it = sll_iterator(list);

    // Make a second list from the first 5 elements
    SinglyLinkedList *list2 = sll_create();
    int count = 0;
    while (it.has_next(&it) && count < 5) {
        void *data = it.next(&it);
        int *copy_val = malloc(sizeof(int));
        *copy_val = *(int*)data;
        sll_insert_back(list2, copy_val);
        count++;
    }

    ASSERT_EQ(list2->size, 5);

    // Create an iterator for the second list
    Iterator it2 = sll_iterator(list2);

    // Create a third list from iterator of second list
    SinglyLinkedList *list3 = sll_from_iterator(&it2, int_copy, int_free);
    ASSERT_EQ(list3->size, 5);

    // Verify contents of third list
    const SinglyLinkedNode *node = list3->head;
    for (int i = 0; i < 5; i++) {
        ASSERT_EQ(*(int*)node->data, i);
        node = node->next;
    }

    it.destroy(&it);
    it2.destroy(&it2);
    sll_destroy(list, int_free);
    sll_destroy(list2, int_free);
    sll_destroy(list3, int_free);
    return TEST_SUCCESS;
}

// Static functions for invalid iterator testing
static int dummy_has_next(const Iterator *it) {
    (void)it;  // Unused parameter
    return 0;
}

static void *dummy_next(const Iterator *it) {
    (void)it;  // Unused parameter

    return NULL;
}

static int dummy_is_valid(const Iterator *it) {
    (void)it;  // Unused parameter

    return 0;
}

int test_from_iterator_null_edge_cases(void) {
    // Test with NULL iterator
    ASSERT_NULL(sll_from_iterator(NULL, int_copy, int_free));
    ASSERT_NULL(sll_from_iterator(NULL, NULL, NULL));

    // Test with invalid iterator (manually created)
    Iterator invalid_it = {0};  // All fields set to NULL/0
    ASSERT_NULL(sll_from_iterator(&invalid_it, int_copy, int_free));

    // Test with partially initialized iterator
    invalid_it.has_next = dummy_has_next;
    invalid_it.next = dummy_next;
    invalid_it.is_valid = dummy_is_valid; // explicitly invalid
    ASSERT_NULL(sll_from_iterator(&invalid_it, int_copy, int_free));

    // Create a valid iterator but destroy it
    SinglyLinkedList *list = sll_create();
    int *a = malloc(sizeof(int)); *a = 42;
    sll_insert_back(list, a);

    Iterator it = sll_iterator(list);
    it.destroy(&it);  // Destroy the iterator's state
    ASSERT_NULL(sll_from_iterator(&it, int_copy, int_free));  // Should handle destroyed iterator safely

    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_iterator_null_list_comprehensive(void) {
    // Create iterator from NULL
    Iterator it = sll_iterator(NULL);

    // Verify iterator is properly initialized as invalid
    ASSERT_NULL(it.data_state);

    // Test that all operations handle the null state gracefully
    ASSERT_EQ(it.has_next(&it), 0);
    ASSERT_NULL(it.next(&it));
    ASSERT_EQ(it.has_prev(&it), 0);
    ASSERT_NULL(it.prev(&it));
    ASSERT_NULL(it.get(&it));
    ASSERT_EQ(it.is_valid(&it), 0);

    // These operations should not crash with invalid iterator
    it.reset(&it);
    it.destroy(&it);

    return TEST_SUCCESS;
}

int test_multiple_iterators(void) {
    SinglyLinkedList *list = sll_create();
    for (int i = 0; i < 5; i++) {
        int *val = malloc(sizeof(int));
        *val = i;
        sll_insert_back(list, val);
    }

    // Create two iterators on the same list
    Iterator it1 = sll_iterator(list);
    Iterator it2 = sll_iterator(list);

    // Advance first iterator by 2
    it1.next(&it1);
    it1.next(&it1);

    // Second iterator should still be at the beginning
    ASSERT_EQ(*(int*)it2.get(&it2), 0);

    // Both iterators should be independent
    ASSERT_EQ(*(int*)it1.get(&it1), 2);
    ASSERT_EQ(*(int*)it2.next(&it2), 0);

    // Advance both and check
    ASSERT_EQ(*(int*)it1.next(&it1), 2);
    ASSERT_EQ(*(int*)it2.next(&it2), 1);

    it1.destroy(&it1);
    it2.destroy(&it2);
    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_iterator_with_modification(void) {
    SinglyLinkedList *list = sll_create();
    for (int i = 0; i < 5; i++) {
        int *val = malloc(sizeof(int));
        *val = i;
        sll_insert_back(list, val);
    }

    // Start iteration
    Iterator it = sll_iterator(list);

    // Get first element
    void *data = it.next(&it);
    ASSERT_EQ(*(int*)data, 0);

    // Modify the list by adding at the front
    int *new_val = malloc(sizeof(int));
    *new_val = 99;
    sll_insert_front(list, new_val);

    // Iterator should continue from where it was
    data = it.next(&it);
    ASSERT_EQ(*(int*)data, 1);

    // Reset and check the modified list
    it.reset(&it);
    data = it.next(&it);
    // Now the first element is 99
    ASSERT_EQ(*(int*)data, 99);

    it.destroy(&it);
    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_iterator_allocation_failure(void) {
    set_alloc_fail_countdown(-1);
    SinglyLinkedList *list = sll_create_custom(failing_alloc, failing_free);
    int *a = malloc(sizeof(int)); *a = 1;
    sll_insert_back(list, a);

    // Set allocator to fail on iterator state allocation
    set_alloc_fail_countdown(0);
    const Iterator it = sll_iterator(list);

    // Iterator should be invalid as state allocation failed
    ASSERT_NULL(it.data_state);
    ASSERT_EQ(it.is_valid(&it), 0);

    set_alloc_fail_countdown(-1);
    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_from_iterator_custom_alloc_failure(void) {
    set_alloc_fail_countdown(-1);
    SinglyLinkedList *list = sll_create();
    for (int i = 0; i < 5; i++) {
        int *val = malloc(sizeof(int)); *val = i;
        sll_insert_back(list, val);
    }
    Iterator it = sll_iterator(list);

    // Case 1: Fail on list creation
    set_alloc_fail_countdown(0);
    SinglyLinkedList *new_list1 = sll_from_iterator_custom(&it, failing_int_copy, failing_free, failing_alloc, failing_free);
    ASSERT_NULL(new_list1);
    it.reset(&it);

    // Case 2: Fail on data copy
    set_alloc_fail_countdown(1); // 1=new list, FAIL on data copy
    SinglyLinkedList *new_list2 = sll_from_iterator_custom(&it, failing_int_copy, failing_free, failing_alloc, failing_free);
    ASSERT_NULL(new_list2);
    it.reset(&it);

    // Case 3: Fail on node insertion
    set_alloc_fail_countdown(2); // 1=new list, 2=data copy, FAIL on node insert
    SinglyLinkedList *new_list3 = sll_from_iterator_custom(&it, failing_int_copy, failing_free, failing_alloc, failing_free);
    ASSERT_NULL(new_list3);

    it.destroy(&it);
    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

typedef struct {
    int (*func)(void);
    const char *name;
} TestCase;

TestCase tests[] = {
    {test_iterator_basic, "test_iterator_basic"},
    {test_iterator_empty_list, "test_iterator_empty_list"},
    {test_iterator_null_list, "test_iterator_null_list"},
    {test_iterator_get, "test_iterator_get"},
    {test_iterator_unsupported_ops, "test_iterator_unsupported_ops"},
    {test_from_iterator_basic, "test_from_iterator_basic"},
    {test_from_iterator_deep_copy, "test_from_iterator_deep_copy"},
    {test_from_iterator_empty, "test_from_iterator_empty"},
    {test_from_iterator_null, "test_from_iterator_null"},
    {test_iterator_chaining, "test_iterator_chaining"},
    {test_from_iterator_null_edge_cases, "test_from_iterator_null_edge_cases"},
    {test_iterator_null_list_comprehensive, "test_iterator_null_list_comprehensive"},
    {test_multiple_iterators, "test_multiple_iterators"},
    {test_iterator_with_modification, "test_iterator_with_modification"},
    {test_iterator_allocation_failure, "test_iterator_allocation_failure"},
    {test_from_iterator_custom_alloc_failure, "test_from_iterator_custom_alloc_failure"},
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
        printf("All SinglyLinkedList Iterator tests passed.\n");
        return 0;
    }

    printf("%d SinglyLinkedList Iterator tests failed.\n", failed);
    return 1;
}