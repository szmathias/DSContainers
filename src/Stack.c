//
// Created by zack on 9/9/25.
//

#include "Stack.h"
#include <string.h>

//==============================================================================
// Helper functions
//==============================================================================

/**
 * Create a new stack node with the given data.
 */
static DSCStackNode* create_node(DSCStack* stack, void* data)
{
    if (!stack || !stack->alloc)
    {
        return NULL;
    }

    DSCStackNode* node = dsc_alloc_malloc(stack->alloc, sizeof(DSCStackNode));
    if (!node)
    {
        return NULL;
    }

    node->data = data;
    node->next = NULL;
    return node;
}

/**
 * Free a stack node and optionally its data.
 */
static void free_node(DSCStack* stack, DSCStackNode* node, const bool should_free_data)
{
    if (!stack || !node)
    {
        return;
    }

    if (should_free_data && node->data && stack->alloc)
    {
        dsc_alloc_data_free(stack->alloc, node->data);
    }

    if (stack->alloc)
    {
        dsc_alloc_free(stack->alloc, node);
    }
}

//==============================================================================
// Creation and destruction functions
//==============================================================================

DSCStack* dsc_stack_create(DSCAlloc* alloc)
{
    if (!alloc)
    {
        return NULL;
    }

    DSCStack* stack = dsc_alloc_malloc(alloc, sizeof(DSCStack));
    if (!stack)
    {
        return NULL;
    }

    stack->top   = NULL;
    stack->size  = 0;
    stack->alloc = alloc;

    return stack;
}

void dsc_stack_destroy(DSCStack* stack, const bool should_free_data)
{
    if (!stack)
    {
        return;
    }

    dsc_stack_clear(stack, should_free_data);

    dsc_alloc_free(stack->alloc, stack);
}

void dsc_stack_clear(DSCStack* stack, const bool should_free_data)
{
    if (!stack)
    {
        return;
    }

    DSCStackNode* current = stack->top;
    while (current)
    {
        DSCStackNode* next = current->next;
        free_node(stack, current, should_free_data);
        current = next;
    }

    stack->top  = NULL;
    stack->size = 0;
}

//==============================================================================
// Information functions
//==============================================================================

size_t dsc_stack_size(const DSCStack* stack)
{
    return stack ? stack->size : 0;
}

int dsc_stack_is_empty(const DSCStack* stack)
{
    return !stack || stack->size == 0;
}

int dsc_stack_equals(const DSCStack* stack1, const DSCStack* stack2, const cmp_func compare)
{
    if (!stack1 || !stack2 || !compare)
    {
        return -1;
    }

    if (stack1 == stack2)
    {
        return 1;
    }

    if (stack1->size != stack2->size)
    {
        return 0;
    }

    const DSCStackNode* node1 = stack1->top;
    const DSCStackNode* node2 = stack2->top;

    while (node1 && node2)
    {
        if (compare(node1->data, node2->data) != 0)
        {
            return 0;
        }
        node1 = node1->next;
        node2 = node2->next;
    }

    return 1;
}

//==============================================================================
// Element access functions
//==============================================================================

void* dsc_stack_peek(const DSCStack* stack)
{
    if (!stack || !stack->top)
    {
        return NULL;
    }
    return stack->top->data;
}

void* dsc_stack_top(const DSCStack* stack)
{
    return dsc_stack_peek(stack);
}

//==============================================================================
// Stack operations
//==============================================================================

int dsc_stack_push(DSCStack* stack, void* data)
{
    if (!stack)
    {
        return -1;
    }

    DSCStackNode* new_node = create_node(stack, data);
    if (!new_node)
    {
        return -1;
    }

    new_node->next = stack->top;
    stack->top     = new_node;
    stack->size++;

    return 0;
}

int dsc_stack_pop(DSCStack* stack, const bool should_free_data)
{
    if (!stack || !stack->top)
    {
        return -1;
    }

    DSCStackNode* old_top = stack->top;
    stack->top            = old_top->next;
    stack->size--;

    free_node(stack, old_top, should_free_data);
    return 0;
}

void* dsc_stack_pop_data(DSCStack* stack)
{
    if (!stack || !stack->top)
    {
        return NULL;
    }

    DSCStackNode* old_top = stack->top;
    void* data            = old_top->data;

    stack->top = old_top->next;
    stack->size--;

    // Free the node but not the data
    free_node(stack, old_top, false);
    return data;
}

//==============================================================================
// Higher-order functions
//==============================================================================

void dsc_stack_for_each(const DSCStack* stack, const action_func action)
{
    if (!stack || !action)
    {
        return;
    }

    const DSCStackNode* current = stack->top;
    while (current)
    {
        action(current->data);
        current = current->next;
    }
}

//==============================================================================
// Stack copying functions
//==============================================================================

DSCStack* dsc_stack_copy(const DSCStack* stack)
{
    if (!stack)
    {
        return NULL;
    }

    DSCStack* new_stack = dsc_stack_create(stack->alloc);
    if (!new_stack)
    {
        return NULL;
    }

    if (stack->size == 0)
    {
        return new_stack;
    }

    // Build a temporary array to reverse the order
    void** temp_array = dsc_alloc_malloc(stack->alloc, stack->size * sizeof(void*));
    if (!temp_array)
    {
        dsc_stack_destroy(new_stack, false);
        return NULL;
    }

    // Collect elements from top to bottom
    const DSCStackNode* current = stack->top;
    size_t index                = 0;
    while (current && index < stack->size)
    {
        temp_array[index++] = current->data;
        current             = current->next;
    }

    // Push elements in reverse order to maintain stack order
    for (size_t i = index; i > 0; i--)
    {
        if (dsc_stack_push(new_stack, temp_array[i - 1]) != 0)
        {
            dsc_alloc_free(stack->alloc, temp_array);
            dsc_stack_destroy(new_stack, false);
            return NULL;
        }
    }

    dsc_alloc_free(stack->alloc, temp_array);
    return new_stack;
}

