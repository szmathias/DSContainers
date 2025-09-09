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

int test_sort_empty(void)
{
    DSCAlloc* alloc           = create_std_allocator();
    DSCDoublyLinkedList* list = dsc_dll_create(alloc);
    ASSERT_EQ(dsc_dll_sort(list, int_cmp), 0); // Empty list is already sorted
    ASSERT_EQ(list->size, 0);
    dsc_dll_destroy(list, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_sort_already_sorted(void)
{
    DSCAlloc* alloc           = create_std_allocator();
    DSCDoublyLinkedList* list = dsc_dll_create(alloc);
    for (int i = 0; i < 5; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_dll_insert_back(list, val);
    }

    ASSERT_EQ(dsc_dll_sort(list, int_cmp), 0);

    // Verify order
    const DSCDoublyLinkedNode* node = list->head;
    for (int i = 0; i < 5; i++)
    {
        ASSERT_EQ(*(int*)node->data, i);
        node = node->next;
    }

    dsc_dll_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_sort_reverse_order(void)
{
    DSCAlloc* alloc           = create_std_allocator();
    DSCDoublyLinkedList* list = dsc_dll_create(alloc);
    for (int i = 4; i >= 0; i--)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_dll_insert_back(list, val);
    }

    ASSERT_EQ(dsc_dll_sort(list, int_cmp), 0);

    // Verify order
    const DSCDoublyLinkedNode* node = list->head;
    for (int i = 0; i < 5; i++)
    {
        ASSERT_EQ(*(int*)node->data, i);
        node = node->next;
    }

    // Check bidirectional links
    const DSCDoublyLinkedNode* tail = list->tail;
    for (int i = 4; i >= 0; i--)
    {
        ASSERT_EQ(*(int*)tail->data, i);
        tail = tail->prev;
    }

    dsc_dll_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_sort_with_duplicates(void)
{
    DSCAlloc* alloc           = create_std_allocator();
    DSCDoublyLinkedList* list = dsc_dll_create(alloc);
    const int values[]        = {5, 2, 9, 5, 7, 2, 9, 5};
    const size_t count        = sizeof(values) / sizeof(values[0]);

    for (size_t i = 0; i < count; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = values[i];
        dsc_dll_insert_back(list, val);
    }

    ASSERT_EQ(dsc_dll_sort(list, int_cmp), 0);

    // Verify order
    const DSCDoublyLinkedNode* node = list->head;
    for (size_t i = 0; i < count; i++)
    {
        const int sorted[] = {2, 2, 5, 5, 5, 7, 9, 9};
        ASSERT_EQ(*(int*)node->data, sorted[i]);
        node = node->next;
    }

    // Check bidirectional links
    const DSCDoublyLinkedNode* tail = list->tail;
    for (size_t i = 0; i < count; i++)
    {
        const int sorted_reverse[] = {9, 9, 7, 5, 5, 5, 2, 2};
        ASSERT_EQ(*(int*)tail->data, sorted_reverse[i]);
        tail = tail->prev;
    }

    dsc_dll_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_sort_large_list(void)
{
    DSCAlloc* alloc           = create_std_allocator();
    DSCDoublyLinkedList* list = dsc_dll_create(alloc);
    const int SIZE            = 1000;

    // Insert in reverse order
    for (int i = SIZE - 1; i >= 0; i--)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_dll_insert_back(list, val);
    }

    const clock_t start = clock();
    ASSERT_EQ(dsc_dll_sort(list, int_cmp), 0);
    const clock_t end = clock();
    printf("DLL Sort %d elements: %.6f seconds\n", SIZE,
           (double)(end - start) / CLOCKS_PER_SEC);

    // Verify order (first few and last few)
    const DSCDoublyLinkedNode* node = list->head;
    for (int i = 0; i < 10; i++)
    {
        ASSERT_EQ(*(int*)node->data, i);
        node = node->next;
    }

    node = list->tail;
    for (int i = SIZE - 1; i >= SIZE - 10; i--)
    {
        ASSERT_EQ(*(int*)node->data, i);
        node = node->prev;
    }

    // Verify list structure
    ASSERT_EQ(list->size, (size_t)SIZE);
    ASSERT_EQ(*(int*)list->head->data, 0);
    ASSERT_EQ(*(int*)list->tail->data, SIZE - 1);

    dsc_dll_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_sort_custom_compare(void)
{
    DSCAlloc* alloc           = create_std_allocator();
    DSCDoublyLinkedList* list = dsc_dll_create(alloc);
    for (int i = 0; i < 5; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_dll_insert_back(list, val);
    }

    // Sort in descending order
    ASSERT_EQ(dsc_dll_sort(list, int_cmp_desc), 0);

    // Verify order
    const DSCDoublyLinkedNode* node = list->head;
    for (int i = 4; i >= 0; i--)
    {
        ASSERT_EQ(*(int*)node->data, i);
        node = node->next;
    }

    dsc_dll_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_sort_null_args(void)
{
    DSCAlloc* alloc           = create_std_allocator();
    DSCDoublyLinkedList* list = dsc_dll_create(alloc);
    ASSERT_EQ(dsc_dll_sort(NULL, int_cmp), -1); // NULL list
    ASSERT_EQ(dsc_dll_sort(list, NULL), -1);    // NULL compare function
    dsc_dll_destroy(list, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_sort_stability(void)
{
    DSCAlloc* alloc           = create_std_allocator();
    DSCDoublyLinkedList* list = dsc_dll_create(alloc);

    // Person structs with same name (for comparison) but different ages
    Person* p1 = create_person("Alice", 30);
    Person* p2 = create_person("Alice", 25); // Same name, different age
    Person* p3 = create_person("Bob", 35);
    Person* p4 = create_person("Alice", 40); // Same name, different age

    dsc_dll_insert_back(list, p1);
    dsc_dll_insert_back(list, p2);
    dsc_dll_insert_back(list, p3);
    dsc_dll_insert_back(list, p4);

    // Sort by name only - ages should remain in insertion order for equal names
    ASSERT_EQ(dsc_dll_sort(list, person_cmp), 0);

    // Verify order: All Alice's should come before Bob
    const DSCDoublyLinkedNode* node = list->head;
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

    dsc_dll_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_reverse(void)
{
    DSCAlloc* alloc           = create_std_allocator();
    DSCDoublyLinkedList* list = dsc_dll_create(alloc);

    // Test empty list
    ASSERT_EQ(dsc_dll_reverse(list), 0);
    ASSERT_EQ(list->size, 0);

    // Test single element
    int* a = malloc(sizeof(int));
    *a     = 10;
    dsc_dll_insert_back(list, a);
    ASSERT_EQ(dsc_dll_reverse(list), 0);
    ASSERT_EQ(list->size, 1);
    ASSERT_EQ(*(int*)list->head->data, 10);
    ASSERT_EQ(list->head, list->tail);

    // Test multiple elements
    int* b = malloc(sizeof(int));
    *b     = 20;
    int* c = malloc(sizeof(int));
    *c     = 30;
    dsc_dll_insert_back(list, b);
    dsc_dll_insert_back(list, c);
    // List is now [10,20,30]

    ASSERT_EQ(dsc_dll_reverse(list), 0);
    // List should now be [30,20,10]

    // Verify head-to-tail traversal
    const DSCDoublyLinkedNode* node = list->head;
    ASSERT_EQ(*(int*)node->data, 30);
    ASSERT_NULL(node->prev);

    node = node->next;
    ASSERT_EQ(*(int*)node->data, 20);
    ASSERT_EQ(*(int*)node->prev->data, 30);

    node = node->next;
    ASSERT_EQ(*(int*)node->data, 10);
    ASSERT_EQ(*(int*)node->prev->data, 20);
    ASSERT_NULL(node->next);
    ASSERT_EQ(node, list->tail);

    // Verify tail-to-head traversal
    node = list->tail;
    ASSERT_EQ(*(int*)node->data, 10);
    ASSERT_NULL(node->next);

    node = node->prev;
    ASSERT_EQ(*(int*)node->data, 20);
    ASSERT_EQ(*(int*)node->next->data, 10);

    node = node->prev;
    ASSERT_EQ(*(int*)node->data, 30);
    ASSERT_EQ(*(int*)node->next->data, 20);
    ASSERT_NULL(node->prev);
    ASSERT_EQ(node, list->head);

    dsc_dll_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_merge(void)
{
    DSCAlloc* alloc            = create_std_allocator();
    DSCDoublyLinkedList* list1 = dsc_dll_create(alloc);
    DSCDoublyLinkedList* list2 = dsc_dll_create(alloc);

    // Test merging empty lists
    ASSERT_EQ(dsc_dll_merge(list1, list2), 0);
    ASSERT_EQ(list1->size, 0);
    ASSERT_EQ(list2->size, 0);

    // Test merging empty with non-empty
    int* a1 = malloc(sizeof(int));
    *a1     = 10;
    int* b1 = malloc(sizeof(int));
    *b1     = 20;
    dsc_dll_insert_back(list2, a1);
    dsc_dll_insert_back(list2, b1);

    ASSERT_EQ(dsc_dll_merge(list1, list2), 0);
    ASSERT_EQ(list1->size, 2);
    ASSERT_EQ(list2->size, 0);
    ASSERT_NULL(list2->head);
    ASSERT_NULL(list2->tail);

    // Verify the merged list
    ASSERT_EQ(*(int*)list1->head->data, 10);
    ASSERT_EQ(*(int*)list1->tail->data, 20);
    ASSERT_NULL(list1->head->prev);
    ASSERT_NULL(list1->tail->next);
    ASSERT_EQ(list1->head->next, list1->tail);
    ASSERT_EQ(list1->tail->prev, list1->head);

    // Test merging two non-empty lists
    DSCDoublyLinkedList* list3 = dsc_dll_create(alloc);
    int* a2                    = malloc(sizeof(int));
    *a2                        = 30;
    int* b2                    = malloc(sizeof(int));
    *b2                        = 40;
    dsc_dll_insert_back(list3, a2);
    dsc_dll_insert_back(list3, b2);

    ASSERT_EQ(dsc_dll_merge(list1, list3), 0);
    ASSERT_EQ(list1->size, 4);
    ASSERT_EQ(list3->size, 0);

    // Verify the final merged list
    ASSERT_EQ(*(int*)list1->head->data, 10);
    ASSERT_EQ(*(int*)list1->tail->data, 40);

    const DSCDoublyLinkedNode* node = list1->head;
    ASSERT_EQ(*(int*)node->data, 10);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 20);
    ASSERT_EQ(*(int*)node->prev->data, 10);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 30);
    ASSERT_EQ(*(int*)node->prev->data, 20);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 40);
    ASSERT_EQ(*(int*)node->prev->data, 30);
    ASSERT_EQ(node, list1->tail);
    ASSERT_NULL(node->next);

    dsc_dll_destroy(list1, true);
    dsc_dll_destroy(list2, false); // Already empty
    dsc_dll_destroy(list3, false); // Already empty
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_splice(void)
{
    DSCAlloc* alloc = create_std_allocator();
    // Test splicing at the beginning
    DSCDoublyLinkedList* dest1 = dsc_dll_create(alloc);
    DSCDoublyLinkedList* src1  = dsc_dll_create(alloc);

    int* a1 = malloc(sizeof(int));
    *a1     = 10;
    int* b1 = malloc(sizeof(int));
    *b1     = 20;
    int* c1 = malloc(sizeof(int));
    *c1     = 30;
    int* d1 = malloc(sizeof(int));
    *d1     = 40;
    int* e1 = malloc(sizeof(int));
    *e1     = 50;

    dsc_dll_insert_back(dest1, a1);
    dsc_dll_insert_back(dest1, b1);
    dsc_dll_insert_back(dest1, c1);
    dsc_dll_insert_back(src1, d1);
    dsc_dll_insert_back(src1, e1);

    ASSERT_EQ(dsc_dll_splice(dest1, src1, 0), 0);
    ASSERT_EQ(dest1->size, 5);
    ASSERT_EQ(src1->size, 0);

    // Verify the spliced list
    ASSERT_EQ(*(int*)dest1->head->data, 40);
    ASSERT_EQ(*(int*)dest1->head->next->data, 50);
    ASSERT_EQ(*(int*)dest1->head->next->next->data, 10);
    ASSERT_EQ(*(int*)dest1->head->next->next->next->data, 20);
    ASSERT_EQ(*(int*)dest1->tail->data, 30);

    // Test bidirectional links
    ASSERT_NULL(dest1->head->prev);
    ASSERT_EQ(*(int*)dest1->head->next->prev->data, 40);
    ASSERT_EQ(*(int*)dest1->tail->prev->data, 20);
    ASSERT_NULL(dest1->tail->next);

    // Test splicing in the middle
    DSCDoublyLinkedList* dest2 = dsc_dll_create(alloc);
    DSCDoublyLinkedList* src2  = dsc_dll_create(alloc);

    int* a2 = malloc(sizeof(int));
    *a2     = 10;
    int* b2 = malloc(sizeof(int));
    *b2     = 20;
    int* c2 = malloc(sizeof(int));
    *c2     = 30;
    int* d2 = malloc(sizeof(int));
    *d2     = 40;
    int* e2 = malloc(sizeof(int));
    *e2     = 50;

    dsc_dll_insert_back(dest2, a2);
    dsc_dll_insert_back(dest2, b2);
    dsc_dll_insert_back(dest2, c2);
    dsc_dll_insert_back(src2, d2);
    dsc_dll_insert_back(src2, e2);

    ASSERT_EQ(dsc_dll_splice(dest2, src2, 1), 0);
    ASSERT_EQ(dest2->size, 5);
    ASSERT_EQ(src2->size, 0);

    // Verify the spliced list
    ASSERT_EQ(*(int*)dest2->head->data, 10);
    ASSERT_EQ(*(int*)dest2->head->next->data, 40);
    ASSERT_EQ(*(int*)dest2->head->next->next->data, 50);
    ASSERT_EQ(*(int*)dest2->head->next->next->next->data, 20);
    ASSERT_EQ(*(int*)dest2->tail->data, 30);

    // Test bidirectional links
    ASSERT_NULL(dest2->head->prev);
    ASSERT_EQ(*(int*)dest2->head->next->prev->data, 10);
    ASSERT_EQ(*(int*)dest2->tail->prev->data, 20);
    ASSERT_NULL(dest2->tail->next);

    // Test splicing at the end
    DSCDoublyLinkedList* dest3 = dsc_dll_create(alloc);
    DSCDoublyLinkedList* src3  = dsc_dll_create(alloc);

    int* a3 = malloc(sizeof(int));
    *a3     = 10;
    int* b3 = malloc(sizeof(int));
    *b3     = 20;
    int* c3 = malloc(sizeof(int));
    *c3     = 30;
    int* d3 = malloc(sizeof(int));
    *d3     = 40;
    int* e3 = malloc(sizeof(int));
    *e3     = 50;

    dsc_dll_insert_back(dest3, a3);
    dsc_dll_insert_back(dest3, b3);
    dsc_dll_insert_back(dest3, c3);
    dsc_dll_insert_back(src3, d3);
    dsc_dll_insert_back(src3, e3);

    ASSERT_EQ(dsc_dll_splice(dest3, src3, 3), 0);
    ASSERT_EQ(dest3->size, 5);
    ASSERT_EQ(src3->size, 0);

    // Verify the spliced list
    ASSERT_EQ(*(int*)dest3->head->data, 10);
    ASSERT_EQ(*(int*)dest3->head->next->data, 20);
    ASSERT_EQ(*(int*)dest3->head->next->next->data, 30);
    ASSERT_EQ(*(int*)dest3->head->next->next->next->data, 40);
    ASSERT_EQ(*(int*)dest3->tail->data, 50);

    // Test bidirectional links
    ASSERT_NULL(dest3->head->prev);
    ASSERT_EQ(*(int*)dest3->head->next->prev->data, 10);
    ASSERT_EQ(*(int*)dest3->tail->prev->data, 40);
    ASSERT_NULL(dest3->tail->next);

    // Test splicing with empty source
    DSCDoublyLinkedList* empty = dsc_dll_create(alloc);
    ASSERT_EQ(dsc_dll_splice(dest1, empty, 2), 0);
    ASSERT_EQ(dest1->size, 5); // Should be unchanged

    // Test splicing with invalid position
    ASSERT_EQ(dsc_dll_splice(dest1, src1, 99), -1);

    dsc_dll_destroy(dest1, true);
    dsc_dll_destroy(src1, false);
    dsc_dll_destroy(dest2, true);
    dsc_dll_destroy(src2, false);
    dsc_dll_destroy(dest3, true);
    dsc_dll_destroy(src3, false);
    dsc_dll_destroy(empty, false);
    destroy_allocator(alloc);

    return TEST_SUCCESS;
}

int test_equals(void)
{
    DSCAlloc* alloc            = create_std_allocator();
    DSCDoublyLinkedList* list1 = dsc_dll_create(alloc);
    DSCDoublyLinkedList* list2 = dsc_dll_create(alloc);

    // Empty lists should be equal
    ASSERT_EQ(dsc_dll_equals(list1, list2, int_cmp), 1);

    // Lists with same elements should be equal
    int* a1 = malloc(sizeof(int));
    *a1     = 10;
    int* b1 = malloc(sizeof(int));
    *b1     = 20;
    int* a2 = malloc(sizeof(int));
    *a2     = 10;
    int* b2 = malloc(sizeof(int));
    *b2     = 20;
    dsc_dll_insert_back(list1, a1);
    dsc_dll_insert_back(list1, b1);
    dsc_dll_insert_back(list2, a2);
    dsc_dll_insert_back(list2, b2);

    ASSERT_EQ(dsc_dll_equals(list1, list2, int_cmp), 1);

    // Lists with different elements should not be equal
    int* c2 = malloc(sizeof(int));
    *c2     = 30;
    dsc_dll_insert_back(list2, c2);

    ASSERT_EQ(dsc_dll_equals(list1, list2, int_cmp), 0);

    // Lists with same size but different elements should not be equal
    DSCDoublyLinkedList* list3 = dsc_dll_create(alloc);
    int* a3                    = malloc(sizeof(int));
    *a3                        = 10;
    int* b3                    = malloc(sizeof(int));
    *b3                        = 30; // Different value
    dsc_dll_insert_back(list3, a3);
    dsc_dll_insert_back(list3, b3);

    ASSERT_EQ(dsc_dll_equals(list1, list3, int_cmp), 0);

    // Error cases
    ASSERT_EQ(dsc_dll_equals(NULL, list2, int_cmp), -1);
    ASSERT_EQ(dsc_dll_equals(list1, NULL, int_cmp), -1);
    ASSERT_EQ(dsc_dll_equals(list1, list2, NULL), -1);

    dsc_dll_destroy(list1, true);
    dsc_dll_destroy(list2, true);
    dsc_dll_destroy(list3, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_filter(void)
{
    DSCAlloc* alloc           = create_std_allocator();
    DSCDoublyLinkedList* list = dsc_dll_create(alloc);

    // Add numbers 0-9
    for (int i = 0; i < 10; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_dll_insert_back(list, val);
    }

    // Filter for even numbers
    DSCDoublyLinkedList* filtered = dsc_dll_filter(list, is_even);
    ASSERT_NOT_NULL(filtered);
    ASSERT_EQ(filtered->size, 5); // Should contain 0,2,4,6,8

    // Verify filtered list
    const DSCDoublyLinkedNode* node = filtered->head;
    for (int i = 0; i < 5; i++)
    {
        const int expected_values[] = {0, 2, 4, 6, 8};
        ASSERT_EQ(*(int*)node->data, expected_values[i]);
        node = node->next;
    }

    // Make sure original list is unchanged
    ASSERT_EQ(list->size, 10);

    // Test empty list
    DSCDoublyLinkedList* empty_list     = dsc_dll_create(alloc);
    DSCDoublyLinkedList* filtered_empty = dsc_dll_filter(empty_list, is_even);
    ASSERT_NOT_NULL(filtered_empty);
    ASSERT_EQ(filtered_empty->size, 0);

    // Test null cases
    ASSERT_NULL(dsc_dll_filter(NULL, is_even));
    ASSERT_NULL(dsc_dll_filter(list, NULL));

    dsc_dll_destroy(list, true);
    dsc_dll_destroy(filtered, false);
    dsc_dll_destroy(empty_list, false);
    dsc_dll_destroy(filtered_empty, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_filter_deep(void)
{
    DSCAlloc* alloc           = create_std_allocator();
    DSCDoublyLinkedList* list = dsc_dll_create(alloc);

    // Add numbers 0-9
    for (int i = 0; i < 10; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_dll_insert_back(list, val);
    }

    // Deep-filter for even numbers
    DSCDoublyLinkedList* filtered = dsc_dll_filter_deep(list, is_even);
    ASSERT_NOT_NULL(filtered);
    ASSERT_EQ(filtered->size, 5); // 0,2,4,6,8

    // Verify values and deep-copy semantics (pointers differ)
    const DSCDoublyLinkedNode* orig = list->head;
    const DSCDoublyLinkedNode* node = filtered->head;
    int idx                         = 0;
    while (node && orig)
    {
        const int expected_values[] = {0, 2, 4, 6, 8};
        // advance orig to next matching even
        while (orig && (*(int*)orig->data % 2) != 0)
            orig = orig->next;
        ASSERT_NOT_NULL(orig);
        ASSERT_EQ(*(int*)node->data, expected_values[idx]);
        // deep copy -> different pointers
        ASSERT_NOT_EQ(orig->data, node->data);
        orig = orig->next;
        node = node->next;
        idx++;
    }

    // Mutate original and ensure filtered copy unchanged
    if (list->head && list->head->data)
    {
        *(int*)list->head->data          = 99; // change 0 -> 99
        const DSCDoublyLinkedNode* fnode = filtered->head;
        ASSERT_EQ(*(int*)fnode->data, 0);
    }

    dsc_dll_destroy(list, true);
    dsc_dll_destroy(filtered, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_transform(void)
{
    DSCAlloc* alloc           = create_std_allocator();
    DSCDoublyLinkedList* list = dsc_dll_create(alloc);

    // Add numbers 1-5
    for (int i = 1; i <= 5; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_dll_insert_back(list, val);
    }

    // Map to double each value
    DSCDoublyLinkedList* transformed = dsc_dll_transform(list, double_value, true);
    ASSERT_NOT_NULL(transformed);
    ASSERT_EQ(transformed->size, 5);

    // Verify mapped list (should be 2,4,6,8,10)
    const DSCDoublyLinkedNode* node = transformed->head;
    for (int i = 1; i <= 5; i++)
    {
        ASSERT_EQ(*(int*)node->data, i * 2);
        node = node->next;
    }

    // Make sure bidirectional links are correct
    node = transformed->head;
    ASSERT_NULL(node->prev);

    node = node->next;
    ASSERT_EQ(*(int*)node->prev->data, 2);

    node = transformed->tail;
    ASSERT_NULL(node->next);
    ASSERT_EQ(*(int*)node->data, 10);

    // Make sure original list is unchanged
    node = list->head;
    for (int i = 1; i <= 5; i++)
    {
        ASSERT_EQ(*(int*)node->data, i);
        node = node->next;
    }

    // Test empty list
    DSCDoublyLinkedList* empty_list        = dsc_dll_create(alloc);
    DSCDoublyLinkedList* transformed_empty = dsc_dll_transform(empty_list, double_value, true);
    ASSERT_NOT_NULL(transformed_empty);
    ASSERT_EQ(transformed_empty->size, 0);

    // Test null cases
    ASSERT_NULL(dsc_dll_transform(NULL, double_value, true));
    ASSERT_NULL(dsc_dll_transform(list, NULL, true));

    dsc_dll_destroy(list, true);
    dsc_dll_destroy(transformed, true);
    dsc_dll_destroy(empty_list, false);
    dsc_dll_destroy(transformed_empty, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_for_each(void)
{
    DSCAlloc* alloc           = create_std_allocator();
    DSCDoublyLinkedList* list = dsc_dll_create(alloc);

    // Add numbers 1-5
    for (int i = 1; i <= 5; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_dll_insert_back(list, val);
    }

    // Increment each value
    dsc_dll_for_each(list, increment);

    // Verify each value is incremented
    const DSCDoublyLinkedNode* node = list->head;
    for (int i = 1; i <= 5; i++)
    {
        ASSERT_EQ(*(int*)node->data, i + 1);
        node = node->next;
    }

    // Test empty list
    DSCDoublyLinkedList* empty_list = dsc_dll_create(alloc);
    dsc_dll_for_each(empty_list, increment); // Should do nothing

    // Test null cases
    dsc_dll_for_each(NULL, increment); // Should do nothing
    dsc_dll_for_each(list, NULL);      // Should do nothing

    dsc_dll_destroy(list, true);
    dsc_dll_destroy(empty_list, false);
    destroy_allocator(alloc);
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
        printf("All DoublyLinkedList Algorithm tests passed.\n");
        return 0;
    }

    printf("%d DoublyLinkedList Algorithm tests failed.\n", failed);
    return 1;
}