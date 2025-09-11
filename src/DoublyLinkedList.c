//
// Created by zack on 8/25/25.
//
// Implementation of doubly linked list functions.

#include "DoublyLinkedList.h"

#include <stdlib.h>

//==============================================================================
// Static helper functions (private to this file)
//==============================================================================

/**
 * Helper function to split the list into two halves (for merge sort).
 *
 * @param head The head of the list to split
 * @return Pointer to the head of the second half
 */
static DSCDoublyLinkedNode* dsc_dll_split(DSCDoublyLinkedNode* head)
{
    if (!head || !head->next)
    {
        return NULL;
    }

    const DSCDoublyLinkedNode* fast = head;
    DSCDoublyLinkedNode* slow       = head;
    DSCDoublyLinkedNode* prev       = NULL;

    while (fast && fast->next)
    {
        fast = fast->next->next;
        prev = slow;
        slow = slow->next;
    }

    if (prev)
    {
        prev->next = NULL;
    }
    slow->prev = NULL;

    return slow;
}

/**
 * Helper function to merge two sorted lists (for merge sort).
 *
 * @param left The head of the first sorted list
 * @param right The head of the second sorted list
 * @param compare Comparison function
 * @return Pointer to the head of the merged sorted list
 */
static DSCDoublyLinkedNode* dsc_dll_sort_helper_merge(DSCDoublyLinkedNode* left, DSCDoublyLinkedNode* right, const cmp_func compare)
{
    if (!left)
    {
        return right;
    }

    if (!right)
    {
        return left;
    }

    DSCDoublyLinkedNode* result;

    // Initialize result pointer
    if (compare(left->data, right->data) <= 0)
    {
        result = left;
        left   = left->next;
    }
    else
    {
        result = right;
        right  = right->next;
    }

    result->prev                 = NULL;
    DSCDoublyLinkedNode* current = result;

    // Merge the two lists
    while (left && right)
    {
        if (compare(left->data, right->data) <= 0)
        {
            current->next = left;
            left->prev    = current;
            left          = left->next;
        }
        else
        {
            current->next = right;
            right->prev   = current;
            right         = right->next;
        }
        current = current->next;
    }

    // Attach the remaining list
    if (left)
    {
        current->next = left;
        left->prev    = current;
    }
    else if (right)
    {
        current->next = right;
        right->prev   = current;
    }
    else
    {
        current->next = NULL;
    }

    return result;
}

/**
 * Helper function for merge sort implementation.
 *
 * @param head The head of the list to sort
 * @param compare Comparison function
 * @return Pointer to the head of the sorted list
 */
static DSCDoublyLinkedNode* dsc_dll_merge_sort(DSCDoublyLinkedNode* head, const cmp_func compare)
{
    if (!head || !head->next)
    {
        return head;
    }

    // Split the list into two halves
    DSCDoublyLinkedNode* right = dsc_dll_split(head);

    // Recursively sort both halves
    DSCDoublyLinkedNode* left_sorted  = dsc_dll_merge_sort(head, compare);
    DSCDoublyLinkedNode* right_sorted = dsc_dll_merge_sort(right, compare);

    // Merge the sorted halves
    return dsc_dll_sort_helper_merge(left_sorted, right_sorted, compare);
}

//==============================================================================
// Iterator state and helper functions
//==============================================================================

typedef struct ListIteratorState
{
    DSCDoublyLinkedNode* current;    // Current node
    DSCDoublyLinkedNode* start;      // Starting position (head or tail)
    const DSCDoublyLinkedList* list; // The list being iterated (const for safety)
} ListIteratorState;

/**
 * Check if iterator has more elements.
 */
static int dsc_dll_iterator_has_next(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return 0;
    }

    const ListIteratorState* state = it->data_state;
    return state->current != NULL;
}

/**
 * Get next element and advance iterator.
 */
