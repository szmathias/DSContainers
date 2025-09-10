//
// HashSet Algorithms test - Set operations
//

#include "TestAssert.h"
#include "TestHelpers.h"
#include "HashSet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper function to create a set with string elements
DSCHashSet* create_string_set(DSCAlloc* alloc, char** elements, const size_t count)
{
    DSCHashSet* set = dsc_hashset_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);
    if (!set) return NULL;

    for (size_t i = 0; i < count; i++)
    {
        if (dsc_hashset_add(set, elements[i]) != 0)
        {
            dsc_hashset_destroy(set, false);
            return NULL;
        }
    }
    return set;
}

// Test union operation
int test_hashset_union(void)
{
    DSCAlloc* alloc = create_std_allocator();

    char* set1_elements[] = {"a", "b", "c"};
    char* set2_elements[] = {"c", "d", "e"};

    DSCHashSet* set1 = create_string_set(alloc, set1_elements, 3);
    DSCHashSet* set2 = create_string_set(alloc, set2_elements, 3);

    DSCHashSet* union_set = dsc_hashset_union(set1, set2);
    ASSERT_NOT_NULL(union_set);
    ASSERT_EQ(dsc_hashset_size(union_set), 5); // a, b, c, d, e

    // Check all elements are present
    ASSERT(dsc_hashset_contains(union_set, "a"));
    ASSERT(dsc_hashset_contains(union_set, "b"));
    ASSERT(dsc_hashset_contains(union_set, "c"));
    ASSERT(dsc_hashset_contains(union_set, "d"));
    ASSERT(dsc_hashset_contains(union_set, "e"));

    dsc_hashset_destroy(set1, false);
    dsc_hashset_destroy(set2, false);
    dsc_hashset_destroy(union_set, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test intersection operation
int test_hashset_intersection(void)
{
    DSCAlloc* alloc = create_std_allocator();

    char* set1_elements[] = {"a", "b", "c", "d"};
    char* set2_elements[] = {"c", "d", "e", "f"};

    DSCHashSet* set1 = create_string_set(alloc, set1_elements, 4);
    DSCHashSet* set2 = create_string_set(alloc, set2_elements, 4);

    DSCHashSet* intersection_set = dsc_hashset_intersection(set1, set2);
    ASSERT_NOT_NULL(intersection_set);
    ASSERT_EQ(dsc_hashset_size(intersection_set), 2); // c, d

    // Check intersection elements
    ASSERT(dsc_hashset_contains(intersection_set, "c"));
    ASSERT(dsc_hashset_contains(intersection_set, "d"));
    ASSERT(!dsc_hashset_contains(intersection_set, "a"));
    ASSERT(!dsc_hashset_contains(intersection_set, "b"));
    ASSERT(!dsc_hashset_contains(intersection_set, "e"));
    ASSERT(!dsc_hashset_contains(intersection_set, "f"));

    dsc_hashset_destroy(set1, false);
    dsc_hashset_destroy(set2, false);
    dsc_hashset_destroy(intersection_set, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test difference operation
int test_hashset_difference(void)
{
    DSCAlloc* alloc = create_std_allocator();

    char* set1_elements[] = {"a", "b", "c", "d"};
    char* set2_elements[] = {"c", "d", "e", "f"};

    DSCHashSet* set1 = create_string_set(alloc, set1_elements, 4);
    DSCHashSet* set2 = create_string_set(alloc, set2_elements, 4);

    DSCHashSet* difference_set = dsc_hashset_difference(set1, set2);
    ASSERT_NOT_NULL(difference_set);
    ASSERT_EQ(dsc_hashset_size(difference_set), 2); // a, b

    // Check difference elements (in set1 but not set2)
    ASSERT(dsc_hashset_contains(difference_set, "a"));
    ASSERT(dsc_hashset_contains(difference_set, "b"));
    ASSERT(!dsc_hashset_contains(difference_set, "c"));
    ASSERT(!dsc_hashset_contains(difference_set, "d"));
    ASSERT(!dsc_hashset_contains(difference_set, "e"));
    ASSERT(!dsc_hashset_contains(difference_set, "f"));

    dsc_hashset_destroy(set1, false);
    dsc_hashset_destroy(set2, false);
    dsc_hashset_destroy(difference_set, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test subset operation
int test_hashset_is_subset(void)
{
    DSCAlloc* alloc = create_std_allocator();

    char* superset_elements[] = {"a", "b", "c", "d", "e"};
    char* subset_elements[] = {"b", "d"};
    char* non_subset_elements[] = {"b", "f"};

    DSCHashSet* superset = create_string_set(alloc, superset_elements, 5);
    DSCHashSet* subset = create_string_set(alloc, subset_elements, 2);
    DSCHashSet* non_subset = create_string_set(alloc, non_subset_elements, 2);
    DSCHashSet* empty_set = dsc_hashset_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);

    // Test valid subset
    ASSERT(dsc_hashset_is_subset(subset, superset));

    // Test non-subset
    ASSERT(!dsc_hashset_is_subset(non_subset, superset));

    // Test empty set is subset of any set
    ASSERT(dsc_hashset_is_subset(empty_set, superset));
    ASSERT(dsc_hashset_is_subset(empty_set, subset));

    // Test set is subset of itself
    ASSERT(dsc_hashset_is_subset(superset, superset));

    dsc_hashset_destroy(superset, false);
    dsc_hashset_destroy(subset, false);
    dsc_hashset_destroy(non_subset, false);
    dsc_hashset_destroy(empty_set, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test empty set operations
int test_hashset_empty_operations(void)
{
    DSCAlloc* alloc = create_std_allocator();

    DSCHashSet* empty1 = dsc_hashset_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);
    DSCHashSet* empty2 = dsc_hashset_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);

    char* set_elements[] = {"a", "b", "c"};
    DSCHashSet* non_empty = create_string_set(alloc, set_elements, 3);

    // Union with empty set
    DSCHashSet* union_result = dsc_hashset_union(empty1, non_empty);
    ASSERT_NOT_NULL(union_result);
    ASSERT_EQ(dsc_hashset_size(union_result), 3);

    // Intersection with empty set
    DSCHashSet* intersection_result = dsc_hashset_intersection(empty1, non_empty);
    ASSERT_NOT_NULL(intersection_result);
    ASSERT_EQ(dsc_hashset_size(intersection_result), 0);

    // Difference with empty set
    DSCHashSet* difference_result = dsc_hashset_difference(non_empty, empty1);
    ASSERT_NOT_NULL(difference_result);
    ASSERT_EQ(dsc_hashset_size(difference_result), 3);

    // Union of two empty sets
    DSCHashSet* empty_union = dsc_hashset_union(empty1, empty2);
    ASSERT_NOT_NULL(empty_union);
    ASSERT_EQ(dsc_hashset_size(empty_union), 0);

    dsc_hashset_destroy(empty1, false);
    dsc_hashset_destroy(empty2, false);
    dsc_hashset_destroy(non_empty, false);
    dsc_hashset_destroy(union_result, false);
    dsc_hashset_destroy(intersection_result, false);
    dsc_hashset_destroy(difference_result, false);
    dsc_hashset_destroy(empty_union, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test NULL parameter handling for set operations
int test_hashset_operations_null_params(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCHashSet* set = dsc_hashset_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);

    // Test NULL parameters
    ASSERT_NULL(dsc_hashset_union(NULL, set));
    ASSERT_NULL(dsc_hashset_union(set, NULL));
    ASSERT_NULL(dsc_hashset_union(NULL, NULL));

    ASSERT_NULL(dsc_hashset_intersection(NULL, set));
    ASSERT_NULL(dsc_hashset_intersection(set, NULL));
    ASSERT_NULL(dsc_hashset_intersection(NULL, NULL));

    ASSERT_NULL(dsc_hashset_difference(NULL, set));
    // Note: dsc_hashset_difference(set, NULL) should return a copy of set
    // since difference with empty set is the original set
    DSCHashSet* diff_result = dsc_hashset_difference(set, NULL);
    ASSERT_NOT_NULL(diff_result);
    dsc_hashset_destroy(diff_result, false);

    ASSERT(!dsc_hashset_is_subset(NULL, set));
    ASSERT(!dsc_hashset_is_subset(set, NULL));
    ASSERT(!dsc_hashset_is_subset(NULL, NULL));

    dsc_hashset_destroy(set, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test set operations with identical sets
int test_hashset_identical_operations(void)
{
    DSCAlloc* alloc = create_std_allocator();

    char* elements[] = {"a", "b", "c"};
    DSCHashSet* set1 = create_string_set(alloc, elements, 3);
    DSCHashSet* set2 = create_string_set(alloc, elements, 3);

    // Union of identical sets
    DSCHashSet* union_result = dsc_hashset_union(set1, set2);
    ASSERT_NOT_NULL(union_result);
    ASSERT_EQ(dsc_hashset_size(union_result), 3);

    // Intersection of identical sets
    DSCHashSet* intersection_result = dsc_hashset_intersection(set1, set2);
    ASSERT_NOT_NULL(intersection_result);
    ASSERT_EQ(dsc_hashset_size(intersection_result), 3);

    // Difference of identical sets
    DSCHashSet* difference_result = dsc_hashset_difference(set1, set2);
    ASSERT_NOT_NULL(difference_result);
    ASSERT_EQ(dsc_hashset_size(difference_result), 0);

    // Subset check with identical sets
    ASSERT(dsc_hashset_is_subset(set1, set2));
    ASSERT(dsc_hashset_is_subset(set2, set1));

    dsc_hashset_destroy(set1, false);
    dsc_hashset_destroy(set2, false);
    dsc_hashset_destroy(union_result, false);
    dsc_hashset_destroy(intersection_result, false);
    dsc_hashset_destroy(difference_result, false);
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
        {test_hashset_union, "test_hashset_union"},
        {test_hashset_intersection, "test_hashset_intersection"},
        {test_hashset_difference, "test_hashset_difference"},
        {test_hashset_is_subset, "test_hashset_is_subset"},
        {test_hashset_empty_operations, "test_hashset_empty_operations"},
        {test_hashset_operations_null_params, "test_hashset_operations_null_params"},
        {test_hashset_identical_operations, "test_hashset_identical_operations"},
    };

    printf("Running HashSet Algorithms tests...\n");

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
        printf("All HashSet Algorithms tests passed!\n");
        return 0;
    }
    printf("%d HashSet Algorithms tests failed!\n", failed);
    return 1;
}
