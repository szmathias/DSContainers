//
// Created by zack on 8/23/25.
//
// Implementation of singly linked list functions.

#include "SinglyLinkedList.h"

#include <stdlib.h>

//==============================================================================
// Static helper functions (private to this file)
//==============================================================================

/**
 * Helper function to merge two sorted lists (for merge sort).
 *
 * @param left The head of the first sorted list
 * @param right The head of the second sorted list
 * @param compare Comparison function
 * @return Pointer to the head of the merged sorted list
 */
static SinglyLinkedNode* sll_sort_helper_merge(SinglyLinkedNode *left, SinglyLinkedNode *right, const cmp_func compare)
{
    if (!left)
    {
        return right;
    }
    if (!right)
    {
        return left;
    }

    SinglyLinkedNode *result;

    if (compare(left->data, right->data) <= 0)
    {
        result = left;
        result->next = sll_sort_helper_merge(left->next, right, compare);
    }
    else
    {
        result = right;
        result->next = sll_sort_helper_merge(left, right->next, compare);
    }

    return result;
}

//==============================================================================
// Creation and destruction functions
//==============================================================================

SinglyLinkedList *sll_create(void)
{
    return sll_create_custom(malloc, free);
}

SinglyLinkedList *sll_create_custom(alloc_func alloc, dealloc_func dealloc)
{
    if (!alloc)
    {
        alloc = malloc;
    }

    if (!dealloc)
    {
        dealloc = free;
    }

    SinglyLinkedList *list = alloc(sizeof(SinglyLinkedList));
    if (!list)
    {
        return NULL;
    }

    list->head = NULL;
    list->size = 0;
    list->alloc = alloc;
    list->dealloc = dealloc;

    return list;
}

void sll_destroy(SinglyLinkedList *list, const data_free_func data_free)
{
    if (list)
    {
        sll_clear(list, data_free);
        list->dealloc(list);
    }
}

void sll_clear(SinglyLinkedList *list, const data_free_func data_free)
{
    if (!list)
    {
        return;
    }

    SinglyLinkedNode *node = list->head;
    while (node)
    {
        SinglyLinkedNode *next = node->next;
        if (data_free && node->data)
        {
            data_free(node->data);
        }
        list->dealloc(node);
        node = next;
    }
    list->head = NULL;
    list->size = 0;
}

//==============================================================================
// Information functions
//==============================================================================

size_t sll_size(const SinglyLinkedList *list)
{
    if (!list)
    {
        return 0;
    }
    return list->size;
}

int sll_is_empty(const SinglyLinkedList *list)
{
    return !list || list->size == 0;
}

