//
// Created by zack on 9/9/25.
//

#include "Queue.h"
#include <stdlib.h>
#include <string.h>

//==============================================================================
// Helper functions
//==============================================================================

/**
 * Create a new queue node with the given data.
 */
static DSCQueueNode* create_node(DSCQueue* queue, void* data)
{
    if (!queue || !queue->alloc || !queue->alloc->alloc_func)
    {
        return NULL;
    }

    DSCQueueNode* node = queue->alloc->alloc_func(sizeof(DSCQueueNode));
    if (!node)
    {
        return NULL;
    }

    node->data = data;
    node->next = NULL;
    return node;
}

/**
 * Free a queue node and optionally its data.
 */
static void free_node(DSCQueue* queue, DSCQueueNode* node, const bool should_free_data)
{
    if (!queue || !node)
    {
        return;
    }

    if (should_free_data && node->data && queue->alloc && queue->alloc->data_free_func)
    {
        queue->alloc->data_free_func(node->data);
    }

    if (queue->alloc && queue->alloc->dealloc_func)
    {
        queue->alloc->dealloc_func(node);
    }
}

//==============================================================================
// Creation and destruction functions
//==============================================================================

DSCQueue* dsc_queue_create(DSCAlloc* alloc)
{
    if (!alloc || !alloc->alloc_func || !alloc->dealloc_func)
    {
        return NULL;
    }

    DSCQueue* queue = alloc->alloc_func(sizeof(DSCQueue));
    if (!queue)
    {
        return NULL;
    }

    queue->front = NULL;
    queue->back  = NULL;
    queue->size  = 0;
    queue->alloc = alloc;

    return queue;
}

void dsc_queue_destroy(DSCQueue* queue, const bool should_free_data)
{
    if (!queue)
    {
        return;
    }

    dsc_queue_clear(queue, should_free_data);

    if (queue->alloc && queue->alloc->dealloc_func)
    {
        queue->alloc->dealloc_func(queue);
    }
}

void dsc_queue_clear(DSCQueue* queue, const bool should_free_data)
{
    if (!queue)
    {
        return;
    }

    DSCQueueNode* current = queue->front;
    while (current)
    {
        DSCQueueNode* next = current->next;
        free_node(queue, current, should_free_data);
        current = next;
    }

    queue->front = NULL;
    queue->back  = NULL;
    queue->size  = 0;
}

//==============================================================================
// Information functions
//==============================================================================

size_t dsc_queue_size(const DSCQueue* queue)
{
    return queue ? queue->size : 0;
}

int dsc_queue_is_empty(const DSCQueue* queue)
{
    return !queue || queue->size == 0;
}

int dsc_queue_equals(const DSCQueue* queue1, const DSCQueue* queue2, const cmp_func compare)
{
    if (!queue1 || !queue2 || !compare)
    {
        return -1;
    }

    if (queue1 == queue2)
    {
        return 1;
    }

    if (queue1->size != queue2->size)
    {
        return 0;
    }

    const DSCQueueNode* node1 = queue1->front;
    const DSCQueueNode* node2 = queue2->front;

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

void* dsc_queue_front(const DSCQueue* queue)
{
    if (!queue || !queue->front)
    {
        return NULL;
    }
    return queue->front->data;
}

void* dsc_queue_back(const DSCQueue* queue)
{
    if (!queue || !queue->back)
    {
        return NULL;
    }
    return queue->back->data;
}

//==============================================================================
// Queue operations
//==============================================================================

int dsc_queue_enqueue(DSCQueue* queue, void* data)
{
    if (!queue)
    {
        return -1;
    }

    DSCQueueNode* new_node = create_node(queue, data);
    if (!new_node)
    {
        return -1;
    }

    // If queue is empty, new node becomes both front and back
    if (queue->size == 0)
    {
        queue->front = new_node;
        queue->back  = new_node;
    }
    else
    {
        // Add to back of queue
        queue->back->next = new_node;
        queue->back       = new_node;
    }

    queue->size++;
    return 0;
}

int dsc_queue_dequeue(DSCQueue* queue, const bool should_free_data)
{
    if (!queue || !queue->front)
    {
        return -1;
    }

    DSCQueueNode* old_front = queue->front;
    queue->front            = old_front->next;

    // If queue becomes empty, update back pointer
    if (!queue->front)
    {
        queue->back = NULL;
    }

    queue->size--;
    free_node(queue, old_front, should_free_data);
    return 0;
}

void* dsc_queue_dequeue_data(DSCQueue* queue)
{
    if (!queue || !queue->front)
    {
        return NULL;
    }

    DSCQueueNode* old_front = queue->front;
    void* data              = old_front->data;

    queue->front = old_front->next;

    // If queue becomes empty, update back pointer
    if (!queue->front)
    {
        queue->back = NULL;
    }

    queue->size--;

    // Free the node but not the data
    free_node(queue, old_front, false);
    return data;
}

//==============================================================================
// Higher-order functions
//==============================================================================

void dsc_queue_for_each(const DSCQueue* queue, const action_func action)
{
    if (!queue || !action)
    {
        return;
    }

    const DSCQueueNode* current = queue->front;
    while (current)
    {
        action(current->data);
        current = current->next;
    }
}

//==============================================================================
// Queue copying functions
//==============================================================================

DSCQueue* dsc_queue_copy(const DSCQueue* queue)
{
    if (!queue)
    {
        return NULL;
    }

    DSCQueue* new_queue = dsc_queue_create(queue->alloc);
    if (!new_queue)
    {
        return NULL;
    }

    if (queue->size == 0)
    {
        return new_queue;
    }

    // Copy elements from front to back to maintain order
    const DSCQueueNode* current = queue->front;
    while (current)
    {
        if (dsc_queue_enqueue(new_queue, current->data) != 0)
        {
            dsc_queue_destroy(new_queue, false);
            return NULL;
        }
        current = current->next;
    }

    return new_queue;
}

DSCQueue* dsc_queue_copy_deep(const DSCQueue* queue, const bool should_free_data)
{
    if (!queue || !queue->alloc || !queue->alloc->copy_func)
    {
        return NULL;
    }

    DSCQueue* new_queue = dsc_queue_create(queue->alloc);
    if (!new_queue)
    {
        return NULL;
    }

    if (queue->size == 0)
    {
        return new_queue;
    }

    // Copy elements from front to back, making deep copies
    const DSCQueueNode* current = queue->front;
    while (current)
    {
        void* copied_data = queue->alloc->copy_func(current->data);
        if (!copied_data)
        {
            dsc_queue_destroy(new_queue, should_free_data);
            return NULL;
        }

        if (dsc_queue_enqueue(new_queue, copied_data) != 0)
        {
            if (should_free_data && queue->alloc->data_free_func)
            {
                queue->alloc->data_free_func(copied_data);
            }
            dsc_queue_destroy(new_queue, should_free_data);
            return NULL;
        }
        current = current->next;
    }

    return new_queue;
}

//==============================================================================
// Iterator implementation
//==============================================================================

typedef struct QueueIteratorState
{
    const DSCQueue* queue;
    DSCQueueNode* current;
    DSCQueueNode* start;
} QueueIteratorState;

static void* queue_iterator_get(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return NULL;
    }

    const QueueIteratorState* state = it->data_state;
    return state->current ? state->current->data : NULL;
}

