//
// Created by zack on 9/9/25.
//

#include "Stack.h"

//==============================================================================
// Helper functions
//==============================================================================

/**
 * Create a new stack node with the given data.
 */
static ANVStackNode* create_node(ANVStack* stack, void* data)
{
    if (!stack || !stack->alloc)
    {
        return NULL;
    }

    ANVStackNode* node = anv_alloc_malloc(stack->alloc, sizeof(ANVStackNode));
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
static void free_node(ANVStack* stack, ANVStackNode* node, const bool should_free_data)
{
    if (!stack || !node)
    {
        return;
    }

    if (should_free_data && node->data && stack->alloc)
    {
        anv_alloc_data_free(stack->alloc, node->data);
    }

    if (stack->alloc)
    {
        anv_alloc_free(stack->alloc, node);
    }
}

//==============================================================================
// Creation and destruction functions
//==============================================================================

ANVStack* anv_stack_create(ANVAllocator* alloc)
{
    if (!alloc)
    {
        return NULL;
    }

    ANVStack* stack = anv_alloc_malloc(alloc, sizeof(ANVStack));
    if (!stack)
    {
        return NULL;
    }

    stack->top = NULL;
    stack->size = 0;
    stack->alloc = alloc;

    return stack;
}

void anv_stack_destroy(ANVStack* stack, const bool should_free_data)
{
    if (!stack)
    {
        return;
    }

    anv_stack_clear(stack, should_free_data);

    anv_alloc_free(stack->alloc, stack);
}

void anv_stack_clear(ANVStack* stack, const bool should_free_data)
{
    if (!stack)
    {
        return;
    }

    ANVStackNode* current = stack->top;
    while (current)
    {
        ANVStackNode* next = current->next;
        free_node(stack, current, should_free_data);
        current = next;
    }

    stack->top = NULL;
    stack->size = 0;
}

//==============================================================================
// Information functions
//==============================================================================

size_t anv_stack_size(const ANVStack* stack)
{
    return stack ? stack->size : 0;
}

int anv_stack_is_empty(const ANVStack* stack)
{
    return !stack || stack->size == 0;
}

int anv_stack_equals(const ANVStack* stack1, const ANVStack* stack2, const cmp_func compare)
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

    const ANVStackNode* node1 = stack1->top;
    const ANVStackNode* node2 = stack2->top;

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

void* anv_stack_peek(const ANVStack* stack)
{
    if (!stack || !stack->top)
    {
        return NULL;
    }
    return stack->top->data;
}

void* anv_stack_top(const ANVStack* stack)
{
    return anv_stack_peek(stack);
}

//==============================================================================
// Stack operations
//==============================================================================

int anv_stack_push(ANVStack* stack, void* data)
{
    if (!stack)
    {
        return -1;
    }

    ANVStackNode* new_node = create_node(stack, data);
    if (!new_node)
    {
        return -1;
    }

    new_node->next = stack->top;
    stack->top = new_node;
    stack->size++;

    return 0;
}

int anv_stack_pop(ANVStack* stack, const bool should_free_data)
{
    if (!stack || !stack->top)
    {
        return -1;
    }

    ANVStackNode* old_top = stack->top;
    stack->top = old_top->next;
    stack->size--;

    free_node(stack, old_top, should_free_data);
    return 0;
}

void* anv_stack_pop_data(ANVStack* stack)
{
    if (!stack || !stack->top)
    {
        return NULL;
    }

    ANVStackNode* old_top = stack->top;
    void* data = old_top->data;

    stack->top = old_top->next;
    stack->size--;

    // Free the node but not the data
    free_node(stack, old_top, false);
    return data;
}

//==============================================================================
// Higher-order functions
//==============================================================================

void anv_stack_for_each(const ANVStack* stack, const action_func action)
{
    if (!stack || !action)
    {
        return;
    }

    const ANVStackNode* current = stack->top;
    while (current)
    {
        action(current->data);
        current = current->next;
    }
}

//==============================================================================
// Stack copying functions
//==============================================================================

ANVStack* anv_stack_copy(const ANVStack* stack)
{
    if (!stack)
    {
        return NULL;
    }

    ANVStack* new_stack = anv_stack_create(stack->alloc);
    if (!new_stack)
    {
        return NULL;
    }

    if (stack->size == 0)
    {
        return new_stack;
    }

    // Build a temporary array to reverse the order
    void** temp_array = anv_alloc_malloc(stack->alloc, stack->size * sizeof(void*));
    if (!temp_array)
    {
        anv_stack_destroy(new_stack, false);
        return NULL;
    }

    // Collect elements from top to bottom
    const ANVStackNode* current = stack->top;
    size_t index = 0;
    while (current && index < stack->size)
    {
        temp_array[index++] = current->data;
        current = current->next;
    }

    // Push elements in reverse order to maintain stack order
    for (size_t i = index; i > 0; i--)
    {
        if (anv_stack_push(new_stack, temp_array[i - 1]) != 0)
        {
            anv_alloc_free(stack->alloc, temp_array);
            anv_stack_destroy(new_stack, false);
            return NULL;
        }
    }

    anv_alloc_free(stack->alloc, temp_array);
    return new_stack;
}

ANVStack* anv_stack_copy_deep(const ANVStack* stack, const bool should_free_data)
{
    if (!stack || !stack->alloc)
    {
        return NULL;
    }

    ANVStack* new_stack = anv_stack_create(stack->alloc);
    if (!new_stack)
    {
        return NULL;
    }

    if (stack->size == 0)
    {
        return new_stack;
    }

    // Build a temporary array to reverse the order
    void** temp_array = anv_alloc_malloc(stack->alloc, stack->size * sizeof(void*));
    if (!temp_array)
    {
        anv_stack_destroy(new_stack, false);
        return NULL;
    }

    // Collect elements from top to bottom, making copies
    const ANVStackNode* current = stack->top;
    size_t index = 0;
    while (current && index < stack->size)
    {
        void* copied_data = anv_alloc_copy(stack->alloc, current->data);
        if (!copied_data)
        {
            // Clean up any copied data on failure
            if (should_free_data)
            {
                for (size_t j = 0; j < index; j++)
                {
                    anv_alloc_data_free(stack->alloc, temp_array[j]);
                }
            }
            anv_alloc_free(stack->alloc, temp_array);
            anv_stack_destroy(new_stack, false);
            return NULL;
        }
        temp_array[index++] = copied_data;
        current = current->next;
    }

    // Push elements in reverse order to maintain stack order
    for (size_t i = index; i > 0; i--)
    {
        if (anv_stack_push(new_stack, temp_array[i - 1]) != 0)
        {
            // Clean up on failure
            if (should_free_data)
            {
                for (size_t j = 0; j < index; j++)
                {
                    anv_alloc_data_free(stack->alloc, temp_array[j]);
                }
            }
            anv_alloc_free(stack->alloc, temp_array);
            anv_stack_destroy(new_stack, false);
            return NULL;
        }
    }

    anv_alloc_free(stack->alloc, temp_array);
    return new_stack;
}

//==============================================================================
// Iterator implementation
//==============================================================================

typedef struct StackIteratorState
{
    const ANVStack* stack;
    ANVStackNode* current;
    ANVStackNode* start;
} StackIteratorState;

static void* stack_iterator_get(const ANVIterator* it)
{
    if (!it || !it->data_state)
    {
        return NULL;
    }

    const StackIteratorState* state = it->data_state;
    return state->current ? state->current->data : NULL;
}

static int stack_iterator_has_next(const ANVIterator* it)
{
    if (!it || !it->data_state)
    {
        return 0;
    }

    const StackIteratorState* state = it->data_state;
    return state->current != NULL;
}

static int stack_iterator_next(const ANVIterator* it)
{
    if (!it || !it->data_state)
    {
        return -1;
    }

    StackIteratorState* state = it->data_state;
    if (!state->current)
    {
        return -1;
    }

    state->current = state->current->next;
    return 0;
}

static int stack_iterator_has_prev(const ANVIterator* it)
{
    // Stack iterator doesn't support backward iteration efficiently
    (void)it;
    return 0;
}

static int stack_iterator_prev(const ANVIterator* it)
{
    // Stack iterator doesn't support backward iteration efficiently
    (void)it;
    return -1;
}

static void stack_iterator_reset(const ANVIterator* it)
{
    if (!it || !it->data_state)
    {
        return;
    }

    StackIteratorState* state = it->data_state;
    state->current = state->start;
}

static int stack_iterator_is_valid(const ANVIterator* it)
{
    return it && it->data_state != NULL;
}

static void stack_iterator_destroy(ANVIterator* it)
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
            anv_alloc_free(state->stack->alloc, state);
        }
    }
    it->data_state = NULL;
}

