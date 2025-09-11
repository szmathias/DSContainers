//
// Created by zack on 9/2/25.
//

#include "SinglyLinkedList.h"
#include "TestAssert.h"
#include "TestHelpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Tests from test_sll.c that fit into algorithms category
int test_sort_empty(void)
{
    DSCAllocator alloc           = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);

    ASSERT_EQ(dsc_sll_sort(list, int_cmp), 0); // Empty list is already sorted
    ASSERT_EQ(list->size, 0);

    dsc_sll_destroy(list, false);
    return TEST_SUCCESS;
}

int test_sort_already_sorted(void)
{
    DSCAllocator alloc           = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);
    for (int i = 0; i < 5; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_sll_insert_back(list, val);
    }

    ASSERT_EQ(dsc_sll_sort(list, int_cmp), 0);

    // Verify order
    const DSCSinglyLinkedNode* node = list->head;
    for (int i = 0; i < 5; i++)
    {
        ASSERT_EQ(*(int*)node->data, i);
        node = node->next;
    }

    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

int test_sort_reverse_order(void)
{
    DSCAllocator alloc           = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);
    for (int i = 4; i >= 0; i--)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_sll_insert_back(list, val);
    }

    ASSERT_EQ(dsc_sll_sort(list, int_cmp), 0);

    // Verify order
    const DSCSinglyLinkedNode* node = list->head;
    for (int i = 0; i < 5; i++)
    {
        ASSERT_EQ(*(int*)node->data, i);
        node = node->next;
    }

    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

int test_sort_random_order(void)
{
    DSCAllocator alloc           = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);
    const int values[]        = {42, 17, 9, 39, 24, 5, 58};
    const int count           = sizeof(values) / sizeof(values[0]);

    for (int i = 0; i < count; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = values[i];
        dsc_sll_insert_back(list, val);
    }

    ASSERT_EQ(dsc_sll_sort(list, int_cmp), 0);

    // Verify order
    const DSCSinglyLinkedNode* node = list->head;
    for (int i = 0; i < count; i++)
    {
        const int sorted[] = {5, 9, 17, 24, 39, 42, 58};
        ASSERT_EQ(*(int*)node->data, sorted[i]);
        node = node->next;
    }

    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

int test_sort_with_duplicates(void)
{
    DSCAllocator alloc           = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);
    const int values[]        = {5, 2, 9, 5, 7, 2, 9, 5};
    const int count           = sizeof(values) / sizeof(values[0]);

    for (int i = 0; i < count; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = values[i];
        dsc_sll_insert_back(list, val);
    }

    ASSERT_EQ(dsc_sll_sort(list, int_cmp), 0);

    // Verify order
    const DSCSinglyLinkedNode* node = list->head;
    for (int i = 0; i < count; i++)
    {
        const int sorted[] = {2, 2, 5, 5, 5, 7, 9, 9};
        ASSERT_EQ(*(int*)node->data, sorted[i]);
        node = node->next;
    }

    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

int test_sort_large_list(void)
{
    DSCAllocator alloc           = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);
    const int SIZE            = 1000;

    // Insert in reverse order
    for (int i = SIZE - 1; i >= 0; i--)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_sll_insert_back(list, val);
    }

    const clock_t start = clock();
    ASSERT_EQ(dsc_sll_sort(list, int_cmp), 0);
    const clock_t end = clock();
    printf("SLL Sort %d elements: %.6f seconds\n", SIZE,
           (double)(end - start) / CLOCKS_PER_SEC);

    // Verify order (first few elements)
    const DSCSinglyLinkedNode* node = list->head;
    for (int i = 0; i < 10; i++)
    {
        ASSERT_EQ(*(int*)node->data, i);
        node = node->next;
    }

    // Verify list structure
    ASSERT_EQ(list->size, (size_t)SIZE);

    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

int test_sort_custom_compare(void)
{
    DSCAllocator alloc           = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);
    for (int i = 0; i < 5; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_sll_insert_back(list, val);
    }

    // Sort in descending order
    ASSERT_EQ(dsc_sll_sort(list, int_cmp_desc), 0);

    // Verify order
    const DSCSinglyLinkedNode* node = list->head;
    for (int i = 4; i >= 0; i--)
    {
        ASSERT_EQ(*(int*)node->data, i);
        node = node->next;
    }

    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

int test_sort_null_args(void)
{
    DSCAllocator alloc           = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);
    ASSERT_EQ(dsc_sll_sort(NULL, int_cmp), -1); // NULL list
    ASSERT_EQ(dsc_sll_sort(list, NULL), -1);    // NULL compare function
    dsc_sll_destroy(list, false);
    return TEST_SUCCESS;
}

