//
// Created by zack on 8/26/25.
//
// Implementation of iterator operations, including transform and filter iterators.

#include "Iterator.h"

/**
 * State structure for transform iterator.
 */
typedef struct TransformState
{
    DSCIterator* base_iterator; // Source iterator
    transform_func transform;   // Transformation function
    void* cached_result;        // Cached transformed result
    int transform_allocates;    // Flag: does transform function allocate memory?
} TransformState;

// Forward declaration for helper functions
static void* transform_get(const DSCIterator* it);

//==============================================================================
// Transform Helper function for memory management
//==============================================================================

/**
 * Helper function to free elements returned by base iterators.
 * Only frees elements that were allocated by transform iterators.
 */
// static void free_if_allocated(const DSCIterator* base_it, void* element)
// {
//     if (!element) return;
//
//     if (base_it->get == transform_get)
//     {
//         const TransformState* state = base_it->data_state;
//         if (state && state->transform_allocates)
//         {
//             dsc_alloc_free(base_it->alloc, element);
//         }
//     }
// }

//==============================================================================
// Transform iterator implementation
//==============================================================================

/**
 * Get current element from transform iterator.
 *
 * Note: The returned pointer is valid until the next iterator operation.
 * Do not free the returned pointer.
 */
static void* transform_get(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return NULL;
    }

    TransformState* state = it->data_state;
    if (!state->base_iterator || !state->base_iterator->get || !state->transform)
    {
        return NULL;
    }

    // Only transform if we don't have a cached result yet
    if (!state->cached_result)
    {
        void* element = state->base_iterator->get(state->base_iterator);
        if (!element)
        {
            return NULL;
        }

        // Apply transformation and cache the result
        state->cached_result = state->transform(element);

        // // Free the base element if it was allocated
        // free_if_allocated(state->base_iterator, element);
    }

    return state->cached_result;
}

/**
 * Check if transform iterator has more elements.
 */
static int transform_has_next(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return 0;
    }

    const TransformState* state = it->data_state;
    if (!state->base_iterator || !state->base_iterator->has_next)
    {
        return 0;
    }

    return state->base_iterator->has_next(state->base_iterator);
}

/**
 * Advance transform iterator to next position.
 */
static int transform_next(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return -1;
    }

    TransformState* state = it->data_state;
    if (!state->base_iterator || !state->base_iterator->next || !state->transform)
    {
        return -1;
    }

    void* element = state->base_iterator->get(state->base_iterator);
    if (!element)
    {
        return -1;
    }

    // Free any previously cached result if it was allocated
    if (state->cached_result && state->transform_allocates)
    {
        dsc_alloc_data_free(it->alloc, state->cached_result);
    }

    // Always clear cached result so next get() will recompute it
    state->cached_result = NULL;

    return state->base_iterator->next(state->base_iterator);
}

/**
 * Check if transform iterator has previous elements (not supported).
 */
static int transform_has_prev(const DSCIterator* it)
{
    (void)it; // Suppress unused parameter warning
    return 0; // Transform iterator does not support has_prev
}

/**
 * Get previous element from transform iterator (not supported).
 */
static int transform_prev(const DSCIterator* it)
{
    (void)it; // Suppress unused parameter warning
    return 0; // Transform iterator does not support prev
}

/**
 * Reset transform iterator (not supported).
 */
static void transform_reset(const DSCIterator* it)
{
    (void)it; // Suppress unused parameter warning
    // Transform iterator does not support reset
}

/**
 * Check if transform iterator is valid.
 */
static int transform_is_valid(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return 0;
    }

    const TransformState* state = it->data_state;
    if (!state->base_iterator || !state->base_iterator->is_valid)
    {
        return 0;
    }

    return state->base_iterator->is_valid(state->base_iterator);
}

/**
 * Free resources used by transform iterator.
 */
static void transform_destroy(DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return;
    }

    TransformState* state = it->data_state;

    // Free cached result if it exists
    if (state->cached_result && state->transform_allocates)
    {
        dsc_alloc_free(it->alloc, state->cached_result);
    }

    // Destroy base iterator
    if (state->base_iterator && state->base_iterator->destroy)
    {
        state->base_iterator->destroy(state->base_iterator);
    }

    dsc_alloc_free(it->alloc, state);
    it->data_state = NULL;
}

/**
 * Create a transforming iterator that applies a function to each element.
 */
