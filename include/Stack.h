//
// Created by zack on 9/9/25.
//

#ifndef DSCONTAINERS_STACK_H
#define DSCONTAINERS_STACK_H

#include <stddef.h>
#include "Alloc.h"
#include "CStandardCompatibility.h"
#include "Iterator.h"
#include "PlatformDefs.h"

//==============================================================================
// Type definitions
//==============================================================================

/**
 * Node of stack (internally implemented as a singly linked list)
 */
typedef struct DSCStackNode
{
    void* data;                // Pointer to user data
    struct DSCStackNode* next; // Pointer to next node (towards bottom)
} DSCStackNode;

/**
 * Stack structure with custom allocator support.
 * Implemented as a LIFO (Last In, First Out) container using a singly linked list.
 * Provides O(1) push, pop, and peek operations.
 */
typedef struct DSCStack
{
    DSCStackNode* top; // Pointer to top node
    size_t size;       // Number of elements in stack
    DSCAllocator* alloc;   // Custom allocator
} DSCStack;

/**
 * Action function for applying an operation to each element.
 * Used in for-each style traversal.
 *
 * @param data Pointer to element data
 */
typedef void (*action_func)(void* data);

//==============================================================================
// Creation and destruction functions
//==============================================================================

/**
 * Create a new, empty stack with custom allocator.
 *
 * @param alloc Custom allocator (required)
 * @return Pointer to new Stack, or NULL on failure
 */
DSC_API DSCStack* dsc_stack_create(DSCAllocator* alloc);

/**
 * Destroy the stack and free all nodes.
 *
 * @param stack The stack to destroy
 * @param should_free_data Whether to free the data elements using alloc->data_free_func
 */
DSC_API void dsc_stack_destroy(DSCStack* stack, bool should_free_data);

/**
 * Clear all elements from the stack, but keep the structure intact.
 *
 * @param stack The stack to clear
 * @param should_free_data Whether to free the data elements
 */
DSC_API void dsc_stack_clear(DSCStack* stack, bool should_free_data);

//==============================================================================
// Information functions
//==============================================================================

/**
 * Get the number of elements in the stack.
 *
 * @param stack The stack to query
 * @return Number of elements, or 0 if stack is NULL
 */
DSC_API size_t dsc_stack_size(const DSCStack* stack);

/**
 * Check if the stack is empty.
 *
 * @param stack The stack to check
 * @return 1 if stack is empty or NULL, 0 if it contains elements
 */
DSC_API int dsc_stack_is_empty(const DSCStack* stack);

/**
 * Compare two stacks for equality using the given comparison function.
 *
 * @param stack1 First stack to compare
 * @param stack2 Second stack to compare
 * @param compare Function to compare elements
 * @return 1 if stacks are equal, 0 if not equal, -1 on error
 */
DSC_API int dsc_stack_equals(const DSCStack* stack1, const DSCStack* stack2, cmp_func compare);

//==============================================================================
// Element access functions
//==============================================================================

/**
 * Get the top element without removing it.
 *
 * @param stack The stack to access
 * @return Pointer to top element data, or NULL if empty or on error
 */
DSC_API void* dsc_stack_peek(const DSCStack* stack);

/**
 * Get the top element without removing it (alias for peek).
 *
 * @param stack The stack to access
 * @return Pointer to top element data, or NULL if empty or on error
 */
DSC_API void* dsc_stack_top(const DSCStack* stack);

//==============================================================================
// Stack operations
//==============================================================================

/**
 * Push an element onto the top of the stack.
 *
 * @param stack The stack to modify
 * @param data Pointer to the data to push (ownership transferred to stack)
 * @return 0 on success, -1 on error
 */
DSC_API int dsc_stack_push(DSCStack* stack, void* data);

/**
 * Pop the top element from the stack.
 *
 * @param stack The stack to modify
 * @param should_free_data Whether to free the popped data
 * @return 0 on success, -1 on error (e.g., empty stack)
 */
DSC_API int dsc_stack_pop(DSCStack* stack, bool should_free_data);

/**
 * Pop the top element and return its data pointer.
 * The caller is responsible for freeing the returned data if needed.
 *
 * @param stack The stack to modify
 * @return Pointer to the popped data, or NULL if stack is empty or on error
 */
DSC_API void* dsc_stack_pop_data(DSCStack* stack);

//==============================================================================
// Higher-order functions
//==============================================================================

/**
 * Apply an action function to each element in the stack (from top to bottom).
 *
 * @param stack The stack to process
 * @param action Function applied to each element
 */
DSC_API void dsc_stack_for_each(const DSCStack* stack, action_func action);

//==============================================================================
// Stack copying functions
//==============================================================================

/**
 * Create a shallow copy of the stack (sharing data pointers).
 *
 * @param stack The stack to copy
 * @return A new stack with same structure but sharing data, or NULL on error
 */
DSC_API DSCStack* dsc_stack_copy(const DSCStack* stack);

/**
 * Create a deep copy of the stack (cloning data using the provided function).
 *
 * @param stack The stack to copy
 * @param should_free_data Whether the copy function should handle data freeing
 * @return A new stack with copies of all data, or NULL on error
 */
DSC_API DSCStack* dsc_stack_copy_deep(const DSCStack* stack, bool should_free_data);

//==============================================================================
// Iterator functions
//==============================================================================

/**
 * Create an iterator for the stack (top to bottom traversal).
 *
 * @param stack The stack to iterate over
 * @return An Iterator object for forward traversal
 */
DSC_API DSCIterator dsc_stack_iterator(const DSCStack* stack);

/**
 * Create a new stack from an iterator with custom allocator.
 *
 * @param it The source iterator (must be valid)
 * @param alloc The custom allocator to use
 * @return A new stack with elements from iterator, or NULL on error
 */
DSC_API DSCStack* dsc_stack_from_iterator(DSCIterator* it, DSCAllocator* alloc);

#endif //DSCONTAINERS_STACK_H