int test_sort_stability(void)
{
    DSCAllocator alloc           = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);

    // Person structs with same name (for comparison) but different ages
    Person* p1 = create_person("Alice", 30);
    Person* p2 = create_person("Alice", 25); // Same name, different age
    Person* p3 = create_person("Bob", 35);
    Person* p4 = create_person("Alice", 40); // Same name, different age

    dsc_sll_insert_back(list, p1);
    dsc_sll_insert_back(list, p2);
    dsc_sll_insert_back(list, p3);
    dsc_sll_insert_back(list, p4);

    // Sort by name only - ages should remain in insertion order for equal names
    ASSERT_EQ(dsc_sll_sort(list, person_cmp), 0);

    // Verify order: All Alice's should come before Bob
    const DSCSinglyLinkedNode* node = list->head;
    const Person* person            = node->data;
    ASSERT_EQ(strcmp(person->name, "Alice"), 0);
    ASSERT_EQ(person->age, 30); // First Alice

    node   = node->next;
    person = node->data;
    ASSERT_EQ(strcmp(person->name, "Alice"), 0);
    ASSERT_EQ(person->age, 25); // Second Alice

    node   = node->next;
    person = node->data;
    ASSERT_EQ(strcmp(person->name, "Alice"), 0);
    ASSERT_EQ(person->age, 40); // Third Alice

    node   = node->next;
    person = node->data;
    ASSERT_EQ(strcmp(person->name, "Bob"), 0);
    ASSERT_EQ(person->age, 35);

    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

int test_reverse(void)
{
    DSCAllocator alloc           = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);

    // Test empty list
    ASSERT_EQ(dsc_sll_reverse(list), 0);
    ASSERT_EQ(list->size, 0);

    // Test single element
    int* a = malloc(sizeof(int));
    *a     = 10;
    dsc_sll_insert_back(list, a);
    ASSERT_EQ(dsc_sll_reverse(list), 0);
    ASSERT_EQ(list->size, 1);
    ASSERT_EQ(*(int*)list->head->data, 10);

    // Test multiple elements
    int* b = malloc(sizeof(int));
    *b     = 20;
    int* c = malloc(sizeof(int));
    *c     = 30;
    dsc_sll_insert_back(list, b);
    dsc_sll_insert_back(list, c);
    // List is now [10,20,30]

    ASSERT_EQ(dsc_sll_reverse(list), 0);
    // List should now be [30,20,10]

    const DSCSinglyLinkedNode* node = list->head;
    ASSERT_EQ(*(int*)node->data, 30);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 20);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 10);
    ASSERT_NULL(node->next);

    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

int test_merge(void)
{
    DSCAllocator alloc            = create_int_allocator();
    DSCSinglyLinkedList* list1 = dsc_sll_create(&alloc);
    DSCSinglyLinkedList* list2 = dsc_sll_create(&alloc);

    // Test merging empty lists
    ASSERT_EQ(dsc_sll_merge(list1, list2), 0);
    ASSERT_EQ(list1->size, 0);
    ASSERT_EQ(list2->size, 0);

    // Test merging empty with non-empty
    int* a1 = malloc(sizeof(int));
    *a1     = 10;
    int* b1 = malloc(sizeof(int));
    *b1     = 20;
    dsc_sll_insert_back(list2, a1);
    dsc_sll_insert_back(list2, b1);

    ASSERT_EQ(dsc_sll_merge(list1, list2), 0);
    ASSERT_EQ(list1->size, 2);
    ASSERT_EQ(list2->size, 0);
    ASSERT_NULL(list2->head);

    const DSCSinglyLinkedNode* node = list1->head;
    ASSERT_EQ(*(int*)node->data, 10);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 20);

    // Test merging two non-empty lists
    DSCSinglyLinkedList* list3 = dsc_sll_create(&alloc);
    int* a2                    = malloc(sizeof(int));
    *a2                        = 30;
    int* b2                    = malloc(sizeof(int));
    *b2                        = 40;
    dsc_sll_insert_back(list3, a2);
    dsc_sll_insert_back(list3, b2);

    ASSERT_EQ(dsc_sll_merge(list1, list3), 0);
    ASSERT_EQ(list1->size, 4);
    ASSERT_EQ(list3->size, 0);

    node = list1->head;
    ASSERT_EQ(*(int*)node->data, 10);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 20);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 30);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 40);

    dsc_sll_destroy(list1, true);
    dsc_sll_destroy(list2, false); // Already empty
    dsc_sll_destroy(list3, false); // Already empty
    return TEST_SUCCESS;
}

