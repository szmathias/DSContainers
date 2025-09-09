//
// Created by zack on 8/30/25.
//

#include "TestHelpers.h"

#include <stdlib.h>
#include <string.h>


// Integer comparison function
int int_cmp(const void* a, const void* b)
{
    return (*(int*)a) - (*(int*)b);
}

// Custom comparison function for descending order
int int_cmp_desc(const void* a, const void* b)
{
    return (*(int*)b) - (*(int*)a);
}

// Integer free function
void int_free(void* a)
{
    free(a);
}

// Custom allocator for testing
void* test_calloc(const size_t size)
{
    return calloc(1, size);
}

void test_dealloc(void* ptr)
{
    free(ptr);
}

// Person comparison function
int person_cmp(const void* a, const void* b)
{
    const Person* p1 = a;
    const Person* p2 = b;
    return strcmp(p1->name, p2->name);
}

// Person free function
void person_free(void* p)
{
    free(p);
}

// Helper to create a person
Person* create_person(const char* name, const int age)
{
    Person* p = malloc(sizeof(Person));
    if (p)
    {
        strncpy(p->name, name, 49);
        p->name[49] = '\0';
        p->age      = age;
    }
    return p;
}

// Clone an integer
void* int_copy(const void* data)
{
    const int* original = data;
    int* copy           = malloc(sizeof(int));
    if (copy)
    {
        *copy = *original;
    }
    return copy;
}

// Clone a person
void* person_copy(const void* data)
{
    const Person* original = data;
    Person* copy           = create_person(original->name, original->age);
    return copy;
}

// Predicate function that returns non-zero for even numbers
int is_even(const void* data)
{
    return (*(int*)data % 2 == 0) ? 1 : 0;
}

// Transform function that doubles a number
void* double_value(const void* data)
{
    int* result = malloc(sizeof(int));
    if (result)
    {
        *result = *(int*)data * 2;
    }
    return result;
}

// Action function that increments a number
void increment(void* data)
{
    (*(int*)data)++;
}

// Predicate: is odd
int is_odd(const void* data)
{
    const int* value = data;
    return (*value % 2 != 0);
}

// Predicate: is greater than 5
int is_greater_than_five(const void* data)
{
    const int* value = data;
    return (*value > 5);
}

// Predicate: is greater than 10
int is_greater_than_10(const void* data)
{
    const int* value = data;
    return (*value > 10);
}

// Predicate: is divisible by 3
int is_divisible_by_3(const void* data)
{
    const int* value = data;
    return (*value % 3 == 0);
}

// Predicate: is divisible by 6
int is_divisible_by_six(const void* data)
{
    const int* value = data;
    return (*value % 6 == 0);
}

// Transform: square a number
void* square_func(const void* data)
{
    const int* original = data;
    int* result         = malloc(sizeof(int));
    if (result)
    {
        *result = (*original) * (*original);
    }
    return result;
}

// Transform: add 1 to a value
void* add_one(const void* data)
{
    const int* original = data;
    int* result         = malloc(sizeof(int));
    *result             = (*original) + 1;
    return result;
}

// Transform: add 5 to a value
void* add_five(const void* data)
{
    const int* original = data;
    int* result         = malloc(sizeof(int));
    *result             = (*original) + 5;
    return result;
}

// Transform: add 10 to a value
void* add_ten_func(const void* data)
{
    const int* original = data;
    int* result         = malloc(sizeof(int));
    *result             = (*original) + 10;
    return result;
}

// Transform: multiply by 3
void* multiply_by_three(const void* data)
{
    const int* original = data;
    int* result         = malloc(sizeof(int));
    *result             = (*original) * 3;
    return result;
}

// State for the failing allocator
static int alloc_fail_countdown = -1; // -1 means never fail

// A custom allocator that will fail after a certain number of calls
void* failing_alloc(const size_t size)
{
    if (alloc_fail_countdown == 0)
    {
        return NULL; // Trigger failure
    }
    if (alloc_fail_countdown > 0)
    {
        alloc_fail_countdown--;
    }
    return calloc(1, size); // Use calloc for safety and consistency
}

void failing_free(void* ptr)
{
    free(ptr);
}

// A copy function that uses the failing allocator
void* failing_int_copy(const void* data)
{
    const int* original = data;
    // Use failing_alloc instead of malloc
    int* copy = failing_alloc(sizeof(int));
    if (copy)
    {
        *copy = *original;
    }
    return copy;
}

// A transform function that uses the failing allocator
void* double_value_failing(const void* data)
{
    int* result = failing_alloc(sizeof(int)); // Use the failing allocator
    if (result)
    {
        *result = *(int*)data * 2;
    }
    return result;
}

// Helper to set up the failing allocator
void set_alloc_fail_countdown(const int count)
{
    alloc_fail_countdown = count;
}

// Allocator helper functions
DSCAlloc* create_std_allocator(void)
{
    DSCAlloc* alloc = malloc(sizeof(DSCAlloc));
    if (alloc)
    {
        alloc->alloc_func     = malloc;
        alloc->dealloc_func   = free;
        alloc->data_free_func = free;
        alloc->copy_func      = int_copy;
    }
    return alloc;
}

DSCAlloc* create_failing_allocator(void)
{
    DSCAlloc* alloc = malloc(sizeof(DSCAlloc));
    if (alloc)
    {
        alloc->alloc_func     = failing_alloc;
        alloc->dealloc_func   = failing_free;
        alloc->data_free_func = failing_free;
        alloc->copy_func      = failing_int_copy;
    }
    return alloc;
}

void destroy_allocator(DSCAlloc* alloc)
{
    if (alloc)
    {
        free(alloc);
    }
}