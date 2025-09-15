//
// HashSet test file - comprehensive testing of set operations
//

#include "TestAssert.h"
#include "TestHelpers.h"
#include "HashSet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Test basic HashSet creation and destruction
int test_hashset_create_destroy(void)
{
    DSCAllocator alloc = create_int_allocator();

    DSCHashSet* set = dsc_hashset_create(&alloc, dsc_hash_string, dsc_key_equals_string, 0);
    ASSERT_NOT_NULL(set);
    ASSERT_EQ(dsc_hashset_size(set), 0);
    ASSERT(dsc_hashset_is_empty(set));

    dsc_hashset_destroy(set, false);
    return TEST_SUCCESS;
}

// Test basic add and contains operations
int test_hashset_add_contains(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCHashSet* set = dsc_hashset_create(&alloc, dsc_hash_string, dsc_key_equals_string, 0);

    char* key1 = "apple";
    char* key2 = "banana";
    char* key3 = "cherry";

    // Initially empty
    ASSERT(!dsc_hashset_contains(set, key1));
    ASSERT(!dsc_hashset_contains(set, key2));
    ASSERT(!dsc_hashset_contains(set, key3));

    // Add elements
    ASSERT_EQ(dsc_hashset_add(set, key1), 0);
    ASSERT_EQ(dsc_hashset_size(set), 1);
    ASSERT(!dsc_hashset_is_empty(set));
    ASSERT(dsc_hashset_contains(set, key1));

    ASSERT_EQ(dsc_hashset_add(set, key2), 0);
    ASSERT_EQ(dsc_hashset_size(set), 2);
    ASSERT(dsc_hashset_contains(set, key2));

    ASSERT_EQ(dsc_hashset_add(set, key3), 0);
    ASSERT_EQ(dsc_hashset_size(set), 3);
    ASSERT(dsc_hashset_contains(set, key3));

    // All should still be present
    ASSERT(dsc_hashset_contains(set, key1));
    ASSERT(dsc_hashset_contains(set, key2));
    ASSERT(dsc_hashset_contains(set, key3));

    dsc_hashset_destroy(set, false);
    return TEST_SUCCESS;
}

// Test duplicate handling
int test_hashset_duplicates(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCHashSet* set = dsc_hashset_create(&alloc, dsc_hash_string, dsc_key_equals_string, 0);

    char* key = "duplicate";

    // Add same key multiple times
    ASSERT_EQ(dsc_hashset_add(set, key), 0);
    ASSERT_EQ(dsc_hashset_size(set), 1);

    ASSERT_EQ(dsc_hashset_add(set, key), 0); // Should be no-op
    ASSERT_EQ(dsc_hashset_size(set), 1);     // Size unchanged

    ASSERT_EQ(dsc_hashset_add(set, key), 0); // Should be no-op
    ASSERT_EQ(dsc_hashset_size(set), 1);     // Size unchanged

    ASSERT(dsc_hashset_contains(set, key));

    dsc_hashset_destroy(set, false);
    return TEST_SUCCESS;
}

// Test add_check function
int test_hashset_add_check(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCHashSet* set = dsc_hashset_create(&alloc, dsc_hash_string, dsc_key_equals_string, 0);

    char* key = "test_key";
    bool was_added;

    // First addition should be new
    ASSERT_EQ(dsc_hashset_add_check(set, key, &was_added), 0);
    ASSERT(was_added);
    ASSERT_EQ(dsc_hashset_size(set), 1);

    // Second addition should not be new
    ASSERT_EQ(dsc_hashset_add_check(set, key, &was_added), 0);
    ASSERT(!was_added);
    ASSERT_EQ(dsc_hashset_size(set), 1);

    dsc_hashset_destroy(set, false);
    return TEST_SUCCESS;
}

// Test remove operations
int test_hashset_remove(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCHashSet* set = dsc_hashset_create(&alloc, dsc_hash_string, dsc_key_equals_string, 0);

    char* key1 = "first";
    char* key2 = "second";
    char* key3 = "third";

    // Add elements
    dsc_hashset_add(set, key1);
    dsc_hashset_add(set, key2);
    dsc_hashset_add(set, key3);
    ASSERT_EQ(dsc_hashset_size(set), 3);

    // Remove middle element
    ASSERT_EQ(dsc_hashset_remove(set, key2, false), 0);
    ASSERT_EQ(dsc_hashset_size(set), 2);
    ASSERT(!dsc_hashset_contains(set, key2));
    ASSERT(dsc_hashset_contains(set, key1));
    ASSERT(dsc_hashset_contains(set, key3));

    // Remove non-existent element
    ASSERT_EQ(dsc_hashset_remove(set, "nonexistent", false), -1);
    ASSERT_EQ(dsc_hashset_size(set), 2);

    // Remove remaining elements
    ASSERT_EQ(dsc_hashset_remove(set, key1, false), 0);
    ASSERT_EQ(dsc_hashset_remove(set, key3, false), 0);
    ASSERT_EQ(dsc_hashset_size(set), 0);
    ASSERT(dsc_hashset_is_empty(set));

    dsc_hashset_destroy(set, false);
    return TEST_SUCCESS;
}

