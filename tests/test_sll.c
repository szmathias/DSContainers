//
// Created by zack on 8/23/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "SLinkedList.h"
#include "TestAssert.h"

// Integer comparison for SLinkedList
int int_cmp(const void *a, const void *b) {
    return (*(int*)a) - (*(int*)b);
}

// Integer free for SLinkedList
void int_free(void *a) {
    free(a);
}

// Custom allocator for testing
void *test_calloc(const size_t size) {
    return calloc(1, size);
}
void test_dealloc(void *ptr) {
    free(ptr);
}

// Complex data type for testing
typedef struct {
    char name[50];
    int age;
} Person;

// Person comparison function
int person_cmp(const void *a, const void *b) {
    const Person *p1 = a;
    const Person *p2 = b;
    return strcmp(p1->name, p2->name);
}

// Person free function
void person_free(void *p) {
    free(p);
}

// Helper to create a person
Person* create_person(const char* name, const int age) {
    Person* p = malloc(sizeof(Person));
    if (p) {
        strncpy(p->name, name, 49);
        p->name[49] = '\0';
        p->age = age;
    }
    return p;
}

int test_create_destroy() {
    SLinkedList *list = sll_create();
    ASSERT_NOT_NULLPTR(list);
    ASSERT_EQ(list->size, 0);
    sll_destroy(list, nullptr);
    return TEST_SUCCESS;
}