int test_splice(void)
{
    DSCAllocator alloc           = create_int_allocator();
    DSCSinglyLinkedList* dest = dsc_sll_create(&alloc);
    DSCSinglyLinkedList* src  = dsc_sll_create(&alloc);

    // Test splicing empty lists
    ASSERT_EQ(dsc_sll_splice(dest, src, 0), 0);
    ASSERT_EQ(dest->size, 0);
    ASSERT_EQ(src->size, 0);

    // Setup lists
    int* a = malloc(sizeof(int));
    *a     = 10;
    int* b = malloc(sizeof(int));
    *b     = 20;
    int* c = malloc(sizeof(int));
    *c     = 30;
    dsc_sll_insert_back(dest, a);
    dsc_sll_insert_back(dest, b);
    dsc_sll_insert_back(dest, c);
    // dest = [10,20,30]

    int* d = malloc(sizeof(int));
    *d     = 40;
    int* e = malloc(sizeof(int));
    *e     = 50;
    dsc_sll_insert_back(src, d);
    dsc_sll_insert_back(src, e);
    // src = [40,50]

    // Test splicing at beginning
    DSCSinglyLinkedList* dest2 = dsc_sll_create(&alloc);
    DSCSinglyLinkedList* src2  = dsc_sll_create(&alloc);
    int* a2                    = malloc(sizeof(int));
    *a2                        = 10;
    int* b2                    = malloc(sizeof(int));
    *b2                        = 20;
    int* c2                    = malloc(sizeof(int));
    *c2                        = 30;
    int* d2                    = malloc(sizeof(int));
    *d2                        = 40;
    int* e2                    = malloc(sizeof(int));
    *e2                        = 50;
    dsc_sll_insert_back(dest2, a2);
    dsc_sll_insert_back(dest2, b2);
    dsc_sll_insert_back(dest2, c2);
    dsc_sll_insert_back(src2, d2);
    dsc_sll_insert_back(src2, e2);

    ASSERT_EQ(dsc_sll_splice(dest2, src2, 0), 0);
    ASSERT_EQ(dest2->size, 5);
    ASSERT_EQ(src2->size, 0);

    const DSCSinglyLinkedNode* node = dest2->head;
    ASSERT_EQ(*(int*)node->data, 40);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 50);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 10);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 20);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 30);

    // Test splicing in the middle
    ASSERT_EQ(dsc_sll_splice(dest, src, 1), 0);
    ASSERT_EQ(dest->size, 5);
    ASSERT_EQ(src->size, 0);

    node = dest->head;
    ASSERT_EQ(*(int*)node->data, 10);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 40);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 50);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 20);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 30);

    // Test splicing at the end
    DSCSinglyLinkedList* dest3 = dsc_sll_create(&alloc);
    DSCSinglyLinkedList* src3  = dsc_sll_create(&alloc);
    int* a3                    = malloc(sizeof(int));
    *a3                        = 10;
    int* b3                    = malloc(sizeof(int));
    *b3                        = 20;
    int* c3                    = malloc(sizeof(int));
    *c3                        = 30;
    int* d3                    = malloc(sizeof(int));
    *d3                        = 40;
    int* e3                    = malloc(sizeof(int));
    *e3                        = 50;
    dsc_sll_insert_back(dest3, a3);
    dsc_sll_insert_back(dest3, b3);
    dsc_sll_insert_back(dest3, c3);
    dsc_sll_insert_back(src3, d3);
    dsc_sll_insert_back(src3, e3);

    ASSERT_EQ(dsc_sll_splice(dest3, src3, 3), 0);
    ASSERT_EQ(dest3->size, 5);
    ASSERT_EQ(src3->size, 0);

    node = dest3->head;
    ASSERT_EQ(*(int*)node->data, 10);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 20);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 30);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 40);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 50);

    dsc_sll_destroy(dest, true);
    dsc_sll_destroy(src, false);
    dsc_sll_destroy(dest2, true);
    dsc_sll_destroy(src2, false);
    dsc_sll_destroy(dest3, true);
    dsc_sll_destroy(src3, false);
    return TEST_SUCCESS;
}