DSCIterator dsc_iterator_transform(DSCIterator* it, const DSCAllocator* alloc, const transform_func transform, const int transform_allocates)
{
    DSCIterator new_it = {0}; // Initialize all fields to NULL/0

    new_it.get = transform_get;
    new_it.has_next = transform_has_next;
    new_it.next = transform_next;
    new_it.has_prev = transform_has_prev;
    new_it.prev = transform_prev;
    new_it.reset = transform_reset;
    new_it.is_valid = transform_is_valid;
    new_it.destroy = transform_destroy;

    if (!it || !transform || !alloc)
    {
        return new_it;
    }

    TransformState* state = dsc_alloc_malloc(alloc, sizeof(TransformState));
    if (!state)
    {
        return new_it;
    }

    state->base_iterator = it;
    state->transform = transform;
    state->cached_result = NULL;
    state->transform_allocates = transform_allocates;

    new_it.alloc = alloc;
    new_it.data_state = state;

    return new_it;
}

//==============================================================================
// Filter iterator implementation
//==============================================================================

/**
 * State structure for filter iterator.
 */
typedef struct FilterState
{
    DSCIterator* base_iterator; // Source iterator
    filter_func filter;         // Predicate function
    void* current_element;      // Cached current element
    int has_current;            // Flag indicating if we have a cached current element
    int current_matches;        // Flag indicating if current element matches filter
} FilterState;

/**
 * Helper function to position filter at next matching element.
 * Uses only get() and next() - no invalidation issues.
 * This is the key function that solves the composition problem.
 */
static void position_at_next_match(FilterState* state)
{
    // If we already have a matching current element, we're done
    if (state->has_current && state->current_matches)
    {
        return;
    }

    const DSCIterator* base_it = state->base_iterator;

    // Search for the next matching element
    while (base_it->has_next(base_it))
    {
        // Peek at the current element without advancing
        void* element = base_it->get(base_it);

        if (element && state->filter && state->filter(element))
        {
            // Found a match - cache it and stop
            state->current_element = element;
            state->has_current = 1;
            state->current_matches = 1;
            return;
        }

        // Element doesn't match, advance to next position
        if (base_it->next(base_it) != 0)
        {
            break; // No more elements
        }
    }

    // No matching element found
    state->has_current = 0;
    state->current_matches = 0;
    state->current_element = NULL;
}

/**
 * Get current element from filter iterator.
 *
 * Note: The returned pointer is valid until the next iterator operation.
 * Do not free the returned pointer.
 */
static void* filter_get(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return NULL;
    }

    FilterState* state = it->data_state;
    if (!state->base_iterator)
    {
        return NULL;
    }

    position_at_next_match(state);

    return state->current_matches ? state->current_element : NULL;
}

/**
 * Check if filter iterator has more elements.
 */
static int filter_has_next(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return 0;
    }

    FilterState* state = it->data_state;
    if (!state->base_iterator)
    {
        return 0;
    }

    // Position at next matching element if needed
    position_at_next_match(state);

    return state->current_matches;
}

/**
 * Advance filter iterator to next position.
 * Returns 0 on success, -1 if no more elements.
 */
static int filter_next(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return -1;
    }

    FilterState* state = it->data_state;
    const DSCIterator* base_it = state->base_iterator;
    if (!base_it)
    {
        return -1;
    }

    // Make sure we're positioned at a matching element
    if (!it->has_next(it))
    {
        return -1;
    }

    if (base_it->next(base_it) != 0)
    {
        return -1;
    }

    // No more elements in base iterator
    state->has_current = 0;
    state->current_matches = 0;
    state->current_element = NULL;

    return 0;
}

/**
 * Check if filter iterator has previous elements (not supported).
 */
static int filter_has_prev(const DSCIterator* it)
{
    (void)it; // Suppress unused parameter warning
    return 0; // Filter iterator does not support has_prev
}

/**
 * Get previous element from filter iterator (not supported).
 */
static int filter_prev(const DSCIterator* it)
{
    (void)it; // Suppress unused parameter warning
    return 0; // Filter iterator does not support prev
}

/**
 * Reset filter iterator (not supported).
 */
static void filter_reset(const DSCIterator* it)
{
    (void)it; // Suppress unused parameter warning
    // Filter iterator does not support reset
}

/**
 * Check if filter iterator is valid.
 */
static int filter_is_valid(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return 0;
    }

    const FilterState* state = it->data_state;
    if (!state->base_iterator || !state->base_iterator->is_valid)
    {
        return 0;
    }

    return state->base_iterator->is_valid(state->base_iterator);
}

/**
 * Free resources used by filter iterator.
 */
static void filter_destroy(DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return;
    }

    FilterState* state = it->data_state;

    // No need to free cached_element as it points to base iterator's memory
    // Base iterator cleanup will handle it

    // Destroy base iterator
    if (state->base_iterator && state->base_iterator->destroy)
    {
        state->base_iterator->destroy(state->base_iterator);
    }

    dsc_alloc_free(it->alloc, state);
    it->data_state = NULL;
}