SinglyLinkedNode *sll_find(const SinglyLinkedList *list, const void *data, const cmp_func compare)
{
    if (!list || !compare)
    {
        return NULL;
    }

    SinglyLinkedNode *curr = list->head;
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

int sll_equals(const SinglyLinkedList *list1, const SinglyLinkedList *list2, const cmp_func compare)
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
    const SinglyLinkedNode *node1 = list1->head;
    const SinglyLinkedNode *node2 = list2->head;

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

int sll_insert_front(SinglyLinkedList *list, void *data)
{
    if (!list)
    {
        return -1;
    }

    SinglyLinkedNode *node = list->alloc(sizeof(SinglyLinkedNode));
    if (!node)
    {
        return -1;
    }

    node->data = data;
    node->next = list->head;
    list->head = node;
    list->size++;
    return 0;
}

int sll_insert_back(SinglyLinkedList *list, void *data)
{
    if (!list)
    {
        return -1;
    }

    SinglyLinkedNode *node = list->alloc(sizeof(SinglyLinkedNode));
    if (!node)
    {
        return -1;
    }

    node->data = data;
    node->next = NULL;
    if (!list->head)
    {
        list->head = node;
    }
    else
    {
        SinglyLinkedNode *curr = list->head;
        while (curr->next)
        {
            curr = curr->next;
        }
        curr->next = node;
    }

    list->size++;
    return 0;
}

int sll_insert_at(SinglyLinkedList *list, const size_t pos, void *data)
{
    if (!list || pos > list->size)
    {
        return -1;
    }

    if (pos == 0)
    {
        return sll_insert_front(list, data);
    }

    SinglyLinkedNode *node = list->alloc(sizeof(SinglyLinkedNode));
    if (!node)
    {
        return -1;
    }

    node->data = data;

    SinglyLinkedNode *prev = list->head;
    for (size_t i = 1; i < pos; ++i)
    {
        prev = prev->next;
        if (!prev)
        {
            list->dealloc(node);
            return -1;
        }
    }

    node->next = prev->next;
    prev->next = node;
    list->size++;
    return 0;
}

//==============================================================================
// Removal functions
//==============================================================================

int sll_remove(SinglyLinkedList *list, const void *data, const cmp_func compare, const data_free_func remove)
{
    if (!list || !compare)
    {
        return -1;
    }

    SinglyLinkedNode *prev = NULL;
    SinglyLinkedNode *curr = list->head;
    while (curr)
    {
        if (compare(curr->data, data) == 0)
        {
            if (prev)
            {
                prev->next = curr->next;
            }
            else
            {
                list->head = curr->next;
            }

            if (remove && curr->data)
            {
                remove(curr->data);
            }

            list->dealloc(curr);
            list->size--;
            return 0;
        }

        prev = curr;
        curr = curr->next;
    }
    return -1;
}

int sll_remove_at(SinglyLinkedList *list, const size_t pos, const data_free_func remove)
{
    if (!list || pos >= list->size)
    {
        return -1;
    }

    SinglyLinkedNode *prev = NULL;
    SinglyLinkedNode *curr = list->head;

    for (size_t i = 0; i < pos; ++i)
    {
        prev = curr;
        curr = curr->next;
    }

    if (prev)
    {
        prev->next = curr->next;
    }
    else
    {
        list->head = curr->next;
    }

    if (remove && curr->data)
    {
        remove(curr->data);
    }

    list->dealloc(curr);
    list->size--;
    return 0;
}

int sll_remove_front(SinglyLinkedList *list, const data_free_func remove)
{
    if (!list || !list->head)
    {
        return -1;
    }

    SinglyLinkedNode *node_to_remove = list->head;
    list->head = node_to_remove->next;

    if (remove && node_to_remove->data)
    {
        remove(node_to_remove->data);
    }

    list->dealloc(node_to_remove);
    list->size--;
    return 0;
}

int sll_remove_back(SinglyLinkedList *list, const data_free_func remove)
{
    if (!list || !list->head)
    {
        return -1;
    }

    // If there's only one node
    if (list->size == 1)
    {
        return sll_remove_front(list, remove);
    }

    // Find the second-to-last node
    SinglyLinkedNode *prev = NULL;
    SinglyLinkedNode *curr = list->head;
    while (curr->next)
    {
        prev = curr;
        curr = curr->next;
    }

    // Remove the last node
    if (prev)
    {
        prev->next = NULL;
    }

    if (remove && curr->data)
    {
        remove(curr->data);
    }

    list->dealloc(curr);
    list->size--;
    return 0;
}

//==============================================================================
// List manipulation functions
//==============================================================================

int sll_sort(SinglyLinkedList *list, const cmp_func compare)
{
    if (!list || !compare)
    {
        return -1;
    }
    if (list->size <= 1)
    {
        return 0;
    }

    // Use an array to hold sorted sublists. Max 64 for 64-bit size_t.
    SinglyLinkedNode *sub_lists[64] = {0};
    int num_sub_lists = 0;

    SinglyLinkedNode *current = list->head;
    while (current)
    {
        SinglyLinkedNode *next = current->next;
        current->next = NULL; // Isolate the node into a sublist of size 1

        int i = 0;
        // Merge with existing sublists of the same "level"
        while (i < num_sub_lists && sub_lists[i] != NULL)
        {
            current = sll_sort_helper_merge(sub_lists[i], current, compare);
            sub_lists[i] = NULL;
            i++;
        }

        // Place the newly merged sublist in the correct slot
        if (i == num_sub_lists)
        {
            num_sub_lists++;
        }
        sub_lists[i] = current;

        current = next;
    }

    // Merge all remaining sublists into the final sorted list
    for (int i = 1; i < num_sub_lists; i++)
    {
        sub_lists[i] = sll_sort_helper_merge(sub_lists[i-1], sub_lists[i], compare);
    }

    list->head = sub_lists[num_sub_lists - 1];

    return 0;
}

int sll_reverse(SinglyLinkedList *list)
{
    if (!list)
    {
        return -1;
    }

    if (list->size <= 1)
    {
        return 0; // Empty or single-element list is already reversed
    }

    SinglyLinkedNode *prev = NULL;
    SinglyLinkedNode *current = list->head;

    while (current)
    {
        SinglyLinkedNode* next = current->next; // Store next node
        current->next   = prev;          // Reverse the link
        prev            = current;       // Move prev one step forward
        current         = next;          // Move current one step forward
    }

    list->head = prev; // Update head to point to the new first node (previously last)
    return 0;
}

int sll_merge(SinglyLinkedList *dest, SinglyLinkedList *src)
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
        dest->size = src->size;
    }
    else
    {
        // Find the last node in dest
        SinglyLinkedNode *last = dest->head;
        while (last->next)
        {
            last = last->next;
        }

        // Connect dest's last node to src's first node
        last->next = src->head;

        // Update size
        dest->size += src->size;
    }

    // Clear src list without destroying nodes
    src->head = NULL;
    src->size = 0;

    return 0;
}