DSCStack* dsc_stack_copy_deep(const DSCStack* stack, const bool should_free_data)
{
    if (!stack || !stack->alloc)
    {
        return NULL;
    }

    DSCStack* new_stack = dsc_stack_create(stack->alloc);
    if (!new_stack)
    {
        return NULL;
    }

    if (stack->size == 0)
    {
        return new_stack;
    }

    // Build a temporary array to reverse the order
    void** temp_array = dsc_alloc_malloc(stack->alloc, stack->size * sizeof(void*));
    if (!temp_array)
    {
        dsc_stack_destroy(new_stack, false);
        return NULL;
    }

    // Collect elements from top to bottom, making copies
    const DSCStackNode* current = stack->top;
    size_t index                = 0;
    while (current && index < stack->size)
    {
        void* copied_data = dsc_alloc_copy(stack->alloc, current->data);
        if (!copied_data)
        {
            // Clean up any copied data on failure
            if (should_free_data)
            {
                for (size_t j = 0; j < index; j++)
                {
                    dsc_alloc_data_free(stack->alloc, temp_array[j]);
                }
            }
            dsc_alloc_free(stack->alloc, temp_array);
            dsc_stack_destroy(new_stack, false);
            return NULL;
        }
        temp_array[index++] = copied_data;
        current             = current->next;
    }

    // Push elements in reverse order to maintain stack order
    for (size_t i = index; i > 0; i--)
    {
        if (dsc_stack_push(new_stack, temp_array[i - 1]) != 0)
        {
            // Clean up on failure
            if (should_free_data)
            {
                for (size_t j = 0; j < index; j++)
                {
                    dsc_alloc_data_free(stack->alloc, temp_array[j]);
                }
            }
            dsc_alloc_free(stack->alloc, temp_array);
            dsc_stack_destroy(new_stack, false);
            return NULL;
        }
    }

    dsc_alloc_free(stack->alloc, temp_array);
    return new_stack;
}

//==============================================================================
// Iterator implementation
//==============================================================================

typedef struct StackIteratorState
{
    const DSCStack* stack;
    DSCStackNode* current;
    DSCStackNode* start;
} StackIteratorState;

static void* stack_iterator_get(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return NULL;
    }

    const StackIteratorState* state = it->data_state;
    return state->current ? state->current->data : NULL;
}

static int stack_iterator_has_next(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return 0;
    }

    const StackIteratorState* state = it->data_state;
    return state->current != NULL;
}

static void* stack_iterator_next(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return NULL;
    }

    StackIteratorState* state = it->data_state;
    if (!state->current)
    {
        return NULL;
    }

    void* data     = state->current->data;
    state->current = state->current->next;
    return data;
}

static int stack_iterator_has_prev(const DSCIterator* it)
{
    // Stack iterator doesn't support backward iteration efficiently
    (void)it;
    return 0;
}

static void* stack_iterator_prev(const DSCIterator* it)
{
    // Stack iterator doesn't support backward iteration efficiently
    (void)it;
    return NULL;
}

static void stack_iterator_reset(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return;
    }

    StackIteratorState* state = it->data_state;
    state->current            = state->start;
}

static int stack_iterator_is_valid(const DSCIterator* it)
{
    return it && it->data_state != NULL;
}

static void stack_iterator_destroy(DSCIterator* it)
{
    if (!it)
    {
        return;
    }

    if (it->data_state)
    {
        StackIteratorState* state = it->data_state;
        if (state->stack && state->stack->alloc)
        {
            dsc_alloc_free(state->stack->alloc, state);
        }
    }
}

DSCIterator dsc_stack_iterator(const DSCStack* stack)
{
    DSCIterator it = {0};

    it.get        = stack_iterator_get;
    it.has_next   = stack_iterator_has_next;
    it.next       = stack_iterator_next;
    it.has_prev   = stack_iterator_has_prev;
    it.prev       = stack_iterator_prev;
    it.reset      = stack_iterator_reset;
    it.is_valid   = stack_iterator_is_valid;
    it.destroy    = stack_iterator_destroy;

    if (!stack || !stack->alloc)
    {
        return it;
    }

    StackIteratorState* state = dsc_alloc_malloc(stack->alloc, sizeof(StackIteratorState));
    if (!state)
    {
        return it;
    }

    state->stack   = stack;
    state->current = stack->top;
    state->start   = stack->top;

    it.alloc = stack->alloc;
    it.data_state = state;

    return it;
}

DSCStack* dsc_stack_from_iterator(DSCIterator* it, DSCAlloc* alloc)
{
    if (!it || !alloc || !it->is_valid(it))
    {
        return NULL;
    }

    DSCStack* stack = dsc_stack_create(alloc);
    if (!stack)
    {
        return NULL;
    }

    // Push elements directly as they come from the iterator
    // This will result in the last element being on top
    while (it->has_next(it))
    {
        void* element = it->next(it);
        if (dsc_stack_push(stack, element) != 0)
        {
            dsc_stack_destroy(stack, false);
            return NULL;
        }
    }

    return stack;
}