static void* dsc_dll_iterator_next(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return NULL;
    }

    ListIteratorState* state = it->data_state;
    if (!state->current)
    {
        return NULL;
    }

    void* data = state->current->data;
    if (state->start == state->list->head)
    {
        state->current = state->current->next;
    }
    else
    {
        state->current = state->current->prev;
    }

    return data;
}

/**
 * Check if iterator has previous elements.
 */
static int dsc_dll_iterator_has_prev(const DSCIterator* it)
{
    if (!it || !it->data_state)
        return 0;

    const ListIteratorState* state = it->data_state;
    if (!state->current)
        return 0;

    // Check if there's a node in the opposite direction of iteration
    if (state->start == state->list->head)
    {
        // For forward iterator, check if we can go backwards
        return state->current->prev != NULL;
    }
    // For reverse iterator, check if we can go forwards
    return state->current->next != NULL;
}

/**
 * Get previous element and move iterator backwards.
 */
static void* dsc_dll_iterator_prev(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return NULL;
    }

    ListIteratorState* state = it->data_state;
    if (!state->current)
    {
        return NULL;
    }

    void* data = state->current->data;
    if (state->start == state->list->head)
    {
        state->current = state->current->prev;
    }
    else
    {
        state->current = state->current->next;
    }

    return data;
}

/**
 * Reset iterator to starting position.
 */
static void dsc_dll_iterator_reset(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return;
    }

    ListIteratorState* state = it->data_state;
    state->current           = state->start;
}

/**
 * Check if iterator is valid.
 */
static int dsc_dll_iterator_is_valid(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return 0;
    }

    const ListIteratorState* state = it->data_state;
    return state->list != NULL;
}

/**
 * Free resources used by iterator.
 */
static void dsc_dll_iterator_destroy(DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return;
    }

    const ListIteratorState* state = it->data_state;
    if (state && state->list)
    {
        dsc_alloc_free(state->list->alloc, it->data_state);
    }
    it->data_state = NULL;
}

/**
 * Get current element without advancing iterator.
 */
static void* dsc_dll_iterator_get(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return NULL;
    }

    const ListIteratorState* state = it->data_state;
    if (!state->current)
    {
        return NULL;
    }

    return state->current->data;
}

//==============================================================================
// Creation and destruction functions
//==============================================================================

DSCDoublyLinkedList* dsc_dll_create(DSCAllocator* alloc)
{
    if (!alloc)
    {
        return NULL;
    }

    DSCDoublyLinkedList* list = dsc_alloc_malloc(alloc, sizeof(DSCDoublyLinkedList));
    if (!list)
    {
        return NULL;
    }

    list->head  = NULL;
    list->tail  = NULL;
    list->size  = 0;
    list->alloc = alloc;

    return list;
}

void dsc_dll_destroy(DSCDoublyLinkedList* list, const bool should_free_data)
{
    if (list)
    {
        dsc_dll_clear(list, should_free_data);
        dsc_alloc_free(list->alloc, list);
    }
}