ANVIterator anv_stack_iterator(const ANVStack* stack)
{
    ANVIterator it = {0};

    it.get = stack_iterator_get;
    it.has_next = stack_iterator_has_next;
    it.next = stack_iterator_next;
    it.has_prev = stack_iterator_has_prev;
    it.prev = stack_iterator_prev;
    it.reset = stack_iterator_reset;
    it.is_valid = stack_iterator_is_valid;
    it.destroy = stack_iterator_destroy;

    if (!stack || !stack->alloc)
    {
        return it;
    }

    StackIteratorState* state = anv_alloc_malloc(stack->alloc, sizeof(StackIteratorState));
    if (!state)
    {
        return it;
    }

    state->stack = stack;
    state->current = stack->top;
    state->start = stack->top;

    it.alloc = stack->alloc;
    it.data_state = state;

    return it;
}

ANVStack* anv_stack_from_iterator(ANVIterator* it, ANVAllocator* alloc, const bool should_copy)
{
    if (!it || !alloc)
    {
        return NULL;
    }
    if (should_copy && !alloc->copy)
    {
        return NULL; // Can't copy without copy function
    }

    if (!it->is_valid || !it->is_valid(it))
    {
        return NULL;
    }

    ANVStack* stack = anv_stack_create(alloc);
    if (!stack)
    {
        return NULL;
    }

    while (it->has_next(it))
    {
        void* element = it->get(it);

        // Skip NULL elements - they indicate iterator issues
        if (!element)
        {
            if (it->next(it) != 0)
            {
                break; // Iterator exhausted or failed
            }
            continue;
        }

        void* element_to_insert = element;
        if (should_copy)
        {
            element_to_insert = alloc->copy(element);
            if (!element_to_insert)
            {
                anv_stack_destroy(stack, true);
                return NULL;
            }
        }

        if (anv_stack_push(stack, element_to_insert) != 0)
        {
            if (should_copy && alloc->data_free)
            {
                anv_alloc_data_free(alloc, element_to_insert);
            }
            anv_stack_destroy(stack, should_copy);
            return NULL;
        }

        if (it->next(it) != 0)
        {
            break; // Iterator done or failed
        }
    }

    return stack;
}