// Test set union operation
int test_hashset_union(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCHashSet* set1 = dsc_hashset_create(&alloc, dsc_hash_string, dsc_key_equals_string, 0);
    DSCHashSet* set2 = dsc_hashset_create(&alloc, dsc_hash_string, dsc_key_equals_string, 0);

    // Populate sets with some overlap
    dsc_hashset_add(set1, "a");
    dsc_hashset_add(set1, "b");
    dsc_hashset_add(set1, "c");

    dsc_hashset_add(set2, "b");
    dsc_hashset_add(set2, "c");
    dsc_hashset_add(set2, "d");

    DSCHashSet* union_set = dsc_hashset_union(set1, set2);
    ASSERT_NOT_NULL(union_set);
    ASSERT_EQ(dsc_hashset_size(union_set), 4); // a, b, c, d

    ASSERT(dsc_hashset_contains(union_set, "a"));
    ASSERT(dsc_hashset_contains(union_set, "b"));
    ASSERT(dsc_hashset_contains(union_set, "c"));
    ASSERT(dsc_hashset_contains(union_set, "d"));

    dsc_hashset_destroy(set1, false);
    dsc_hashset_destroy(set2, false);
    dsc_hashset_destroy(union_set, false);
    return TEST_SUCCESS;
}

// Test set intersection operation
int test_hashset_intersection(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCHashSet* set1 = dsc_hashset_create(&alloc, dsc_hash_string, dsc_key_equals_string, 0);
    DSCHashSet* set2 = dsc_hashset_create(&alloc, dsc_hash_string, dsc_key_equals_string, 0);

    // Populate sets with some overlap
    dsc_hashset_add(set1, "a");
    dsc_hashset_add(set1, "b");
    dsc_hashset_add(set1, "c");

    dsc_hashset_add(set2, "b");
    dsc_hashset_add(set2, "c");
    dsc_hashset_add(set2, "d");

    DSCHashSet* intersection_set = dsc_hashset_intersection(set1, set2);
    ASSERT_NOT_NULL(intersection_set);
    ASSERT_EQ(dsc_hashset_size(intersection_set), 2); // b, c

    ASSERT(!dsc_hashset_contains(intersection_set, "a"));
    ASSERT(dsc_hashset_contains(intersection_set, "b"));
    ASSERT(dsc_hashset_contains(intersection_set, "c"));
    ASSERT(!dsc_hashset_contains(intersection_set, "d"));

    dsc_hashset_destroy(set1, false);
    dsc_hashset_destroy(set2, false);
    dsc_hashset_destroy(intersection_set, false);
    return TEST_SUCCESS;
}

// Test set difference operation
int test_hashset_difference(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCHashSet* set1 = dsc_hashset_create(&alloc, dsc_hash_string, dsc_key_equals_string, 0);
    DSCHashSet* set2 = dsc_hashset_create(&alloc, dsc_hash_string, dsc_key_equals_string, 0);

    // Populate sets
    dsc_hashset_add(set1, "a");
    dsc_hashset_add(set1, "b");
    dsc_hashset_add(set1, "c");

    dsc_hashset_add(set2, "b");
    dsc_hashset_add(set2, "d");

    DSCHashSet* difference_set = dsc_hashset_difference(set1, set2);
    ASSERT_NOT_NULL(difference_set);
    ASSERT_EQ(dsc_hashset_size(difference_set), 2); // a, c

    ASSERT(dsc_hashset_contains(difference_set, "a"));
    ASSERT(!dsc_hashset_contains(difference_set, "b"));
    ASSERT(dsc_hashset_contains(difference_set, "c"));
    ASSERT(!dsc_hashset_contains(difference_set, "d"));

    dsc_hashset_destroy(set1, false);
    dsc_hashset_destroy(set2, false);
    dsc_hashset_destroy(difference_set, false);
    return TEST_SUCCESS;
}

// Test subset operation
int test_hashset_is_subset(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCHashSet* set1 = dsc_hashset_create(&alloc, dsc_hash_string, dsc_key_equals_string, 0);
    DSCHashSet* set2 = dsc_hashset_create(&alloc, dsc_hash_string, dsc_key_equals_string, 0);
    DSCHashSet* set3 = dsc_hashset_create(&alloc, dsc_hash_string, dsc_key_equals_string, 0);

    // set1: {a, b}
    dsc_hashset_add(set1, "a");
    dsc_hashset_add(set1, "b");

    // set2: {a, b, c, d}
    dsc_hashset_add(set2, "a");
    dsc_hashset_add(set2, "b");
    dsc_hashset_add(set2, "c");
    dsc_hashset_add(set2, "d");

    // set3: {a, e}
    dsc_hashset_add(set3, "a");
    dsc_hashset_add(set3, "e");

    // set1 is subset of set2
    ASSERT(dsc_hashset_is_subset(set1, set2));

    // set2 is not subset of set1
    ASSERT(!dsc_hashset_is_subset(set2, set1));

    // set3 is not subset of set2
    ASSERT(!dsc_hashset_is_subset(set3, set2));

    // Every set is subset of itself
    ASSERT(dsc_hashset_is_subset(set1, set1));
    ASSERT(dsc_hashset_is_subset(set2, set2));

    dsc_hashset_destroy(set1, false);
    dsc_hashset_destroy(set2, false);
    dsc_hashset_destroy(set3, false);
    return TEST_SUCCESS;
}

