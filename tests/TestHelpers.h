//
// Created by zack on 9/1/25.
//

#ifndef DSCONTAINERS_TESTHELPERS_H
#define DSCONTAINERS_TESTHELPERS_H

#include "../include/Alloc.h"

// Complex data type for testing
typedef struct {
    char name[50];
    int age;
} Person;

int int_cmp(const void *a, const void *b);

// Custom comparison function for descending order
int int_cmp_desc(const void *a, const void *b);

// Integer free function
void int_free(void *a);

// Custom allocator for testing
void *test_calloc(size_t size);
void test_dealloc(void *ptr);

// Person comparison function
int person_cmp(const void *a, const void *b);
// Person free function
void person_free(void *p);
// Helper to create a person
Person* create_person(const char* name, int age);

// Clone an integer
void *int_copy(const void *data);
// Clone a person
void *person_copy(const void *data);

// Predicate function that returns non-zero for even numbers
int is_even(const void *data);

// Transform function that doubles a number
void *double_value(const void *data);

// Action function that increments a number
void increment(void *data);

// Predicate: is odd
int is_odd(const void* data);

// Predicate: is greater than 5
int is_greater_than_five(const void* data);

// Predicate: is greater than 10
int is_greater_than_10(const void* data);

// Predicate: is divisible by 3
int is_divisible_by_3(const void* data);

// Predicate: is divisible by 6
int is_divisible_by_six(const void* data);

// Transform: square a number
void* square_func(const void* data);

// Transform: add 1 to a value
void* add_one(const void* data);

// Transform: add 5 to a value
void* add_five(const void* data);

// Transform: add 10 to a value
void* add_ten_func(const void* data);

// Transform: multiply by 3
void* multiply_by_three(const void* data);

// A custom allocator that will fail after a certain number of calls
void* failing_alloc(size_t size);

void failing_free(void* ptr);

// A copy function that uses the failing allocator
void *failing_int_copy(const void *data);

// A transform function that uses the failing allocator
void *double_value_failing(const void *data);

// Helper to set up the failing allocator
void set_alloc_fail_countdown(int count);

// Allocator helper functions
DSCAlloc* create_std_allocator(void);
DSCAlloc* create_failing_allocator(void);
void destroy_allocator(DSCAlloc* alloc);

#endif //DSCONTAINERS_TESTHELPERS_H