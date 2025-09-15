//
// SinglyLinkedList.c
// Implementation of singly linked list functions.
//
// This file implements a lightweight singly linked list with support for a
// customizable allocator (DSCAllocator). The API mirrors the doubly-linked list
// style used in DoublyLinkedList.c: creation/destruction, insertion,
// removal, higher-order operations, copying, and iterator helpers. Public
// functions return 0 on success or non-zero on failure when applicable.

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
static DSCSinglyLinkedNode* sll_sort_helper_merge(DSCSinglyLinkedNode* left, DSCSinglyLinkedNode* right, const cmp_func compare)
{
    if (!left)
    {
        return right;
    }
    if (!right)
    {
        return left;
    }

    DSCSinglyLinkedNode* result;

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

/**
 * Create a new singly linked list using the provided allocator.
 * The allocator's function pointers are defaulted to malloc/free when NULL.
 */
DSCSinglyLinkedList* dsc_sll_create(DSCAllocator* alloc)
{
    if (!alloc)
    {
        return NULL;
    }

    DSCSinglyLinkedList* list = dsc_alloc_malloc(alloc, sizeof(DSCSinglyLinkedList));
    if (!list)
    {
        return NULL;
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    list->alloc = alloc;

    return list;
}

/**
 * Destroy the list and free nodes. When should_free_data is true the
 * allocator's data_free_func is used on stored element pointers.
 */
void dsc_sll_destroy(DSCSinglyLinkedList* list, const bool should_free_data)
{
    if (list)
    {
        dsc_sll_clear(list, should_free_data);
        dsc_alloc_free(list->alloc, list);
    }
}

/**
 * Clear all nodes but keep the list structure.
 */
void dsc_sll_clear(DSCSinglyLinkedList* list, const bool should_free_data)
{
    if (!list)
    {
        return;
    }

    DSCSinglyLinkedNode* node = list->head;
    while (node)
    {
        DSCSinglyLinkedNode* next = node->next;
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

/**
 * Return the number of elements in the list.
 */
size_t dsc_sll_size(const DSCSinglyLinkedList* list)
{
    if (!list)
    {
        return 0;
    }
    return list->size;
}

/**
 * Return non-zero if the list is empty or NULL.
 */
int dsc_sll_is_empty(const DSCSinglyLinkedList* list)
{
    return !list || list->size == 0;
}

/**
 * Find the first node equal to data using compare.
 */
DSCSinglyLinkedNode* dsc_sll_find(const DSCSinglyLinkedList* list, const void* data, const cmp_func compare)
{
    if (!list || !compare)
    {
        return NULL;
    }

    DSCSinglyLinkedNode* curr = list->head;
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

/**
 * Compare two lists element-wise using compare.
 * Returns 1 if equal, 0 if not equal, -1 on error.
 */
int dsc_sll_equals(const DSCSinglyLinkedList* list1, const DSCSinglyLinkedList* list2, const cmp_func compare)
{
    if (!list1 || !list2 || !compare)
    {
        return -1;
    }

    if (list1->size != list2->size)
    {
        return 0;
    }

    if (list1->size == 0)
    {
        return 1;
    }

    const DSCSinglyLinkedNode* node1 = list1->head;
    const DSCSinglyLinkedNode* node2 = list2->head;

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
// Insertion functions
//==============================================================================

/**
 * Insert data at the front of the list.
 */
int dsc_sll_insert_front(DSCSinglyLinkedList* list, void* data)
{
    if (!list)
    {
        return -1;
    }

    DSCSinglyLinkedNode* node = dsc_alloc_malloc(list->alloc, sizeof(DSCSinglyLinkedNode));
    if (!node)
    {
        return -1;
    }

    node->data = data;
    node->next = list->head;
    list->head = node;

    // If this is the first node, also set tail
    if (!list->tail)
    {
        list->tail = node;
    }

    list->size++;
    return 0;
}

/**
 * Insert data at the back of the list.
 */
int dsc_sll_insert_back(DSCSinglyLinkedList* list, void* data)
{
    if (!list)
    {
        return -1;
    }

    DSCSinglyLinkedNode* node = dsc_alloc_malloc(list->alloc, sizeof(DSCSinglyLinkedNode));
    if (!node)
    {
        return -1;
    }

    node->data = data;
    node->next = NULL;

    if (!list->head)
    {
        // Empty list - set both head and tail
        list->head = node;
        list->tail = node;
    }
    else
    {
        // Non-empty list - append to tail in O(1)
        list->tail->next = node;
        list->tail = node;
    }

    list->size++;
    return 0;
}

/**
 * Insert data at a specific zero-based position (0...size).
 */
int dsc_sll_insert_at(DSCSinglyLinkedList* list, const size_t pos, void* data)
{
    if (!list || pos > list->size)
    {
        return -1;
    }

    if (pos == 0)
    {
        return dsc_sll_insert_front(list, data);
    }

    DSCSinglyLinkedNode* node = dsc_alloc_malloc(list->alloc, sizeof(DSCSinglyLinkedNode));
    if (!node)
    {
        return -1;
    }

    node->data = data;

    DSCSinglyLinkedNode* prev = list->head;
    for (size_t i = 1; i < pos; ++i)
    {
        prev = prev->next;
        if (!prev)
        {
            dsc_alloc_free(list->alloc, node);
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

/**
 * Remove first element matching data using compare. Optionally free data.
 */
int dsc_sll_remove(DSCSinglyLinkedList* list, const void* data, const cmp_func compare, const bool should_free_data)
{
    if (!list || !compare)
    {
        return -1;
    }

    DSCSinglyLinkedNode* prev = NULL;
    DSCSinglyLinkedNode* curr = list->head;
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

            // Update tail if we're removing the last node
            if (curr == list->tail)
            {
                list->tail = prev;
            }

            if (should_free_data && curr->data)
            {
                dsc_alloc_data_free(list->alloc, curr->data);
            }

            dsc_alloc_free(list->alloc, curr);
            list->size--;
            return 0;
        }

        prev = curr;
        curr = curr->next;
    }
    return -1;
}

/**
 * Remove element at zero-based position. Optionally free data.
 */
int dsc_sll_remove_at(DSCSinglyLinkedList* list, const size_t pos, const bool should_free_data)
{
    if (!list || pos >= list->size)
    {
        return -1;
    }

    DSCSinglyLinkedNode* prev = NULL;
    DSCSinglyLinkedNode* curr = list->head;

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

    if (should_free_data && curr->data)
    {
        dsc_alloc_data_free(list->alloc, curr->data);
    }

    dsc_alloc_free(list->alloc, curr);
    list->size--;
    return 0;
}

/**
 * Remove first element.
 */
int dsc_sll_remove_front(DSCSinglyLinkedList* list, const bool should_free_data)
{
    if (!list || !list->head)
    {
        return -1;
    }

    DSCSinglyLinkedNode* node_to_remove = list->head;
    list->head = node_to_remove->next;

    if (should_free_data && node_to_remove->data)
    {
        dsc_alloc_data_free(list->alloc, node_to_remove->data);
    }

    dsc_alloc_free(list->alloc, node_to_remove);
    list->size--;
    return 0;
}

/**
 * Remove last element.
 */
int dsc_sll_remove_back(DSCSinglyLinkedList* list, const bool should_free_data)
{
    if (!list || !list->head)
    {
        return -1;
    }

    if (list->size == 1)
    {
        return dsc_sll_remove_front(list, should_free_data);
    }

    DSCSinglyLinkedNode* prev = NULL;
    DSCSinglyLinkedNode* curr = list->head;
    while (curr->next)
    {
        prev = curr;
        curr = curr->next;
    }

    if (prev)
    {
        prev->next = NULL;
    }

    if (should_free_data && curr->data)
    {
        dsc_alloc_data_free(list->alloc, curr->data);
    }

    dsc_alloc_free(list->alloc, curr);
    list->size--;
    return 0;
}

//==============================================================================
// List manipulation functions
//==============================================================================

/**
 * Sort the list in-place using an iterative merge approach.
 */
int dsc_sll_sort(DSCSinglyLinkedList* list, const cmp_func compare)
{
    if (!list || !compare)
    {
        return -1;
    }
    if (list->size <= 1)
    {
        return 0;
    }

    DSCSinglyLinkedNode* sub_lists[64] = {0};
    int num_sub_lists = 0;

    DSCSinglyLinkedNode* current = list->head;
    while (current)
    {
        DSCSinglyLinkedNode* next = current->next;
        current->next = NULL;

        int i = 0;
        while (i < num_sub_lists && sub_lists[i] != NULL)
        {
            current = sll_sort_helper_merge(sub_lists[i], current, compare);
            sub_lists[i] = NULL;
            i++;
        }

        if (i == num_sub_lists)
        {
            num_sub_lists++;
        }
        sub_lists[i] = current;

        current = next;
    }

    for (int i = 1; i < num_sub_lists; i++)
    {
        sub_lists[i] = sll_sort_helper_merge(sub_lists[i - 1], sub_lists[i], compare);
    }

    list->head = sub_lists[num_sub_lists - 1];

    return 0;
}

/**
 * Reverse the list in-place.
 */
int dsc_sll_reverse(DSCSinglyLinkedList* list)
{
    if (!list)
    {
        return -1;
    }

    if (list->size <= 1)
    {
        return 0;
    }

    DSCSinglyLinkedNode* prev = NULL;
    DSCSinglyLinkedNode* current = list->head;

    while (current)
    {
        DSCSinglyLinkedNode* next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }

    list->head = prev;
    return 0;
}

/**
 * Append src to dest; src is emptied but not destroyed.
 */
int dsc_sll_merge(DSCSinglyLinkedList* dest, DSCSinglyLinkedList* src)
{
    if (!dest || !src)
    {
        return -1;
    }

    if (src->size == 0)
    {
        return 0;
    }

    if (dest->size == 0)
    {
        dest->head = src->head;
        dest->size = src->size;
    }
    else
    {
        DSCSinglyLinkedNode* last = dest->head;
        while (last->next)
        {
            last = last->next;
        }
        last->next = src->head;
        dest->size += src->size;
    }

    src->head = NULL;
    src->size = 0;

    return 0;
}

/**
 * Splice src into dest at position pos. Src is emptied but not destroyed.
 */
int dsc_sll_splice(DSCSinglyLinkedList* dest, DSCSinglyLinkedList* src, const size_t pos)
{
    if (!dest || !src || pos > dest->size)
    {
        return -1;
    }

    if (dsc_sll_is_empty(src))
    {
        return 0;
    }

    if (pos == dest->size)
    {
        return dsc_sll_merge(dest, src);
    }

    DSCSinglyLinkedNode* src_last = src->head;
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
        DSCSinglyLinkedNode* prev = dest->head;
        for (size_t i = 1; i < pos; ++i)
        {
            prev = prev->next;
        }
        src_last->next = prev->next;
        prev->next = src->head;
    }

    dest->size += src->size;

    src->head = NULL;
    src->size = 0;

    return 0;
}

//==============================================================================
// Higher-order functions
//==============================================================================

DSCSinglyLinkedList* dsc_sll_filter(const DSCSinglyLinkedList* list, const pred_func pred)
{
    if (!list || !pred)
    {
        return NULL;
    }

    DSCSinglyLinkedList* filtered = dsc_sll_create(list->alloc);
    if (!filtered)
    {
        return NULL;
    }

    const DSCSinglyLinkedNode* curr = list->head;
    while (curr)
    {
        if (pred(curr->data))
        {
            if (dsc_sll_insert_back(filtered, curr->data) != 0)
            {
                dsc_sll_destroy(filtered, false);
                return NULL;
            }
        }
        curr = curr->next;
    }

    return filtered;
}

DSCSinglyLinkedList* dsc_sll_filter_deep(const DSCSinglyLinkedList* list, const pred_func pred)
{
    if (!list || !pred || !list->alloc || !list->alloc->copy_func)
    {
        return NULL;
    }

    DSCSinglyLinkedList* filtered = dsc_sll_create(list->alloc);
    if (!filtered)
    {
        return NULL;
    }

    const DSCSinglyLinkedNode* curr = list->head;
    while (curr)
    {
        if (pred(curr->data))
        {
            void* filtered_data = dsc_alloc_copy(filtered->alloc, curr->data);
            if (dsc_sll_insert_back(filtered, filtered_data) != 0)
            {
                if (filtered_data)
                {
                    dsc_alloc_data_free(filtered->alloc, filtered_data);
                }
                dsc_sll_destroy(filtered, true);
                return NULL;
            }
        }
        curr = curr->next;
    }

    return filtered;
}

DSCSinglyLinkedList* dsc_sll_transform(const DSCSinglyLinkedList* list, const transform_func transform, const bool should_free_data)
{
    if (!list || !transform)
    {
        return NULL;
    }

    DSCSinglyLinkedList* transformed = dsc_sll_create(list->alloc);
    if (!transformed)
    {
        return NULL;
    }

    const DSCSinglyLinkedNode* curr = list->head;
    while (curr)
    {
        void* new_data = transform(curr->data);
        if (dsc_sll_insert_back(transformed, new_data) != 0)
        {
            if (should_free_data && new_data)
            {
                dsc_alloc_data_free(transformed->alloc, new_data);
            }
            dsc_sll_destroy(transformed, should_free_data);
            return NULL;
        }
        curr = curr->next;
    }

    return transformed;
}

void dsc_sll_for_each(const DSCSinglyLinkedList* list, const action_func action)
{
    if (!list || !action)
    {
        return;
    }

    const DSCSinglyLinkedNode* curr = list->head;
    while (curr)
    {
        action(curr->data);
        curr = curr->next;
    }
}

//==============================================================================
// List copying functions
//==============================================================================

DSCSinglyLinkedList* dsc_sll_copy(const DSCSinglyLinkedList* list)
{
    if (!list)
    {
        return NULL;
    }

    DSCSinglyLinkedList* clone = dsc_sll_create(list->alloc);
    if (!clone)
    {
        return NULL;
    }

    if (list->size == 0)
    {
        return clone;
    }

    const DSCSinglyLinkedNode* curr = list->head;
    while (curr)
    {
        if (dsc_sll_insert_back(clone, curr->data) != 0)
        {
            dsc_sll_destroy(clone, false);
            return NULL;
        }
        curr = curr->next;
    }

    return clone;
}

DSCSinglyLinkedList* dsc_sll_copy_deep(const DSCSinglyLinkedList* list, const copy_func copy_data, const bool should_free_data)
{
    if (!list || !copy_data)
    {
        return NULL;
    }

    DSCSinglyLinkedList* clone = dsc_sll_create(list->alloc);
    if (!clone)
    {
        return NULL;
    }

    if (list->size == 0)
    {
        return clone;
    }

    const DSCSinglyLinkedNode* curr = list->head;
    while (curr)
    {
        void* data_copy = copy_data(curr->data);
        if (!data_copy)
        {
            dsc_sll_destroy(clone, should_free_data);
            return NULL;
        }
        if (dsc_sll_insert_back(clone, data_copy) != 0)
        {
            if (should_free_data)
            {
                dsc_alloc_data_free(list->alloc, data_copy);
            }
            dsc_sll_destroy(clone, should_free_data);
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
    DSCSinglyLinkedNode* current; // Current node
    DSCSinglyLinkedList* list;    // The list being iterated
} SListIteratorState;

static int sll_iterator_has_next(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return 0;
    }

    const SListIteratorState* state = it->data_state;
    return state->current != NULL;
}

static void* sll_iterator_get(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return NULL;
    }

    const SListIteratorState* state = it->data_state;
    if (!state->current)
    {
        return NULL;
    }

    return state->current->data;
}

static int sll_iterator_next(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return -1;
    }

    SListIteratorState* state = it->data_state;
    if (!state->current)
    {
        return -1;
    }

    state->current = state->current->next;
    return 0;
}

static int sll_iterator_has_prev(const DSCIterator* it)
{
    (void)it;
    return 0;
}

static int sll_iterator_prev(const DSCIterator* it)
{
    (void)it;
    return -1; // SLL iterator doesn't support backward iteration
}

static void sll_iterator_reset(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return;
    }

    SListIteratorState* state = it->data_state;
    state->current = state->list->head;
}

static int sll_iterator_is_valid(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return 0;
    }

    const SListIteratorState* state = it->data_state;
    return state->list != NULL;
}

static void sll_iterator_destroy(DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return;
    }

    const SListIteratorState* state = it->data_state;
    dsc_alloc_free(state->list->alloc, it->data_state);
    it->data_state = NULL;
}

DSCIterator dsc_sll_iterator(const DSCSinglyLinkedList* list)
{
    DSCIterator it = {0};

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

    SListIteratorState* state = dsc_alloc_malloc(list->alloc, sizeof(SListIteratorState));
    if (!state)
    {
        return it;
    }

    state->current = list->head;
    state->list = (DSCSinglyLinkedList*)list;

    it.alloc = list->alloc;
    it.data_state = state;

    return it;
}

DSCSinglyLinkedList* dsc_sll_from_iterator(DSCIterator* it, DSCAllocator* alloc, const bool should_copy)
{
    if (!it || !alloc)
    {
        return NULL;
    }
    if (should_copy && !alloc->copy_func)
    {
        return NULL; // Can't copy without copy function
    }

    if (!it->is_valid || !it->is_valid(it))
    {
        return NULL;
    }

    DSCSinglyLinkedList* list = dsc_sll_create(alloc);
    if (!list)
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
            element_to_insert = alloc->copy_func(element);
            if (!element_to_insert)
            {
                dsc_sll_destroy(list, true);
                return NULL;
            }
        }

        if (dsc_sll_insert_back(list, element_to_insert) != 0)
        {
            if (should_copy)
            {
                dsc_alloc_data_free(alloc, element_to_insert);
            }
            dsc_sll_destroy(list, should_copy);
            return NULL;
        }

        if (it->next(it) != 0)
        {
            break; // Iterator done or failed
        }
    }

    return list;
}