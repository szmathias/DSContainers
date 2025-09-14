//
// Created by zack on 9/2/25.
//

#include "SinglyLinkedList.h"
#include "TestAssert.h"
#include "TestHelpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Tests from test_sll.c that fit into CRUD category
int test_create_destroy(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);
    ASSERT_NOT_NULL(list);
    ASSERT_EQ(list->size, 0);
    dsc_sll_destroy(list, false);
    return TEST_SUCCESS;
}

int test_insert_front_back_find(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);
    int* a = malloc(sizeof(int));
    *a = 1;
    int* b = malloc(sizeof(int));
    *b = 2;
    int* c = malloc(sizeof(int));
    *c = 3;
    ASSERT_EQ(dsc_sll_insert_front(list, a), 0);
    ASSERT_EQ(dsc_sll_insert_back(list, b), 0);
    ASSERT_EQ(dsc_sll_insert_back(list, c), 0);
    ASSERT_EQ(list->size, 3);

    const int key = 2;
    const DSCSinglyLinkedNode* found = dsc_sll_find(list, &key, int_cmp);
    ASSERT_NOT_NULL(found);
    ASSERT_EQ(*(int*)found->data, 2);

    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

int test_remove(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);
    int* a = malloc(sizeof(int));
    *a = 1;
    int* b = malloc(sizeof(int));
    *b = 2;
    int* c = malloc(sizeof(int));
    *c = 3;
    dsc_sll_insert_back(list, a);
    dsc_sll_insert_back(list, b);
    dsc_sll_insert_back(list, c);

    const int key = 2;
    ASSERT_EQ(dsc_sll_remove(list, &key, int_cmp, true), 0);
    ASSERT_EQ(list->size, 2);
    DSCSinglyLinkedNode* found = dsc_sll_find(list, &key, int_cmp);
    ASSERT_NULL(found);

    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

int test_remove_not_found(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);
    int* a = malloc(sizeof(int));
    *a = 1;
    dsc_sll_insert_back(list, a);

    const int key = 99;
    ASSERT_EQ(dsc_sll_remove(list, &key, int_cmp, true), -1);

    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

int test_NULL_handling(void)
{
    ASSERT_EQ(dsc_sll_insert_back(NULL, NULL), -1);
    ASSERT_EQ(dsc_sll_insert_front(NULL, NULL), -1);
    ASSERT_EQ_PTR(dsc_sll_find(NULL, NULL, NULL), NULL);
    ASSERT_EQ(dsc_sll_remove(NULL, NULL, NULL, false), -1);
    dsc_sll_destroy(NULL, false); // Should not crash
    return TEST_SUCCESS;
}

int test_insert_at(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);
    int* a = malloc(sizeof(int));
    *a = 1;
    int* b = malloc(sizeof(int));
    *b = 2;
    int* c = malloc(sizeof(int));
    *c = 3;
    ASSERT_EQ(dsc_sll_insert_back(list, a), 0);  // [1]
    ASSERT_EQ(dsc_sll_insert_back(list, c), 0);  // [1,3]
    ASSERT_EQ(dsc_sll_insert_at(list, 1, b), 0); // [1,2,3]
    ASSERT_EQ(list->size, 3);

    const int key = 2;
    const DSCSinglyLinkedNode* found = dsc_sll_find(list, &key, int_cmp);
    ASSERT_NOT_NULL(found);
    ASSERT_EQ(*(int*)found->data, 2);

    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

int test_remove_at(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);
    int* a = malloc(sizeof(int));
    *a = 10;
    int* b = malloc(sizeof(int));
    *b = 20;
    int* c = malloc(sizeof(int));
    *c = 30;
    dsc_sll_insert_back(list, a); // [10]
    dsc_sll_insert_back(list, b); // [10,20]
    dsc_sll_insert_back(list, c); // [10,20,30]

    ASSERT_EQ(dsc_sll_remove_at(list, 1, true), 0); // remove 20
    ASSERT_EQ(list->size, 2);

    const int key = 20;
    DSCSinglyLinkedNode* found = dsc_sll_find(list, &key, int_cmp);
    ASSERT_NULL(found);

    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

int test_remove_at_head(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);
    int* a = malloc(sizeof(int));
    *a = 100;
    int* b = malloc(sizeof(int));
    *b = 200;
    dsc_sll_insert_back(list, a); // [100]
    dsc_sll_insert_back(list, b); // [100,200]

    ASSERT_EQ(dsc_sll_remove_at(list, 0, true), 0); // remove head (100)
    ASSERT_EQ(list->size, 1);

    const int key = 100;
    DSCSinglyLinkedNode* found = dsc_sll_find(list, &key, int_cmp);
    ASSERT_NULL(found);

    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

int test_remove_at_last(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);
    int* a = malloc(sizeof(int));
    *a = 1;
    int* b = malloc(sizeof(int));
    *b = 2;
    int* c = malloc(sizeof(int));
    *c = 3;
    dsc_sll_insert_back(list, a); // [1]
    dsc_sll_insert_back(list, b); // [1,2]
    dsc_sll_insert_back(list, c); // [1,2,3]

    ASSERT_EQ(dsc_sll_remove_at(list, 2, true), 0); // remove last (3)
    ASSERT_EQ(list->size, 2);

    const int key = 3;
    DSCSinglyLinkedNode* found = dsc_sll_find(list, &key, int_cmp);
    ASSERT_NULL(found);

    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

