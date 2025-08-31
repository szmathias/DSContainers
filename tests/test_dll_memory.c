//
// Created by zack on 9/2/25.
//

#include "DoublyLinkedList.h"
#include "TestAssert.h"
#include "TestHelpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int test_custom_allocator(void) {
    DoublyLinkedList *list = dll_create_custom(test_calloc, test_dealloc);
    ASSERT_NOT_NULL(list);
    int *a = malloc(sizeof(int)); *a = 42;
    ASSERT_EQ(dll_insert_back(list, a), 0);
    ASSERT_EQ(list->size, 1);
    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_clear(void) {
    DoublyLinkedList *list = dll_create();

    // Add some elements
    for (int i = 0; i < 5; i++) {
        int *val = malloc(sizeof(int));
        *val = i;
        dll_insert_back(list, val);
    }
    ASSERT_EQ(list->size, 5);

    // Clear the list
    dll_clear(list, int_free);

    // Verify list state
    ASSERT_NULL(list->head);
    ASSERT_NULL(list->tail);
    ASSERT_EQ(list->size, 0);
    ASSERT_EQ(dll_is_empty(list), 1);

    // Make sure we can still add elements after clearing
    int *val = malloc(sizeof(int));
    *val = 42;
    ASSERT_EQ(dll_insert_back(list, val), 0);
    ASSERT_EQ(list->size, 1);

    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_clear_empty(void) {
    DoublyLinkedList *list = dll_create();

    // Clear an already empty list
    dll_clear(list, int_free);
    ASSERT_NULL(list->head);
    ASSERT_NULL(list->tail);
    ASSERT_EQ(list->size, 0);

    dll_destroy(list, NULL);
    return TEST_SUCCESS;
}

int test_clear_null(void) {
    // Calling clear on NULL shouldn't crash
    dll_clear(NULL, int_free);
    return TEST_SUCCESS;
}

int test_stress(void) {
    DoublyLinkedList *list = dll_create();
    const int NUM_ELEMENTS = 10000;

    // Add many elements
    for (int i = 0; i < NUM_ELEMENTS; i++) {
        int *val = malloc(sizeof(int));
        *val = i;
        ASSERT_EQ(dll_insert_back(list, val), 0);
    }
    ASSERT_EQ(list->size, (size_t)NUM_ELEMENTS);

    // Find an element in the middle
    int key = NUM_ELEMENTS / 2;
    const DoublyLinkedNode *found = dll_find(list, &key, int_cmp);
    ASSERT_NOT_NULL(found);
    ASSERT_EQ(*(int*)found->data, key);

    // Remove elements from the front
    for (int i = 0; i < NUM_ELEMENTS / 2; i++) {
        ASSERT_EQ(dll_remove_front(list, int_free), 0);
    }
    ASSERT_EQ(list->size, (size_t)NUM_ELEMENTS / 2);

    // The first element should now be NUM_ELEMENTS/2
    key = NUM_ELEMENTS / 2;
    ASSERT_EQ(*(int*)list->head->data, key);

    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_performance(void) {
    const int SIZES[] = {100, 1000, 10000};
    const size_t NUM_SIZES = sizeof(SIZES)/sizeof(SIZES[0]);

    printf("\nDLL Performance tests:\n");
    for (size_t s = 0; s < NUM_SIZES; s++) {
        const int SIZE = SIZES[s];
        DoublyLinkedList *list = dll_create();

        // Measure insertion time
        clock_t start = clock();
        for (int i = 0; i < SIZE; i++) {
            int *val = malloc(sizeof(int));
            *val = i;
            dll_insert_back(list, val);
        }
        clock_t end = clock();
        printf("Insert %d elements: %.6f seconds\n", SIZE,
               (double)(end - start) / CLOCKS_PER_SEC);

        // Measure search time for last element
        start = clock();
        int key = SIZE - 1;
        const DoublyLinkedNode *found = dll_find(list, &key, int_cmp);
        end = clock();
        printf("Find last element in %d elements: %.6f seconds\n", SIZE,
               (double)(end - start) / CLOCKS_PER_SEC);
        ASSERT_NOT_NULL(found);

        // Cleanup
        dll_destroy(list, int_free);
    }

    return TEST_SUCCESS;
}

int test_copy_shallow(void) {
    DoublyLinkedList *list = dll_create();

    // Add some elements
    for (int i = 0; i < 5; i++) {
        int *val = malloc(sizeof(int));
        *val = i * 10;
        dll_insert_back(list, val);
    }

    // Create shallow clone
    DoublyLinkedList *copy = dll_copy(list);
    ASSERT_NOT_NULL(copy);
    ASSERT_EQ(copy->size, list->size);

    // Verify structure
    DoublyLinkedNode *orig_node = list->head;
    DoublyLinkedNode *copy_node = copy->head;
    while (orig_node && copy_node) {
        // Data pointers should be identical in shallow clone
        ASSERT_EQ(orig_node->data, copy_node->data);
        // But nodes themselves should be different
        ASSERT_NOT_EQ(orig_node, copy_node);

        // Verify next pointers
        if (orig_node->next) {
            ASSERT_NOT_EQ(orig_node->next, copy_node->next);
        } else {
            ASSERT_NULL(copy_node->next);
        }

        // Verify prev pointers
        if (orig_node->prev) {
            ASSERT_NOT_EQ(orig_node->prev, copy_node->prev);
        } else {
            ASSERT_NULL(copy_node->prev);
        }

        orig_node = orig_node->next;
        copy_node = copy_node->next;
    }

    // Verify bidirectional links in the clone
    ASSERT_NULL(copy->head->prev);
    ASSERT_NULL(copy->tail->next);

    // Modifying data should affect both lists (shared pointers)
    int *first_value = list->head->data;
    *first_value = 999;
    ASSERT_EQ(*(int*)copy->head->data, 999);

    // Cleanup - free each int only once since they're shared
    dll_destroy(list, int_free);
    dll_destroy(copy, NULL);

    return TEST_SUCCESS;
}

int test_copy_deep(void) {
    DoublyLinkedList *list = dll_create();

    // Add some elements
    for (int i = 0; i < 5; i++) {
        int *val = malloc(sizeof(int));
        *val = i * 10;
        dll_insert_back(list, val);
    }

    // Create deep clone
    DoublyLinkedList *copy = dll_copy_deep(list, int_copy, free);
    ASSERT_NOT_NULL(copy);
    ASSERT_EQ(copy->size, list->size);

    // Verify structure and values
    const DoublyLinkedNode *orig_node = list->head;
    const DoublyLinkedNode *copy_node = copy->head;
    while (orig_node && copy_node) {
        // Data pointers should be different in deep clone
        ASSERT_NOT_EQ(orig_node->data, copy_node->data);
        // But values should be the same
        ASSERT_EQ(*(int*)orig_node->data, *(int*)copy_node->data);

        orig_node = orig_node->next;
        copy_node = copy_node->next;
    }

    // Verify bidirectional links in the clone
    ASSERT_NULL(copy->head->prev);
    ASSERT_NULL(copy->tail->next);

    // Check traversal in reverse
    orig_node = list->tail;
    copy_node = copy->tail;
    while (orig_node && copy_node) {
        ASSERT_NOT_EQ(orig_node->data, copy_node->data);
        ASSERT_EQ(*(int*)orig_node->data, *(int*)copy_node->data);

        orig_node = orig_node->prev;
        copy_node = copy_node->prev;
    }

    // Modifying data should not affect the other list (independent copies)
    int *first_value = list->head->data;
    *first_value = 999;
    ASSERT_NOT_EQ(*(int*)copy->head->data, 999);

    // Cleanup - each list has its own data
    dll_destroy(list, int_free);
    dll_destroy(copy, int_free);

    return TEST_SUCCESS;
}

int test_copy_complex_data(void) {
    DoublyLinkedList *list = dll_create();

    // Add some people
    Person *p1 = create_person("Alice", 30);
    Person *p2 = create_person("Bob", 25);
    Person *p3 = create_person("Charlie", 40);

    dll_insert_back(list, p1);
    dll_insert_back(list, p2);
    dll_insert_back(list, p3);
    ASSERT_EQ(list->size, 3);

    // Create deep clone
    DoublyLinkedList *copy = dll_copy_deep(list, person_copy, person_free);
    ASSERT_NOT_NULL(copy);
    ASSERT_EQ(copy->size, list->size);

    // Verify structure and values
    const DoublyLinkedNode *orig_node = list->head;
    const DoublyLinkedNode *copy_node = copy->head;
    while (orig_node && copy_node) {
        Person *orig_person = orig_node->data;
        Person *clone_person = copy_node->data;

        // Data pointers should be different
        ASSERT_NOT_EQ(orig_person, clone_person);
        // But values should be the same
        ASSERT_EQ(strcmp(orig_person->name, clone_person->name), 0);
        ASSERT_EQ(orig_person->age, clone_person->age);

        orig_node = orig_node->next;
        copy_node = copy_node->next;
    }

    // Modifying should not affect the other list
    Person *first_person = list->head->data;
    first_person->age = 99;
    const Person *copy_first = copy->head->data;
    ASSERT_NOT_EQ(first_person->age, copy_first->age);

    // Cleanup
    dll_destroy(list, person_free);
    dll_destroy(copy, person_free);

    return TEST_SUCCESS;
}

int test_copy_empty(void) {
    DoublyLinkedList *list = dll_create();

    // Clone empty list
    DoublyLinkedList *shallow_copy = dll_copy(list);
    ASSERT_NOT_NULL(shallow_copy);
    ASSERT_EQ(shallow_copy->size, 0);
    ASSERT_NULL(shallow_copy->head);
    ASSERT_NULL(shallow_copy->tail);

    DoublyLinkedList *deep_copy = dll_copy_deep(list, int_copy, free);
    ASSERT_NOT_NULL(deep_copy);
    ASSERT_EQ(deep_copy->size, 0);
    ASSERT_NULL(deep_copy->head);
    ASSERT_NULL(deep_copy->tail);

    // Cleanup
    dll_destroy(list, NULL);
    dll_destroy(shallow_copy, NULL);
    dll_destroy(deep_copy, NULL);

    return TEST_SUCCESS;
}

int test_copy_null(void) {
    // Should handle NULL gracefully
    ASSERT_NULL(dll_copy(NULL));
    ASSERT_NULL(dll_copy_deep(NULL, int_copy, free));

    // Should require a valid copy function
    DoublyLinkedList *list = dll_create();
    ASSERT_NULL(dll_copy_deep(list, NULL, free));
    dll_destroy(list, NULL);

    return TEST_SUCCESS;
}

int test_insert_allocation_failure(void) {
    set_alloc_fail_countdown(-1);
    DoublyLinkedList *list = dll_create_custom(failing_alloc, failing_free);
    int *a = malloc(sizeof(int)); *a = 1;
    dll_insert_back(list, a);
    ASSERT_EQ(list->size, 1);

    // Set allocator to fail on the next allocation (for the node)
    set_alloc_fail_countdown(0);
    int *b = malloc(sizeof(int)); *b = 2;
    ASSERT_EQ(dll_insert_back(list, b), -1);

    // Verify list is unchanged
    ASSERT_EQ(list->size, 1);
    ASSERT_NOT_NULL(list->head);
    ASSERT_EQ(list->head, list->tail);
    ASSERT_NULL(list->head->next);

    set_alloc_fail_countdown(-1);
    dll_destroy(list, int_free);
    free(b); // 'b' was never added to the list, so we must free it manually
    return TEST_SUCCESS;
}

int test_copy_deep_allocation_failure(void) {
    set_alloc_fail_countdown(-1);
    DoublyLinkedList *list = dll_create_custom(failing_alloc, failing_free);
    for (int i = 0; i < 5; i++) {
        int *val = malloc(sizeof(int)); *val = i;
        dll_insert_back(list, val);
    }

    // Case 1: Fail allocating the new list struct itself
    set_alloc_fail_countdown(0);
    DoublyLinkedList *clone1 = dll_copy_deep(list, failing_int_copy, failing_free);
    ASSERT_NULL(clone1);

    // Case 2: Fail allocating the data partway through
    set_alloc_fail_countdown(3); // 1=clone list, 2=data0, 3=node0, FAIL on data1
    DoublyLinkedList *clone2 = dll_copy_deep(list, failing_int_copy, failing_free);
    ASSERT_NULL(clone2);

    // Case 3: Fail allocating a node partway through
    set_alloc_fail_countdown(2); // 1=clone list, 2=data0, FAIL on node0
    DoublyLinkedList *clone3 = dll_copy_deep(list, failing_int_copy, failing_free);
    ASSERT_NULL(clone3);

    set_alloc_fail_countdown(-1); // Reset for cleanup
    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_transform_allocation_failure(void) {
    set_alloc_fail_countdown(-1);
    DoublyLinkedList *list = dll_create_custom(failing_alloc, failing_free);
    for (int i = 0; i < 5; i++) {
        int *val = malloc(sizeof(int)); *val = i;
        dll_insert_back(list, val);
    }

    // Case 1: Fail on creation of the result list
    set_alloc_fail_countdown(0);
    DoublyLinkedList *mapped1 = dll_transform(list, double_value_failing, failing_free);
    ASSERT_NULL(mapped1);

    // Case 2: Fail on data allocation inside the transform function
    set_alloc_fail_countdown(1); // 1=result list, FAIL on data for first element
    DoublyLinkedList *mapped2 = dll_transform(list, double_value_failing, failing_free);
    ASSERT_NULL(mapped2);

    // Case 3: Fail on node allocation inside dll_insert_back
    set_alloc_fail_countdown(2); // 1=result list, 2=data for first element, FAIL on 3=node
    DoublyLinkedList *mapped3 = dll_transform(list, double_value_failing, failing_free);
    ASSERT_NULL(mapped3);

    set_alloc_fail_countdown(-1);
    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_from_iterator_custom_alloc_failure(void) {
    set_alloc_fail_countdown(-1);
    DoublyLinkedList *list = dll_create_custom(failing_alloc, failing_free);
    for (int i = 0; i < 5; i++) {
        int *val = malloc(sizeof(int)); *val = i;
        dll_insert_back(list, val);
    }
    Iterator it = dll_iterator(list);

    // Case 1: Fail on list creation
    set_alloc_fail_countdown(0);
    DoublyLinkedList *new_list1 = dll_from_iterator_custom(&it, failing_int_copy, failing_free, failing_alloc, failing_free);
    ASSERT_NULL(new_list1);
    it.reset(&it);

    // Case 2: Fail on data copy
    set_alloc_fail_countdown(1); // 1=new list, FAIL on data copy
    DoublyLinkedList *new_list2 = dll_from_iterator_custom(&it, failing_int_copy, failing_free, failing_alloc, failing_free);
    ASSERT_NULL(new_list2);
    it.reset(&it);

    // Case 3: Fail on node insertion
    set_alloc_fail_countdown(2); // 1=new list, 2=data copy, FAIL on node insert
    DoublyLinkedList *new_list3 = dll_from_iterator_custom(&it, failing_int_copy, failing_free, failing_alloc, failing_free);
    ASSERT_NULL(new_list3);

    it.destroy(&it);
    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

typedef struct {
    int (*func)(void);
    const char *name;
} TestCase;

TestCase tests[] = {
    {test_custom_allocator, "test_custom_allocator"},
    {test_clear, "test_clear"},
    {test_clear_empty, "test_clear_empty"},
    {test_clear_null, "test_clear_null"},
    {test_stress, "test_stress"},
    {test_performance, "test_performance"},
    {test_copy_shallow, "test_copy_shallow"},
    {test_copy_deep, "test_copy_deep"},
    {test_copy_complex_data, "test_copy_complex_data"},
    {test_copy_empty, "test_copy_empty"},
    {test_copy_null, "test_copy_null"},
    {test_insert_allocation_failure, "test_insert_allocation_failure"},
    {test_copy_deep_allocation_failure, "test_copy_deep_allocation_failure"},
    {test_transform_allocation_failure, "test_transform_allocation_failure"},
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
        printf("All DoublyLinkedList Memory tests passed.\n");
        return 0;
    }

    printf("%d DoublyLinkedList Memory tests failed.\n", failed);
    return 1;
}