int test_equals(void)
{
    DSCAllocator alloc            = create_int_allocator();
    DSCSinglyLinkedList* list1 = dsc_sll_create(&alloc);
    DSCSinglyLinkedList* list2 = dsc_sll_create(&alloc);

    // Empty lists should be equal
    ASSERT_EQ(dsc_sll_equals(list1, list2, int_cmp), 1);

    // Lists with same elements should be equal
    int* a1 = malloc(sizeof(int));
    *a1     = 10;
    int* b1 = malloc(sizeof(int));
    *b1     = 20;
    int* a2 = malloc(sizeof(int));
    *a2     = 10;
    int* b2 = malloc(sizeof(int));
    *b2     = 20;
    dsc_sll_insert_back(list1, a1);
    dsc_sll_insert_back(list1, b1);
    dsc_sll_insert_back(list2, a2);
    dsc_sll_insert_back(list2, b2);

    ASSERT_EQ(dsc_sll_equals(list1, list2, int_cmp), 1);

    // Lists with different elements should not be equal
    int* c2 = malloc(sizeof(int));
    *c2     = 30;
    dsc_sll_insert_back(list2, c2);

    ASSERT_EQ(dsc_sll_equals(list1, list2, int_cmp), 0);

    // Lists with same size but different elements should not be equal
    DSCSinglyLinkedList* list3 = dsc_sll_create(&alloc);
    int* a3                    = malloc(sizeof(int));
    *a3                        = 10;
    int* b3                    = malloc(sizeof(int));
    *b3                        = 30; // Different value
    dsc_sll_insert_back(list3, a3);
    dsc_sll_insert_back(list3, b3);

    ASSERT_EQ(dsc_sll_equals(list1, list3, int_cmp), 0);

    // Error cases
    ASSERT_EQ(dsc_sll_equals(NULL, list2, int_cmp), -1);
    ASSERT_EQ(dsc_sll_equals(list1, NULL, int_cmp), -1);
    ASSERT_EQ(dsc_sll_equals(list1, list2, NULL), -1);

    dsc_sll_destroy(list1, true);
    dsc_sll_destroy(list2, true);
    dsc_sll_destroy(list3, true);
    return TEST_SUCCESS;
}

int test_filter(void)
{
    DSCAllocator alloc           = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);

    // Add numbers 0-9
    for (int i = 0; i < 10; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_sll_insert_back(list, val);
    }

    // Filter for even numbers
    DSCSinglyLinkedList* filtered = dsc_sll_filter(list, is_even);
    ASSERT_NOT_NULL(filtered);
    ASSERT_EQ(filtered->size, 5); // Should contain 0,2,4,6,8

    // Verify filtered list
    const DSCSinglyLinkedNode* node = filtered->head;
    for (int i = 0; i < 5; i++)
    {
        const int expected_values[] = {0, 2, 4, 6, 8};
        ASSERT_EQ(*(int*)node->data, expected_values[i]);
        node = node->next;
    }

    // Make sure original list is unchanged
    ASSERT_EQ(list->size, 10);

    // Test empty list
    DSCSinglyLinkedList* empty_list     = dsc_sll_create(&alloc);
    DSCSinglyLinkedList* filtered_empty = dsc_sll_filter(empty_list, is_even);
    ASSERT_NOT_NULL(filtered_empty);
    ASSERT_EQ(filtered_empty->size, 0);

    // Test null cases
    ASSERT_NULL(dsc_sll_filter(NULL, is_even));
    ASSERT_NULL(dsc_sll_filter(list, NULL));

    dsc_sll_destroy(list, true);
    dsc_sll_destroy(filtered, false);
    dsc_sll_destroy(empty_list, false);
    dsc_sll_destroy(filtered_empty, false);
    return TEST_SUCCESS;
}

int test_filter_deep(void)
{
    DSCAllocator alloc           = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);

    // Add numbers 0-9
    for (int i = 0; i < 10; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_sll_insert_back(list, val);
    }

    // Deep-filter for even numbers
    DSCSinglyLinkedList* filtered = dsc_sll_filter_deep(list, is_even);
    ASSERT_NOT_NULL(filtered);
    ASSERT_EQ(filtered->size, 5); // Should contain 0,2,4,6,8

    // Verify filtered list values and that data pointers are different (deep copy)
    const DSCSinglyLinkedNode* orig = list->head;
    const DSCSinglyLinkedNode* node = filtered->head;
    int idx                         = 0;
    while (node && orig)
    {
        const int expected_values[] = {0, 2, 4, 6, 8};
        // advance orig until next even
        while (orig && (*(int*)orig->data % 2) != 0)
            orig = orig->next;
        ASSERT_NOT_NULL(orig);
        ASSERT_EQ(*(int*)node->data, expected_values[idx]);
        // pointers must be different for deep copy
        ASSERT_NOT_EQ(orig->data, node->data);
        orig = orig->next;
        node = node->next;
        idx++;
    }

    // Modify original data and ensure filtered copy is unaffected
    if (list->head && list->head->data)
    {
        *(int*)list->head->data          = 99; // change 0 -> 99
        const DSCSinglyLinkedNode* fnode = filtered->head;
        ASSERT_EQ(*(int*)fnode->data, 0);
    }

    dsc_sll_destroy(list, true);
    dsc_sll_destroy(filtered, true);
    return TEST_SUCCESS;
}

