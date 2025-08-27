//
// Created by zack on 8/26/25.
//
// Implementation of iterator operations, including transform and filter iterators.

#include "Iterator.h"

#include <stdlib.h>

//==============================================================================
// Iterator implementation state structures
//==============================================================================

/**
 * State structure for transform iterator.
 */
typedef struct TransformState
{
    Iterator *base_iterator;      // Source iterator
    transform_func transform;     // Transformation function
} TransformState;

/**
 * State structure for filter iterator.
 */
typedef struct FilterState
{
    Iterator *base_iterator;      // Source iterator
    filter_func filter;           // Predicate function
    void *next_element;           // Next matching element (cached)
    int has_cached_element;       // Flag indicating cache validity
} FilterState;

/**
 * State structure for range iterator (not implemented).
 */
// typedef struct RangeState
// {
//     int current;               // Current value
//     int end;                   // End value (exclusive)
//     int step;                  // Increment value
// } RangeState;

//==============================================================================
// Transform iterator implementation
//==============================================================================

/**
 * Get current element from transform iterator.
 */
static void *transform_get(const Iterator *it)
{
    if (!it || !it->data_state)
    {
        return nullptr;
    }

    const TransformState *state = it->data_state;
    if (!state->base_iterator || !state->base_iterator->get)
    {
        return nullptr;
    }

    void* element = state->base_iterator->get(state->base_iterator);
    if (!element)
    {
        return nullptr;
    }

    // Apply transformation
    void* transformed = state->transform(element);

    // If the element came from another transform's get, free it
    // This is critical to avoid leaks in nested transform chains
    if (state->base_iterator->get == transform_get)
    {
        free(element);
    }

    return transformed;
}

/**
 * Check if transform iterator has more elements.
 */
static int transform_has_next(const Iterator *it)
{
    if (!it || !it->data_state)
    {
        return 0;
    }

    const TransformState *state = it->data_state;
    if (!state->base_iterator || !state->base_iterator->has_next)
    {
        return 0;
    }

    return state->base_iterator->has_next(state->base_iterator);
}

/**
 * Get next element from transform iterator and advance.
 */
static void *transform_next(const Iterator *it)
{
    if (!it || !it->data_state)
    {
        return nullptr;
    }

    const TransformState *state = it->data_state;
    if (!state->base_iterator || !state->base_iterator->next || !state->transform)
    {
        return nullptr;
    }

    if (!state->base_iterator->has_next(state->base_iterator))
    {
        return nullptr;
    }

    void *element = state->base_iterator->next(state->base_iterator);
    if (!element)
    {
        return nullptr;
    }

    // Apply transformation and mark ownership
    void* transformed = state->transform(element);

    // Always free elements coming from another transform
    if (state->base_iterator->next == transform_next)
    {
        free(element);
    }

    return transformed;
}

/**
 * Check if transform iterator has previous elements (not supported).
 */
static int transform_has_prev(const Iterator *it)
{
    (void)it;  // Suppress unused parameter warning
    return 0;  // Transform iterator does not support has_prev
}

/**
 * Get previous element from transform iterator (not supported).
 */
static void *transform_prev(const Iterator *it)
{
    (void)it;  // Suppress unused parameter warning
    return nullptr;  // Transform iterator does not support prev
}

/**
 * Reset transform iterator (not supported).
 */
static void transform_reset(const Iterator *it)
{
    (void)it;  // Suppress unused parameter warning
    // Transform iterator does not support reset
}

/**
 * Check if transform iterator is valid.
 */
static int transform_is_valid(const Iterator *it)
{
    if (!it || !it->data_state)
    {
        return 0;
    }

    const TransformState *state = it->data_state;
    if (!state->base_iterator || !state->base_iterator->is_valid)
    {
        return 0;
    }

    return state->base_iterator->is_valid(state->base_iterator);
}

/**
 * Free resources used by transform iterator.
 */
static void transform_destroy(Iterator *it)
{
    if (!it || !it->data_state)
    {
        return;
    }

    TransformState *state = it->data_state;

    if (state->base_iterator && state->base_iterator->destroy)
    {
        state->base_iterator->destroy(state->base_iterator);
    }

    free(state);
    it->data_state = nullptr;
}

/**
 * Create a transforming iterator that applies a function to each element.
 */
Iterator iterator_transform(Iterator *it, const transform_func transform)
{
    Iterator new_it = {};  // Initialize all fields to nullptr/0

    new_it.get = transform_get;
    new_it.has_next = transform_has_next;
    new_it.next = transform_next;
    new_it.has_prev = transform_has_prev;
    new_it.prev = transform_prev;
    new_it.reset = transform_reset;
    new_it.is_valid = transform_is_valid;
    new_it.destroy = transform_destroy;

    if (!it || !transform)
    {
        return new_it;
    }

    TransformState *state = calloc(1, sizeof(TransformState));
    if (!state)
    {
        return new_it;
    }

    state->base_iterator = it;
    state->transform = transform;
    new_it.data_state = state;

    return new_it;
}

//==============================================================================
// Filter iterator implementation
//==============================================================================

/**
 * Get current element from filter iterator.
 */
