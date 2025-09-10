//
// HashSet Properties test
//

#include "TestAssert.h"
#include "TestHelpers.h"
#include "HashSet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Test load factor properties
int test_hashset_load_factor(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCHashSet* set = dsc_hashset_create(alloc, dsc_hash_string, dsc_key_equals_string, 4);

    // Empty set should have 0.0 load factor
    ASSERT_EQ(dsc_hashset_load_factor(set), 0.0);

    // Add elements and check load factor increases
    ASSERT_EQ(dsc_hashset_add(set, "key1"), 0);
    const double lf1 = dsc_hashset_load_factor(set);
    ASSERT(lf1 > 0.0);

    ASSERT_EQ(dsc_hashset_add(set, "key2"), 0);
    const double lf2 = dsc_hashset_load_factor(set);
    ASSERT(lf2 > lf1);

    // Remove element and check load factor decreases
    ASSERT_EQ(dsc_hashset_remove(set, "key1", false), 0);
    const double lf3 = dsc_hashset_load_factor(set);
    ASSERT(lf3 < lf2);
    ASSERT(lf3 > 0.0);

    dsc_hashset_destroy(set, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test size consistency
int test_hashset_size_consistency(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCHashSet* set = dsc_hashset_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);

    // Initially empty
    ASSERT_EQ(dsc_hashset_size(set), 0);
    ASSERT(dsc_hashset_is_empty(set));

    // Add elements
    char* keys[] = {"apple", "banana", "cherry", "date"};
    for (int i = 0; i < 4; i++)
    {
        ASSERT_EQ(dsc_hashset_add(set, keys[i]), 0);
        ASSERT_EQ(dsc_hashset_size(set), (size_t)i + 1);
        ASSERT(!dsc_hashset_is_empty(set));
    }

    // Add duplicate - size should not change
    ASSERT_EQ(dsc_hashset_add(set, "apple"), 0);
    ASSERT_EQ(dsc_hashset_size(set), 4);

    // Remove elements
    for (int i = 0; i < 4; i++)
    {
        ASSERT_EQ(dsc_hashset_remove(set, keys[i], false), 0);
        ASSERT_EQ(dsc_hashset_size(set), 4 - (size_t)i - 1);
    }

    ASSERT(dsc_hashset_is_empty(set));

    dsc_hashset_destroy(set, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test uniqueness property
int test_hashset_uniqueness(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCHashSet* set = dsc_hashset_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);

    char* key = "unique_key";

    // Add same key multiple times
    for (int i = 0; i < 10; i++)
    {
        ASSERT_EQ(dsc_hashset_add(set, key), 0);
        ASSERT_EQ(dsc_hashset_size(set), 1);
    }

    // Should still contain only one element
    ASSERT(dsc_hashset_contains(set, key));
    ASSERT_EQ(dsc_hashset_size(set), 1);

    // Get all elements - should return only one
    void** elements;
    size_t count;
    ASSERT_EQ(dsc_hashset_get_elements(set, &elements, &count), 0);
    ASSERT_EQ(count, 1);
    ASSERT_EQ_STR((char*)elements[0], key);

    alloc->dealloc_func(elements);
    dsc_hashset_destroy(set, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Global counter for testing
static int visit_count = 0;

// Action function that counts visits
static void count_action(void* key)
{
    (void)key; // Unused
    visit_count++;
}

// Test for_each functionality
int test_hashset_for_each(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCHashSet* set = dsc_hashset_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);

    for (int i = 0; i < 3; i++)
    {
        char* keys[] = {"a", "b", "c"};
        ASSERT_EQ(dsc_hashset_add(set, keys[i]), 0);
    }

    // Reset counter
    visit_count = 0;

    dsc_hashset_for_each(set, count_action);
    ASSERT_EQ(visit_count, 3);

    dsc_hashset_destroy(set, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test get_elements completeness
int test_hashset_get_elements_completeness(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCHashSet* set = dsc_hashset_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);

    char* original_keys[] = {"first", "second", "third", "fourth"};
    for (int i = 0; i < 4; i++)
    {
        ASSERT_EQ(dsc_hashset_add(set, original_keys[i]), 0);
    }

    void** retrieved_keys;
    size_t count;
    ASSERT_EQ(dsc_hashset_get_elements(set, &retrieved_keys, &count), 0);
    ASSERT_EQ(count, 4);

    // Verify all original keys are present in retrieved keys
    for (int i = 0; i < 4; i++)
    {
        bool found = false;
        for (size_t j = 0; j < count; j++)
        {
            if (strcmp(original_keys[i], (char*)retrieved_keys[j]) == 0)
            {
                found = true;
                break;
            }
        }
        ASSERT(found);
    }

    alloc->dealloc_func(retrieved_keys);
    dsc_hashset_destroy(set, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test copy preservation of properties
int test_hashset_copy_properties(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCHashSet* original = dsc_hashset_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);

    // Add elements to original
    char* keys[] = {"alpha", "beta", "gamma"};
    for (int i = 0; i < 3; i++)
    {
        ASSERT_EQ(dsc_hashset_add(original, keys[i]), 0);
    }

    // Create copy
    DSCHashSet* copy = dsc_hashset_copy(original);
    ASSERT_NOT_NULL(copy);

    // Verify copy has same properties
    ASSERT_EQ(dsc_hashset_size(copy), dsc_hashset_size(original));
    ASSERT_EQ(dsc_hashset_is_empty(copy), dsc_hashset_is_empty(original));

    // Verify all elements are present in copy
    for (int i = 0; i < 3; i++)
    {
        ASSERT(dsc_hashset_contains(copy, keys[i]));
    }

    // Modify original - copy should be unaffected
    ASSERT_EQ(dsc_hashset_add(original, "delta"), 0);
    ASSERT_EQ(dsc_hashset_size(original), 4);
    ASSERT_EQ(dsc_hashset_size(copy), 3);
    ASSERT(!dsc_hashset_contains(copy, "delta"));

    dsc_hashset_destroy(original, false);
    dsc_hashset_destroy(copy, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test set operation result properties
int test_hashset_operation_properties(void)
{
    DSCAlloc* alloc = create_std_allocator();

    DSCHashSet* set1 = dsc_hashset_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);
    DSCHashSet* set2 = dsc_hashset_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);

    // Set1: {a, b, c}
    ASSERT_EQ(dsc_hashset_add(set1, "a"), 0);
    ASSERT_EQ(dsc_hashset_add(set1, "b"), 0);
    ASSERT_EQ(dsc_hashset_add(set1, "c"), 0);

    // Set2: {b, c, d}
    ASSERT_EQ(dsc_hashset_add(set2, "b"), 0);
    ASSERT_EQ(dsc_hashset_add(set2, "c"), 0);
    ASSERT_EQ(dsc_hashset_add(set2, "d"), 0);

    // Union should have size 4: {a, b, c, d}
    DSCHashSet* union_set = dsc_hashset_union(set1, set2);
    ASSERT_NOT_NULL(union_set);
    ASSERT_EQ(dsc_hashset_size(union_set), 4);

    // Intersection should have size 2: {b, c}
    DSCHashSet* intersection_set = dsc_hashset_intersection(set1, set2);
    ASSERT_NOT_NULL(intersection_set);
    ASSERT_EQ(dsc_hashset_size(intersection_set), 2);

    // Difference should have size 1: {a}
    DSCHashSet* difference_set = dsc_hashset_difference(set1, set2);
    ASSERT_NOT_NULL(difference_set);
    ASSERT_EQ(dsc_hashset_size(difference_set), 1);

    dsc_hashset_destroy(set1, false);
    dsc_hashset_destroy(set2, false);
    dsc_hashset_destroy(union_set, false);
    dsc_hashset_destroy(intersection_set, false);
    dsc_hashset_destroy(difference_set, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test iterator consistency
int test_hashset_iterator_consistency(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCHashSet* set = dsc_hashset_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);

    for (int i = 0; i < 5; i++)
    {
        char* keys[] = {"one", "two", "three", "four", "five"};
        ASSERT_EQ(dsc_hashset_add(set, keys[i]), 0);
    }

    // Create iterator and count elements
    DSCIterator it = dsc_hashset_iterator(set);
    int iter_count = 0;
    while (it.has_next(&it))
    {
        const void* key = it.next(&it);
        ASSERT_NOT_NULL(key);

        // Verify the key exists in the set
        ASSERT(dsc_hashset_contains(set, key));
        iter_count++;
    }

    // Iterator should visit all elements exactly once
    ASSERT_EQ((size_t)iter_count, dsc_hashset_size(set));

    it.destroy(&it);
    dsc_hashset_destroy(set, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Main test runner
typedef struct
{
    int (*func)(void);
    const char* name;
} TestCase;

int main(void)
{
    const TestCase tests[] = {
        {test_hashset_load_factor, "test_hashset_load_factor"},
        {test_hashset_size_consistency, "test_hashset_size_consistency"},
        {test_hashset_uniqueness, "test_hashset_uniqueness"},
        {test_hashset_for_each, "test_hashset_for_each"},
        {test_hashset_get_elements_completeness, "test_hashset_get_elements_completeness"},
        {test_hashset_copy_properties, "test_hashset_copy_properties"},
        {test_hashset_operation_properties, "test_hashset_operation_properties"},
        {test_hashset_iterator_consistency, "test_hashset_iterator_consistency"},
    };

    printf("Running HashSet Properties tests...\n");

    int failed = 0;
    const int num_tests = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < num_tests; i++)
    {
        printf("Running %s... ", tests[i].name);
        fflush(stdout);

        const int result = tests[i].func();
        if (result == TEST_SUCCESS)
        {
            printf("PASSED\n");
        }
        else
        {
            printf("FAILED\n");
            failed++;
        }
    }

    if (failed == 0)
    {
        printf("All HashSet Properties tests passed!\n");
        return 0;
    }
    printf("%d HashSet Properties tests failed!\n", failed);
    return 1;
}