void dsc_dll_clear(DSCDoublyLinkedList* list, const bool should_free_data)
{
    if (!list)
    {
        return;
    }

    DSCDoublyLinkedNode* node = list->head;
    while (node)
    {
        DSCDoublyLinkedNode* next = node->next;
        if (should_free_data && node->data)
        {
            dsc_alloc_data_free(list->alloc, node->data);
        }

        dsc_alloc_free(list->alloc, node);
        node = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

//==============================================================================
// Information functions
//==============================================================================

size_t dsc_dll_size(const DSCDoublyLinkedList* list)
{
    if (!list)
    {
        return 0;
    }

    return list->size;
}

int dsc_dll_is_empty(const DSCDoublyLinkedList* list)
{
    return !list || list->size == 0;
}

DSCDoublyLinkedNode* dsc_dll_find(DSCDoublyLinkedList* list, const void* data, const cmp_func compare)
{
    if (!list || !compare)
    {
        return NULL;
    }

    DSCDoublyLinkedNode* curr = list->head;
    while (curr)
    {
        if (compare(curr->data, data) == 0)
        {
            return curr;
        }

        curr = curr->next;
    }

    return NULL;
}

int dsc_dll_equals(const DSCDoublyLinkedList* list1, const DSCDoublyLinkedList* list2, const cmp_func compare)
{
    if (!list1 || !list2 || !compare)
    {
        return -1;
    }

    // Lists of different sizes cannot be equal
    if (list1->size != list2->size)
    {
        return 0;
    }

    // Empty lists are equal
    if (list1->size == 0)
    {
        return 1;
    }

    // Compare each node
    const DSCDoublyLinkedNode* node1 = list1->head;
    const DSCDoublyLinkedNode* node2 = list2->head;

    while (node1 && node2)
    {
        if (compare(node1->data, node2->data) != 0)
        {
            return 0; // Data not equal
        }

        node1 = node1->next;
        node2 = node2->next;
    }

    return 1; // Lists are equal
}

//==============================================================================
// Insertion functions
//==============================================================================

int dsc_dll_insert_front(DSCDoublyLinkedList* list, void* data)
{
    if (!list)
    {
        return -1;
    }

    DSCDoublyLinkedNode* node = dsc_alloc_malloc(list->alloc, sizeof(DSCDoublyLinkedNode));
    if (!node)
    {
        return -1;
    }

    node->data = data;
    node->prev = NULL;
    node->next = list->head;

    if (list->head)
    {
        list->head->prev = node;
    }
    else
    {
        // Empty list, set tail
        list->tail = node;
    }

    list->head = node;
    list->size++;

    return 0;
}

int dsc_dll_insert_back(DSCDoublyLinkedList* list, void* data)
{
    if (!list)
    {
        return -1;
    }

    DSCDoublyLinkedNode* node = dsc_alloc_malloc(list->alloc, sizeof(DSCDoublyLinkedNode));
    if (!node)
    {
        return -1;
    }

    node->data = data;
    node->next = NULL;
    node->prev = list->tail;

    if (list->tail)
    {
        list->tail->next = node;
    }
    else
    {
        // Empty list, set head
        list->head = node;
    }

    list->tail = node;
    list->size++;

    return 0;
}

int dsc_dll_insert_at(DSCDoublyLinkedList* list, const size_t pos, void* data)
{
    if (!list || pos > list->size)
    {
        return -1;
    }

    if (pos == 0)
    {
        return dsc_dll_insert_front(list, data);
    }

    if (pos == list->size)
    {
        return dsc_dll_insert_back(list, data);
    }

    DSCDoublyLinkedNode* node = dsc_alloc_malloc(list->alloc, sizeof(DSCDoublyLinkedNode));
    if (!node)
    {
        return -1;
    }

    node->data = data;

    // Determine if it's more efficient to start from head or tail
    if (pos <= list->size / 2)
    {
        // Start from head and move forward
        DSCDoublyLinkedNode* curr = list->head;
        for (size_t i = 0; i < pos - 1; ++i)
        {
            curr = curr->next;
        }

        node->next       = curr->next;
        node->prev       = curr;
        curr->next->prev = node;
        curr->next       = node;
    }
    else
    {
        // Start from tail and move backward
        DSCDoublyLinkedNode* curr = list->tail;
        for (size_t i = list->size - 1; i > pos; --i)
        {
            curr = curr->prev;
        }

        node->prev       = curr->prev;
        node->next       = curr;
        curr->prev->next = node;
        curr->prev       = node;
    }

    list->size++;

    return 0;
}

//==============================================================================
// Removal functions
//==============================================================================

int dsc_dll_remove(DSCDoublyLinkedList* list, const void* data, const cmp_func compare, const bool should_free_data)
{
    if (!list || !compare || list->size == 0)
    {
        return -1;
    }

    DSCDoublyLinkedNode* curr = list->head;
    while (curr)
    {
        if (compare(curr->data, data) == 0)
        {
            // Found the node to remove
            if (curr->prev)
            {
                curr->prev->next = curr->next;
            }
            else
            {
                // Removing head node
                list->head = curr->next;
            }

            if (curr->next)
            {
                curr->next->prev = curr->prev;
            }
            else
            {
                // Removing tail node
                list->tail = curr->prev;
            }

            if (should_free_data && curr->data)
            {
                dsc_alloc_data_free(list->alloc, curr->data);
            }
            dsc_alloc_free(list->alloc, curr);
            list->size--;

            return 0;
        }
        curr = curr->next;
    }

    return -1; // Node not found
}

int dsc_dll_remove_at(DSCDoublyLinkedList* list, const size_t pos, const bool should_free_data)
{
    if (!list || pos >= list->size)
    {
        return -1;
    }

    DSCDoublyLinkedNode* node_to_remove;

    if (pos == 0)
    {
        // Remove head
        node_to_remove = list->head;
        list->head     = node_to_remove->next;

        if (list->head)
        {
            list->head->prev = NULL;
        }
        else
        {
            // Removed the only node
            list->tail = NULL;
        }
    }
    else if (pos == list->size - 1)
    {
        // Remove tail
        node_to_remove   = list->tail;
        list->tail       = node_to_remove->prev;
        list->tail->next = NULL;
    }
    else if (pos <= list->size / 2)
    {
        // Find node from head (more efficient for first half)
        node_to_remove = list->head;
        for (size_t i = 0; i < pos; ++i)
        {
            node_to_remove = node_to_remove->next;
        }
        node_to_remove->prev->next = node_to_remove->next;
        node_to_remove->next->prev = node_to_remove->prev;
    }
    else
    {
        // Find node from tail (more efficient for second half)
        node_to_remove = list->tail;
        for (size_t i = list->size - 1; i > pos; --i)
        {
            node_to_remove = node_to_remove->prev;
        }
        node_to_remove->prev->next = node_to_remove->next;
        node_to_remove->next->prev = node_to_remove->prev;
    }

    if (should_free_data && node_to_remove->data)
    {
        dsc_alloc_data_free(list->alloc, node_to_remove->data);
    }
    dsc_alloc_free(list->alloc, node_to_remove);
    list->size--;

    return 0;
}

int dsc_dll_remove_front(DSCDoublyLinkedList* list, const bool should_free_data)
{
    if (!list || !list->head)
    {
        return -1;
    }

    DSCDoublyLinkedNode* node_to_remove = list->head;
    list->head                          = node_to_remove->next;

    if (list->head)
    {
        list->head->prev = NULL;
    }
    else
    {
        // Removed the only node
        list->tail = NULL;
    }

    if (should_free_data && node_to_remove->data)
    {
        dsc_alloc_data_free(list->alloc, node_to_remove->data);
    }

    dsc_alloc_free(list->alloc, node_to_remove);
    list->size--;

    return 0;
}

int dsc_dll_remove_back(DSCDoublyLinkedList* list, const bool should_free_data)
{
    if (!list || !list->tail)
    {
        return -1;
    }

    DSCDoublyLinkedNode* node_to_remove = list->tail;
    list->tail                          = node_to_remove->prev;

    if (list->tail)
    {
        list->tail->next = NULL;
    }
    else
    {
        // Removed the only node
        list->head = NULL;
    }

    if (should_free_data && node_to_remove->data)
    {
        dsc_alloc_data_free(list->alloc, node_to_remove->data);
    }

    // Free the node memory using the allocator's dealloc function
    dsc_alloc_free(list->alloc, node_to_remove);
    list->size--;
    return 0;
}

//==============================================================================
// List manipulation functions
//==============================================================================

int dsc_dll_sort(DSCDoublyLinkedList* list, const cmp_func compare)
{
    if (!list || !compare || list->size <= 1)
    {
        return !list || !compare ? -1 : 0;
    }

    // Perform the merge sort
    list->head = dsc_dll_merge_sort(list->head, compare);

    // Update the tail pointer
    DSCDoublyLinkedNode* current = list->head;
    while (current && current->next)
    {
        current = current->next;
    }
    list->tail = current;

    return 0;
}

int dsc_dll_reverse(DSCDoublyLinkedList* list)
{
    if (!list)
    {
        return -1;
    }

    if (list->size <= 1)
    {
        return 0; // Empty or single-element list is already reversed
    }

    DSCDoublyLinkedNode* current = list->head;
    DSCDoublyLinkedNode* temp;

    // Swap next and prev pointers for all nodes
    while (current)
    {
        // Swap next and prev pointers
        temp          = current->prev;
        current->prev = current->next;
        current->next = temp;

        // Move to the next node (which is now current->prev because we swapped)
        current = current->prev;
    }

    // Swap head and tail pointers
    temp       = list->head;
    list->head = list->tail;
    list->tail = temp;

    return 0;
}

int dsc_dll_merge(DSCDoublyLinkedList* dest, DSCDoublyLinkedList* src)
{
    if (!dest || !src)
    {
        return -1;
    }

    // If src is empty, nothing to merge
    if (src->size == 0)
    {
        return 0;
    }

    // If dest is empty, make it equal to src
    if (dest->size == 0)
    {
        dest->head = src->head;
        dest->tail = src->tail;
        dest->size = src->size;
    }
    else
    {
        // Connect dest's tail to src's head
        dest->tail->next = src->head;
        src->head->prev  = dest->tail;

        // Update dest's tail
        dest->tail = src->tail;

        // Update size
        dest->size += src->size;
    }

    // Clear src list without destroying nodes
    src->head = NULL;
    src->tail = NULL;
    src->size = 0;

    return 0;
}

int dsc_dll_splice(DSCDoublyLinkedList* dest, DSCDoublyLinkedList* src, const size_t pos)
{
    if (!dest || !src || pos > dest->size)
    {
        return -1;
    }

    // If src is empty, nothing to splice
    if (src->size == 0)
    {
        return 0;
    }

    // If inserting at the beginning
    if (pos == 0)
    {
        if (dest->size == 0)
        {
            dest->head = src->head;
            dest->tail = src->tail;
        }
        else
        {
            src->tail->next  = dest->head;
            dest->head->prev = src->tail;
            dest->head       = src->head;
        }
    }
    // If inserting at the end
    else if (pos == dest->size)
    {
        dest->tail->next = src->head;
        src->head->prev  = dest->tail;
        dest->tail       = src->tail;
    }
    // If inserting in the middle
    else
    {
        DSCDoublyLinkedNode* curr;

        // Find the node at position pos
        if (pos <= dest->size / 2)
        {
            // Search from head
            curr = dest->head;
            for (size_t i = 0; i < pos; ++i)
            {
                curr = curr->next;
            }
        }
        else
        {
            // Search from tail
            curr = dest->tail;
            for (size_t i = dest->size - 1; i > pos; --i)
            {
                curr = curr->prev;
            }
        }

        // Connect the node at position pos-1 to src's first node
        curr->prev->next = src->head;
        src->head->prev  = curr->prev;

        // Connect src's last node to the node at position pos
        src->tail->next = curr;
        curr->prev      = src->tail;
    }

    // Update size
    dest->size += src->size;

    // Clear src list without destroying nodes
    src->head = NULL;
    src->tail = NULL;
    src->size = 0;

    return 0;
}

//==============================================================================
// Higher-order functions
//==============================================================================

DSCDoublyLinkedList* dsc_dll_filter(const DSCDoublyLinkedList* list, const pred_func pred)
{
    if (!list || !pred)
    {
        return NULL;
    }

    DSCDoublyLinkedList* filtered = dsc_dll_create(list->alloc);
    if (!filtered)
    {
        return NULL;
    }

    const DSCDoublyLinkedNode* curr = list->head;
    while (curr)
    {
        if (pred(curr->data))
        {
            if (dsc_dll_insert_back(filtered, curr->data) != 0)
            {
                dsc_dll_destroy(filtered, false);
                return NULL;
            }
        }
        curr = curr->next;
    }

    return filtered;
}

DSCDoublyLinkedList* dsc_dll_filter_deep(const DSCDoublyLinkedList* list, const pred_func pred)
{
    if (!list || !pred || !list->alloc->copy_func)
    {
        return NULL;
    }

    DSCDoublyLinkedList* filtered = dsc_dll_create(list->alloc);
    if (!filtered)
    {
        return NULL;
    }

    const DSCDoublyLinkedNode* curr = list->head;
    while (curr)
    {
        if (pred(curr->data))
        {
            void* filtered_data = dsc_alloc_copy(filtered->alloc, curr->data);

            if (dsc_dll_insert_back(filtered, filtered_data) != 0)
            {
                if (filtered_data)
                {
                    dsc_alloc_data_free(filtered->alloc, filtered_data);
                }
                dsc_dll_destroy(filtered, true);
                return NULL;
            }
        }
        curr = curr->next;
    }

    return filtered;
}

DSCDoublyLinkedList* dsc_dll_transform(const DSCDoublyLinkedList* list, const transform_func transform, const bool should_free_data)
{
    if (!list || !transform)
    {
        return NULL;
    }

    DSCDoublyLinkedList* transformed = dsc_dll_create(list->alloc);
    if (!transformed)
    {
        return NULL;
    }

    const DSCDoublyLinkedNode* curr = list->head;
    while (curr)
    {
        void* new_data = transform(curr->data);
        if (dsc_dll_insert_back(transformed, new_data) != 0)
        {
            if (should_free_data && new_data)
            {
                dsc_alloc_data_free(transformed->alloc, new_data); // Free the transformed data if insertion fails
            }
            // Free any data already successfully inserted into the new list
            dsc_dll_destroy(transformed, should_free_data);
            return NULL;
        }
        curr = curr->next;
    }

    return transformed;
}

void dsc_dll_for_each(const DSCDoublyLinkedList* list, const action_func action)
{
    if (!list || !action)
    {
        return;
    }

    const DSCDoublyLinkedNode* curr = list->head;
    while (curr)
    {
        action(curr->data);
        curr = curr->next;
    }
}

//==============================================================================
// List copying functions
//==============================================================================

DSCDoublyLinkedList* dsc_dll_copy(const DSCDoublyLinkedList* list)
{
    if (!list)
    {
        return NULL;
    }

    DSCDoublyLinkedList* copy = dsc_dll_create(list->alloc);
    if (!copy)
    {
        return NULL;
    }

    // Empty list case
    if (list->size == 0)
    {
        return copy;
    }

    // Copy nodes, sharing data pointers
    const DSCDoublyLinkedNode* curr = list->head;
    while (curr)
    {
        if (dsc_dll_insert_back(copy, curr->data) != 0)
        {
            dsc_dll_destroy(copy, false); // Don't free data - they're shared
            return NULL;
        }
        curr = curr->next;
    }

    return copy;
}

DSCDoublyLinkedList* dsc_dll_copy_deep(const DSCDoublyLinkedList* list, const bool should_free_data)
{
    if (!list || !list->alloc->copy_func)
    {
        return NULL;
    }

    DSCDoublyLinkedList* copy = dsc_dll_create(list->alloc);
    if (!copy)
    {
        return NULL;
    }

    // Empty list case
    if (list->size == 0)
    {
        return copy;
    }

    // Copy nodes and clone data
    const DSCDoublyLinkedNode* curr = list->head;
    while (curr)
    {
        void* data_copy = dsc_alloc_copy(copy->alloc, curr->data);
        if (!data_copy)
        {
            // On failure, destroy the partially built clone, freeing any data it contains
            dsc_dll_destroy(copy, should_free_data);
            return NULL;
        }
        if (dsc_dll_insert_back(copy, data_copy) != 0)
        {
            // If insertion fails, free the orphaned copy and destroy the partial clone
            if (should_free_data)
            {
                dsc_alloc_data_free(list->alloc, data_copy);
            }
            dsc_dll_destroy(copy, should_free_data);
            return NULL;
        }
        curr = curr->next;
    }

    return copy;
}

// Create a list from an iterator using a provided allocator. This function
// matches the header declaration `dsc_dll_from_iterator_custom(Iterator*, Alloc*)`.
DSCDoublyLinkedList* dsc_dll_from_iterator(DSCIterator* it, DSCAllocator* alloc)
{
    if (!it)
    {
        return NULL;
    }

    if (!it->is_valid || !it->is_valid(it))
    {
        return NULL;
    }

    DSCDoublyLinkedList* list = dsc_dll_create(alloc);
    if (!list)
    {
        return NULL;
    }

    while (it->has_next(it))
    {
        void* data = it->next(it);
        if (!data)
        {
            continue; // Skip NULL entries
        }

        void* data_to_insert = data;
        if (alloc->copy_func)
        {
            data_to_insert = dsc_alloc_copy(alloc, data);
            if (!data_to_insert)
            {
                dsc_dll_destroy(list, true);
                return NULL;
            }
        }

        if (dsc_dll_insert_back(list, data_to_insert) != 0)
        {
            if (alloc->copy_func)
            {
                dsc_alloc_data_free(alloc, data_to_insert); // Free the copy we just made
            }
            dsc_dll_destroy(list, alloc->copy_func != NULL);
            return NULL;
        }
    }

    return list;
}

//==============================================================================
// Iterator functions
//==============================================================================

DSCIterator dsc_dll_iterator(const DSCDoublyLinkedList* list)
{
    DSCIterator it = {0}; // Initialize all fields to NULL/0

    it.get      = dsc_dll_iterator_get;
    it.next     = dsc_dll_iterator_next;
    it.has_next = dsc_dll_iterator_has_next;
    it.prev     = dsc_dll_iterator_prev;
    it.has_prev = dsc_dll_iterator_has_prev;
    it.reset    = dsc_dll_iterator_reset;
    it.is_valid = dsc_dll_iterator_is_valid;
    it.destroy  = dsc_dll_iterator_destroy;

    if (!list)
    {
        return it;
    }

    ListIteratorState* state = dsc_alloc_malloc(list->alloc, sizeof(ListIteratorState));
    if (!state)
    {
        return it;
    }

    state->current = list->head;
    state->start   = list->head;
    state->list    = list;

    it.alloc = list->alloc;
    it.data_state = state;

    return it;
}

DSCIterator dsc_dll_iterator_reverse(const DSCDoublyLinkedList* list)
{
    DSCIterator it = {0}; // Initialize all fields to NULL/0

    it.get      = dsc_dll_iterator_get;
    it.next     = dsc_dll_iterator_next;
    it.has_next = dsc_dll_iterator_has_next;
    it.prev     = dsc_dll_iterator_prev;
    it.has_prev = dsc_dll_iterator_has_prev;
    it.reset    = dsc_dll_iterator_reset;
    it.is_valid = dsc_dll_iterator_is_valid;
    it.destroy  = dsc_dll_iterator_destroy;

    if (!list)
    {
        return it;
    }

    ListIteratorState* state = dsc_alloc_malloc(list->alloc, sizeof(ListIteratorState));
    if (!state)
    {
        return it;
    }

    state->current = list->tail;
    state->start   = list->tail;
    state->list    = list;

    it.alloc = list->alloc;
    it.data_state = state;

    return it;
}