// Test iterator functionality
int test_hashset_iterator(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCHashSet* set = dsc_hashset_create(&alloc, dsc_hash_string, dsc_key_equals_string, 0);

    char* keys[] = {"alpha", "beta", "gamma", "delta"};
    const int num_keys = 4;

    // Add elements
    for (int i = 0; i < num_keys; i++)
    {
        dsc_hashset_add(set, keys[i]);
    }

    // Iterate and verify all elements are visited
    DSCIterator it = dsc_hashset_iterator(set);
    int visited_count = 0;
    bool found[4] = {false, false, false, false};

    while (it.has_next(&it))
    {
        const char* key = it.get(&it);
        ASSERT_NOT_NULL(key);
        it.next(&it);

        // Find which key this is
        for (int i = 0; i < num_keys; i++)
        {
            if (strcmp(key, keys[i]) == 0)
            {
                ASSERT(!found[i]); // Should not have seen it before
                found[i] = true;
                break;
            }
        }
        visited_count++;
    }

    ASSERT_EQ(visited_count, num_keys);
    for (int i = 0; i < num_keys; i++)
    {
        ASSERT(found[i]);
    }

    it.destroy(&it);
    dsc_hashset_destroy(set, false);
    return TEST_SUCCESS;
}

// Test clear operation
int test_hashset_clear(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCHashSet* set = dsc_hashset_create(&alloc, dsc_hash_string, dsc_key_equals_string, 0);

    // Add elements
    dsc_hashset_add(set, "one");
    dsc_hashset_add(set, "two");
    dsc_hashset_add(set, "three");
    ASSERT_EQ(dsc_hashset_size(set), 3);

    // Clear
    dsc_hashset_clear(set, false);
    ASSERT_EQ(dsc_hashset_size(set), 0);
    ASSERT(dsc_hashset_is_empty(set));

    // Should be able to add again
    dsc_hashset_add(set, "new");
    ASSERT_EQ(dsc_hashset_size(set), 1);
    ASSERT(dsc_hashset_contains(set, "new"));

    dsc_hashset_destroy(set, false);
    return TEST_SUCCESS;
}

// Test copy operations
int test_hashset_copy(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCHashSet* original = dsc_hashset_create(&alloc, dsc_hash_string, dsc_key_equals_string, 0);

    // Add elements
    dsc_hashset_add(original, "x");
    dsc_hashset_add(original, "y");
    dsc_hashset_add(original, "z");

    // Shallow copy
    DSCHashSet* copy = dsc_hashset_copy(original);
    ASSERT_NOT_NULL(copy);
    ASSERT_EQ(dsc_hashset_size(copy), 3);

    ASSERT(dsc_hashset_contains(copy, "x"));
    ASSERT(dsc_hashset_contains(copy, "y"));
    ASSERT(dsc_hashset_contains(copy, "z"));

    // Modify original - copy should be independent
    dsc_hashset_add(original, "w");
    ASSERT_EQ(dsc_hashset_size(original), 4);
    ASSERT_EQ(dsc_hashset_size(copy), 3);
    ASSERT(!dsc_hashset_contains(copy, "w"));

    dsc_hashset_destroy(original, false);
    dsc_hashset_destroy(copy, false);
    return TEST_SUCCESS;
}

typedef struct
{
    int (*func)(void);
    const char* name;
} TestCase;

int main(void)
{
    const TestCase tests[] = {
        {test_hashset_create_destroy, "test_hashset_create_destroy"},
        {test_hashset_add_contains, "test_hashset_add_contains"},
        {test_hashset_duplicates, "test_hashset_duplicates"},
        {test_hashset_add_check, "test_hashset_add_check"},
        {test_hashset_remove, "test_hashset_remove"},
        {test_hashset_union, "test_hashset_union"},
        {test_hashset_intersection, "test_hashset_intersection"},
        {test_hashset_difference, "test_hashset_difference"},
        {test_hashset_is_subset, "test_hashset_is_subset"},
        {test_hashset_iterator, "test_hashset_iterator"},
        {test_hashset_clear, "test_hashset_clear"},
        {test_hashset_copy, "test_hashset_copy"},
    };

    printf("Running HashSet tests...\n");

    int failed = 0;
    const int num_tests = sizeof(tests) / sizeof(tests[0]);
    for (int i = 0; i < num_tests; i++)
    {
        printf("Running %s... ", tests[i].name);
        if (tests[i].func() != TEST_SUCCESS)
        {
            printf("FAILED\n");
            failed++;
        }
        else
        {
            printf("PASSED\n");
        }
    }

    if (failed == 0)
    {
        printf("\nAll HashSet tests passed!\n");
        return 0;
    }

    printf("\n%d HashSet tests failed.\n", failed);
    return 1;
}