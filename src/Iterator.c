//
// Created by zack on 8/26/25.
//
// Implementation of iterator operations, including transform and filter iterators.

#include "Iterator.h"

#include <stdio.h>
#include <stdlib.h>

//==============================================================================
// Iterator implementation state structures
//==============================================================================

/**
 * State structure for transform iterator.
 */
typedef struct TransformState
{
    DSCIterator* base_iterator; // Source iterator
    transform_func transform;   // Transformation function
    int has_owner;              // Flag indicating ownership of base element
} TransformState;

/**
 * State structure for filter iterator.
 */
typedef struct FilterState
{
    DSCIterator* base_iterator; // Source iterator
    filter_func filter;         // Predicate function
    void* next_element;         // Next matching element (cached)
    int has_cached_element;     // Flag indicating cache validity
    int has_owner;              // Flag indicating ownership of cached element
} FilterState;

/**
 * State structure for range iterator.
 */
typedef struct RangeState
{
    int start;   // Starting value (stored for reset/has_prev)
    int current; // Current value
    int end;     // End value (exclusive)
    int step;    // Increment value
    int is_forward;
} RangeState;

//==============================================================================
// Forward declarations of iterator implementation functions
//==============================================================================

// Transform iterator functions
static void* transform_get(const DSCIterator* it);
static int transform_has_next(const DSCIterator* it);
static void* transform_next(const DSCIterator* it);
static int transform_has_prev(const DSCIterator* it);
static void* transform_prev(const DSCIterator* it);
static void transform_reset(const DSCIterator* it);
static int transform_is_valid(const DSCIterator* it);
static void transform_destroy(DSCIterator * it);

// Filter iterator functions
static void* filter_get(const DSCIterator* it);
static int filter_has_next(const DSCIterator* it);
static void* filter_next(const DSCIterator* it);
static int filter_has_prev(const DSCIterator* it);
static void* filter_prev(const DSCIterator* it);
static void filter_reset(const DSCIterator* it);
static int filter_is_valid(const DSCIterator* it);
static void filter_destroy(DSCIterator * it);

// Range iterator functions
static void* range_get(const DSCIterator* it);
static int range_has_next(const DSCIterator* it);
static void* range_next(const DSCIterator* it);
static int range_has_prev(const DSCIterator* it);
static void* range_prev(const DSCIterator* it);
static void range_reset(const DSCIterator* it);
static int range_is_valid(const DSCIterator* it);
static void range_destroy(DSCIterator * it);

//==============================================================================
// Transform iterator implementation
//==============================================================================

/**
 * Get current element from transform iterator.
 */
static void* transform_get(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return NULL;
    }

    const TransformState* state = it->data_state;
    if (!state->base_iterator || !state->base_iterator->get)
    {
        return NULL;
    }

    void* element = state->base_iterator->get(state->base_iterator);
    if (!element)
    {
        return NULL;
    }

    // Apply transformation
    void* transformed = state->transform(element);

    // If the element came from another transform's get, free it
    // This is critical to avoid leaks in nested transform chains
    if (state->base_iterator->get == transform_get || state->base_iterator->get == range_get)
    {
        free(element);
        element = NULL;
    }

    if (state->base_iterator->get == filter_get)
    {
        const FilterState* next_it = state->base_iterator->data_state;
        if (!next_it->has_owner && element)
        {
            free(element);
        }
    }

    return transformed;
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
 * Get next element from transform iterator and advance.
 */
static void* transform_next(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return NULL;
    }

    const TransformState* state = it->data_state;
    const DSCIterator* base_it  = state->base_iterator;
    if (!base_it || !base_it->next || !state->transform)
    {
        return NULL;
    }

    if (!base_it->has_next(base_it))
    {
        return NULL;
    }

    void* element = base_it->next(base_it);
    if (!element)
    {
        return NULL;
    }

    // Apply transformation and mark ownership
    void* transformed = state->transform(element);

    // Always free elements coming from another transform
    if (base_it->next == transform_next || base_it->next == range_next || !state->has_owner)
    {
        free(element);
        element = NULL;
    }

    if (base_it->get == filter_get)
    {
        const FilterState* next_it = base_it->data_state;
        if (!next_it->has_owner && element)
        {
            free(element);
        }
    }

    return transformed;
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
static void* transform_prev(const DSCIterator* it)
{
    (void)it;    // Suppress unused parameter warning
    return NULL; // Transform iterator does not support prev
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

    if (state->base_iterator && state->base_iterator->destroy)
    {
        state->base_iterator->destroy(state->base_iterator);
    }

    free(state);
    it->data_state = NULL;
}