/**
 * Create a filtering iterator that only yields elements matching a predicate.
 */
DSCIterator dsc_iterator_filter(DSCIterator* it, const DSCAllocator* alloc, const filter_func filter)
{
    DSCIterator new_it = {0}; // Initialize all fields to NULL/0

    new_it.get = filter_get;
    new_it.has_next = filter_has_next;
    new_it.next = filter_next;
    new_it.has_prev = filter_has_prev;
    new_it.prev = filter_prev;
    new_it.reset = filter_reset;
    new_it.is_valid = filter_is_valid;
    new_it.destroy = filter_destroy;

    if (!it || !filter || !alloc)
    {
        return new_it;
    }

    FilterState* state = dsc_alloc_malloc(alloc, sizeof(FilterState));
    if (!state)
    {
        return new_it;
    }

    state->base_iterator = it;
    state->filter = filter;
    state->current_element = NULL;
    state->has_current = 0;

    new_it.alloc = alloc;
    new_it.data_state = state;

    return new_it;
}

//==============================================================================
// Range iterator implementation
//==============================================================================

/**
 * State structure for range iterator.
 */
typedef struct RangeState
{
    int start;        // Starting value (stored for reset/has_prev)
    int current;      // Current value
    int end;          // End value (exclusive)
    int step;         // Increment value
    int cached_value; // Cached value to return pointers to
} RangeState;

/**
 * Get current element from range iterator.
 *
 * Note: The returned pointer is valid until the next iterator operation.
 * Do not free the returned pointer.
 */
static void* range_get(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return NULL;
    }

    RangeState* state = it->data_state;

    // For ongoing iteration, return the current value
    if ((state->step > 0 && state->current < state->end) ||
        (state->step < 0 && state->current > state->end))
    {
        state->cached_value = state->current;
        return &state->cached_value;
    }

    return NULL;
}

/**
 * Check if range iterator has more elements.
 */
static int range_has_next(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return 0;
    }

    const RangeState* state = it->data_state;

    // For all cases, check if next step would be within bounds
    if (state->step > 0)
    {
        return state->current < state->end; // For positive step
    }

    if (state->step < 0)
    {
        return state->current > state->end; // For negative step
    }

    return 0; // Zero step, no more iterations
}

/**
 * Advance range iterator to next position.
 * Returns 0 on success, -1 if no more elements.
 */
static int range_next(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return -1;
    }

    RangeState* state = it->data_state;

    if (!range_has_next(it))
    {
        return -1;
    }

    state->current += state->step;
    return 0;
}

/**
 * Check if range iterator has previous elements.
 */
static int range_has_prev(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return 0;
    }

    const RangeState* state = it->data_state;

    // For all cases, check if previous step would be within bounds
    if (state->step > 0)
    {
        return state->current > state->start; // For positive step
    }

    if (state->step < 0)
    {
        return state->current < state->start; // For negative step
    }

    return 0; // Zero step, no more iterations
}

/**
 * Get previous element from range iterator and move back.
 *
 * Note: The returned pointer is valid until the next iterator operation.
 * Do not free the returned pointer.
 */
static int range_prev(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return -1;
    }

    RangeState* state = it->data_state;

    if (!range_has_prev(it))
    {
        return -1;
    }

    state->current -= state->step;
    return 0;
}

/**
 * Reset range iterator to starting position.
 */
static void range_reset(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return;
    }

    RangeState* state = it->data_state;

    // Reset to initial state using the stored start value
    state->current = state->start;
    // Note: cached_value doesn't need to be reset as it will be overwritten
}

/**
 * Check if range iterator is valid.
 */
static int range_is_valid(const DSCIterator* it)
{
    return it && it->data_state != NULL;
}

/**
 * Free resources used by range iterator.
 */
static void range_destroy(DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return;
    }

    dsc_alloc_free(it->alloc, it->data_state);
    it->data_state = NULL;
}

/**
 * Create an iterator that yields integers in a specified range.
 *
 * @param start Starting value (inclusive)
 * @param end Ending value (exclusive)
 * @param step Step value (positive or negative, non-zero)
 * @param alloc The allocator to use for the new iterator's state.
 * @return A new iterator yielding integers in the specified range
 *
 * Note: Pointers returned by get() and next() are valid until the next
 * iterator operation. Do not free these pointers - they point to iterator-owned memory.
 */