int test_remove_at_invalid(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);
    int* a = malloc(sizeof(int));
    *a = 1;
    dsc_sll_insert_back(list, a); // [1]

    ASSERT_EQ(dsc_sll_remove_at(list, 5, true), -1);          // invalid position
    ASSERT_EQ(dsc_sll_remove_at(list, (size_t)-1, true), -1); // negative as size_t (very large)

    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

int test_remove_at_empty(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);
    ASSERT_EQ(dsc_sll_remove_at(list, 0, true), -1); // nothing to remove
    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

int test_remove_at_single_element(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);
    int* a = malloc(sizeof(int));
    *a = 123;
    dsc_sll_insert_back(list, a);                   // [123]
    ASSERT_EQ(dsc_sll_remove_at(list, 0, true), 0); // remove only element
    ASSERT_EQ(list->size, 0);
    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

int test_remove_at_single_element_invalid_pos(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);
    int* a = malloc(sizeof(int));
    *a = 123;
    dsc_sll_insert_back(list, a);                    // [123]
    ASSERT_EQ(dsc_sll_remove_at(list, 1, true), -1); // invalid position
    ASSERT_EQ(list->size, 1);
    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

int test_insert_at_out_of_bounds(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);
    int* a = malloc(sizeof(int));
    *a = 1;
    ASSERT_EQ(dsc_sll_insert_at(list, 2, a), -1);          // out of bounds (list size is 0)
    ASSERT_EQ(dsc_sll_insert_at(list, (size_t)-1, a), -1); // very large index
    dsc_sll_destroy(list, true);
    free(a);
    return TEST_SUCCESS;
}

int test_insert_remove_null_data(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);
    ASSERT_EQ(dsc_sll_insert_back(list, NULL), 0); // allow nullptr data
    ASSERT_EQ(list->size, 1);
    ASSERT_EQ(dsc_sll_remove_at(list, 0, false), 0); // remove node with nullptr data, no free_func
    ASSERT_EQ(list->size, 0);
    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

int test_mixed_operations_integrity(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);
    int* a = malloc(sizeof(int));
    *a = 10;
    int* b = malloc(sizeof(int));
    *b = 20;
    int* c = malloc(sizeof(int));
    *c = 30;
    dsc_sll_insert_back(list, a);  // [10]
    dsc_sll_insert_front(list, b); // [20,10]
    dsc_sll_insert_at(list, 1, c); // [20,30,10]
    ASSERT_EQ(list->size, 3);

    ASSERT_EQ(dsc_sll_remove_at(list, 1, true), 0); // remove 30, [20,10]
    int key = 30;
    DSCSinglyLinkedNode* found = dsc_sll_find(list, &key, int_cmp);
    ASSERT_NULL(found);

    key = 20;
    found = dsc_sll_find(list, &key, int_cmp);
    ASSERT_NOT_NULL(found);

    key = 10;
    found = dsc_sll_find(list, &key, int_cmp);
    ASSERT_NOT_NULL(found);

    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

int test_size(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);
    ASSERT_EQ(dsc_sll_size(list), 0);

    int* a = malloc(sizeof(int));
    *a = 10;
    int* b = malloc(sizeof(int));
    *b = 20;
    dsc_sll_insert_back(list, a);
    ASSERT_EQ(dsc_sll_size(list), 1);
    dsc_sll_insert_back(list, b);
    ASSERT_EQ(dsc_sll_size(list), 2);

    dsc_sll_remove_at(list, 0, true);
    ASSERT_EQ(dsc_sll_size(list), 1);

    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

int test_is_empty(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);
    ASSERT_EQ(dsc_sll_is_empty(list), 1); // Empty list

    int* a = malloc(sizeof(int));
    *a = 10;
    dsc_sll_insert_back(list, a);
    ASSERT_EQ(dsc_sll_is_empty(list), 0); // Non-empty list

    dsc_sll_remove_at(list, 0, true);
    ASSERT_EQ(dsc_sll_is_empty(list), 1); // Empty again

    ASSERT_EQ(dsc_sll_is_empty(NULL), 1); // NULL list should be considered empty

    dsc_sll_destroy(list, false);
    return TEST_SUCCESS;
}

int test_complex_data_type(void)
{
    DSCAllocator alloc = create_person_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);

    Person* p1 = create_person("Alice", 30);
    Person* p2 = create_person("Bob", 25);
    Person* p3 = create_person("Charlie", 40);

    dsc_sll_insert_back(list, p1);
    dsc_sll_insert_back(list, p2);
    dsc_sll_insert_back(list, p3);
    ASSERT_EQ(list->size, 3);

    Person search_key;
    strcpy(search_key.name, "Bob");
    search_key.age = 0; // Age doesn't matter for comparison

    const DSCSinglyLinkedNode* found = dsc_sll_find(list, &search_key, person_cmp);
    ASSERT_NOT_NULL(found);
    const Person* found_person = found->data;
    ASSERT_EQ(found_person->age, 25);

    // Clean up
    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