/**
 * Create a transforming iterator that applies a function to each element.
 */
DSCIterator dsc_iterator_transform(DSCIterator* it, const transform_func transform)
{
    DSCIterator new_it = {0}; // Initialize all fields to NULL/0

    new_it.get      = transform_get;
    new_it.has_next = transform_has_next;
    new_it.next     = transform_next;
    new_it.has_prev = transform_has_prev;
    new_it.prev     = transform_prev;
    new_it.reset    = transform_reset;
    new_it.is_valid = transform_is_valid;
    new_it.destroy  = transform_destroy;

    if (!it || !transform)
    {
        return new_it;
    }

    TransformState* state = calloc(1, sizeof(TransformState));
    if (!state)
    {
        return new_it;
    }

    int has_owner = 1;
    if (it->get == transform_get)
    {
        const TransformState* next_it = it->data_state;
        has_owner &= next_it->has_owner;
    }

    if (it->get == filter_get)
    {
        const FilterState* next_it = it->data_state;
        has_owner &= next_it->has_owner;
    }

    state->base_iterator = it;
    state->transform     = transform;
    state->has_owner     = (it->get != range_get && has_owner);
    new_it.data_state    = state;

    return new_it;
}

//==============================================================================
// Filter iterator implementation
//==============================================================================

/**
 * Get current element from filter iterator.
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

    // Return cached element if available
    if (state->has_cached_element)
    {
        return state->next_element;
    }

    void* element             = state->base_iterator->get(state->base_iterator);
    state->next_element       = element;
    state->has_cached_element = 1;
    return state->next_element;
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
    if (!state->base_iterator || !state->base_iterator->has_next)
    {
        return 0;
    }

    if (state->has_cached_element)
    {
        return 1;
    }

    const DSCIterator* base_it = state->base_iterator;

    while (base_it->has_next(base_it))
    {
        int* element = base_it->get(base_it);
        if (!element)
        {
            // Advance the iterator without getting the value
            // This avoids the memory leak in range and transform iterators
            base_it->next(base_it);
            continue;
        }

        if (state->filter && state->filter(element))
        {
            // Found a matching element, cache it
            state->next_element       = element;
            state->has_cached_element = 1;
            return 1;
        }

        // Element doesn't match filter, free it if it came from transform or range
        if (base_it->get == transform_get || base_it->get == range_get)
        {
            free(element);
            int* next_elem = base_it->next(base_it);
            if (next_elem)
            {
                free(next_elem);
            }
        }
        else
        {
            if (base_it->next == filter_next)
            {
                const FilterState* next_it = base_it->data_state;
                if (!next_it->has_owner)
                {
                    free(base_it->next(base_it));
                }
                else
                {
                    base_it->next(base_it);
                }
            }
            else
            {
                base_it->next(base_it);
            }
        }
    }

    return 0;
}

/**
 * Get next element from filter iterator and advance.
 */
static void* filter_next(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return NULL;
    }

    FilterState* state         = it->data_state;
    const DSCIterator* base_it = state->base_iterator;
    if (!base_it || !base_it->next)
    {
        return NULL;
    }

    // Return cached element if we have one
    if (state->has_cached_element)
    {
        state->has_cached_element = 0;
        int* result               = state->next_element;
        state->next_element       = NULL;

        if (base_it->next == transform_next || base_it->next == range_next)
        {
            void* next_elem = base_it->next(base_it);
            if (next_elem)
            {
                free(next_elem);
            }
            // if (base_it->next == transform_next)
            // {
            //     const TransformState *next_it = base_it->data_state;
            //     if (!next_it->has_owner)
            //     {
            //         free(result);
            //         result = NULL;
            //     }
            //
            // }
        }
        else
        {
            base_it->next(base_it);
        }

        return result;
    }

    return NULL;
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
static void* filter_prev(const DSCIterator* it)
{
    (void)it;    // Suppress unused parameter warning
    return NULL; // Filter iterator does not support prev
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

    // Free any cached element from transforms
    if (state->has_cached_element)
    {
        if (state->base_iterator &&
            (state->base_iterator->next == transform_next ||
             state->base_iterator->get == transform_get))
        {
            free(state->next_element);
        }
        state->next_element       = NULL;
        state->has_cached_element = 0;
    }

    if (state->base_iterator && state->base_iterator->destroy)
    {
        state->base_iterator->destroy(state->base_iterator);
    }

    free(state);
    it->data_state = NULL;
}

/**
 * Create a filtering iterator that only yields elements matching a predicate.
 */