DSCIterator dsc_iterator_range(const int start, const int end, const int step, const DSCAllocator* alloc)
{
    DSCIterator it = {0}; // Initialize all fields to NULL/0

    it.get = range_get;
    it.has_next = range_has_next;
    it.next = range_next;
    it.has_prev = range_has_prev;
    it.prev = range_prev;
    it.reset = range_reset;
    it.is_valid = range_is_valid;
    it.destroy = range_destroy;

    // Handle invalid step
    if (step == 0 || !alloc ||
        (start < end && step < 0) ||
        (start > end && step > 0))
    {
        return it; // Return invalid iterator with NULL data_state
    }

    RangeState* state = dsc_alloc_malloc(alloc, sizeof(RangeState));
    if (!state)
    {
        return it;
    }

    state->start = start;
    state->current = start;
    state->end = end;
    state->step = step;
    state->cached_value = start; // Initialize cached value

    it.alloc = alloc;
    it.data_state = state;

    return it;
}

//==============================================================================
// Copy iterator implementation
//==============================================================================

/**
 * State structure for copy iterator.
 */
typedef struct CopyState
{
    DSCIterator* base_iterator; // Source iterator
    copy_func copy;             // Copy function
    void* cached_copy;          // Cached copied element (user owns this)
} CopyState;

/**
 * Get current element from copy iterator.
 * Returns a deep copy that the USER OWNS and must free.
 */
static void* copy_get(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return NULL;
    }

    CopyState* state = it->data_state;
    if (!state->base_iterator || !state->copy)
    {
        return NULL;
    }

    // Only create copy if we don't have a cached one yet
    if (!state->cached_copy)
    {
        void* element = state->base_iterator->get(state->base_iterator);
        if (!element)
        {
            return NULL;
        }

        // Create copy using the provided copy function
        state->cached_copy = state->copy(element);
    }

    return state->cached_copy;
}

/**
 * Check if copy iterator has more elements.
 */
static int copy_has_next(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return 0;
    }

    const CopyState* state = it->data_state;
    if (!state->base_iterator || !state->base_iterator->has_next)
    {
        return 0;
    }

    return state->base_iterator->has_next(state->base_iterator);
}

/**
 * Advance copy iterator to next position.
 * Returns 0 on success, -1 if no more elements.
 */
static int copy_next(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return -1;
    }

    CopyState* state = it->data_state;
    if (!state->base_iterator)
    {
        return -1;
    }

    // Clear cached copy so next get() will create a new one
    // Note: We don't free cached_copy here because the user owns it
    state->cached_copy = NULL;

    return state->base_iterator->next(state->base_iterator);
}

/**
 * Check if copy iterator has previous elements (not supported).
 */
static int copy_has_prev(const DSCIterator* it)
{
    (void)it; // Suppress unused parameter warning
    return 0; // Copy iterator does not support has_prev
}

/**
 * Get previous element from copy iterator (not supported).
 */
static int copy_prev(const DSCIterator* it)
{
    (void)it;  // Suppress unused parameter warning
    return -1; // Copy iterator does not support prev
}

/**
 * Reset copy iterator (not supported).
 */
static void copy_reset(const DSCIterator* it)
{
    (void)it; // Suppress unused parameter warning
    // Copy iterator does not support reset
}

/**
 * Check if copy iterator is valid.
 */
static int copy_is_valid(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return 0;
    }

    const CopyState* state = it->data_state;
    if (!state->base_iterator || !state->base_iterator->is_valid || !state->copy)
    {
        return 0;
    }

    return state->base_iterator->is_valid(state->base_iterator);
}

/**
 * Free resources used by copy iterator.
 * Note: Does NOT free cached_copy as that is owned by the user.
 */
static void copy_destroy(DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return;
    }

    CopyState* state = it->data_state;

    // Do NOT free cached_copy - it's owned by the user

    // Destroy base iterator
    if (state->base_iterator && state->base_iterator->destroy)
    {
        state->base_iterator->destroy(state->base_iterator);
    }

    dsc_alloc_free(it->alloc, state);
    it->data_state = NULL;
}

/**
 * Create a copy iterator that returns deep copies of elements.
 */
DSCIterator dsc_iterator_copy(DSCIterator* it, const DSCAllocator* alloc, const copy_func copy)
{
    DSCIterator new_it = {0}; // Initialize all fields to NULL/0

    new_it.get = copy_get;
    new_it.has_next = copy_has_next;
    new_it.next = copy_next;
    new_it.has_prev = copy_has_prev;
    new_it.prev = copy_prev;
    new_it.reset = copy_reset;
    new_it.is_valid = copy_is_valid;
    new_it.destroy = copy_destroy;

    if (!it || !copy || !alloc)
    {
        return new_it;
    }

    CopyState* state = dsc_alloc_malloc(alloc, sizeof(CopyState));
    if (!state)
    {
        return new_it;
    }

    state->base_iterator = it;
    state->copy = copy;
    state->cached_copy = NULL;

    new_it.alloc = alloc;
    new_it.data_state = state;

    return new_it;
}