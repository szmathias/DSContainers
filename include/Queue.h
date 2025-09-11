//
// Created by zack on 9/9/25.
//

#ifndef DSCONTAINERS_QUEUE_H
#define DSCONTAINERS_QUEUE_H

#include <stddef.h>
#include "Alloc.h"
#include "CStandardCompatibility.h"
#include "Iterator.h"

//==============================================================================
// Type definitions
//==============================================================================

/**
 * Node of queue (internally implemented as a singly linked list)
 */
typedef struct DSCQueueNode
{
    void* data;                // Pointer to user data
    struct DSCQueueNode* next; // Pointer to next node (towards back)
} DSCQueueNode;

/**
 * Queue structure with custom allocator support.
 * Implemented as a FIFO (First In, First Out) container using a singly linked list.
 * Provides O(1) enqueue, dequeue, and front/back operations.
 */
typedef struct DSCQueue
{
    DSCQueueNode* front; // Pointer to front node (first to be dequeued)
    DSCQueueNode* back;  // Pointer to back node (last enqueued)
    size_t size;         // Number of elements in queue
    DSCAllocator* alloc;     // Custom allocator
} DSCQueue;

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
 * Create a new, empty queue with custom allocator.
 *
 * @param alloc Custom allocator (required)
 * @return Pointer to new Queue, or NULL on failure
 */
DSC_API DSCQueue* dsc_queue_create(DSCAllocator* alloc);

/**
 * Destroy the queue and free all nodes.
 *
 * @param queue The queue to destroy
 * @param should_free_data Whether to free the data elements using alloc->data_free_func
 */
DSC_API void dsc_queue_destroy(DSCQueue* queue, bool should_free_data);

/**
 * Clear all elements from the queue, but keep the structure intact.
 *
 * @param queue The queue to clear
 * @param should_free_data Whether to free the data elements
 */
DSC_API void dsc_queue_clear(DSCQueue* queue, bool should_free_data);

//==============================================================================
// Information functions
//==============================================================================

/**
 * Get the number of elements in the queue.
 *
 * @param queue The queue to query
 * @return Number of elements, or 0 if queue is NULL
 */
DSC_API size_t dsc_queue_size(const DSCQueue* queue);

/**
 * Check if the queue is empty.
 *
 * @param queue The queue to check
 * @return 1 if queue is empty or NULL, 0 if it contains elements
 */
DSC_API int dsc_queue_is_empty(const DSCQueue* queue);

/**
 * Compare two queues for equality using the given comparison function.
 *
 * @param queue1 First queue to compare
 * @param queue2 Second queue to compare
 * @param compare Function to compare elements
 * @return 1 if queues are equal, 0 if not equal, -1 on error
 */
DSC_API int dsc_queue_equals(const DSCQueue* queue1, const DSCQueue* queue2, cmp_func compare);

//==============================================================================
// Element access functions
//==============================================================================

/**
 * Get the front element without removing it.
 *
 * @param queue The queue to access
 * @return Pointer to front element data, or NULL if empty or on error
 */
DSC_API void* dsc_queue_front(const DSCQueue* queue);

/**
 * Get the back element without removing it.
 *
 * @param queue The queue to access
 * @return Pointer to back element data, or NULL if empty or on error
 */
DSC_API void* dsc_queue_back(const DSCQueue* queue);

//==============================================================================
// Queue operations
//==============================================================================

/**
 * Add an element to the back of the queue.
 *
 * @param queue The queue to modify
 * @param data Pointer to the data to enqueue (ownership transferred to queue)
 * @return 0 on success, -1 on error
 */
DSC_API int dsc_queue_enqueue(DSCQueue* queue, void* data);

/**
 * Remove the front element from the queue.
 *
 * @param queue The queue to modify
 * @param should_free_data Whether to free the dequeued data
 * @return 0 on success, -1 on error (e.g., empty queue)
 */
DSC_API int dsc_queue_dequeue(DSCQueue* queue, bool should_free_data);

/**
 * Remove the front element and return its data pointer.
 * The caller is responsible for freeing the returned data if needed.
 *
 * @param queue The queue to modify
 * @return Pointer to the dequeued data, or NULL if queue is empty or on error
 */
DSC_API void* dsc_queue_dequeue_data(DSCQueue* queue);

//==============================================================================
// Higher-order functions
//==============================================================================

/**
 * Apply an action function to each element in the queue (from front to back).
 *
 * @param queue The queue to process
 * @param action Function applied to each element
 */
DSC_API void dsc_queue_for_each(const DSCQueue* queue, action_func action);

//==============================================================================
// Queue copying functions
//==============================================================================

/**
 * Create a shallow copy of the queue (sharing data pointers).
 *
 * @param queue The queue to copy
 * @return A new queue with same structure but sharing data, or NULL on error
 */
DSC_API DSCQueue* dsc_queue_copy(const DSCQueue* queue);

/**
 * Create a deep copy of the queue (cloning data using the provided function).
 *
 * @param queue The queue to copy
 * @param should_free_data Whether the copy function should handle data freeing
 * @return A new queue with copies of all data, or NULL on error
 */
DSC_API DSCQueue* dsc_queue_copy_deep(const DSCQueue* queue, bool should_free_data);

//==============================================================================
// Iterator functions
//==============================================================================

/**
 * Create an iterator for the queue (front to back traversal).
 *
 * @param queue The queue to iterate over
 * @return An Iterator object for forward traversal
 */
DSC_API DSCIterator dsc_queue_iterator(const DSCQueue* queue);

/**
 * Create a new queue from an iterator with custom allocator.
 *
 * @param it The source iterator (must be valid)
 * @param alloc The custom allocator to use
 * @return A new queue with elements from iterator, or NULL on error
 */
DSC_API DSCQueue* dsc_queue_from_iterator(DSCIterator* it, DSCAllocator* alloc);

#endif //DSCONTAINERS_QUEUE_H