DSCIterator dsc_iterator_filter(DSCIterator* it, const filter_func filter)
{
    DSCIterator new_it = {0}; // Initialize all fields to NULL/0

    new_it.get      = filter_get;
    new_it.has_next = filter_has_next;
    new_it.next     = filter_next;
    new_it.has_prev = filter_has_prev;
    new_it.prev     = filter_prev;
    new_it.reset    = filter_reset;
    new_it.is_valid = filter_is_valid;
    new_it.destroy  = filter_destroy;

    if (!it || !filter)
    {
        return new_it;
    }

    FilterState* state = calloc(1, sizeof(FilterState));
    if (!state)
    {
        return new_it;
    }

    int has_owner = 1;
    if (it->get == transform_get)
    {
        const TransformState* next_it = it->data_state;
        has_owner &= next_it->has_owner;
    }

    if (it->get == filter_get)
    {
        const FilterState* next_it = it->data_state;
        has_owner &= next_it->has_owner;
    }

    state->base_iterator      = it;
    state->filter             = filter;
    state->next_element       = NULL;
    state->has_cached_element = 0;
    state->has_owner          = (it->get != range_get && has_owner);

    new_it.data_state = state;

    return new_it;
}

//==============================================================================
// Range iterator implementation
//==============================================================================

/**
 * Get current element from range iterator.
 */
static void* range_get(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return NULL;
    }

    const RangeState* state = it->data_state;

    // For ongoing iteration, return the current value
    if ((state->step > 0 && state->current < state->end) ||
        (state->step < 0 && state->current > state->end))
    {
        int* value = malloc(sizeof(int));
        if (!value)
        {
            return NULL;
        }
        *value = state->current;
        return value;
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
    else if (state->step < 0)
    {
        return state->current > state->end; // For negative step
    }

    return 0; // Zero step, no more iterations
}

/**
 * Get next element from range iterator and advance.
 */
static void* range_next(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return NULL;
    }

    RangeState* state = it->data_state;

    if (!it->has_next(it))
    {
        return NULL;
    }

    int* value = malloc(sizeof(int));
    if (!value)
    {
        return NULL;
    }

    if (state->is_forward)
    {
        *value = state->current;
        state->current += state->step;
    }
    else
    {
        state->is_forward = 1;
        state->current += state->step;
        if (!it->has_next(it))
        {
            free(value);
            return NULL;
        }
        state->current += state->step;
        *value = state->current;
        state->current += state->step;
    }

    return value;
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

    // For all cases, check if next step would be within bounds
    if (state->step > 0)
    {
        return state->current >= state->start; // For positive step
    }
    else if (state->step < 0)
    {
        return state->current <= state->start; // For negative step
    }

    return 0; // Zero step, no more iterations
}

/**
 * Get previous element from range iterator and move back.
 */
static void* range_prev(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return NULL;
    }

    if (!it->has_prev(it))
    {
        return NULL;
    }

    RangeState* state = it->data_state;

    int* value = malloc(sizeof(int));
    if (!value)
    {
        return NULL;
    }

    if (!state->is_forward)
    {
        *value = state->current;
        state->current -= state->step;
    }
    else
    {
        state->is_forward = 0;
        state->current -= state->step;
        if (!it->has_prev(it))
        {
            free(value);
            return NULL;
        }
        state->current -= state->step;
        *value = state->current;
        state->current -= state->step;
    }


    return value;
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
    state->current    = state->start;
    state->is_forward = 1;
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

    free(it->data_state);
    it->data_state = NULL;
}

/**
 * Create an iterator that yields integers in a specified range.
 *
 * @param start Starting value (inclusive)
 * @param end Ending value (exclusive)
 * @param step Step value (positive or negative)
 * @return A new iterator yielding integers in the specified range
 */
DSCIterator dsc_iterator_range(const int start, const int end, const int step)
{
    DSCIterator it = {0}; // Initialize all fields to NULL/0

    it.get      = range_get;
    it.has_next = range_has_next;
    it.next     = range_next;
    it.has_prev = range_has_prev;
    it.prev     = range_prev;
    it.reset    = range_reset;
    it.is_valid = range_is_valid;
    it.destroy  = range_destroy;

    // Handle invalid step
    if (step == 0 ||
        (start < end && step < 0) ||
        (start > end && step > 0))
    {
        return it; // Return invalid iterator with NULL data_state
    }
    RangeState* state = calloc(1, sizeof(RangeState));
    if (!state)
    {
        return it;
    }

    state->start      = start; // Store the initial start value
    state->current    = start;
    state->end        = end;
    state->step       = step;
    state->is_forward = 1;


    it.data_state = state;

    return it;
}