int test_insert_front_back_find() {
    SLinkedList *list = sll_create();
    int *a = malloc(sizeof(int)); *a = 1;
    int *b = malloc(sizeof(int)); *b = 2;
    int *c = malloc(sizeof(int)); *c = 3;
    ASSERT_EQ(sll_insert_front(list, a), 0);
    ASSERT_EQ(sll_insert_back(list, b), 0);
    ASSERT_EQ(sll_insert_back(list, c), 0);
    ASSERT_EQ(list->size, 3);

    constexpr int key = 2;
    const SListNode *found = sll_find(list, &key, int_cmp);
    ASSERT_NOT_NULLPTR(found);
    ASSERT_EQ(*(int*)found->data, 2);

    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_remove() {
    SLinkedList *list = sll_create();
    int *a = malloc(sizeof(int)); *a = 1;
    int *b = malloc(sizeof(int)); *b = 2;
    int *c = malloc(sizeof(int)); *c = 3;
    sll_insert_back(list, a);
    sll_insert_back(list, b);
    sll_insert_back(list, c);

    constexpr int key = 2;
    ASSERT_EQ(sll_remove(list, &key, int_cmp, int_free), 0);
    ASSERT_EQ(list->size, 2);
    SListNode *found = sll_find(list, &key, int_cmp);
    ASSERT_NULLPTR(found);

    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_remove_not_found() {
    SLinkedList *list = sll_create();
    int *a = malloc(sizeof(int)); *a = 1;
    sll_insert_back(list, a);

    constexpr int key = 99;
    ASSERT_EQ(sll_remove(list, &key, int_cmp, int_free), -1);

    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_custom_allocator() {
    SLinkedList *list = sll_create_custom(test_calloc, test_dealloc);
    ASSERT_NOT_NULLPTR(list);
    int *a = malloc(sizeof(int)); *a = 42;
    ASSERT_EQ(sll_insert_back(list, a), 0);
    ASSERT_EQ(list->size, 1);
    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_nullptr_handling() {
    ASSERT_EQ(sll_insert_back(nullptr, nullptr), -1);
    ASSERT_EQ(sll_insert_front(nullptr, nullptr), -1);
    ASSERT_EQ_PTR(sll_find(nullptr, nullptr, nullptr), nullptr);
    ASSERT_EQ(sll_remove(nullptr, nullptr, nullptr, nullptr), -1);
    sll_destroy(nullptr, nullptr); // Should not crash
    return TEST_SUCCESS;
}

int test_insert_at() {
    SLinkedList *list = sll_create();
    int *a = malloc(sizeof(int)); *a = 1;
    int *b = malloc(sizeof(int)); *b = 2;
    int *c = malloc(sizeof(int)); *c = 3;
    ASSERT_EQ(sll_insert_back(list, a), 0); // [1]
    ASSERT_EQ(sll_insert_back(list, c), 0); // [1,3]
    ASSERT_EQ(sll_insert_at(list, 1, b), 0); // [1,2,3]
    ASSERT_EQ(list->size, 3);

    constexpr int key = 2;
    const SListNode *found = sll_find(list, &key, int_cmp);
    ASSERT_NOT_NULLPTR(found);
    ASSERT_EQ(*(int*)found->data, 2);

    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_remove_at() {
    SLinkedList *list = sll_create();
    int *a = malloc(sizeof(int)); *a = 10;
    int *b = malloc(sizeof(int)); *b = 20;
    int *c = malloc(sizeof(int)); *c = 30;
    sll_insert_back(list, a); // [10]
    sll_insert_back(list, b); // [10,20]
    sll_insert_back(list, c); // [10,20,30]

    ASSERT_EQ(sll_remove_at(list, 1, int_free), 0); // remove 20
    ASSERT_EQ(list->size, 2);

    constexpr int key = 20;
    SListNode *found = sll_find(list, &key, int_cmp);
    ASSERT_NULLPTR(found);

    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_remove_at_head() {
    SLinkedList *list = sll_create();
    int *a = malloc(sizeof(int)); *a = 100;
    int *b = malloc(sizeof(int)); *b = 200;
    sll_insert_back(list, a); // [100]
    sll_insert_back(list, b); // [100,200]

    ASSERT_EQ(sll_remove_at(list, 0, int_free), 0); // remove head (100)
    ASSERT_EQ(list->size, 1);

    constexpr int key = 100;
    SListNode *found = sll_find(list, &key, int_cmp);
    ASSERT_NULLPTR(found);

    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_remove_at_last() {
    SLinkedList *list = sll_create();
    int *a = malloc(sizeof(int)); *a = 1;
    int *b = malloc(sizeof(int)); *b = 2;
    int *c = malloc(sizeof(int)); *c = 3;
    sll_insert_back(list, a); // [1]
    sll_insert_back(list, b); // [1,2]
    sll_insert_back(list, c); // [1,2,3]

    ASSERT_EQ(sll_remove_at(list, 2, int_free), 0); // remove last (3)
    ASSERT_EQ(list->size, 2);

    constexpr int key = 3;
    SListNode *found = sll_find(list, &key, int_cmp);
    ASSERT_NULLPTR(found);

    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_remove_at_invalid() {
    SLinkedList *list = sll_create();
    int *a = malloc(sizeof(int)); *a = 1;
    sll_insert_back(list, a); // [1]

    ASSERT_EQ(sll_remove_at(list, 5, int_free), -1); // invalid position
    ASSERT_EQ(sll_remove_at(list, (size_t)-1, int_free), -1); // negative as size_t (very large)

    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_remove_at_empty() {
    SLinkedList *list = sll_create();
    ASSERT_EQ(sll_remove_at(list, 0, int_free), -1); // nothing to remove
    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_remove_at_single_element() {
    SLinkedList *list = sll_create();
    int *a = malloc(sizeof(int)); *a = 123;
    sll_insert_back(list, a); // [123]
    ASSERT_EQ(sll_remove_at(list, 0, int_free), 0); // remove only element
    ASSERT_EQ(list->size, 0);
    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_remove_at_single_element_invalid_pos() {
    SLinkedList *list = sll_create();
    int *a = malloc(sizeof(int)); *a = 123;
    sll_insert_back(list, a); // [123]
    ASSERT_EQ(sll_remove_at(list, 1, int_free), -1); // invalid position
    ASSERT_EQ(list->size, 1);
    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_insert_at_out_of_bounds() {
    SLinkedList *list = sll_create();
    int *a = malloc(sizeof(int)); *a = 1;
    ASSERT_EQ(sll_insert_at(list, 2, a), -1); // out of bounds (list size is 0)
    ASSERT_EQ(sll_insert_at(list, (size_t)-1, a), -1); // very large index
    sll_destroy(list, int_free);
    free(a);
    return TEST_SUCCESS;
}

int test_insert_remove_null_data() {
    SLinkedList *list = sll_create();
    ASSERT_EQ(sll_insert_back(list, NULL), 0); // allow NULL data
    ASSERT_EQ(list->size, 1);
    ASSERT_EQ(sll_remove_at(list, 0, nullptr), 0); // remove node with NULL data, no free_func
    ASSERT_EQ(list->size, 0);
    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_mixed_operations_integrity() {
    SLinkedList *list = sll_create();
    int *a = malloc(sizeof(int)); *a = 10;
    int *b = malloc(sizeof(int)); *b = 20;
    int *c = malloc(sizeof(int)); *c = 30;
    sll_insert_back(list, a); // [10]
    sll_insert_front(list, b); // [20,10]
    sll_insert_at(list, 1, c); // [20,30,10]
    ASSERT_EQ(list->size, 3);

    ASSERT_EQ(sll_remove_at(list, 1, int_free), 0); // remove 30, [20,10]
    int key = 30;
    SListNode *found = sll_find(list, &key, int_cmp);
    ASSERT_NULLPTR(found);

    key = 20;
    found = sll_find(list, &key, int_cmp);
    ASSERT_NOT_NULLPTR(found);

    key = 10;
    found = sll_find(list, &key, int_cmp);
    ASSERT_NOT_NULLPTR(found);

    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_size() {
    SLinkedList *list = sll_create();
    ASSERT_EQ(sll_size(list), 0);

    int *a = malloc(sizeof(int)); *a = 10;
    int *b = malloc(sizeof(int)); *b = 20;
    sll_insert_back(list, a);
    ASSERT_EQ(sll_size(list), 1);
    sll_insert_back(list, b);
    ASSERT_EQ(sll_size(list), 2);

    sll_remove_at(list, 0, int_free);
    ASSERT_EQ(sll_size(list), 1);

    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_is_empty() {
    SLinkedList *list = sll_create();
    ASSERT_EQ(sll_is_empty(list), 1); // Empty list

    int *a = malloc(sizeof(int)); *a = 10;
    sll_insert_back(list, a);
    ASSERT_EQ(sll_is_empty(list), 0); // Non-empty list

    sll_remove_at(list, 0, int_free);
    ASSERT_EQ(sll_is_empty(list), 1); // Empty again

    ASSERT_EQ(sll_is_empty(nullptr), 1); // NULL list should be considered empty

    sll_destroy(list, nullptr);
    return TEST_SUCCESS;
}

int test_complex_data_type() {
    SLinkedList *list = sll_create();

    Person *p1 = create_person("Alice", 30);
    Person *p2 = create_person("Bob", 25);
    Person *p3 = create_person("Charlie", 40);

    sll_insert_back(list, p1);
    sll_insert_back(list, p2);
    sll_insert_back(list, p3);
    ASSERT_EQ(list->size, 3);

    Person search_key;
    strcpy(search_key.name, "Bob");
    search_key.age = 0; // Age doesn't matter for comparison

    const SListNode *found = sll_find(list, &search_key, person_cmp);
    ASSERT_NOT_NULLPTR(found);
    const Person *found_person = found->data;
    ASSERT_EQ(found_person->age, 25);

    // Clean up
    sll_destroy(list, person_free);
    return TEST_SUCCESS;
}

int test_remove_all() {
    SLinkedList *list = sll_create();

    // Add 10 elements
    for (int i = 0; i < 10; i++) {
        int *val = malloc(sizeof(int));
        *val = i;
        sll_insert_back(list, val);
    }
    ASSERT_EQ(list->size, 10);

    // Remove all elements one by one
    while (!sll_is_empty(list)) {
        sll_remove_at(list, 0, int_free);
    }

    ASSERT_EQ(list->size, 0);
    ASSERT_NULLPTR(list->head);

    sll_destroy(list, nullptr); // Already freed all data
    return TEST_SUCCESS;
}

int test_stress() {
    SLinkedList *list = sll_create();
    constexpr int NUM_ELEMENTS = 10000;

    // Add many elements
    for (int i = 0; i < NUM_ELEMENTS; i++) {
        int *val = malloc(sizeof(int));
        *val = i;
        ASSERT_EQ(sll_insert_back(list, val), 0);
    }
    ASSERT_EQ(list->size, NUM_ELEMENTS);

    // Find an element in the middle
    constexpr int key = NUM_ELEMENTS / 2;
    const SListNode *found = sll_find(list, &key, int_cmp);
    ASSERT_NOT_NULLPTR(found);
    ASSERT_EQ(*(int*)found->data, key);

    // Remove elements from the front
    for (int i = 0; i < NUM_ELEMENTS / 2; i++) {
        ASSERT_EQ(sll_remove_at(list, 0, int_free), 0);
    }
    ASSERT_EQ(list->size, NUM_ELEMENTS / 2);

    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_performance() {
    constexpr int NUM_SIZES = 3;

    printf("\nPerformance tests:\n");
    for (int s = 0; s < NUM_SIZES; s++) {
        constexpr int SIZES[] = {100, 1000, 10000};
        const int SIZE = SIZES[s];
        SLinkedList *list = sll_create();

        // Measure insertion time
        clock_t start = clock();
        for (int i = 0; i < SIZE; i++) {
            int *val = malloc(sizeof(int));
            *val = i;
            sll_insert_back(list, val);
        }
        clock_t end = clock();
        printf("Insert %d elements: %.6f seconds\n", SIZE,
               (double)(end - start) / CLOCKS_PER_SEC);

        // Measure search time for last element
        start = clock();
        int key = SIZE - 1;
        const SListNode *found = sll_find(list, &key, int_cmp);
        end = clock();
        printf("Find last element in %d elements: %.6f seconds\n", SIZE,
               (double)(end - start) / CLOCKS_PER_SEC);
        ASSERT_NOT_NULLPTR(found);

        // Cleanup
        sll_destroy(list, int_free);
    }

    return TEST_SUCCESS;
}

int test_clear() {
    SLinkedList *list = sll_create();

    // Add some elements
    for (int i = 0; i < 5; i++) {
        int *val = malloc(sizeof(int));
        *val = i;
        sll_insert_back(list, val);
    }
    ASSERT_EQ(list->size, 5);

    // Clear the list
    sll_clear(list, int_free);

    // Verify list state
    ASSERT_NULLPTR(list->head);
    ASSERT_EQ(list->size, 0);
    ASSERT_EQ(sll_is_empty(list), 1);

    // Make sure we can still add elements after clearing
    int *val = malloc(sizeof(int));
    *val = 42;
    ASSERT_EQ(sll_insert_back(list, val), 0);
    ASSERT_EQ(list->size, 1);

    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_clear_empty() {
    SLinkedList *list = sll_create();

    // Clear an already empty list
    sll_clear(list, int_free);
    ASSERT_NULLPTR(list->head);
    ASSERT_EQ(list->size, 0);

    sll_destroy(list, nullptr);
    return TEST_SUCCESS;
}

int test_clear_null() {
    // Calling clear on NULL shouldn't crash
    sll_clear(nullptr, int_free);
    return TEST_SUCCESS;
}

int test_remove_front() {
    SLinkedList *list = sll_create();

    // Test on empty list
    ASSERT_EQ(sll_remove_front(list, int_free), -1);

    // Add elements
    int *a = malloc(sizeof(int)); *a = 10;
    int *b = malloc(sizeof(int)); *b = 20;
    int *c = malloc(sizeof(int)); *c = 30;
    sll_insert_back(list, a);
    sll_insert_back(list, b);
    sll_insert_back(list, c);
    ASSERT_EQ(list->size, 3);

    // Remove front
    ASSERT_EQ(sll_remove_front(list, int_free), 0);
    ASSERT_EQ(list->size, 2);

    // Check first element is now 20
    int key = 10;
    ASSERT_NULLPTR(sll_find(list, &key, int_cmp));
    key = 20;
    ASSERT_NOT_NULLPTR(sll_find(list, &key, int_cmp));

    // Remove until empty
    ASSERT_EQ(sll_remove_front(list, int_free), 0);
    ASSERT_EQ(sll_remove_front(list, int_free), 0);
    ASSERT_EQ(list->size, 0);
    ASSERT_NULLPTR(list->head);

    sll_destroy(list, nullptr);
    return TEST_SUCCESS;
}

int test_remove_back() {
    SLinkedList *list = sll_create();

    // Test on empty list
    ASSERT_EQ(sll_remove_back(list, int_free), -1);

    // Test on single element list
    int *a = malloc(sizeof(int)); *a = 10;
    sll_insert_back(list, a);
    ASSERT_EQ(sll_remove_back(list, int_free), 0);
    ASSERT_EQ(list->size, 0);
    ASSERT_NULLPTR(list->head);

    // Test with multiple elements
    int *b = malloc(sizeof(int)); *b = 20;
    int *c = malloc(sizeof(int)); *c = 30;
    int *d = malloc(sizeof(int)); *d = 40;
    sll_insert_back(list, b);
    sll_insert_back(list, c);
    sll_insert_back(list, d);
    ASSERT_EQ(list->size, 3);

    // Remove back
    ASSERT_EQ(sll_remove_back(list, int_free), 0);
    ASSERT_EQ(list->size, 2);

    // Check last element was removed
    int key = 40;
    ASSERT_NULLPTR(sll_find(list, &key, int_cmp));
    key = 30;
    ASSERT_NOT_NULLPTR(sll_find(list, &key, int_cmp));

    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_sort_empty() {
    SLinkedList *list = sll_create();
    ASSERT_EQ(sll_sort(list, int_cmp), 0); // Empty list is already sorted
    ASSERT_EQ(list->size, 0);
    sll_destroy(list, nullptr);
    return TEST_SUCCESS;
}

int test_sort_already_sorted() {
    SLinkedList *list = sll_create();
    for (int i = 0; i < 5; i++) {
        int *val = malloc(sizeof(int));
        *val = i;
        sll_insert_back(list, val);
    }

    ASSERT_EQ(sll_sort(list, int_cmp), 0);

    // Verify order
    const SListNode *node = list->head;
    for (int i = 0; i < 5; i++) {
        ASSERT_EQ(*(int*)node->data, i);
        node = node->next;
    }

    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_sort_reverse_order() {
    SLinkedList *list = sll_create();
    for (int i = 4; i >= 0; i--) {
        int *val = malloc(sizeof(int));
        *val = i;
        sll_insert_back(list, val);
    }

    ASSERT_EQ(sll_sort(list, int_cmp), 0);

    // Verify order
    const SListNode *node = list->head;
    for (int i = 0; i < 5; i++) {
        ASSERT_EQ(*(int*)node->data, i);
        node = node->next;
    }

    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_sort_random_order() {
    SLinkedList *list = sll_create();
    const int values[] = {42, 17, 9, 39, 24, 5, 58};
    constexpr int count = sizeof(values) / sizeof(values[0]);

    for (int i = 0; i < count; i++) {
        int *val = malloc(sizeof(int));
        *val = values[i];
        sll_insert_back(list, val);
    }

    ASSERT_EQ(sll_sort(list, int_cmp), 0);

    // Verify order
    const SListNode *node = list->head;
    for (int i = 0; i < count; i++) {
        const int sorted[] = {5, 9, 17, 24, 39, 42, 58};
        ASSERT_EQ(*(int*)node->data, sorted[i]);
        node = node->next;
    }

    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_sort_with_duplicates() {
    SLinkedList *list = sll_create();
    const int values[] = {5, 2, 9, 5, 7, 2, 9, 5};
    constexpr int count = sizeof(values) / sizeof(values[0]);

    for (int i = 0; i < count; i++) {
        int *val = malloc(sizeof(int));
        *val = values[i];
        sll_insert_back(list, val);
    }

    ASSERT_EQ(sll_sort(list, int_cmp), 0);

    // Verify order
    const SListNode *node = list->head;
    for (int i = 0; i < count; i++) {
        const int sorted[] = {2, 2, 5, 5, 5, 7, 9, 9};
        ASSERT_EQ(*(int*)node->data, sorted[i]);
        node = node->next;
    }

    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_sort_large_list() {
    SLinkedList *list = sll_create();
    constexpr int SIZE = 1000;

    // Insert in reverse order
    for (int i = SIZE - 1; i >= 0; i--) {
        int *val = malloc(sizeof(int));
        *val = i;
        sll_insert_back(list, val);
    }

    const clock_t start = clock();
    ASSERT_EQ(sll_sort(list, int_cmp), 0);
    const clock_t end = clock();
    printf("SLL Sort %d elements: %.6f seconds\n", SIZE,
           (double)(end - start) / CLOCKS_PER_SEC);

    // Verify order (first few elements)
    const SListNode *node = list->head;
    for (int i = 0; i < 10; i++) {
        ASSERT_EQ(*(int*)node->data, i);
        node = node->next;
    }

    // Verify list structure
    ASSERT_EQ(list->size, SIZE);

    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

// Custom comparison function for descending order
int int_cmp_desc(const void *a, const void *b) {
    return (*(int*)b) - (*(int*)a);
}

int test_sort_custom_compare() {
    SLinkedList *list = sll_create();
    for (int i = 0; i < 5; i++) {
        int *val = malloc(sizeof(int));
        *val = i;
        sll_insert_back(list, val);
    }

    // Sort in descending order
    ASSERT_EQ(sll_sort(list, int_cmp_desc), 0);

    // Verify order
    const SListNode *node = list->head;
    for (int i = 4; i >= 0; i--) {
        ASSERT_EQ(*(int*)node->data, i);
        node = node->next;
    }

    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_sort_null_args() {
    SLinkedList *list = sll_create();
    ASSERT_EQ(sll_sort(nullptr, int_cmp), -1); // NULL list
    ASSERT_EQ(sll_sort(list, nullptr), -1);    // NULL compare function
    sll_destroy(list, nullptr);
    return TEST_SUCCESS;
}

int test_sort_stability() {
    SLinkedList *list = sll_create();

    // Person structs with same name (for comparison) but different ages
    Person *p1 = create_person("Alice", 30);
    Person *p2 = create_person("Alice", 25);  // Same name, different age
    Person *p3 = create_person("Bob", 35);
    Person *p4 = create_person("Alice", 40);  // Same name, different age

    sll_insert_back(list, p1);
    sll_insert_back(list, p2);
    sll_insert_back(list, p3);
    sll_insert_back(list, p4);

    // Sort by name only - ages should remain in insertion order for equal names
    ASSERT_EQ(sll_sort(list, person_cmp), 0);

    // Verify order: All Alice's should come before Bob
    const SListNode *node = list->head;
    const Person *person = node->data;
    ASSERT_EQ(strcmp(person->name, "Alice"), 0);
    ASSERT_EQ(person->age, 30); // First Alice

    node = node->next;
    person = node->data;
    ASSERT_EQ(strcmp(person->name, "Alice"), 0);
    ASSERT_EQ(person->age, 25); // Second Alice

    node = node->next;
    person = node->data;
    ASSERT_EQ(strcmp(person->name, "Alice"), 0);
    ASSERT_EQ(person->age, 40); // Third Alice

    node = node->next;
    person = node->data;
    ASSERT_EQ(strcmp(person->name, "Bob"), 0);
    ASSERT_EQ(person->age, 35);

    sll_destroy(list, person_free);
    return TEST_SUCCESS;
}

int test_reverse() {
    SLinkedList *list = sll_create();

    // Test empty list
    ASSERT_EQ(sll_reverse(list), 0);
    ASSERT_EQ(list->size, 0);

    // Test single element
    int *a = malloc(sizeof(int)); *a = 10;
    sll_insert_back(list, a);
    ASSERT_EQ(sll_reverse(list), 0);
    ASSERT_EQ(list->size, 1);
    ASSERT_EQ(*(int*)list->head->data, 10);

    // Test multiple elements
    int *b = malloc(sizeof(int)); *b = 20;
    int *c = malloc(sizeof(int)); *c = 30;
    sll_insert_back(list, b);
    sll_insert_back(list, c);
    // List is now [10,20,30]

    ASSERT_EQ(sll_reverse(list), 0);
    // List should now be [30,20,10]

    const SListNode *node = list->head;
    ASSERT_EQ(*(int*)node->data, 30);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 20);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 10);
    ASSERT_NULLPTR(node->next);

    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_merge() {
    SLinkedList *list1 = sll_create();
    SLinkedList *list2 = sll_create();

    // Test merging empty lists
    ASSERT_EQ(sll_merge(list1, list2), 0);
    ASSERT_EQ(list1->size, 0);
    ASSERT_EQ(list2->size, 0);

    // Test merging empty with non-empty
    int *a1 = malloc(sizeof(int)); *a1 = 10;
    int *b1 = malloc(sizeof(int)); *b1 = 20;
    sll_insert_back(list2, a1);
    sll_insert_back(list2, b1);

    ASSERT_EQ(sll_merge(list1, list2), 0);
    ASSERT_EQ(list1->size, 2);
    ASSERT_EQ(list2->size, 0);
    ASSERT_NULLPTR(list2->head);

    const SListNode *node = list1->head;
    ASSERT_EQ(*(int*)node->data, 10);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 20);

    // Test merging two non-empty lists
    SLinkedList *list3 = sll_create();
    int *a2 = malloc(sizeof(int)); *a2 = 30;
    int *b2 = malloc(sizeof(int)); *b2 = 40;
    sll_insert_back(list3, a2);
    sll_insert_back(list3, b2);

    ASSERT_EQ(sll_merge(list1, list3), 0);
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

    sll_destroy(list1, int_free);
    sll_destroy(list2, nullptr); // Already empty
    sll_destroy(list3, nullptr); // Already empty
    return TEST_SUCCESS;
}

int test_splice() {
    SLinkedList *dest = sll_create();
    SLinkedList *src = sll_create();

    // Test splicing empty lists
    ASSERT_EQ(sll_splice(dest, src, 0), 0);
    ASSERT_EQ(dest->size, 0);
    ASSERT_EQ(src->size, 0);

    // Setup lists
    int *a = malloc(sizeof(int)); *a = 10;
    int *b = malloc(sizeof(int)); *b = 20;
    int *c = malloc(sizeof(int)); *c = 30;
    sll_insert_back(dest, a);
    sll_insert_back(dest, b);
    sll_insert_back(dest, c);
    // dest = [10,20,30]

    int *d = malloc(sizeof(int)); *d = 40;
    int *e = malloc(sizeof(int)); *e = 50;
    sll_insert_back(src, d);
    sll_insert_back(src, e);
    // src = [40,50]

    // Test splicing at beginning
    SLinkedList *dest2 = sll_create();
    SLinkedList *src2 = sll_create();
    int *a2 = malloc(sizeof(int)); *a2 = 10;
    int *b2 = malloc(sizeof(int)); *b2 = 20;
    int *c2 = malloc(sizeof(int)); *c2 = 30;
    int *d2 = malloc(sizeof(int)); *d2 = 40;
    int *e2 = malloc(sizeof(int)); *e2 = 50;
    sll_insert_back(dest2, a2);
    sll_insert_back(dest2, b2);
    sll_insert_back(dest2, c2);
    sll_insert_back(src2, d2);
    sll_insert_back(src2, e2);

    ASSERT_EQ(sll_splice(dest2, src2, 0), 0);
    ASSERT_EQ(dest2->size, 5);
    ASSERT_EQ(src2->size, 0);

    const SListNode *node = dest2->head;
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
    ASSERT_EQ(sll_splice(dest, src, 1), 0);
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
    SLinkedList *dest3 = sll_create();
    SLinkedList *src3 = sll_create();
    int *a3 = malloc(sizeof(int)); *a3 = 10;
    int *b3 = malloc(sizeof(int)); *b3 = 20;
    int *c3 = malloc(sizeof(int)); *c3 = 30;
    int *d3 = malloc(sizeof(int)); *d3 = 40;
    int *e3 = malloc(sizeof(int)); *e3 = 50;
    sll_insert_back(dest3, a3);
    sll_insert_back(dest3, b3);
    sll_insert_back(dest3, c3);
    sll_insert_back(src3, d3);
    sll_insert_back(src3, e3);

    ASSERT_EQ(sll_splice(dest3, src3, 3), 0);
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

    sll_destroy(dest, int_free);
    sll_destroy(src, nullptr);
    sll_destroy(dest2, int_free);
    sll_destroy(src2, nullptr);
    sll_destroy(dest3, int_free);
    sll_destroy(src3, nullptr);

    return TEST_SUCCESS;
}

int test_equals() {
    SLinkedList *list1 = sll_create();
    SLinkedList *list2 = sll_create();

    // Empty lists should be equal
    ASSERT_EQ(sll_equals(list1, list2, int_cmp), 1);

    // Lists with same elements should be equal
    int *a1 = malloc(sizeof(int)); *a1 = 10;
    int *b1 = malloc(sizeof(int)); *b1 = 20;
    int *a2 = malloc(sizeof(int)); *a2 = 10;
    int *b2 = malloc(sizeof(int)); *b2 = 20;
    sll_insert_back(list1, a1);
    sll_insert_back(list1, b1);
    sll_insert_back(list2, a2);
    sll_insert_back(list2, b2);

    ASSERT_EQ(sll_equals(list1, list2, int_cmp), 1);

    // Lists with different elements should not be equal
    int *c2 = malloc(sizeof(int)); *c2 = 30;
    sll_insert_back(list2, c2);

    ASSERT_EQ(sll_equals(list1, list2, int_cmp), 0);

    // Lists with same size but different elements should not be equal
    SLinkedList *list3 = sll_create();
    int *a3 = malloc(sizeof(int)); *a3 = 10;
    int *b3 = malloc(sizeof(int)); *b3 = 30; // Different value
    sll_insert_back(list3, a3);
    sll_insert_back(list3, b3);

    ASSERT_EQ(sll_equals(list1, list3, int_cmp), 0);

    // Error cases
    ASSERT_EQ(sll_equals(nullptr, list2, int_cmp), -1);
    ASSERT_EQ(sll_equals(list1, nullptr, int_cmp), -1);
    ASSERT_EQ(sll_equals(list1, list2, nullptr), -1);

    sll_destroy(list1, int_free);
    sll_destroy(list2, int_free);
    sll_destroy(list3, int_free);
    return TEST_SUCCESS;
}

// Predicate function that returns non-zero for even numbers
int is_even(const void *data) {
    return (*(int*)data % 2 == 0) ? 1 : 0;
}

// Transform function that doubles a number
void *double_value(const void *data) {
    int *result = malloc(sizeof(int));
    if (result) {
        *result = *(int*)data * 2;
    }
    return result;
}

// Action function that increments a number
void increment(void *data) {
    (*(int*)data)++;
}

int test_filter() {
    SLinkedList *list = sll_create();

    // Add numbers 0-9
    for (int i = 0; i < 10; i++) {
        int *val = malloc(sizeof(int));
        *val = i;
        sll_insert_back(list, val);
    }

    // Filter for even numbers
    SLinkedList *filtered = sll_filter(list, is_even);
    ASSERT_NOT_NULLPTR(filtered);
    ASSERT_EQ(filtered->size, 5);  // Should contain 0,2,4,6,8

    // Verify filtered list
    const SListNode *node = filtered->head;
    for (int i = 0; i < 5; i++) {
        const int expected_values[] = {0, 2, 4, 6, 8};
        ASSERT_EQ(*(int*)node->data, expected_values[i]);
        node = node->next;
    }

    // Make sure original list is unchanged
    ASSERT_EQ(list->size, 10);

    // Test empty list
    SLinkedList *empty_list = sll_create();
    SLinkedList *filtered_empty = sll_filter(empty_list, is_even);
    ASSERT_NOT_NULLPTR(filtered_empty);
    ASSERT_EQ(filtered_empty->size, 0);

    // Test null cases
    ASSERT_NULLPTR(sll_filter(nullptr, is_even));
    ASSERT_NULLPTR(sll_filter(list, nullptr));

    sll_destroy(list, int_free);
    sll_destroy(filtered, nullptr);  // Don't free data; it's owned by the original list
    sll_destroy(empty_list, nullptr);
    sll_destroy(filtered_empty, nullptr);
    return TEST_SUCCESS;
}

int test_transform() {
    SLinkedList *list = sll_create();

    // Add numbers 1-5
    for (int i = 1; i <= 5; i++) {
        int *val = malloc(sizeof(int));
        *val = i;
        sll_insert_back(list, val);
    }

    // Map to double each value
    SLinkedList *mapped = sll_transform(list, double_value, free);
    ASSERT_NOT_NULLPTR(mapped);
    ASSERT_EQ(mapped->size, 5);

    // Verify mapped list (should be 2,4,6,8,10)
    const SListNode *node = mapped->head;
    for (int i = 1; i <= 5; i++) {
        ASSERT_EQ(*(int*)node->data, i * 2);
        node = node->next;
    }

    // Make sure original list is unchanged
    node = list->head;
    for (int i = 1; i <= 5; i++) {
        ASSERT_EQ(*(int*)node->data, i);
        node = node->next;
    }

    // Test empty list
    SLinkedList *empty_list = sll_create();
    SLinkedList *mapped_empty = sll_transform(empty_list, double_value, free);
    ASSERT_NOT_NULLPTR(mapped_empty);
    ASSERT_EQ(mapped_empty->size, 0);

    // Test null cases
    ASSERT_NULLPTR(sll_transform(nullptr, double_value, free));
    ASSERT_NULLPTR(sll_transform(list, nullptr, nullptr));

    sll_destroy(list, int_free);
    sll_destroy(mapped, int_free);
    sll_destroy(empty_list, nullptr);
    sll_destroy(mapped_empty, nullptr);
    return TEST_SUCCESS;
}

int test_for_each() {
    SLinkedList *list = sll_create();

    // Add numbers 1-5
    for (int i = 1; i <= 5; i++) {
        int *val = malloc(sizeof(int));
        *val = i;
        sll_insert_back(list, val);
    }

    // Increment each value
    sll_for_each(list, increment);

    // Verify each value is incremented
    const SListNode *node = list->head;
    for (int i = 1; i <= 5; i++) {
        ASSERT_EQ(*(int*)node->data, i + 1);
        node = node->next;
    }

    // Test empty list
    SLinkedList *empty_list = sll_create();
    sll_for_each(empty_list, increment);  // Should do nothing

    // Test null cases
    sll_for_each(nullptr, increment);  // Should do nothing
    sll_for_each(list, nullptr);       // Should do nothing

    sll_destroy(list, int_free);
    sll_destroy(empty_list, nullptr);
    return TEST_SUCCESS;
}

// Clone an integer
void *int_copy(const void *data) {
    const int *original = data;
    int *copy = malloc(sizeof(int));
    if (copy) {
        *copy = *original;
    }
    return copy;
}

// Clone a person
void *person_copy(const void *data) {
    const Person *original = data;
    Person *copy = create_person(original->name, original->age);
    return copy;
}

int test_copy_shallow() {
    SLinkedList *list = sll_create();

    // Add some elements
    for (int i = 0; i < 5; i++) {
        int *val = malloc(sizeof(int));
        *val = i * 10;
        sll_insert_back(list, val);
    }

    // Create shallow clone
    SLinkedList *clone = sll_copy(list);
    ASSERT_NOT_NULLPTR(clone);
    ASSERT_EQ(clone->size, list->size);

    // Verify structure
    SListNode *orig_node = list->head;
    SListNode *clone_node = clone->head;
    while (orig_node && clone_node) {
        // Data pointers should be identical in shallow clone
        ASSERT_EQ(orig_node->data, clone_node->data);
        // But nodes themselves should be different
        ASSERT_NOT_EQ(orig_node, clone_node);

        orig_node = orig_node->next;
        clone_node = clone_node->next;
    }

    // Modifying data should affect both lists (shared pointers)
    int *first_value = list->head->data;
    *first_value = 999;
    ASSERT_EQ(*(int*)clone->head->data, 999);

    // Cleanup - free each int only once since they're shared
    sll_destroy(list, int_free);
    sll_destroy(clone, nullptr);

    return TEST_SUCCESS;
}

int test_copy_deep() {
    SLinkedList *list = sll_create();

    // Add some elements
    for (int i = 0; i < 5; i++) {
        int *val = malloc(sizeof(int));
        *val = i * 10;
        sll_insert_back(list, val);
    }

    // Create deep clone
    SLinkedList *clone = sll_copy_deep(list, int_copy, int_free);
    ASSERT_NOT_NULLPTR(clone);
    ASSERT_EQ(clone->size, list->size);

    // Verify structure and values
    const SListNode *orig_node = list->head;
    const SListNode *clone_node = clone->head;
    while (orig_node && clone_node) {
        // Data pointers should be different in deep clone
        ASSERT_NOT_EQ(orig_node->data, clone_node->data);
        // But values should be the same
        ASSERT_EQ(*(int*)orig_node->data, *(int*)clone_node->data);

        orig_node = orig_node->next;
        clone_node = clone_node->next;
    }

    // Modifying data should not affect the other list (independent copies)
    int *first_value = list->head->data;
    *first_value = 999;
    ASSERT_NOT_EQ(*(int*)clone->head->data, 999);

    // Cleanup - each list has its own data
    sll_destroy(list, int_free);
    sll_destroy(clone, int_free);

    return TEST_SUCCESS;
}

int test_copy_complex_data() {
    SLinkedList *list = sll_create();

    // Add some people
    Person *p1 = create_person("Alice", 30);
    Person *p2 = create_person("Bob", 25);
    Person *p3 = create_person("Charlie", 40);

    sll_insert_back(list, p1);
    sll_insert_back(list, p2);
    sll_insert_back(list, p3);

    // Create deep clone
    SLinkedList *clone = sll_copy_deep(list, person_copy, person_free);
    ASSERT_NOT_NULLPTR(clone);
    ASSERT_EQ(clone->size, list->size);

    // Verify structure and values
    const SListNode *orig_node = list->head;
    const SListNode *clone_node = clone->head;
    while (orig_node && clone_node) {
        Person *orig_person = orig_node->data;
        Person *clone_person = clone_node->data;

        // Data pointers should be different
        ASSERT_NOT_EQ(orig_person, clone_person);
        // But values should be the same
        ASSERT_EQ(strcmp(orig_person->name, clone_person->name), 0);
        ASSERT_EQ(orig_person->age, clone_person->age);

        orig_node = orig_node->next;
        clone_node = clone_node->next;
    }

    // Modifying should not affect the other list
    Person *first_person = list->head->data;
    first_person->age = 99;
    const Person *clone_first = clone->head->data;
    ASSERT_NOT_EQ(first_person->age, clone_first->age);

    // Cleanup
    sll_destroy(list, person_free);
    sll_destroy(clone, person_free);

    return TEST_SUCCESS;
}

int test_copy_empty() {
    SLinkedList *list = sll_create();

    // Clone empty list
    SLinkedList *shallow_clone = sll_copy(list);
    ASSERT_NOT_NULLPTR(shallow_clone);
    ASSERT_EQ(shallow_clone->size, 0);
    ASSERT_NULLPTR(shallow_clone->head);

    SLinkedList *deep_clone = sll_copy_deep(list, int_copy, int_free);
    ASSERT_NOT_NULLPTR(deep_clone);
    ASSERT_EQ(deep_clone->size, 0);
    ASSERT_NULLPTR(deep_clone->head);

    // Cleanup
    sll_destroy(list, nullptr);
    sll_destroy(shallow_clone, nullptr);
    sll_destroy(deep_clone, nullptr);

    return TEST_SUCCESS;
}

int test_copy_null() {
    // Should handle NULL gracefully
    ASSERT_NULLPTR(sll_copy(nullptr));
    ASSERT_NULLPTR(sll_copy_deep(nullptr, int_copy, int_free));

    // Should require a valid copy function
    SLinkedList *list = sll_create();
    ASSERT_NULLPTR(sll_copy_deep(list, nullptr, nullptr));
    sll_destroy(list, nullptr);

    return TEST_SUCCESS;
}

int test_iterator_basic() {
    SLinkedList *list = sll_create();
    int *a = malloc(sizeof(int)); *a = 10;
    int *b = malloc(sizeof(int)); *b = 20;
    int *c = malloc(sizeof(int)); *c = 30;
    sll_insert_back(list, a);
    sll_insert_back(list, b);
    sll_insert_back(list, c);

    Iterator it = sll_iterator(list);
    ASSERT_NOT_NULLPTR(it.data_state);
    ASSERT_EQ(it.is_valid(&it), 1);
    ASSERT_EQ(it.has_next(&it), 1);

    // Test getting first element
    void *data = it.next(&it);
    ASSERT_NOT_NULLPTR(data);
    ASSERT_EQ(*(int*)data, 10);

    // Test getting second element
    ASSERT_EQ(it.has_next(&it), 1);
    data = it.next(&it);
    ASSERT_NOT_NULLPTR(data);
    ASSERT_EQ(*(int*)data, 20);

    // Test getting third element
    ASSERT_EQ(it.has_next(&it), 1);
    data = it.next(&it);
    ASSERT_NOT_NULLPTR(data);
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

int test_iterator_empty_list() {
    SLinkedList *list = sll_create();

    Iterator it = sll_iterator(list);
    ASSERT_NOT_NULLPTR(it.data_state);
    ASSERT_EQ(it.is_valid(&it), 1);
    ASSERT_EQ(it.has_next(&it), 0);
    ASSERT_NULLPTR(it.next(&it));

    it.destroy(&it);
    sll_destroy(list, nullptr);
    return TEST_SUCCESS;
}

int test_iterator_null_list() {
    const Iterator it = sll_iterator(nullptr);
    ASSERT_NULLPTR(it.data_state);

    // No need to destroy, it's invalid
    return TEST_SUCCESS;
}

int test_iterator_get() {
    SLinkedList *list = sll_create();
    int *a = malloc(sizeof(int)); *a = 10;
    int *b = malloc(sizeof(int)); *b = 20;
    sll_insert_back(list, a);
    sll_insert_back(list, b);

    Iterator it = sll_iterator(list);

    // Test get() without advancing
    void *data = it.get(&it);
    ASSERT_NOT_NULLPTR(data);
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

int test_iterator_unsupported_ops() {
    SLinkedList *list = sll_create();
    int *a = malloc(sizeof(int)); *a = 10;
    sll_insert_back(list, a);

    Iterator it = sll_iterator(list);

    // has_prev and prev shouldn't work for SLL
    ASSERT_EQ(it.has_prev(&it), 0);
    ASSERT_NULLPTR(it.prev(&it));

    it.destroy(&it);
    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_from_iterator_basic() {
    SLinkedList *list = sll_create();
    int *a = malloc(sizeof(int)); *a = 10;
    int *b = malloc(sizeof(int)); *b = 20;
    int *c = malloc(sizeof(int)); *c = 30;
    sll_insert_back(list, a);
    sll_insert_back(list, b);
    sll_insert_back(list, c);

    Iterator it = sll_iterator(list);

    // Create new list from iterator (shallow copy)
    SLinkedList *new_list = sll_from_iterator(&it, nullptr, nullptr);
    ASSERT_NOT_NULLPTR(new_list);
    ASSERT_EQ(new_list->size, 3);

    // Check that values are the same
    const SListNode *node = new_list->head;
    ASSERT_EQ(*(int*)node->data, 10);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 20);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 30);

    // Check that pointers are the same (shallow copy)
    const SListNode *orig = list->head;
    node = new_list->head;
    while (orig && node) {
        ASSERT_EQ(orig->data, node->data);
        orig = orig->next;
        node = node->next;
    }

    it.destroy(&it);
    sll_destroy(list, int_free);
    sll_destroy(new_list, nullptr); // Don't free shared data
    return TEST_SUCCESS;
}

int test_from_iterator_deep_copy() {
    SLinkedList *list = sll_create();
    int *a = malloc(sizeof(int)); *a = 10;
    int *b = malloc(sizeof(int)); *b = 20;
    int *c = malloc(sizeof(int)); *c = 30;
    sll_insert_back(list, a);
    sll_insert_back(list, b);
    sll_insert_back(list, c);

    Iterator it = sll_iterator(list);

    // Create new list from iterator with deep copy
    SLinkedList *new_list = sll_from_iterator(&it, int_copy, int_free);
    ASSERT_NOT_NULLPTR(new_list);
    ASSERT_EQ(new_list->size, 3);

    // Check that values are the same
    const SListNode *node = new_list->head;
    ASSERT_EQ(*(int*)node->data, 10);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 20);
    node = node->next;
    ASSERT_EQ(*(int*)node->data, 30);

    // Check that pointers are different (deep copy)
    const SListNode *orig = list->head;
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

int test_from_iterator_empty() {
    SLinkedList *list = sll_create();

    Iterator it = sll_iterator(list);

    // Create new list from empty iterator
    SLinkedList *new_list = sll_from_iterator(&it, int_copy, int_free);
    ASSERT_NOT_NULLPTR(new_list);
    ASSERT_EQ(new_list->size, 0);
    ASSERT_NULLPTR(new_list->head);

    it.destroy(&it);
    sll_destroy(list, nullptr);
    sll_destroy(new_list, nullptr);
    return TEST_SUCCESS;
}

int test_from_iterator_null() {
    ASSERT_NULLPTR(sll_from_iterator(nullptr, int_copy, int_free));

    SLinkedList *list = sll_create();
    Iterator it = sll_iterator(list);

    // Iterator should be valid but copy function is optional
    SLinkedList *new_list = sll_from_iterator(&it, nullptr, nullptr);
    ASSERT_NOT_NULLPTR(new_list);

    it.destroy(&it);
    sll_destroy(list, nullptr);
    sll_destroy(new_list, nullptr);
    return TEST_SUCCESS;
}

int test_iterator_chaining() {
    SLinkedList *list = sll_create();
    for (int i = 0; i < 10; i++) {
        int *val = malloc(sizeof(int));
        *val = i;
        sll_insert_back(list, val);
    }

    // Create an iterator
    Iterator it = sll_iterator(list);

    // Make a second list from the first 5 elements
    SLinkedList *list2 = sll_create();
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
    SLinkedList *list3 = sll_from_iterator(&it2, int_copy, int_free);
    ASSERT_EQ(list3->size, 5);

    // Verify contents of third list
    const SListNode *node = list3->head;
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

    return nullptr;
}

static int dummy_is_valid(const Iterator *it) {
    (void)it;  // Unused parameter

    return 0;
}

int test_from_iterator_null_edge_cases() {
    // Test with nullptr iterator
    ASSERT_NULLPTR(sll_from_iterator(nullptr, int_copy, int_free));
    ASSERT_NULLPTR(sll_from_iterator(nullptr, nullptr, nullptr));

    // Test with invalid iterator (manually created)
    Iterator invalid_it = {};  // All fields set to nullptr/0
    ASSERT_NULLPTR(sll_from_iterator(&invalid_it, int_copy, int_free));

    // Test with partially initialized iterator
    invalid_it.has_next = dummy_has_next;
    invalid_it.next = dummy_next;
    invalid_it.is_valid = dummy_is_valid; // explicitly invalid
    ASSERT_NULLPTR(sll_from_iterator(&invalid_it, int_copy, int_free));

    // Create a valid iterator but destroy it
    SLinkedList *list = sll_create();
    int *a = malloc(sizeof(int)); *a = 42;
    sll_insert_back(list, a);

    Iterator it = sll_iterator(list);
    it.destroy(&it);  // Destroy the iterator's state
    ASSERT_NULLPTR(sll_from_iterator(&it, int_copy, int_free));  // Should handle destroyed iterator safely

    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_iterator_null_list_comprehensive() {
    // Create iterator from nullptr
    Iterator it = sll_iterator(nullptr);

    // Verify iterator is properly initialized as invalid
    ASSERT_NULLPTR(it.data_state);

    // Test that all operations handle the null state gracefully
    ASSERT_EQ(it.has_next(&it), 0);
    ASSERT_NULLPTR(it.next(&it));
    ASSERT_EQ(it.has_prev(&it), 0);
    ASSERT_NULLPTR(it.prev(&it));
    ASSERT_NULLPTR(it.get(&it));
    ASSERT_EQ(it.is_valid(&it), 0);

    // These operations should not crash with invalid iterator
    it.reset(&it);
    it.destroy(&it);

    return TEST_SUCCESS;
}

int test_multiple_iterators() {
    SLinkedList *list = sll_create();
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

int test_iterator_with_modification() {
    SLinkedList *list = sll_create();
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

// --- Helper for testing allocation failures ---

// State for the failing allocator
static int alloc_fail_countdown = -1; // -1 means never fail

// A custom allocator that will fail after a certain number of calls
void* failing_alloc(size_t size) {
    if (alloc_fail_countdown == 0) {
        return NULL; // Trigger failure
    }
    if (alloc_fail_countdown > 0) {
        alloc_fail_countdown--;
    }
    return calloc(1, size); // Use calloc for safety and consistency
}

void failing_free(void* ptr) {
    free(ptr);
}

// A copy function that uses the failing allocator
void *failing_int_copy(const void *data) {
    const int *original = data;
    // Use failing_alloc instead of malloc
    int *copy = failing_alloc(sizeof(int));
    if (copy) {
        *copy = *original;
    }
    return copy;
}

// A transform function that uses the failing allocator
void *double_value_failing(const void *data) {
    int *result = failing_alloc(sizeof(int)); // Use the failing allocator
    if (result) {
        *result = *(int*)data * 2;
    }
    return result;
}

// Helper to set up the failing allocator
void set_alloc_fail_countdown(int count) {
    alloc_fail_countdown = count;
}

int test_transform_allocation_failure() {
    set_alloc_fail_countdown(-1); // Ensure normal allocation for setup
    SLinkedList *list = sll_create_custom(failing_alloc, failing_free);
    ASSERT_NOT_NULLPTR(list);
    for (int i = 0; i < 5; i++) {
        int *val = malloc(sizeof(int)); *val = i;
        sll_insert_back(list, val);
    }

    // Case 1: Fail on creation of the result list
    set_alloc_fail_countdown(0);
    SLinkedList *mapped1 = sll_transform(list, double_value_failing, failing_free);
    ASSERT_NULLPTR(mapped1);

    // Case 2: Fail on data allocation inside the transform function
    // Allocations: 1=result list, FAIL on 2=data for first element
    set_alloc_fail_countdown(1);
    SLinkedList *mapped2 = sll_transform(list, double_value_failing, failing_free);
    ASSERT_NULLPTR(mapped2); // sll_transform should handle this and clean up

    // Case 3: Fail on node allocation inside sll_insert_back
    // Allocations: 1=result list, 2=data for first element, FAIL on 3=node for first element
    set_alloc_fail_countdown(2);
    SLinkedList *mapped3 = sll_transform(list, double_value_failing, failing_free);
    ASSERT_NULLPTR(mapped3);

    set_alloc_fail_countdown(-1);
    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_copy_deep_allocation_failure() {
    set_alloc_fail_countdown(-1);
    SLinkedList *list = sll_create_custom(failing_alloc, failing_free);
    for (int i = 0; i < 5; i++) {
        int *val = malloc(sizeof(int)); *val = i;
        sll_insert_back(list, val);
    }

    // Case 1: Fail allocating the new list struct itself
    set_alloc_fail_countdown(0);
    SLinkedList *clone1 = sll_copy_deep(list, failing_int_copy, failing_free);
    ASSERT_NULLPTR(clone1);

    // Case 2: Fail allocating a node partway through
    set_alloc_fail_countdown(3); // 1=clone list, 2=data0, 3=node0, FAIL on data1
    SLinkedList *clone2 = sll_copy_deep(list, failing_int_copy, failing_free);
    ASSERT_NULLPTR(clone2);

    // Case 3: Fail allocating the *data* partway through
    set_alloc_fail_countdown(2); // 1=clone list, 2=data0, FAIL on node0
    SLinkedList *clone3 = sll_copy_deep(list, failing_int_copy, failing_free);
    ASSERT_NULLPTR(clone3);

    set_alloc_fail_countdown(-1); // Reset for cleanup
    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_insert_allocation_failure() {
    set_alloc_fail_countdown(-1);
    SLinkedList *list = sll_create_custom(failing_alloc, failing_free);
    int *a = malloc(sizeof(int)); *a = 1;
    sll_insert_back(list, a);
    ASSERT_EQ(list->size, 1);

    // Set allocator to fail on the next allocation
    set_alloc_fail_countdown(0);
    int *b = malloc(sizeof(int)); *b = 2;
    ASSERT_EQ(sll_insert_back(list, b), -1);

    // Verify list is unchanged
    ASSERT_EQ(list->size, 1);
    ASSERT_NOT_NULLPTR(list->head);
    ASSERT_NULLPTR(list->head->next);

    set_alloc_fail_countdown(-1);
    sll_destroy(list, int_free);
    free(b); // 'b' was never added to the list, so we must free it manually
    return TEST_SUCCESS;
}

int test_iterator_allocation_failure() {
    set_alloc_fail_countdown(-1);
    SLinkedList *list = sll_create_custom(failing_alloc, failing_free);
    int *a = malloc(sizeof(int)); *a = 1;
    sll_insert_back(list, a);

    // Set allocator to fail on iterator state allocation
    set_alloc_fail_countdown(0);
    Iterator it = sll_iterator(list);

    // Iterator should be invalid as state allocation failed
    ASSERT_NULLPTR(it.data_state);
    ASSERT_EQ(it.is_valid(&it), 0);

    set_alloc_fail_countdown(-1);
    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_from_iterator_custom_alloc_failure() {
    set_alloc_fail_countdown(-1);
    SLinkedList *list = sll_create();
    for (int i = 0; i < 5; i++) {
        int *val = malloc(sizeof(int)); *val = i;
        sll_insert_back(list, val);
    }
    Iterator it = sll_iterator(list);

    // Case 1: Fail on list creation
    set_alloc_fail_countdown(0);
    SLinkedList *new_list1 = sll_from_iterator_custom(&it, failing_int_copy, failing_free, failing_alloc, failing_free);
    ASSERT_NULLPTR(new_list1);
    it.reset(&it);

    // Case 2: Fail on data copy
    set_alloc_fail_countdown(1); // 1=new list, FAIL on data copy
    SLinkedList *new_list2 = sll_from_iterator_custom(&it, failing_int_copy, failing_free, failing_alloc, failing_free);
    ASSERT_NULLPTR(new_list2);
    it.reset(&it);

    // Case 3: Fail on node insertion
    set_alloc_fail_countdown(2); // 1=new list, 2=data copy, FAIL on node insert
    SLinkedList *new_list3 = sll_from_iterator_custom(&it, failing_int_copy, failing_free, failing_alloc, failing_free);
    ASSERT_NULLPTR(new_list3);

    it.destroy(&it);
    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int main(void)
{
    int failed = 0;
    if (test_create_destroy() != TEST_SUCCESS) {
        printf("test_create_destroy failed\n");
        failed++;
    }
    if (test_insert_front_back_find() != TEST_SUCCESS) {
        printf("test_insert_front_back_find failed\n");
        failed++;
    }
    if (test_remove() != TEST_SUCCESS) {
        printf("test_remove failed\n");
        failed++;
    }
    if (test_remove_not_found() != TEST_SUCCESS) {
        printf("test_remove_not_found failed\n");
        failed++;
    }
    if (test_custom_allocator() != TEST_SUCCESS) {
        printf("test_custom_allocator failed\n");
        failed++;
    }
    if (test_nullptr_handling() != TEST_SUCCESS) {
        printf("test_nullptr_handling failed\n");
        failed++;
    }
    if (test_insert_at() != TEST_SUCCESS) {
        printf("test_insert_at failed\n");
        failed++;
    }
    if (test_remove_at() != TEST_SUCCESS) {
        printf("test_remove_at failed\n");
        failed++;
    }
    if (test_remove_at_head() != TEST_SUCCESS) {
        printf("test_remove_at_head failed\n");
        failed++;
    }
    if (test_remove_at_last() != TEST_SUCCESS) {
        printf("test_remove_at_last failed\n");
        failed++;
    }
    if (test_remove_at_invalid() != TEST_SUCCESS) {
        printf("test_remove_at_invalid failed\n");
        failed++;
    }
    if (test_remove_at_empty() != TEST_SUCCESS) {
        printf("test_remove_at_empty failed\n");
        failed++;
    }
    if (test_remove_at_single_element() != TEST_SUCCESS) {
        printf("test_remove_at_single_element failed\n");
        failed++;
    }

    if (test_remove_at_single_element_invalid_pos() != TEST_SUCCESS) {
        printf("test_remove_at_single_element_invalid_pos failed\n");
        failed++;
    }
    if (test_insert_at_out_of_bounds() != TEST_SUCCESS) {
        printf("test_insert_at_out_of_bounds failed\n");
        failed++;
    }
    if (test_insert_remove_null_data() != TEST_SUCCESS) {
        printf("test_insert_remove_null_data failed\n");
        failed++;
    }
    if (test_mixed_operations_integrity() != TEST_SUCCESS) {
        printf("test_mixed_operations_integrity failed\n");
        failed++;
    }
    if (test_size() != TEST_SUCCESS) {
        printf("test_size failed\n");
        failed++;
    }
    if (test_is_empty() != TEST_SUCCESS) {
        printf("test_is_empty failed\n");
        failed++;
    }
    if (test_complex_data_type() != TEST_SUCCESS) {
        printf("test_complex_data_type failed\n");
        failed++;
    }
    if (test_remove_all() != TEST_SUCCESS) {
        printf("test_remove_all failed\n");
        failed++;
    }
    if (test_stress() != TEST_SUCCESS) {
        printf("test_stress failed\n");
        failed++;
    }
    if (test_performance() != TEST_SUCCESS) {
        printf("test_performance failed\n");
        failed++;
    }
    if (test_clear() != TEST_SUCCESS) {
        printf("test_clear failed\n");
        failed++;
    }
    if (test_clear_empty() != TEST_SUCCESS) {
        printf("test_clear_empty failed\n");
        failed++;
    }
    if (test_clear_null() != TEST_SUCCESS) {
        printf("test_clear_null failed\n");
        failed++;
    }
    if (test_remove_front() != TEST_SUCCESS) {
        printf("test_remove_front failed\n");
        failed++;
    }
    if (test_remove_back() != TEST_SUCCESS) {
        printf("test_remove_back failed\n");
        failed++;
    }
    if (test_sort_empty() != TEST_SUCCESS) {
        printf("test_sort_empty failed\n");
        failed++;
    }
    if (test_sort_already_sorted() != TEST_SUCCESS) {
        printf("test_sort_already_sorted failed\n");
        failed++;
    }
    if (test_sort_reverse_order() != TEST_SUCCESS) {
        printf("test_sort_reverse_order failed\n");
        failed++;
    }
    if (test_sort_random_order() != TEST_SUCCESS) {
        printf("test_sort_random_order failed\n");
        failed++;
    }
    if (test_sort_with_duplicates() != TEST_SUCCESS) {
        printf("test_sort_with_duplicates failed\n");
        failed++;
    }
    if (test_sort_large_list() != TEST_SUCCESS) {
        printf("test_sort_large_list failed\n");
        failed++;
    }
    if (test_sort_custom_compare() != TEST_SUCCESS) {
        printf("test_sort_custom_compare failed\n");
        failed++;
    }
    if (test_sort_null_args() != TEST_SUCCESS) {
        printf("test_sort_null_args failed\n");
        failed++;
    }
    if (test_sort_stability() != TEST_SUCCESS) {
        printf("test_sort_stability failed\n");
        failed++;
    }
    if (test_reverse() != TEST_SUCCESS) {
        printf("test_reverse failed\n");
        failed++;
    }
    if (test_merge() != TEST_SUCCESS) {
        printf("test_merge failed\n");
        failed++;
    }
    if (test_splice() != TEST_SUCCESS) {
        printf("test_splice failed\n");
        failed++;
    }
    if (test_equals() != TEST_SUCCESS) {
        printf("test_equals failed\n");
        failed++;
    }
    if (test_filter() != TEST_SUCCESS) {
        printf("test_filter failed\n");
        failed++;
    }
    if (test_transform() != TEST_SUCCESS) {
        printf("test_transform failed\n");
        failed++;
    }
    if (test_copy_shallow() != TEST_SUCCESS) {
        printf("test_copy_shallow failed\n");
        failed++;
    }
    if (test_copy_deep() != TEST_SUCCESS) {
        printf("test_copy_deep failed\n");
        failed++;
    }
    if (test_copy_complex_data() != TEST_SUCCESS) {
        printf("test_copy_complex_data failed\n");
        failed++;
    }
    if (test_copy_empty() != TEST_SUCCESS) {
        printf("test_copy_empty failed\n");
        failed++;
    }
    if (test_copy_null() != TEST_SUCCESS) {
        printf("test_copy_null failed\n");
        failed++;
    }

    // Add new tests
    if (test_multiple_iterators() != TEST_SUCCESS) {
        printf("test_multiple_iterators failed\n");
        failed++;
    }
    if (test_iterator_with_modification() != TEST_SUCCESS) {
        printf("test_iterator_with_modification failed\n");
        failed++;
    }
    if (test_transform_allocation_failure() != TEST_SUCCESS) {
        printf("test_transform_allocation_failure failed\n");
        failed++;
    }
    if (test_copy_deep_allocation_failure() != TEST_SUCCESS) {
        printf("test_copy_deep_allocation_failure failed\n");
        failed++;
    }
    if (test_insert_allocation_failure() != TEST_SUCCESS) {
        printf("test_insert_allocation_failure failed\n");
        failed++;
    }
    if (test_iterator_allocation_failure() != TEST_SUCCESS) {
        printf("test_iterator_allocation_failure failed\n");
        failed++;
    }
    if (test_from_iterator_custom_alloc_failure() != TEST_SUCCESS) {
        printf("test_from_iterator_custom_alloc_failure failed\n");
        failed++;
    }

    if (failed == 0) {
        printf("All SLinkedList tests passed.\n");
        return 0;
    }
    printf("%d SLinkedList tests failed.\n", failed);
    return 1;
}
