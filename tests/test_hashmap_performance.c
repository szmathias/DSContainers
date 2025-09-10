//
// HashMap performance test - converted from HashTable performance test
//

#include "TestAssert.h"
#include "TestHelpers.h"
#include "HashMap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Test insertion performance
int test_hashmap_performance_insertion(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCHashMap* map = dsc_hashmap_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);

    const int num_items = 1000;
    char keys[1000][20];
    char values[1000][20];

    // Prepare test data
    for (int i = 0; i < num_items; i++)
    {
        sprintf(keys[i], "key_%d", i);
        sprintf(values[i], "value_%d", i);
    }

    clock_t start = clock();

    // Insert all items
    for (int i = 0; i < num_items; i++)
    {
        ASSERT_EQ(dsc_hashmap_put(map, keys[i], values[i]), 0);
    }

    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Inserted %d items in %f seconds\n", num_items, time_taken);
    ASSERT_EQ(dsc_hashmap_size(map), (size_t)num_items);

    dsc_hashmap_destroy(map, false, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test lookup performance
int test_hashmap_performance_lookup(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCHashMap* map = dsc_hashmap_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);

    const int num_items = 1000;
    char keys[1000][20];
    char values[1000][20];

    // Prepare and insert test data
    for (int i = 0; i < num_items; i++)
    {
        sprintf(keys[i], "key_%d", i);
        sprintf(values[i], "value_%d", i);
        dsc_hashmap_put(map, keys[i], values[i]);
    }

    clock_t start = clock();

    // Perform lookups
    for (int i = 0; i < num_items; i++)
    {
        void* result = dsc_hashmap_get(map, keys[i]);
        ASSERT_NOT_NULL(result);
        ASSERT_EQ_STR((char*)result, values[i]);
    }

    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Performed %d lookups in %f seconds\n", num_items, time_taken);

    dsc_hashmap_destroy(map, false, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test removal performance
int test_hashmap_performance_removal(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCHashMap* map = dsc_hashmap_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);

    const int num_items = 1000;
    char keys[1000][20];
    char values[1000][20];

    // Prepare and insert test data
    for (int i = 0; i < num_items; i++)
    {
        sprintf(keys[i], "key_%d", i);
        sprintf(values[i], "value_%d", i);
        dsc_hashmap_put(map, keys[i], values[i]);
    }

    clock_t start = clock();

    // Remove all items
    for (int i = 0; i < num_items; i++)
    {
        ASSERT_EQ(dsc_hashmap_remove(map, keys[i], false, false), 0);
    }

    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Removed %d items in %f seconds\n", num_items, time_taken);
    ASSERT_EQ(dsc_hashmap_size(map), 0);

    dsc_hashmap_destroy(map, false, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test copy performance
int test_hashmap_performance_copy(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCHashMap* original = dsc_hashmap_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);

    const int num_items = 1000;
    char keys[1000][20];
    char values[1000][20];

    // Prepare and insert test data
    for (int i = 0; i < num_items; i++)
    {
        sprintf(keys[i], "key_%d", i);
        sprintf(values[i], "value_%d", i);
        dsc_hashmap_put(original, keys[i], values[i]);
    }

    clock_t start = clock();

    DSCHashMap* copy = dsc_hashmap_copy(original);

    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Copied %d items in %f seconds\n", num_items, time_taken);
    ASSERT_NOT_NULL(copy);
    ASSERT_EQ(dsc_hashmap_size(copy), (size_t)num_items);

    dsc_hashmap_destroy(original, false, false);
    dsc_hashmap_destroy(copy, false, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test iteration performance
int test_hashmap_performance_iteration(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCHashMap* map = dsc_hashmap_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);

    const int num_items = 1000;
    char keys[1000][20];
    char values[1000][20];

    // Prepare and insert test data
    for (int i = 0; i < num_items; i++)
    {
        sprintf(keys[i], "key_%d", i);
        sprintf(values[i], "value_%d", i);
        dsc_hashmap_put(map, keys[i], values[i]);
    }

    clock_t start = clock();

    // Iterate through all items
    DSCIterator it = dsc_hashmap_iterator(map);
    int visited_count = 0;
    while (it.has_next(&it))
    {
        DSCKeyValuePair* pair = (DSCKeyValuePair*)it.next(&it);
        ASSERT_NOT_NULL(pair);
        visited_count++;
    }

    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Iterated through %d items in %f seconds\n", visited_count, time_taken);
    ASSERT_EQ(visited_count, num_items);

    it.destroy(&it);
    dsc_hashmap_destroy(map, false, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test resize performance under heavy load
int test_hashmap_performance_resize(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCHashMap* map = dsc_hashmap_create(alloc, dsc_hash_string, dsc_key_equals_string, 4); // Small initial size

    const int num_items = 1000;
    char keys[1000][20];
    char values[1000][20];

    // Prepare test data
    for (int i = 0; i < num_items; i++)
    {
        sprintf(keys[i], "key_%d", i);
        sprintf(values[i], "value_%d", i);
    }

    clock_t start = clock();

    // Insert items that will trigger multiple resizes
    for (int i = 0; i < num_items; i++)
    {
        ASSERT_EQ(dsc_hashmap_put(map, keys[i], values[i]), 0);
    }

    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Inserted %d items with resizing in %f seconds\n", num_items, time_taken);
    ASSERT_EQ(dsc_hashmap_size(map), (size_t)num_items);

    // Verify all items are still accessible
    for (int i = 0; i < num_items; i++)
    {
        void* result = dsc_hashmap_get(map, keys[i]);
        ASSERT_NOT_NULL(result);
    }

    dsc_hashmap_destroy(map, false, false);
    destroy_allocator(alloc);
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
        {test_hashmap_performance_insertion, "test_hashmap_performance_insertion"},
        {test_hashmap_performance_lookup, "test_hashmap_performance_lookup"},
        {test_hashmap_performance_removal, "test_hashmap_performance_removal"},
        {test_hashmap_performance_copy, "test_hashmap_performance_copy"},
        {test_hashmap_performance_iteration, "test_hashmap_performance_iteration"},
        {test_hashmap_performance_resize, "test_hashmap_performance_resize"},
    };

    printf("Running HashMap performance tests...\n");

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
        printf("All HashMap performance tests passed!\n");
        return 0;
    }

    printf("%d HashMap performance tests failed.\n", failed);
    return 1;
}