int sll_splice(SinglyLinkedList *dest, SinglyLinkedList *src, const size_t pos)
{
    if (!dest || !src || pos > dest->size)
    {
        return -1;
    }

    // If src is empty, nothing to splice
    if (sll_is_empty(src))
    {
        return 0;
    }

    if (pos == dest->size)
    {
        return sll_merge(dest, src);
    }

    // Find the last node of the source list.
    SinglyLinkedNode *src_last = src->head;
    while (src_last->next)
    {
        src_last = src_last->next;
    }

    if (pos == 0)
    {
        src_last->next = dest->head;
        dest->head = src->head;
    }
    else
    {
        // Find the node in dest to insert after.
        SinglyLinkedNode *prev = dest->head;
        for (size_t i = 1; i < pos; ++i)
        {
            prev = prev->next;
        }
        src_last->next = prev->next;
        prev->next = src->head;
    }

    dest->size += src->size;

    // Clear src list without destroying nodes
    src->head = NULL;
    src->size = 0;

    return 0;
}

//==============================================================================
// Higher-order functions
//==============================================================================

SinglyLinkedList *sll_filter(const SinglyLinkedList *list, const pred_func pred)
{
    if (!list || !pred)
    {
        return NULL;
    }

    SinglyLinkedList *result = sll_create_custom(list->alloc, list->dealloc);
    if (!result)
    {
        return NULL;
    }

    const SinglyLinkedNode *curr = list->head;
    while (curr)
    {
        if (pred(curr->data))
        {
            // Include elements that match the predicate
            if (sll_insert_back(result, curr->data) != 0)
            {
                sll_destroy(result, NULL); // Don't free data; it's still in source list
                return NULL;
            }
        }
        curr = curr->next;
    }

    return result;
}

SinglyLinkedList *sll_transform(const SinglyLinkedList *list, const transform_func transform, const data_free_func new_data_free)
{
    if (!list || !transform)
    {
        return NULL;
    }

    SinglyLinkedList *result = sll_create_custom(list->alloc, list->dealloc);
    if (!result)
    {
        return NULL;
    }

    const SinglyLinkedNode *curr = list->head;
    while (curr)
    {
        void *new_data = transform(curr->data);
        if (sll_insert_back(result, new_data) != 0)
        {
            if (new_data_free && new_data)
            {
                new_data_free(new_data);  // Free the transformed data if insertion fails
            }
            // Free any data already successfully inserted into the new list
            sll_destroy(result, new_data_free);
            return NULL;
        }
        curr = curr->next;
    }

    return result;
}

void sll_for_each(const SinglyLinkedList *list, const action_func action)
{
    if (!list || !action)
    {
        return;
    }

    const SinglyLinkedNode *curr = list->head;
    while (curr)
    {
        action(curr->data);
        curr = curr->next;
    }
}

//==============================================================================
// List copying functions
//==============================================================================

SinglyLinkedList *sll_copy(const SinglyLinkedList *list)
{
    if (!list)
    {
        return NULL;
    }

    SinglyLinkedList *clone = sll_create_custom(list->alloc, list->dealloc);
    if (!clone)
    {
        return NULL;
    }

    // Empty list case
    if (list->size == 0)
    {
        return clone;
    }

    // Copy nodes, sharing data pointers
    const SinglyLinkedNode *curr = list->head;
    while (curr)
    {
        if (sll_insert_back(clone, curr->data) != 0)
        {
            sll_destroy(clone, NULL);  // Don't free data - they're shared
            return NULL;
        }
        curr = curr->next;
    }

    return clone;
}

SinglyLinkedList *sll_copy_deep(const SinglyLinkedList *list, const copy_func copy_data, const data_free_func copied_data_free)
{
    if (!list || !copy_data)
    {
        return NULL;
    }

    SinglyLinkedList *clone = sll_create_custom(list->alloc, list->dealloc);
    if (!clone)
    {
        return NULL;
    }

    // Empty list case
    if (list->size == 0)
    {
        return clone;
    }

    // Copy nodes and clone data
    const SinglyLinkedNode *curr = list->head;
    while (curr)
    {
        void *data_copy = copy_data(curr->data);
        if (!data_copy)
        {
            // On failure, destroy the partially built clone, freeing any data it contains
            sll_destroy(clone, copied_data_free);
            return NULL;
        }
        if (sll_insert_back(clone, data_copy) != 0)
        {
            // If insertion fails, free the orphaned copy and destroy the partial clone
            if (copied_data_free) {
                copied_data_free(data_copy);
            }
            sll_destroy(clone, copied_data_free);
            return NULL;
        }
        curr = curr->next;
    }

    return clone;
}