int test_remove_all(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);

    // Add 10 elements
    for (int i = 0; i < 10; i++)
    {
        int* val = malloc(sizeof(int));
        *val = i;
        dsc_sll_insert_back(list, val);
    }
    ASSERT_EQ(list->size, 10);

    // Remove all elements one by one
    while (!dsc_sll_is_empty(list))
    {
        dsc_sll_remove_at(list, 0, true);
    }

    ASSERT_EQ(list->size, 0);
    ASSERT_NULL(list->head);

    dsc_sll_destroy(list, false); // Already freed all data
    return TEST_SUCCESS;
}

int test_remove_front(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);

    // Test on empty list
    ASSERT_EQ(dsc_sll_remove_front(list, true), -1);

    // Add elements
    int* a = malloc(sizeof(int));
    *a = 10;
    int* b = malloc(sizeof(int));
    *b = 20;
    int* c = malloc(sizeof(int));
    *c = 30;
    dsc_sll_insert_back(list, a);
    dsc_sll_insert_back(list, b);
    dsc_sll_insert_back(list, c);
    ASSERT_EQ(list->size, 3);

    // Remove front
    ASSERT_EQ(dsc_sll_remove_front(list, true), 0);
    ASSERT_EQ(list->size, 2);

    // Check first element is now 20
    int key = 10;
    ASSERT_NULL(dsc_sll_find(list, &key, int_cmp));
    key = 20;
    ASSERT_NOT_NULL(dsc_sll_find(list, &key, int_cmp));

    // Remove until empty
    ASSERT_EQ(dsc_sll_remove_front(list, true), 0);
    ASSERT_EQ(dsc_sll_remove_front(list, true), 0);
    ASSERT_EQ(list->size, 0);
    ASSERT_NULL(list->head);

    dsc_sll_destroy(list, false);
    return TEST_SUCCESS;
}

int test_remove_back(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCSinglyLinkedList* list = dsc_sll_create(&alloc);

    // Test on empty list
    ASSERT_EQ(dsc_sll_remove_back(list, true), -1);

    // Test on single element list
    int* a = malloc(sizeof(int));
    *a = 10;
    dsc_sll_insert_back(list, a);
    ASSERT_EQ(dsc_sll_remove_back(list, true), 0);
    ASSERT_EQ(list->size, 0);
    ASSERT_NULL(list->head);

    // Test with multiple elements
    int* b = malloc(sizeof(int));
    *b = 20;
    int* c = malloc(sizeof(int));
    *c = 30;
    int* d = malloc(sizeof(int));
    *d = 40;
    dsc_sll_insert_back(list, b);
    dsc_sll_insert_back(list, c);
    dsc_sll_insert_back(list, d);
    ASSERT_EQ(list->size, 3);

    // Remove back
    ASSERT_EQ(dsc_sll_remove_back(list, true), 0);
    ASSERT_EQ(list->size, 2);

    // Check last element was removed
    int key = 40;
    ASSERT_NULL(dsc_sll_find(list, &key, int_cmp));
    key = 30;
    ASSERT_NOT_NULL(dsc_sll_find(list, &key, int_cmp));

    dsc_sll_destroy(list, true);
    return TEST_SUCCESS;
}

typedef struct
{
    int (*func)(void);
    const char* name;
} TestCase;

TestCase tests[] = {
    {test_create_destroy, "test_create_destroy"},
    {test_insert_front_back_find, "test_insert_front_back_find"},
    {test_remove, "test_remove"},
    {test_remove_not_found, "test_remove_not_found"},
    {test_NULL_handling, "test_NULL_handling"},
    {test_insert_at, "test_insert_at"},
    {test_remove_at, "test_remove_at"},
    {test_remove_at_head, "test_remove_at_head"},
    {test_remove_at_last, "test_remove_at_last"},
    {test_remove_at_invalid, "test_remove_at_invalid"},
    {test_remove_at_empty, "test_remove_at_empty"},
    {test_remove_at_single_element, "test_remove_at_single_element"},
    {test_remove_at_single_element_invalid_pos, "test_remove_at_single_element_invalid_pos"},
    {test_insert_at_out_of_bounds, "test_insert_at_out_of_bounds"},
    {test_insert_remove_null_data, "test_insert_remove_null_data"},
    {test_mixed_operations_integrity, "test_mixed_operations_integrity"},
    {test_size, "test_size"},
    {test_is_empty, "test_is_empty"},
    {test_complex_data_type, "test_complex_data_type"},
    {test_remove_all, "test_remove_all"},
    {test_remove_front, "test_remove_front"},
    {test_remove_back, "test_remove_back"},
};

int main(void)
{
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

    if (failed == 0)
    {
        printf("All SinglyLinkedList CRUD tests passed.\n");
        return 0;
    }

    printf("%d SinglyLinkedList CRUD tests failed.\n", failed);
    return 1;
}