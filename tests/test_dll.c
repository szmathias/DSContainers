//
// Created by zack on 8/25/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "DLinkedList.h"
#include "TestAssert.h"

// Integer comparison for DLinkedList
int int_cmp(const void *a, const void *b) {
    return (*(int*)a) - (*(int*)b);
}

// Integer free for DLinkedList
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
    DLinkedList *list = dll_create();
    ASSERT_NOT_NULLPTR(list);
    ASSERT_EQ(list->size, 0);
    ASSERT_NULLPTR(list->head);
    ASSERT_NULLPTR(list->tail);
    dll_destroy(list, nullptr);
    return TEST_SUCCESS;
}

int test_insert_front_back_find() {
    DLinkedList *list = dll_create();
    int *a = malloc(sizeof(int)); *a = 1;
    int *b = malloc(sizeof(int)); *b = 2;
    int *c = malloc(sizeof(int)); *c = 3;
    ASSERT_EQ(dll_insert_front(list, a), 0);
    ASSERT_EQ(dll_insert_back(list, b), 0);
    ASSERT_EQ(dll_insert_back(list, c), 0);
    ASSERT_EQ(list->size, 3);

    // Verify head and tail pointers
    ASSERT_EQ(*(int*)list->head->data, 1);
    ASSERT_EQ(*(int*)list->tail->data, 3);

    // Verify next and prev pointers
    ASSERT_NULLPTR(list->head->prev);
    ASSERT_NOT_NULLPTR(list->head->next);
    ASSERT_NOT_NULLPTR(list->tail->prev);
    ASSERT_NULLPTR(list->tail->next);

    constexpr int key = 2;
    const DListNode *found = dll_find(list, &key, int_cmp);
    ASSERT_NOT_NULLPTR(found);
    ASSERT_EQ(*(int*)found->data, 2);

    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_remove() {
    DLinkedList *list = dll_create();
    int *a = malloc(sizeof(int)); *a = 1;
    int *b = malloc(sizeof(int)); *b = 2;
    int *c = malloc(sizeof(int)); *c = 3;
    dll_insert_back(list, a);
    dll_insert_back(list, b);
    dll_insert_back(list, c);

    constexpr int key = 2;
    ASSERT_EQ(dll_remove(list, &key, int_cmp, int_free), 0);
    ASSERT_EQ(list->size, 2);
    DListNode *found = dll_find(list, &key, int_cmp);
    ASSERT_NULLPTR(found);

    // Verify links are properly maintained
    ASSERT_EQ(*(int*)list->head->data, 1);
    ASSERT_EQ(*(int*)list->head->next->data, 3);
    ASSERT_EQ(*(int*)list->tail->data, 3);
    ASSERT_EQ(list->tail->prev, list->head);

    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_remove_not_found() {
    DLinkedList *list = dll_create();
    int *a = malloc(sizeof(int)); *a = 1;
    dll_insert_back(list, a);

    constexpr int key = 99;
    ASSERT_EQ(dll_remove(list, &key, int_cmp, int_free), -1);

    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_custom_allocator() {
    DLinkedList *list = dll_create_custom(test_calloc, test_dealloc);
    ASSERT_NOT_NULLPTR(list);
    int *a = malloc(sizeof(int)); *a = 42;
    ASSERT_EQ(dll_insert_back(list, a), 0);
    ASSERT_EQ(list->size, 1);
    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_nullptr_handling() {
    ASSERT_EQ(dll_insert_back(nullptr, nullptr), -1);
    ASSERT_EQ(dll_insert_front(nullptr, nullptr), -1);
    ASSERT_EQ_PTR(dll_find(nullptr, nullptr, nullptr), nullptr);
    ASSERT_EQ(dll_remove(nullptr, nullptr, nullptr, nullptr), -1);
    dll_destroy(nullptr, nullptr); // Should not crash
    return TEST_SUCCESS;
}

int test_insert_at() {
    DLinkedList *list = dll_create();
    int *a = malloc(sizeof(int)); *a = 1;
    int *b = malloc(sizeof(int)); *b = 2;
    int *c = malloc(sizeof(int)); *c = 3;
    ASSERT_EQ(dll_insert_back(list, a), 0); // [1]
    ASSERT_EQ(dll_insert_back(list, c), 0); // [1,3]
    ASSERT_EQ(dll_insert_at(list, 1, b), 0); // [1,2,3]
    ASSERT_EQ(list->size, 3);

    constexpr int key = 2;
    const DListNode *found = dll_find(list, &key, int_cmp);
    ASSERT_NOT_NULLPTR(found);
    ASSERT_EQ(*(int*)found->data, 2);

    // Verify prev/next links
    ASSERT_EQ(found->prev, list->head);
    ASSERT_EQ(found->next, list->tail);
    ASSERT_EQ(list->head->next, found);
    ASSERT_EQ(list->tail->prev, found);

    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_remove_at() {
    DLinkedList *list = dll_create();
    int *a = malloc(sizeof(int)); *a = 10;
    int *b = malloc(sizeof(int)); *b = 20;
    int *c = malloc(sizeof(int)); *c = 30;
    dll_insert_back(list, a); // [10]
    dll_insert_back(list, b); // [10,20]
    dll_insert_back(list, c); // [10,20,30]

    ASSERT_EQ(dll_remove_at(list, 1, int_free), 0); // remove 20
    ASSERT_EQ(list->size, 2);

    constexpr int key = 20;
    DListNode *found = dll_find(list, &key, int_cmp);
    ASSERT_NULLPTR(found);

    // Check links
    ASSERT_EQ(list->head->next, list->tail);
    ASSERT_EQ(list->tail->prev, list->head);

    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_remove_front() {
    DLinkedList *list = dll_create();

    // Test on empty list
    ASSERT_EQ(dll_remove_front(list, int_free), -1);

    // Add elements
    int *a = malloc(sizeof(int)); *a = 10;
    int *b = malloc(sizeof(int)); *b = 20;
    int *c = malloc(sizeof(int)); *c = 30;
    dll_insert_back(list, a);
    dll_insert_back(list, b);
    dll_insert_back(list, c);
    ASSERT_EQ(list->size, 3);

    // Remove front
    ASSERT_EQ(dll_remove_front(list, int_free), 0);
    ASSERT_EQ(list->size, 2);

    // Check first element is now 20
    int key = 10;
    ASSERT_NULLPTR(dll_find(list, &key, int_cmp));
    key = 20;
    ASSERT_NOT_NULLPTR(dll_find(list, &key, int_cmp));
    ASSERT_EQ(list->head->data, dll_find(list, &key, int_cmp)->data);
    ASSERT_NULLPTR(list->head->prev);

    // Remove until empty
    ASSERT_EQ(dll_remove_front(list, int_free), 0);
    ASSERT_EQ(dll_remove_front(list, int_free), 0);
    ASSERT_EQ(list->size, 0);
    ASSERT_NULLPTR(list->head);
    ASSERT_NULLPTR(list->tail);

    dll_destroy(list, nullptr);
    return TEST_SUCCESS;
}

int test_remove_back() {
    DLinkedList *list = dll_create();

    // Test on empty list
    ASSERT_EQ(dll_remove_back(list, int_free), -1);

    // Test on single element list
    int *a = malloc(sizeof(int)); *a = 10;
    dll_insert_back(list, a);
    ASSERT_EQ(dll_remove_back(list, int_free), 0);
    ASSERT_EQ(list->size, 0);
    ASSERT_NULLPTR(list->head);
    ASSERT_NULLPTR(list->tail);

    // Test with multiple elements
    int *b = malloc(sizeof(int)); *b = 20;
    int *c = malloc(sizeof(int)); *c = 30;
    int *d = malloc(sizeof(int)); *d = 40;
    dll_insert_back(list, b);
    dll_insert_back(list, c);
    dll_insert_back(list, d);
    ASSERT_EQ(list->size, 3);

    // Remove back
    ASSERT_EQ(dll_remove_back(list, int_free), 0);
    ASSERT_EQ(list->size, 2);

    // Check last element was removed
    int key = 40;
    ASSERT_NULLPTR(dll_find(list, &key, int_cmp));
    key = 30;
    ASSERT_NOT_NULLPTR(dll_find(list, &key, int_cmp));
    ASSERT_EQ(list->tail->data, dll_find(list, &key, int_cmp)->data);
    ASSERT_NULLPTR(list->tail->next);

    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_remove_at_head() {
    DLinkedList *list = dll_create();
    int *a = malloc(sizeof(int)); *a = 100;
    int *b = malloc(sizeof(int)); *b = 200;
    dll_insert_back(list, a); // [100]
    dll_insert_back(list, b); // [100,200]

    ASSERT_EQ(dll_remove_at(list, 0, int_free), 0); // remove head (100)
    ASSERT_EQ(list->size, 1);
    ASSERT_EQ(*(int*)list->head->data, 200);
    ASSERT_EQ(list->head, list->tail);

    constexpr int key = 100;
    DListNode *found = dll_find(list, &key, int_cmp);
    ASSERT_NULLPTR(found);

    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_remove_at_last() {
    DLinkedList *list = dll_create();
    int *a = malloc(sizeof(int)); *a = 1;
    int *b = malloc(sizeof(int)); *b = 2;
    int *c = malloc(sizeof(int)); *c = 3;
    dll_insert_back(list, a); // [1]
    dll_insert_back(list, b); // [1,2]
    dll_insert_back(list, c); // [1,2,3]

    ASSERT_EQ(dll_remove_at(list, 2, int_free), 0); // remove last (3)
    ASSERT_EQ(list->size, 2);
    ASSERT_EQ(*(int*)list->tail->data, 2);

    constexpr int key = 3;
    DListNode *found = dll_find(list, &key, int_cmp);
    ASSERT_NULLPTR(found);

    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_remove_at_invalid() {
    DLinkedList *list = dll_create();
    int *a = malloc(sizeof(int)); *a = 1;
    dll_insert_back(list, a); // [1]

    ASSERT_EQ(dll_remove_at(list, 5, int_free), -1); // invalid position
    ASSERT_EQ(dll_remove_at(list, (size_t)-1, int_free), -1); // negative as size_t (very large)

    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_remove_at_empty() {
    DLinkedList *list = dll_create();
    ASSERT_EQ(dll_remove_at(list, 0, int_free), -1); // nothing to remove
    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_remove_at_single_element() {
    DLinkedList *list = dll_create();
    int *a = malloc(sizeof(int)); *a = 123;
    dll_insert_back(list, a); // [123]
    ASSERT_EQ(dll_remove_at(list, 0, int_free), 0); // remove only element
    ASSERT_EQ(list->size, 0);
    ASSERT_NULLPTR(list->head);
    ASSERT_NULLPTR(list->tail);
    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_remove_at_single_element_invalid_pos() {
    DLinkedList *list = dll_create();
    int *a = malloc(sizeof(int)); *a = 123;
    dll_insert_back(list, a); // [123]
    ASSERT_EQ(dll_remove_at(list, 1, int_free), -1); // invalid position
    ASSERT_EQ(list->size, 1);
    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_insert_at_out_of_bounds() {
    DLinkedList *list = dll_create();
    int *a = malloc(sizeof(int)); *a = 1;
    ASSERT_EQ(dll_insert_at(list, 2, a), -1); // out of bounds (list size is 0)
    ASSERT_EQ(dll_insert_at(list, (size_t)-1, a), -1); // very large index
    dll_destroy(list, int_free);
    free(a);
    return TEST_SUCCESS;
}

int test_insert_remove_null_data() {
    DLinkedList *list = dll_create();
    ASSERT_EQ(dll_insert_back(list, NULL), 0); // allow NULL data
    ASSERT_EQ(list->size, 1);
    ASSERT_EQ(dll_remove_at(list, 0, nullptr), 0); // remove node with NULL data, no free_func
    ASSERT_EQ(list->size, 0);
    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_mixed_operations_integrity() {
    DLinkedList *list = dll_create();
    int *a = malloc(sizeof(int)); *a = 10;
    int *b = malloc(sizeof(int)); *b = 20;
    int *c = malloc(sizeof(int)); *c = 30;
    dll_insert_back(list, a); // [10]
    dll_insert_front(list, b); // [20,10]
    dll_insert_at(list, 1, c); // [20,30,10]
    ASSERT_EQ(list->size, 3);

    // Verify the bidirectional links
    ASSERT_EQ(*(int*)list->head->data, 20);
    ASSERT_EQ(*(int*)list->head->next->data, 30);
    ASSERT_EQ(*(int*)list->head->next->next->data, 10);
    ASSERT_EQ(*(int*)list->tail->data, 10);
    ASSERT_EQ(*(int*)list->tail->prev->data, 30);
    ASSERT_EQ(*(int*)list->tail->prev->prev->data, 20);

    ASSERT_EQ(dll_remove_at(list, 1, int_free), 0); // remove 30, [20,10]
    constexpr int key = 30;
    DListNode *found = dll_find(list, &key, int_cmp);
    ASSERT_NULLPTR(found);

    // Verify new links
    ASSERT_EQ(*(int*)list->head->data, 20);
    ASSERT_EQ(*(int*)list->head->next->data, 10);
    ASSERT_EQ(*(int*)list->tail->data, 10);
    ASSERT_EQ(*(int*)list->tail->prev->data, 20);
    ASSERT_NULLPTR(list->head->prev);
    ASSERT_NULLPTR(list->tail->next);

    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_size() {
    DLinkedList *list = dll_create();
    ASSERT_EQ(dll_size(list), 0);

    int *a = malloc(sizeof(int)); *a = 10;
    int *b = malloc(sizeof(int)); *b = 20;
    dll_insert_back(list, a);
    ASSERT_EQ(dll_size(list), 1);
    dll_insert_back(list, b);
    ASSERT_EQ(dll_size(list), 2);

    dll_remove_at(list, 0, int_free);
    ASSERT_EQ(dll_size(list), 1);

    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_is_empty() {
    DLinkedList *list = dll_create();
    ASSERT_EQ(dll_is_empty(list), 1); // Empty list

    int *a = malloc(sizeof(int)); *a = 10;
    dll_insert_back(list, a);
    ASSERT_EQ(dll_is_empty(list), 0); // Non-empty list

    dll_remove_at(list, 0, int_free);
    ASSERT_EQ(dll_is_empty(list), 1); // Empty again

    ASSERT_EQ(dll_is_empty(nullptr), 1); // NULL list should be considered empty

    dll_destroy(list, nullptr);
    return TEST_SUCCESS;
}

int test_clear() {
    DLinkedList *list = dll_create();

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
    ASSERT_NULLPTR(list->head);
    ASSERT_NULLPTR(list->tail);
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

int test_clear_empty() {
    DLinkedList *list = dll_create();

    // Clear an already empty list
    dll_clear(list, int_free);
    ASSERT_NULLPTR(list->head);
    ASSERT_NULLPTR(list->tail);
    ASSERT_EQ(list->size, 0);

    dll_destroy(list, nullptr);
    return TEST_SUCCESS;
}

int test_clear_null() {
    // Calling clear on NULL shouldn't crash
    dll_clear(nullptr, int_free);
    return TEST_SUCCESS;
}

int test_complex_data_type() {
    DLinkedList *list = dll_create();

    Person *p1 = create_person("Alice", 30);
    Person *p2 = create_person("Bob", 25);
    Person *p3 = create_person("Charlie", 40);

    dll_insert_back(list, p1);
    dll_insert_back(list, p2);
    dll_insert_back(list, p3);
    ASSERT_EQ(list->size, 3);

    Person search_key;
    strcpy(search_key.name, "Bob");
    search_key.age = 0; // Age doesn't matter for comparison

    const DListNode *found = dll_find(list, &search_key, person_cmp);
    ASSERT_NOT_NULLPTR(found);
    const Person *found_person = found->data;
    ASSERT_EQ(found_person->age, 25);

    // Clean up
    dll_destroy(list, person_free);
    return TEST_SUCCESS;
}

int test_remove_all() {
    DLinkedList *list = dll_create();

    // Add 10 elements
    for (int i = 0; i < 10; i++) {
        int *val = malloc(sizeof(int));
        *val = i;
        dll_insert_back(list, val);
    }
    ASSERT_EQ(list->size, 10);

    // Remove all elements one by one
    while (!dll_is_empty(list)) {
        dll_remove_front(list, int_free);
    }

    ASSERT_EQ(list->size, 0);
    ASSERT_NULLPTR(list->head);
    ASSERT_NULLPTR(list->tail);

    dll_destroy(list, nullptr); // Already freed all data
    return TEST_SUCCESS;
}

int test_stress() {
    DLinkedList *list = dll_create();
    constexpr int NUM_ELEMENTS = 10000;

    // Add many elements
    for (int i = 0; i < NUM_ELEMENTS; i++) {
        int *val = malloc(sizeof(int));
        *val = i;
        ASSERT_EQ(dll_insert_back(list, val), 0);
    }
    ASSERT_EQ(list->size, NUM_ELEMENTS);

    // Find an element in the middle
    int key = NUM_ELEMENTS / 2;
    const DListNode *found = dll_find(list, &key, int_cmp);
    ASSERT_NOT_NULLPTR(found);
    ASSERT_EQ(*(int*)found->data, key);

    // Remove elements from the front
    for (int i = 0; i < NUM_ELEMENTS / 2; i++) {
        ASSERT_EQ(dll_remove_front(list, int_free), 0);
    }
    ASSERT_EQ(list->size, NUM_ELEMENTS / 2);

    // The first element should now be NUM_ELEMENTS/2
    key = NUM_ELEMENTS / 2;
    ASSERT_EQ(*(int*)list->head->data, key);

    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_performance() {
    constexpr int NUM_SIZES = 3;

    printf("\nDLL Performance tests:\n");
    for (int s = 0; s < NUM_SIZES; s++) {
        constexpr int SIZES[] = {100, 1000, 10000};
        const int SIZE = SIZES[s];
        DLinkedList *list = dll_create();

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
        const DListNode *found = dll_find(list, &key, int_cmp);
        end = clock();
        printf("Find last element in %d elements: %.6f seconds\n", SIZE,
               (double)(end - start) / CLOCKS_PER_SEC);
        ASSERT_NOT_NULLPTR(found);

        // Cleanup
        dll_destroy(list, int_free);
    }

    return TEST_SUCCESS;
}

int test_sort_empty() {
    DLinkedList *list = dll_create();
    ASSERT_EQ(dll_sort(list, int_cmp), 0); // Empty list is already sorted
    ASSERT_EQ(list->size, 0);
    dll_destroy(list, nullptr);
    return TEST_SUCCESS;
}

int test_sort_already_sorted() {
    DLinkedList *list = dll_create();
    for (int i = 0; i < 5; i++) {
        int *val = malloc(sizeof(int));
        *val = i;
        dll_insert_back(list, val);
    }

    ASSERT_EQ(dll_sort(list, int_cmp), 0);

    // Verify order
    const DListNode *node = list->head;
    for (int i = 0; i < 5; i++) {
        ASSERT_EQ(*(int*)node->data, i);
        node = node->next;
    }

    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_sort_reverse_order() {
    DLinkedList *list = dll_create();
    for (int i = 4; i >= 0; i--) {
        int *val = malloc(sizeof(int));
        *val = i;
        dll_insert_back(list, val);
    }

    ASSERT_EQ(dll_sort(list, int_cmp), 0);

    // Verify order
    const DListNode *node = list->head;
    for (int i = 0; i < 5; i++) {
        ASSERT_EQ(*(int*)node->data, i);
        node = node->next;
    }

    // Check bidirectional links
    const DListNode *tail = list->tail;
    for (int i = 4; i >= 0; i--) {
        ASSERT_EQ(*(int*)tail->data, i);
        tail = tail->prev;
    }

    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_sort_random_order() {
    DLinkedList *list = dll_create();
    const int values[] = {42, 17, 9, 39, 24, 5, 58};
    constexpr int count = sizeof(values) / sizeof(values[0]);

    for (int i = 0; i < count; i++) {
        int *val = malloc(sizeof(int));
        *val = values[i];
        dll_insert_back(list, val);
    }

    ASSERT_EQ(dll_sort(list, int_cmp), 0);

    // Verify order
    const DListNode *node = list->head;
    for (int i = 0; i < count; i++) {
        const int sorted[] = {5, 9, 17, 24, 39, 42, 58};
        ASSERT_EQ(*(int*)node->data, sorted[i]);
        node = node->next;
    }

    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_sort_with_duplicates() {
    DLinkedList *list = dll_create();
    const int values[] = {5, 2, 9, 5, 7, 2, 9, 5};
    constexpr int count = sizeof(values) / sizeof(values[0]);

    for (int i = 0; i < count; i++) {
        int *val = malloc(sizeof(int));
        *val = values[i];
        dll_insert_back(list, val);
    }

    ASSERT_EQ(dll_sort(list, int_cmp), 0);

    // Verify order
    const DListNode *node = list->head;
    for (int i = 0; i < count; i++) {
        const int sorted[] = {2, 2, 5, 5, 5, 7, 9, 9};
        ASSERT_EQ(*(int*)node->data, sorted[i]);
        node = node->next;
    }

    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_sort_large_list() {
    DLinkedList *list = dll_create();
    constexpr int SIZE = 1000;

    // Insert in reverse order
    for (int i = SIZE - 1; i >= 0; i--) {
        int *val = malloc(sizeof(int));
        *val = i;
        dll_insert_back(list, val);
    }

    const clock_t start = clock();
    ASSERT_EQ(dll_sort(list, int_cmp), 0);
    const clock_t end = clock();
    printf("Sort %d elements: %.6f seconds\n", SIZE,
           (double)(end - start) / CLOCKS_PER_SEC);

    // Verify order (first few and last few)
    const DListNode *node = list->head;
    for (int i = 0; i < 10; i++) {
        ASSERT_EQ(*(int*)node->data, i);
        node = node->next;
    }

    node = list->tail;
    for (int i = SIZE - 1; i >= SIZE - 10; i--) {
        ASSERT_EQ(*(int*)node->data, i);
        node = node->prev;
    }

    // Verify list structure
    ASSERT_EQ(list->size, SIZE);
    ASSERT_EQ(*(int*)list->head->data, 0);
    ASSERT_EQ(*(int*)list->tail->data, SIZE - 1);

    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

// Custom comparison function for descending order
int int_cmp_desc(const void *a, const void *b) {
    return (*(int*)b) - (*(int*)a);
}

int test_sort_custom_compare() {
    DLinkedList *list = dll_create();
    for (int i = 0; i < 5; i++) {
        int *val = malloc(sizeof(int));
        *val = i;
        dll_insert_back(list, val);
    }

    // Sort in descending order
    ASSERT_EQ(dll_sort(list, int_cmp_desc), 0);

    // Verify order
    const DListNode *node = list->head;
    for (int i = 4; i >= 0; i--) {
        ASSERT_EQ(*(int*)node->data, i);
        node = node->next;
    }

    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_sort_null_args() {
    DLinkedList *list = dll_create();
    ASSERT_EQ(dll_sort(nullptr, int_cmp), -1); // NULL list
    ASSERT_EQ(dll_sort(list, nullptr), -1);    // NULL compare function
    dll_destroy(list, nullptr);
    return TEST_SUCCESS;
}

int test_sort_stability() {
    DLinkedList *list = dll_create();

    // Person structs with same name (for comparison) but different ages
    Person *p1 = create_person("Alice", 30);
    Person *p2 = create_person("Alice", 25);  // Same name, different age
    Person *p3 = create_person("Bob", 35);
    Person *p4 = create_person("Alice", 40);  // Same name, different age

    dll_insert_back(list, p1);
    dll_insert_back(list, p2);
    dll_insert_back(list, p3);
    dll_insert_back(list, p4);

    // Sort by name only - ages should remain in insertion order for equal names
    ASSERT_EQ(dll_sort(list, person_cmp), 0);

    // Verify order: All Alice's should come before Bob
    const DListNode *node = list->head;
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

    dll_destroy(list, person_free);
    return TEST_SUCCESS;
}

int test_reverse() {
    DLinkedList *list = dll_create();

    // Test empty list
    ASSERT_EQ(dll_reverse(list), 0);
    ASSERT_EQ(list->size, 0);

    // Test single element
    int *a = malloc(sizeof(int)); *a = 10;
    dll_insert_back(list, a);
    ASSERT_EQ(dll_reverse(list), 0);
    ASSERT_EQ(list->size, 1);
    ASSERT_EQ(*(int*)list->head->data, 10);
    ASSERT_EQ(list->head, list->tail);

    // Test multiple elements
    int *b = malloc(sizeof(int)); *b = 20;
    int *c = malloc(sizeof(int)); *c = 30;
    dll_insert_back(list, b);
    dll_insert_back(list, c);
    // List is now [10,20,30]

    ASSERT_EQ(dll_reverse(list), 0);
    // List should now be [30,20,10]

    // Verify head-to-tail traversal
    const DListNode *node = list->head;
    ASSERT_EQ(*(int*)node->data, 30);
    ASSERT_NULLPTR(node->prev);

    node = node->next;
    ASSERT_EQ(*(int*)node->data, 20);
    ASSERT_EQ(*(int*)node->prev->data, 30);

    node = node->next;
    ASSERT_EQ(*(int*)node->data, 10);
    ASSERT_EQ(*(int*)node->prev->data, 20);
    ASSERT_NULLPTR(node->next);
    ASSERT_EQ(node, list->tail);

    // Verify tail-to-head traversal
    node = list->tail;
    ASSERT_EQ(*(int*)node->data, 10);
    ASSERT_NULLPTR(node->next);

    node = node->prev;
    ASSERT_EQ(*(int*)node->data, 20);
    ASSERT_EQ(*(int*)node->next->data, 10);

    node = node->prev;
    ASSERT_EQ(*(int*)node->data, 30);
    ASSERT_EQ(*(int*)node->next->data, 20);
    ASSERT_NULLPTR(node->prev);
    ASSERT_EQ(node, list->head);

    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int test_merge() {
    DLinkedList *list1 = dll_create();
    DLinkedList *list2 = dll_create();

    // Test merging empty lists
    ASSERT_EQ(dll_merge(list1, list2), 0);
    ASSERT_EQ(list1->size, 0);
    ASSERT_EQ(list2->size, 0);

    // Test merging empty with non-empty
    int *a1 = malloc(sizeof(int)); *a1 = 10;
    int *b1 = malloc(sizeof(int)); *b1 = 20;
    dll_insert_back(list2, a1);
    dll_insert_back(list2, b1);

    ASSERT_EQ(dll_merge(list1, list2), 0);
    ASSERT_EQ(list1->size, 2);
    ASSERT_EQ(list2->size, 0);
    ASSERT_NULLPTR(list2->head);
    ASSERT_NULLPTR(list2->tail);

    // Verify the merged list
    ASSERT_EQ(*(int*)list1->head->data, 10);
    ASSERT_EQ(*(int*)list1->tail->data, 20);
    ASSERT_NULLPTR(list1->head->prev);
    ASSERT_NULLPTR(list1->tail->next);
    ASSERT_EQ(list1->head->next, list1->tail);
    ASSERT_EQ(list1->tail->prev, list1->head);

    // Test merging two non-empty lists
    DLinkedList *list3 = dll_create();
    int *a2 = malloc(sizeof(int)); *a2 = 30;
    int *b2 = malloc(sizeof(int)); *b2 = 40;
    dll_insert_back(list3, a2);
    dll_insert_back(list3, b2);

    ASSERT_EQ(dll_merge(list1, list3), 0);
    ASSERT_EQ(list1->size, 4);
    ASSERT_EQ(list3->size, 0);

    // Verify the final merged list
    ASSERT_EQ(*(int*)list1->head->data, 10);
    ASSERT_EQ(*(int*)list1->tail->data, 40);

    const DListNode *node = list1->head;
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
    ASSERT_NULLPTR(node->next);

    dll_destroy(list1, int_free);
    dll_destroy(list2, nullptr); // Already empty
    dll_destroy(list3, nullptr); // Already empty
    return TEST_SUCCESS;
}

int test_splice() {
    // Test splicing at the beginning
    DLinkedList *dest1 = dll_create();
    DLinkedList *src1 = dll_create();

    int *a1 = malloc(sizeof(int)); *a1 = 10;
    int *b1 = malloc(sizeof(int)); *b1 = 20;
    int *c1 = malloc(sizeof(int)); *c1 = 30;
    int *d1 = malloc(sizeof(int)); *d1 = 40;
    int *e1 = malloc(sizeof(int)); *e1 = 50;

    dll_insert_back(dest1, a1);
    dll_insert_back(dest1, b1);
    dll_insert_back(dest1, c1);
    dll_insert_back(src1, d1);
    dll_insert_back(src1, e1);

    ASSERT_EQ(dll_splice(dest1, src1, 0), 0);
    ASSERT_EQ(dest1->size, 5);
    ASSERT_EQ(src1->size, 0);

    // Verify the spliced list
    ASSERT_EQ(*(int*)dest1->head->data, 40);
    ASSERT_EQ(*(int*)dest1->head->next->data, 50);
    ASSERT_EQ(*(int*)dest1->head->next->next->data, 10);
    ASSERT_EQ(*(int*)dest1->head->next->next->next->data, 20);
    ASSERT_EQ(*(int*)dest1->tail->data, 30);

    // Test bidirectional links
    ASSERT_NULLPTR(dest1->head->prev);
    ASSERT_EQ(*(int*)dest1->head->next->prev->data, 40);
    ASSERT_EQ(*(int*)dest1->tail->prev->data, 20);
    ASSERT_NULLPTR(dest1->tail->next);

    // Test splicing in the middle
    DLinkedList *dest2 = dll_create();
    DLinkedList *src2 = dll_create();

    int *a2 = malloc(sizeof(int)); *a2 = 10;
    int *b2 = malloc(sizeof(int)); *b2 = 20;
    int *c2 = malloc(sizeof(int)); *c2 = 30;
    int *d2 = malloc(sizeof(int)); *d2 = 40;
    int *e2 = malloc(sizeof(int)); *e2 = 50;

    dll_insert_back(dest2, a2);
    dll_insert_back(dest2, b2);
    dll_insert_back(dest2, c2);
    dll_insert_back(src2, d2);
    dll_insert_back(src2, e2);

    ASSERT_EQ(dll_splice(dest2, src2, 1), 0);
    ASSERT_EQ(dest2->size, 5);
    ASSERT_EQ(src2->size, 0);

    // Verify the spliced list
    ASSERT_EQ(*(int*)dest2->head->data, 10);
    ASSERT_EQ(*(int*)dest2->head->next->data, 40);
    ASSERT_EQ(*(int*)dest2->head->next->next->data, 50);
    ASSERT_EQ(*(int*)dest2->head->next->next->next->data, 20);
    ASSERT_EQ(*(int*)dest2->tail->data, 30);

    // Test bidirectional links
    ASSERT_NULLPTR(dest2->head->prev);
    ASSERT_EQ(*(int*)dest2->head->next->prev->data, 10);
    ASSERT_EQ(*(int*)dest2->tail->prev->data, 20);
    ASSERT_NULLPTR(dest2->tail->next);

    // Test splicing at the end
    DLinkedList *dest3 = dll_create();
    DLinkedList *src3 = dll_create();

    int *a3 = malloc(sizeof(int)); *a3 = 10;
    int *b3 = malloc(sizeof(int)); *b3 = 20;
    int *c3 = malloc(sizeof(int)); *c3 = 30;
    int *d3 = malloc(sizeof(int)); *d3 = 40;
    int *e3 = malloc(sizeof(int)); *e3 = 50;

    dll_insert_back(dest3, a3);
    dll_insert_back(dest3, b3);
    dll_insert_back(dest3, c3);
    dll_insert_back(src3, d3);
    dll_insert_back(src3, e3);

    ASSERT_EQ(dll_splice(dest3, src3, 3), 0);
    ASSERT_EQ(dest3->size, 5);
    ASSERT_EQ(src3->size, 0);

    // Verify the spliced list
    ASSERT_EQ(*(int*)dest3->head->data, 10);
    ASSERT_EQ(*(int*)dest3->head->next->data, 20);
    ASSERT_EQ(*(int*)dest3->head->next->next->data, 30);
    ASSERT_EQ(*(int*)dest3->head->next->next->next->data, 40);
    ASSERT_EQ(*(int*)dest3->tail->data, 50);

    // Test bidirectional links
    ASSERT_NULLPTR(dest3->head->prev);
    ASSERT_EQ(*(int*)dest3->head->next->prev->data, 10);
    ASSERT_EQ(*(int*)dest3->tail->prev->data, 40);
    ASSERT_NULLPTR(dest3->tail->next);

    // Test splicing with empty source
    DLinkedList *empty = dll_create();
    ASSERT_EQ(dll_splice(dest1, empty, 2), 0);
    ASSERT_EQ(dest1->size, 5); // Should be unchanged

    // Test splicing with invalid position
    ASSERT_EQ(dll_splice(dest1, src1, 99), -1);

    dll_destroy(dest1, int_free);
    dll_destroy(src1, nullptr);
    dll_destroy(dest2, int_free);
    dll_destroy(src2, nullptr);
    dll_destroy(dest3, int_free);
    dll_destroy(src3, nullptr);
    dll_destroy(empty, nullptr);

    return TEST_SUCCESS;
}

int test_equals() {
    DLinkedList *list1 = dll_create();
    DLinkedList *list2 = dll_create();

    // Empty lists should be equal
    ASSERT_EQ(dll_equals(list1, list2, int_cmp), 1);

    // Lists with same elements should be equal
    int *a1 = malloc(sizeof(int)); *a1 = 10;
    int *b1 = malloc(sizeof(int)); *b1 = 20;
    int *a2 = malloc(sizeof(int)); *a2 = 10;
    int *b2 = malloc(sizeof(int)); *b2 = 20;
    dll_insert_back(list1, a1);
    dll_insert_back(list1, b1);
    dll_insert_back(list2, a2);
    dll_insert_back(list2, b2);

    ASSERT_EQ(dll_equals(list1, list2, int_cmp), 1);

    // Lists with different elements should not be equal
    int *c2 = malloc(sizeof(int)); *c2 = 30;
    dll_insert_back(list2, c2);

    ASSERT_EQ(dll_equals(list1, list2, int_cmp), 0);

    // Lists with same size but different elements should not be equal
    DLinkedList *list3 = dll_create();
    int *a3 = malloc(sizeof(int)); *a3 = 10;
    int *b3 = malloc(sizeof(int)); *b3 = 30; // Different value
    dll_insert_back(list3, a3);
    dll_insert_back(list3, b3);

    ASSERT_EQ(dll_equals(list1, list3, int_cmp), 0);

    // Error cases
    ASSERT_EQ(dll_equals(nullptr, list2, int_cmp), -1);
    ASSERT_EQ(dll_equals(list1, nullptr, int_cmp), -1);
    ASSERT_EQ(dll_equals(list1, list2, nullptr), -1);

    dll_destroy(list1, int_free);
    dll_destroy(list2, int_free);
    dll_destroy(list3, int_free);
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
    DLinkedList *list = dll_create();

    // Add numbers 0-9
    for (int i = 0; i < 10; i++) {
        int *val = malloc(sizeof(int));
        *val = i;
        dll_insert_back(list, val);
    }

    // Filter for even numbers
    DLinkedList *filtered = dll_filter(list, is_even);
    ASSERT_NOT_NULLPTR(filtered);
    ASSERT_EQ(filtered->size, 5);  // Should contain 0,2,4,6,8

    // Verify filtered list
    const DListNode *node = filtered->head;
    for (int i = 0; i < 5; i++) {
        const int expected_values[] = {0, 2, 4, 6, 8};
        ASSERT_EQ(*(int*)node->data, expected_values[i]);
        node = node->next;
    }

    // Make sure original list is unchanged
    ASSERT_EQ(list->size, 10);

    // Test empty list
    DLinkedList *empty_list = dll_create();
    DLinkedList *filtered_empty = dll_filter(empty_list, is_even);
    ASSERT_NOT_NULLPTR(filtered_empty);
    ASSERT_EQ(filtered_empty->size, 0);

    // Test null cases
    ASSERT_NULLPTR(dll_filter(nullptr, is_even));
    ASSERT_NULLPTR(dll_filter(list, nullptr));

    dll_destroy(list, int_free);
    dll_destroy(filtered, nullptr);  // Don't free data; it's owned by the original list
    dll_destroy(empty_list, nullptr);
    dll_destroy(filtered_empty, nullptr);
    return TEST_SUCCESS;
}

int test_transform() {
    DLinkedList *list = dll_create();

    // Add numbers 1-5
    for (int i = 1; i <= 5; i++) {
        int *val = malloc(sizeof(int));
        *val = i;
        dll_insert_back(list, val);
    }

    // Map to double each value
    DLinkedList *transformed = dll_transform(list, double_value);
    ASSERT_NOT_NULLPTR(transformed);
    ASSERT_EQ(transformed->size, 5);

    // Verify mapped list (should be 2,4,6,8,10)
    const DListNode *node = transformed->head;
    for (int i = 1; i <= 5; i++) {
        ASSERT_EQ(*(int*)node->data, i * 2);
        node = node->next;
    }

    // Make sure bidirectional links are correct
    node = transformed->head;
    ASSERT_NULLPTR(node->prev);

    node = node->next;
    ASSERT_EQ(*(int*)node->prev->data, 2);

    node = transformed->tail;
    ASSERT_NULLPTR(node->next);
    ASSERT_EQ(*(int*)node->data, 10);

    // Make sure original list is unchanged
    node = list->head;
    for (int i = 1; i <= 5; i++) {
        ASSERT_EQ(*(int*)node->data, i);
        node = node->next;
    }

    // Test empty list
    DLinkedList *empty_list = dll_create();
    DLinkedList *transformed_empty = dll_transform(empty_list, double_value);
    ASSERT_NOT_NULLPTR(transformed_empty);
    ASSERT_EQ(transformed_empty->size, 0);

    // Test null cases
    ASSERT_NULLPTR(dll_transform(nullptr, double_value));
    ASSERT_NULLPTR(dll_transform(list, nullptr));

    dll_destroy(list, int_free);
    dll_destroy(transformed, int_free);
    dll_destroy(empty_list, nullptr);
    dll_destroy(transformed_empty, nullptr);
    return TEST_SUCCESS;
}

int test_for_each() {
    DLinkedList *list = dll_create();

    // Add numbers 1-5
    for (int i = 1; i <= 5; i++) {
        int *val = malloc(sizeof(int));
        *val = i;
        dll_insert_back(list, val);
    }

    // Increment each value
    dll_for_each(list, increment);

    // Verify each value is incremented
    const DListNode *node = list->head;
    for (int i = 1; i <= 5; i++) {
        ASSERT_EQ(*(int*)node->data, i + 1);
        node = node->next;
    }

    // Test empty list
    DLinkedList *empty_list = dll_create();
    dll_for_each(empty_list, increment);  // Should do nothing

    // Test null cases
    dll_for_each(nullptr, increment);  // Should do nothing
    dll_for_each(list, nullptr);       // Should do nothing

    dll_destroy(list, int_free);
    dll_destroy(empty_list, nullptr);
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
    DLinkedList *list = dll_create();

    // Add some elements
    for (int i = 0; i < 5; i++) {
        int *val = malloc(sizeof(int));
        *val = i * 10;
        dll_insert_back(list, val);
    }

    // Create shallow clone
    DLinkedList *copy = dll_copy(list);
    ASSERT_NOT_NULLPTR(copy);
    ASSERT_EQ(copy->size, list->size);

    // Verify structure
    DListNode *orig_node = list->head;
    DListNode *copy_node = copy->head;
    while (orig_node && copy_node) {
        // Data pointers should be identical in shallow clone
        ASSERT_EQ(orig_node->data, copy_node->data);
        // But nodes themselves should be different
        ASSERT_NOT_EQ(orig_node, copy_node);

        // Verify next pointers
        if (orig_node->next) {
            ASSERT_NOT_EQ(orig_node->next, copy_node->next);
        } else {
            ASSERT_NULLPTR(copy_node->next);
        }

        // Verify prev pointers
        if (orig_node->prev) {
            ASSERT_NOT_EQ(orig_node->prev, copy_node->prev);
        } else {
            ASSERT_NULLPTR(copy_node->prev);
        }

        orig_node = orig_node->next;
        copy_node = copy_node->next;
    }

    // Verify bidirectional links in the clone
    ASSERT_NULLPTR(copy->head->prev);
    ASSERT_NULLPTR(copy->tail->next);

    // Modifying data should affect both lists (shared pointers)
    int *first_value = list->head->data;
    *first_value = 999;
    ASSERT_EQ(*(int*)copy->head->data, 999);

    // Cleanup - free each int only once since they're shared
    dll_destroy(list, int_free);
    dll_destroy(copy, nullptr);

    return TEST_SUCCESS;
}

int test_copy_deep() {
    DLinkedList *list = dll_create();

    // Add some elements
    for (int i = 0; i < 5; i++) {
        int *val = malloc(sizeof(int));
        *val = i * 10;
        dll_insert_back(list, val);
    }

    // Create deep clone
    DLinkedList *copy = dll_copy_deep(list, int_copy);
    ASSERT_NOT_NULLPTR(copy);
    ASSERT_EQ(copy->size, list->size);

    // Verify structure and values
    const DListNode *orig_node = list->head;
    const DListNode *copy_node = copy->head;
    while (orig_node && copy_node) {
        // Data pointers should be different in deep clone
        ASSERT_NOT_EQ(orig_node->data, copy_node->data);
        // But values should be the same
        ASSERT_EQ(*(int*)orig_node->data, *(int*)copy_node->data);

        orig_node = orig_node->next;
        copy_node = copy_node->next;
    }

    // Verify bidirectional links in the clone
    ASSERT_NULLPTR(copy->head->prev);
    ASSERT_NULLPTR(copy->tail->next);

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

int test_copy_complex_data() {
    DLinkedList *list = dll_create();

    // Add some people
    Person *p1 = create_person("Alice", 30);
    Person *p2 = create_person("Bob", 25);
    Person *p3 = create_person("Charlie", 40);

    dll_insert_back(list, p1);
    dll_insert_back(list, p2);
    dll_insert_back(list, p3);
    ASSERT_EQ(list->size, 3);

    // Create deep clone
    DLinkedList *copy = dll_copy_deep(list, person_copy);
    ASSERT_NOT_NULLPTR(copy);
    ASSERT_EQ(copy->size, list->size);

    // Verify structure and values
    const DListNode *orig_node = list->head;
    const DListNode *copy_node = copy->head;
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

int test_copy_empty() {
    DLinkedList *list = dll_create();

    // Clone empty list
    DLinkedList *shallow_copy = dll_copy(list);
    ASSERT_NOT_NULLPTR(shallow_copy);
    ASSERT_EQ(shallow_copy->size, 0);
    ASSERT_NULLPTR(shallow_copy->head);
    ASSERT_NULLPTR(shallow_copy->tail);

    DLinkedList *deep_copy = dll_copy_deep(list, int_copy);
    ASSERT_NOT_NULLPTR(deep_copy);
    ASSERT_EQ(deep_copy->size, 0);
    ASSERT_NULLPTR(deep_copy->head);
    ASSERT_NULLPTR(deep_copy->tail);

    // Cleanup
    dll_destroy(list, nullptr);
    dll_destroy(shallow_copy, nullptr);
    dll_destroy(deep_copy, nullptr);

    return TEST_SUCCESS;
}

int test_copy_null() {
    // Should handle NULL gracefully
    ASSERT_NULLPTR(dll_copy(nullptr));
    ASSERT_NULLPTR(dll_copy_deep(nullptr, int_copy));

    // Should require a valid copy function
    DLinkedList *list = dll_create();
    ASSERT_NULLPTR(dll_copy_deep(list, nullptr));
    dll_destroy(list, nullptr);

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
        printf("test_map failed\n");
        failed++;
    }
    if (test_for_each() != TEST_SUCCESS) {
        printf("test_for_each failed\n");
        failed++;
    }
    if (test_copy_shallow() != TEST_SUCCESS) {
        printf("test_clone_shallow failed\n");
        failed++;
    }
    if (test_copy_deep() != TEST_SUCCESS) {
        printf("test_clone_deep failed\n");
        failed++;
    }
    if (test_copy_complex_data() != TEST_SUCCESS) {
        printf("test_clone_complex_data failed\n");
        failed++;
    }
    if (test_copy_empty() != TEST_SUCCESS) {
        printf("test_clone_empty failed\n");
        failed++;
    }
    if (test_copy_null() != TEST_SUCCESS) {
        printf("test_clone_null failed\n");
        failed++;
    }


    if (failed == 0) {
        printf("All DLinkedList tests passed.\n");
        return 0;
    }
    printf("%d DLinkedList tests failed.\n", failed);
    return 1;
}