int test_transform(void)
{
    DSCAllocator alloc           = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);

    // Add numbers 1-5
    for (int i = 1; i <= 5; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_sll_insert_back(list, val);
    }

    // Map to double each value
    DSCSinglyLinkedList* mapped = dsc_sll_transform(list, double_value, true);
    ASSERT_NOT_NULL(mapped);
    ASSERT_EQ(mapped->size, 5);

    // Verify mapped list (should be 2,4,6,8,10)
    const DSCSinglyLinkedNode* node = mapped->head;
    for (int i = 1; i <= 5; i++)
    {
        ASSERT_EQ(*(int*)node->data, i * 2);
        node = node->next;
    }

    // Make sure original list is unchanged
    node = list->head;
    for (int i = 1; i <= 5; i++)
    {
        ASSERT_EQ(*(int*)node->data, i);
        node = node->next;
    }

    // Test empty list
    DSCSinglyLinkedList* empty_list   = dsc_sll_create(&alloc);
    DSCSinglyLinkedList* mapped_empty = dsc_sll_transform(empty_list, double_value, true);
    ASSERT_NOT_NULL(mapped_empty);
    ASSERT_EQ(mapped_empty->size, 0);

    // Test null cases
    ASSERT_NULL(dsc_sll_transform(NULL, double_value, true));
    ASSERT_NULL(dsc_sll_transform(list, NULL, false));

    dsc_sll_destroy(list, true);
    dsc_sll_destroy(mapped, true);
    dsc_sll_destroy(empty_list, false);
    dsc_sll_destroy(mapped_empty, false);
    return TEST_SUCCESS;
}

int test_for_each(void)
{
    DSCAllocator alloc           = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);

    // Add numbers 1-5
    for (int i = 1; i <= 5; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_sll_insert_back(list, val);
    }

    // Increment each value
    dsc_sll_for_each(list, increment);

    // Verify each value is incremented
    const DSCSinglyLinkedNode* node = list->head;
    for (int i = 1; i <= 5; i++)
    {
        ASSERT_EQ(*(int*)node->data, i + 1);
        node = node->next;
    }

    // Test empty list
    DSCSinglyLinkedList* empty_list = dsc_sll_create(&alloc);
    dsc_sll_for_each(empty_list, increment); // Should do nothing

    // Test null cases
    dsc_sll_for_each(NULL, increment); // Should do nothing
    dsc_sll_for_each(list, NULL);      // Should do nothing

    dsc_sll_destroy(list, true);
    dsc_sll_destroy(empty_list, false);
    return TEST_SUCCESS;
}

typedef struct
{
    int (*func)(void);
    const char* name;
} TestCase;

TestCase tests[] = {
    {test_sort_empty, "test_sort_empty"},
    {test_sort_already_sorted, "test_sort_already_sorted"},
    {test_sort_reverse_order, "test_sort_reverse_order"},
    {test_sort_random_order, "test_sort_random_order"},
    {test_sort_with_duplicates, "test_sort_with_duplicates"},
    {test_sort_large_list, "test_sort_large_list"},
    {test_sort_custom_compare, "test_sort_custom_compare"},
    {test_sort_null_args, "test_sort_null_args"},
    {test_sort_stability, "test_sort_stability"},
    {test_reverse, "test_reverse"},
    {test_merge, "test_merge"},
    {test_splice, "test_splice"},
    {test_equals, "test_equals"},
    {test_filter, "test_filter"},
    {test_filter_deep, "test_filter_deep"},
    {test_transform, "test_transform"},
    {test_for_each, "test_for_each"},
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
        printf("All SinglyLinkedList Algorithm tests passed.\n");
        return 0;
    }

    printf("%d SinglyLinkedList Algorithm tests failed.\n", failed);
    return 1;
}