static void *filter_get(const Iterator *it)
{
    if (!it || !it->data_state)
    {
        return nullptr;
    }

    FilterState *state = it->data_state;
    if (!state->base_iterator)
    {
        return nullptr;
    }

    // Return cached element if available
    if (state->has_cached_element)
    {
        return state->next_element;
    }

    void *element = state->base_iterator->get(state->base_iterator);
    state->next_element = element;
    state->has_cached_element = 1;
    return state->next_element;
}

/**
 * Check if filter iterator has more elements.
 */
static int filter_has_next(const Iterator *it)
{
    if (!it || !it->data_state)
    {
        return 0;
    }

    FilterState *state = it->data_state;
    if (!state->base_iterator || !state->base_iterator->has_next)
    {
        return 0;
    }

    if (state->has_cached_element)
    {
        return 1;
    }

    const Iterator *base_it = state->base_iterator;

    while (base_it->has_next(base_it))
    {
        void *element = base_it->get(base_it);
        if (!element)
        {
            base_it->next(base_it);
            continue;
        }

        if (state->filter && state->filter(element))
        {
            state->next_element = element;
            state->has_cached_element = 1;
            return 1;
        }

        const int from_transform = (base_it->next == transform_next);
        if (from_transform)
        {
            free(element);
            void *next_elem = base_it->next(base_it);
            if (next_elem)
            {
                free(next_elem);
            }
        }
        else
        {
            base_it->next(base_it);
        }
    }

    return 0;
}

/**
 * Get next element from filter iterator and advance.
 */
static void *filter_next(const Iterator *it)
{
    if (!it || !it->data_state)
    {
        return nullptr;
    }

    FilterState *state = it->data_state;
    if (!state->base_iterator || !state->base_iterator->next)
    {
        return nullptr;
    }

    // Return cached element if we have one
    if (state->has_cached_element)
    {
        state->has_cached_element = 0;
        void *result = state->next_element;
        state->next_element = nullptr;

        if (state->base_iterator->next == transform_next)
        {
            void *next_elem = state->base_iterator->next(state->base_iterator);
            if (next_elem)
            {
                free(next_elem);
            }
        }
        else
        {
            state->base_iterator->next(state->base_iterator);
        }

        return result;
    }

    return nullptr;
}

/**
 * Check if filter iterator has previous elements (not supported).
 */
static int filter_has_prev(const Iterator *it)
{
    (void)it;  // Suppress unused parameter warning
    return 0;  // Filter iterator does not support has_prev
}

/**
 * Get previous element from filter iterator (not supported).
 */
static void *filter_prev(const Iterator *it)
{
    (void)it;  // Suppress unused parameter warning
    return nullptr;  // Filter iterator does not support prev
}

/**
 * Reset filter iterator (not supported).
 */
static void filter_reset(const Iterator *it)
{
    (void)it;  // Suppress unused parameter warning
    // Filter iterator does not support reset
}

/**
 * Check if filter iterator is valid.
 */
static int filter_is_valid(const Iterator *it)
{
    if (!it || !it->data_state)
    {
        return 0;
    }

    const FilterState *state = it->data_state;
    if (!state->base_iterator || !state->base_iterator->is_valid)
    {
        return 0;
    }

    return state->base_iterator->is_valid(state->base_iterator);
}

/**
 * Free resources used by filter iterator.
 */
static void filter_destroy(Iterator *it)
{
    if (!it || !it->data_state)
    {
        return;
    }

    FilterState *state = it->data_state;

    // Free any cached element from transforms
    if (state->has_cached_element)
    {
        if (state->base_iterator &&
            (state->base_iterator->next == transform_next ||
             state->base_iterator->get == transform_get))
        {
            free(state->next_element);
        }
        state->next_element = nullptr;
        state->has_cached_element = 0;
    }

    if (state->base_iterator && state->base_iterator->destroy)
    {
        state->base_iterator->destroy(state->base_iterator);
    }

    free(state);
    it->data_state = nullptr;
}

/**
 * Create a filtering iterator that only yields elements matching a predicate.
 */
Iterator iterator_filter(Iterator *it, const filter_func filter)
{
    Iterator new_it = {};  // Initialize all fields to nullptr/0

    new_it.get = filter_get;
    new_it.has_next = filter_has_next;
    new_it.next = filter_next;
    new_it.has_prev = filter_has_prev;
    new_it.prev = filter_prev;
    new_it.reset = filter_reset;
    new_it.is_valid = filter_is_valid;
    new_it.destroy = filter_destroy;

    if (!it || !filter)
    {
        return new_it;
    }

    FilterState *state = calloc(1, sizeof(FilterState));
    if (!state)
    {
        return new_it;
    }

    state->base_iterator = it;
    state->filter = filter;
    state->next_element = nullptr;
    state->has_cached_element = 0;

    new_it.data_state = state;

    return new_it;
}

//==============================================================================
// Range iterator implementation (placeholder)
//==============================================================================

/**
 * Create an iterator that yields integers in a specified range.
 * Currently not implemented.
 */
// Iterator iterator_range(int start, int end, int step)
// {
//     // Implementation of iterator_range
//     Iterator it = {};  // Initialize all fields to nullptr/0
//     return it;
// }