//==============================================================================
// Iterator state and helper functions
//==============================================================================

typedef struct SListIteratorState
{
    SinglyLinkedNode *current;      // Current node
    SinglyLinkedList *list;       // The list being iterated
} SListIteratorState;

/**
 * Check if iterator has more elements.
 */
static int sll_iterator_has_next(const Iterator *it)
{
    if (!it || !it->data_state)
    {
        return 0;
    }

    const SListIteratorState *state = it->data_state;
    return state->current != NULL;
}

/**
 * Get next element and advance iterator.
 */
static void *sll_iterator_next(const Iterator *it)
{
    if (!it || !it->data_state)
    {
        return NULL;
    }

    SListIteratorState *state = it->data_state;
    if (!state->current)
    {
        return NULL;
    }

    void *data = state->current->data;
    state->current = state->current->next;
    return data;
}

/**
 * Check if iterator has previous elements (not supported for singly linked list).
 */
static int sll_iterator_has_prev(const Iterator *it)
{
    (void)it;  // Suppress unused parameter warning
    return 0;  // Singly linked list cannot go backwards
}

/**
 * Get previous element and move iterator backwards (not supported for singly linked list).
 */
static void *sll_iterator_prev(const Iterator *it)
{
    (void)it;  // Suppress unused parameter warning
    return NULL;  // Singly linked list cannot go backwards
}

/**
 * Reset iterator to starting position.
 */
static void sll_iterator_reset(const Iterator *it)
{
    if (!it || !it->data_state)
    {
        return;
    }

    SListIteratorState *state = it->data_state;
    state->current = state->list->head;
}

/**
 * Check if iterator is valid.
 */
static int sll_iterator_is_valid(const Iterator *it)
{
    if (!it || !it->data_state)
    {
        return 0;
    }

    const SListIteratorState *state = it->data_state;
    return state->list != NULL;
}

/**
 * Free resources used by iterator.
 */
static void sll_iterator_destroy(Iterator *it)
{
    if (!it || !it->data_state)
    {
        return;
    }

    const SListIteratorState *state = it->data_state;
    if (state && state->list)
    {
        state->list->dealloc(it->data_state);
    }
    it->data_state = NULL;
}

/**
 * Get current element without advancing iterator.
 */
static void *sll_iterator_get(const Iterator *it)
{
    if (!it || !it->data_state)
    {
        return NULL;
    }

    const SListIteratorState *state = it->data_state;
    if (!state->current)
    {
        return NULL;
    }

    return state->current->data;
}

//==============================================================================
// Iterator functions
//==============================================================================

Iterator sll_iterator(const SinglyLinkedList *list)
{
    Iterator it = {0};  // Initialize all fields to NULL/0

    it.get = sll_iterator_get;
    it.next = sll_iterator_next;
    it.has_next = sll_iterator_has_next;
    it.prev = sll_iterator_prev;
    it.has_prev = sll_iterator_has_prev;
    it.reset = sll_iterator_reset;
    it.is_valid = sll_iterator_is_valid;
    it.destroy = sll_iterator_destroy;

    if (!list)
    {
        return it;
    }

    SListIteratorState *state = list->alloc(sizeof(SListIteratorState));
    if (!state)
    {
        return it;
    }

    state->current = list->head;
    state->list = (SinglyLinkedList *)list;

    it.data_state = state;

    return it;
}

SinglyLinkedList *sll_from_iterator(Iterator *it, const copy_func copy, const data_free_func copied_data_free)
{
    return sll_from_iterator_custom(it, copy, copied_data_free, malloc, free);
}

SinglyLinkedList *sll_from_iterator_custom(Iterator *it, const copy_func copy, const data_free_func copied_data_free, const alloc_func alloc, const dealloc_func dealloc)
{
    if (!it)
    {
        return NULL;
    }

    if(!it->is_valid)
    {
        return NULL;
    }

    if (!it->is_valid(it))
    {
        return NULL;
    }

    SinglyLinkedList *list = sll_create_custom(alloc, dealloc);
    if (!list)
    {
        return NULL;
    }

    while (it->has_next(it))
    {
        void *data = it->next(it);
        if (!data)
        {
            continue;  // Skip nullptr entries
        }

        void *data_to_insert = data;
        if (copy)
        {
            data_to_insert = copy(data);
            if (!data_to_insert)
            {
                sll_destroy(list, copied_data_free);
                return NULL;
            }
        }

        if (sll_insert_back(list, data_to_insert) != 0)
        {
            if (copy && copied_data_free)
            {
                copied_data_free(data_to_insert);  // Free the copy we just made
            }
            sll_destroy(list, copy ? copied_data_free : NULL);
            return NULL;
        }
    }

    return list;
}
