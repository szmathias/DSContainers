//
// Created by zack on 9/2/25.
//

#include "SinglyLinkedList.h"
#include "TestAssert.h"
#include "TestHelpers.h"
#include <stdio.h>
#include <stdlib.h>

// Tests from test_sll.c that fit into iterator category
int test_iterator_basic(void)
{
    DSCAlloc* alloc           = create_std_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(alloc);
    int* a                    = malloc(sizeof(int));
    *a                        = 10;
    int* b                    = malloc(sizeof(int));
    *b                        = 20;
    int* c                    = malloc(sizeof(int));
    *c                        = 30;
    dsc_sll_insert_back(list, a);
    dsc_sll_insert_back(list, b);
    dsc_sll_insert_back(list, c);

    DSCIterator it = dsc_sll_iterator(list);
    ASSERT_NOT_NULL(it.data_state);
    ASSERT_EQ(it.is_valid(&it), 1);
    ASSERT_EQ(it.has_next(&it), 1);

    // Test getting first element
    void* data = it.next(&it);
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
    dsc_sll_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_iterator_empty_list(void)
{
    DSCAlloc* alloc           = create_std_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(alloc);

    DSCIterator it = dsc_sll_iterator(list);
    ASSERT_NOT_NULL(it.data_state);
    ASSERT_EQ(it.is_valid(&it), 1);
    ASSERT_EQ(it.has_next(&it), 0);
    ASSERT_NULL(it.next(&it));

    it.destroy(&it);
    dsc_sll_destroy(list, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_iterator_null_list(void)
{
    const DSCIterator it = dsc_sll_iterator(NULL);
    ASSERT_NULL(it.data_state);

    // No need to destroy, it's invalid
    return TEST_SUCCESS;
}

int test_iterator_get(void)
{
    DSCAlloc* alloc           = create_std_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(alloc);
    int* a                    = malloc(sizeof(int));
    *a                        = 10;
    int* b                    = malloc(sizeof(int));
    *b                        = 20;
    dsc_sll_insert_back(list, a);
    dsc_sll_insert_back(list, b);

    DSCIterator it = dsc_sll_iterator(list);

    // Test get() without advancing
    void* data = it.get(&it);
    ASSERT_NOT_NULL(data);
    ASSERT_EQ(*(int*)data, 10);

    // Should still be at first element
    data = it.next(&it);
    ASSERT_EQ(*(int*)data, 10);

    // Now at second element
    data = it.get(&it);
    ASSERT_EQ(*(int*)data, 20);

    it.destroy(&it);
    dsc_sll_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_iterator_unsupported_ops(void)
{
    DSCAlloc* alloc           = create_std_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(alloc);
    int* a                    = malloc(sizeof(int));
    *a                        = 10;
    dsc_sll_insert_back(list, a);

    DSCIterator it = dsc_sll_iterator(list);

    // has_prev and prev shouldn't work for SLL
    ASSERT_EQ(it.has_prev(&it), 0);
    ASSERT_NULL(it.prev(&it));

    it.destroy(&it);
    dsc_sll_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_from_iterator_basic(void)
{
    DSCAlloc* alloc           = create_std_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(alloc);
    int* a                    = malloc(sizeof(int));
    *a                        = 10;
    int* b                    = malloc(sizeof(int));
    *b                        = 20;
    int* c                    = malloc(sizeof(int));
    *c                        = 30;
    dsc_sll_insert_back(list, a);
    dsc_sll_insert_back(list, b);
    dsc_sll_insert_back(list, c);

    DSCIterator it = dsc_sll_iterator(list);

    // Create new list from iterator (shallow copy)
    DSCAlloc* new_alloc           = create_std_allocator();
    new_alloc->copy_func          = NULL;
    DSCSinglyLinkedList* new_list = dsc_sll_from_iterator(&it, new_alloc);
    ASSERT_NOT_NULL(new_list);
    ASSERT_EQ(new_list->size, 3);

    // Check that values are the same
    const DSCSinglyLinkedNode* node = new_list->head;
    ASSERT_EQ(*(int*)node->data, 10);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 20);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 30);

    // Check that pointers are the same (shallow copy)
    const DSCSinglyLinkedNode* orig = list->head;
    node                            = new_list->head;
    while (orig && node)
    {
        ASSERT_EQ(orig->data, node->data);
        orig = orig->next;
        node = node->next;
    }

    it.destroy(&it);
    dsc_sll_destroy(list, true);
    dsc_sll_destroy(new_list, false); // Don't free shared data
    destroy_allocator(new_alloc);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_from_iterator_deep_copy(void)
{
    DSCAlloc* alloc           = create_std_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(alloc);
    int* a                    = malloc(sizeof(int));
    *a                        = 10;
    int* b                    = malloc(sizeof(int));
    *b                        = 20;
    int* c                    = malloc(sizeof(int));
    *c                        = 30;
    dsc_sll_insert_back(list, a);
    dsc_sll_insert_back(list, b);
    dsc_sll_insert_back(list, c);

    DSCIterator it = dsc_sll_iterator(list);

    // Create new list from iterator with deep copy
    DSCAlloc* deep_alloc          = create_std_allocator();
    deep_alloc->copy_func         = int_copy;
    deep_alloc->data_free_func    = int_free;
    DSCSinglyLinkedList* new_list = dsc_sll_from_iterator(&it, deep_alloc);
    ASSERT_NOT_NULL(new_list);
    ASSERT_EQ(new_list->size, 3);

    // Check that values are the same
    const DSCSinglyLinkedNode* node = new_list->head;
    ASSERT_EQ(*(int*)node->data, 10);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 20);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 30);

    // Check that pointers are different (deep copy)
    const DSCSinglyLinkedNode* orig = list->head;
    node                            = new_list->head;
    while (orig && node)
    {
        ASSERT_NOT_EQ(orig->data, node->data);
        orig = orig->next;
        node = node->next;
    }

    // Modify original list data
    *(int*)list->head->data = 99;

    // Check that new list data is unchanged
    ASSERT_EQ(*(int*)new_list->head->data, 10);

    it.destroy(&it);
    dsc_sll_destroy(list, true);
    dsc_sll_destroy(new_list, true); // Free independent copies
    destroy_allocator(deep_alloc);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_from_iterator_empty(void)
{
    DSCAlloc* alloc           = create_std_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(alloc);

    DSCIterator it = dsc_sll_iterator(list);

    // Create new list from empty iterator
    DSCAlloc* new_alloc           = create_std_allocator();
    new_alloc->copy_func          = int_copy; // optional, doesn't matter for empty
    DSCSinglyLinkedList* new_list = dsc_sll_from_iterator(&it, new_alloc);
    ASSERT_NOT_NULL(new_list);
    ASSERT_EQ(new_list->size, 0);
    ASSERT_NULL(new_list->head);

    it.destroy(&it);
    dsc_sll_destroy(list, false);
    dsc_sll_destroy(new_list, false);
    destroy_allocator(new_alloc);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_from_iterator_null(void)
{
    ASSERT_NULL(dsc_sll_from_iterator(NULL, NULL));

    DSCAlloc* alloc           = create_std_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(alloc);
    DSCIterator it            = dsc_sll_iterator(list);

    // Iterator should be valid but copy function is optional
    DSCAlloc* new_alloc           = create_std_allocator();
    DSCSinglyLinkedList* new_list = dsc_sll_from_iterator(&it, new_alloc);
    ASSERT_NOT_NULL(new_list);

    it.destroy(&it);
    dsc_sll_destroy(list, false);
    dsc_sll_destroy(new_list, false);
    destroy_allocator(new_alloc);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_iterator_chaining(void)
{
    DSCAlloc* alloc           = create_std_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(alloc);
    for (int i = 0; i < 10; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_sll_insert_back(list, val);
    }

    // Create an iterator
    DSCIterator it = dsc_sll_iterator(list);

    // Make a second list from the first 5 elements
    DSCAlloc* alloc2           = create_std_allocator();
    DSCSinglyLinkedList* list2 = dsc_sll_create(alloc2);
    int count                  = 0;
    while (it.has_next(&it) && count < 5)
    {
        void* data    = it.next(&it);
        int* copy_val = malloc(sizeof(int));
        *copy_val     = *(int*)data;
        dsc_sll_insert_back(list2, copy_val);
        count++;
    }

    ASSERT_EQ(list2->size, 5);

    // Create an iterator for the second list
    DSCIterator it2 = dsc_sll_iterator(list2);

    // Create a third list from iterator of second list
    DSCAlloc* tmp_alloc        = create_std_allocator();
    tmp_alloc->copy_func       = int_copy;
    tmp_alloc->data_free_func  = int_free;
    DSCSinglyLinkedList* list3 = dsc_sll_from_iterator(&it2, tmp_alloc);
    ASSERT_EQ(list3->size, 5);

    // Verify contents of third list
    const DSCSinglyLinkedNode* node = list3->head;
    for (int i = 0; i < 5; i++)
    {
        ASSERT_EQ(*(int*)node->data, i);
        node = node->next;
    }

    it.destroy(&it);
    it2.destroy(&it2);
    dsc_sll_destroy(list, true);
    dsc_sll_destroy(list2, true);
    dsc_sll_destroy(list3, true);
    destroy_allocator(tmp_alloc);
    destroy_allocator(alloc2);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Static functions for invalid iterator testing
static int dummy_has_next(const DSCIterator* it)
{
    (void)it; // Unused parameter
    return 0;
}

static void* dummy_next(const DSCIterator* it)
{
    (void)it; // Unused parameter

    return NULL;
}

static int dummy_is_valid(const DSCIterator* it)
{
    (void)it; // Unused parameter

    return 0;
}

int test_from_iterator_null_edge_cases(void)
{
    // Test with NULL iterator
    DSCAlloc* tmp1       = create_std_allocator();
    tmp1->copy_func      = int_copy;
    tmp1->data_free_func = int_free;
    ASSERT_NULL(dsc_sll_from_iterator(NULL, tmp1));
    destroy_allocator(tmp1);

    DSCAlloc* tmp2 = create_std_allocator();
    ASSERT_NULL(dsc_sll_from_iterator(NULL, tmp2));
    destroy_allocator(tmp2);

    // Test with invalid iterator (manually created)
    DSCIterator invalid_it = {0}; // All fields set to NULL/0
    DSCAlloc* tmp3         = create_std_allocator();
    tmp3->copy_func        = int_copy;
    tmp3->data_free_func   = int_free;
    ASSERT_NULL(dsc_sll_from_iterator(&invalid_it, tmp3));
    destroy_allocator(tmp3);

    // Test with partially initialized iterator
    invalid_it.has_next  = dummy_has_next;
    invalid_it.next      = dummy_next;
    invalid_it.is_valid  = dummy_is_valid; // explicitly invalid
    DSCAlloc* tmp4       = create_std_allocator();
    tmp4->copy_func      = int_copy;
    tmp4->data_free_func = int_free;
    ASSERT_NULL(dsc_sll_from_iterator(&invalid_it, tmp4));
    destroy_allocator(tmp4);

    // Create a valid iterator but destroy it
    DSCAlloc* alloc           = create_std_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(alloc);
    int* a                    = malloc(sizeof(int));
    *a                        = 42;
    dsc_sll_insert_back(list, a);

    DSCIterator it = dsc_sll_iterator(list);
    it.destroy(&it); // Destroy the iterator's state
    DSCAlloc* tmp5 = create_std_allocator();
    ASSERT_NULL(dsc_sll_from_iterator(&it, tmp5)); // Should handle destroyed iterator safely
    destroy_allocator(tmp5);

    dsc_sll_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_iterator_null_list_comprehensive(void)
{
    // Create iterator from NULL
    DSCIterator it = dsc_sll_iterator(NULL);

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

int test_multiple_iterators(void)
{
    DSCAlloc* alloc           = create_std_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(alloc);
    for (int i = 0; i < 5; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_sll_insert_back(list, val);
    }

    // Create two iterators on the same list
    DSCIterator it1 = dsc_sll_iterator(list);
    DSCIterator it2 = dsc_sll_iterator(list);

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
    dsc_sll_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_iterator_with_modification(void)
{
    DSCAlloc* alloc           = create_std_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(alloc);
    for (int i = 0; i < 5; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_sll_insert_back(list, val);
    }

    // Start iteration
    DSCIterator it = dsc_sll_iterator(list);

    // Get first element
    void* data = it.next(&it);
    ASSERT_EQ(*(int*)data, 0);

    // Modify the list by adding at the front
    int* new_val = malloc(sizeof(int));
    *new_val     = 99;
    dsc_sll_insert_front(list, new_val);

    // Iterator should continue from where it was
    data = it.next(&it);
    ASSERT_EQ(*(int*)data, 1);

    // Reset and check the modified list
    it.reset(&it);
    data = it.next(&it);
    // Now the first element is 99
    ASSERT_EQ(*(int*)data, 99);

    it.destroy(&it);
    dsc_sll_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_iterator_allocation_failure(void)
{
    set_alloc_fail_countdown(-1);
    DSCAlloc* failAlloc       = create_failing_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(failAlloc);
    int* a                    = malloc(sizeof(int));
    *a                        = 1;
    dsc_sll_insert_back(list, a);

    // Set allocator to fail on iterator state allocation
    set_alloc_fail_countdown(0);
    const DSCIterator it = dsc_sll_iterator(list);

    // Iterator should be invalid as state allocation failed
    ASSERT_NULL(it.data_state);
    ASSERT_EQ(it.is_valid(&it), 0);

    set_alloc_fail_countdown(-1);
    dsc_sll_destroy(list, true);
    destroy_allocator(failAlloc);
    return TEST_SUCCESS;
}

int test_from_iterator_custom_alloc_failure(void)
{
    set_alloc_fail_countdown(-1);
    DSCAlloc* alloc           = create_std_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(alloc);
    for (int i = 0; i < 5; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_sll_insert_back(list, val);
    }
    DSCIterator it = dsc_sll_iterator(list);

    // Case 1: Fail on list creation
    set_alloc_fail_countdown(0);
    DSCAlloc* failing = create_failing_allocator();
    // Fail on new list allocation
    DSCSinglyLinkedList* new_list1 = dsc_sll_from_iterator(&it, failing);
    ASSERT_NULL(new_list1);
    it.reset(&it);

    // Case 2: Fail on data copy - use failing allocator but with copy func that fails
    set_alloc_fail_countdown(1);
    DSCAlloc* failing_copy_alloc       = create_failing_allocator();
    failing_copy_alloc->copy_func      = failing_int_copy;
    failing_copy_alloc->data_free_func = failing_free;
    DSCSinglyLinkedList* new_list2     = dsc_sll_from_iterator(&it, failing_copy_alloc);
    ASSERT_NULL(new_list2);
    it.reset(&it);

    // Case 3: Fail on node insertion - set countdown accordingly
    set_alloc_fail_countdown(2);
    DSCAlloc* failing_node_alloc       = create_failing_allocator();
    failing_node_alloc->copy_func      = failing_int_copy;
    failing_node_alloc->data_free_func = failing_free;
    DSCSinglyLinkedList* new_list3     = dsc_sll_from_iterator(&it, failing_node_alloc);
    ASSERT_NULL(new_list3);

    it.destroy(&it);
    dsc_sll_destroy(list, true);
    destroy_allocator(alloc);
    destroy_allocator(failing);
    destroy_allocator(failing_copy_alloc);
    destroy_allocator(failing_node_alloc);
    return TEST_SUCCESS;
}

typedef struct
{
    int (*func)(void);
    const char* name;
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
        printf("All SinglyLinkedList Iterator tests passed.\n");
        return 0;
    }

    printf("%d SinglyLinkedList Iterator tests failed.\n", failed);
    return 1;
}