static int queue_iterator_has_next(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return 0;
    }

    const QueueIteratorState* state = it->data_state;
    return state->current != NULL;
}

static void* queue_iterator_next(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return NULL;
    }

    QueueIteratorState* state = it->data_state;
    if (!state->current)
    {
        return NULL;
    }

    void* data     = state->current->data;
    state->current = state->current->next;
    return data;
}

static int queue_iterator_has_prev(const DSCIterator* it)
{
    // Queue iterator doesn't support backward iteration efficiently
    (void)it;
    return 0;
}

static void* queue_iterator_prev(const DSCIterator* it)
{
    // Queue iterator doesn't support backward iteration efficiently
    (void)it;
    return NULL;
}

static void queue_iterator_reset(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return;
    }

    QueueIteratorState* state = it->data_state;
    state->current            = state->start;
}

static int queue_iterator_is_valid(const DSCIterator* it)
{
    return it && it->data_state != NULL;
}

static void queue_iterator_destroy(DSCIterator* it)
{
    if (!it)
    {
        return;
    }

    if (it->data_state)
    {
        QueueIteratorState* state = it->data_state;
        if (state->queue && state->queue->alloc && state->queue->alloc->dealloc_func)
        {
            state->queue->alloc->dealloc_func(state);
        }
    }
}

DSCIterator dsc_queue_iterator(const DSCQueue* queue)
{
    DSCIterator it = {0};

    it.data_state = NULL;
    it.get        = queue_iterator_get;
    it.has_next   = queue_iterator_has_next;
    it.next       = queue_iterator_next;
    it.has_prev   = queue_iterator_has_prev;
    it.prev       = queue_iterator_prev;
    it.reset      = queue_iterator_reset;
    it.is_valid   = queue_iterator_is_valid;
    it.destroy    = queue_iterator_destroy;

    if (!queue || !queue->alloc || !queue->alloc->alloc_func)
    {
        return it;
    }

    QueueIteratorState* state = queue->alloc->alloc_func(sizeof(QueueIteratorState));
    if (!state)
    {
        return it;
    }

    it.data_state = state;

    state->queue   = queue;
    state->current = queue->front;
    state->start   = queue->front;

    return it;
}

DSCQueue* dsc_queue_from_iterator(DSCIterator* it, DSCAlloc* alloc)
{
    if (!it || !alloc || !it->is_valid(it))
    {
        return NULL;
    }

    DSCQueue* queue = dsc_queue_create(alloc);
    if (!queue)
    {
        return NULL;
    }

    // Add elements from iterator in order (no reversal needed like in stack)
    while (it->has_next(it))
    {
        void* data = it->next(it);
        if (dsc_queue_enqueue(queue, data) != 0)
        {
            dsc_queue_destroy(queue, false);
            return NULL;
        }
    }

    return queue;
}
