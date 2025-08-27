//
// Created by zack on 8/25/25.
//
// Implementation of doubly linked list functions.

#include "DLinkedList.h"

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
static DListNode* dll_split(DListNode *head)
{
    if (head == nullptr || !head->next)
    {
        return nullptr;
    }

    const DListNode *fast = head;
    DListNode *slow = head;
    DListNode *prev = nullptr;

    while (fast && fast->next)
    {
        fast = fast->next->next;
        prev = slow;
        slow = slow->next;
    }

    if (prev)
    {
        prev->next = nullptr;
    }
    slow->prev = nullptr;

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
static DListNode* dll_sort_helper_merge(DListNode *left, DListNode *right, const cmp_func compare)
{
    if (!left)
    {
        return right;
    }

    if (!right)
    {
        return left;
    }

    DListNode *result;

    // Initialize result pointer
    if (compare(left->data, right->data) <= 0)
    {
        result = left;
        left = left->next;
    }
    else
    {
        result = right;
        right = right->next;
    }

    result->prev = nullptr;
    DListNode* current = result;

    // Merge the two lists
    while (left && right)
    {
        if (compare(left->data, right->data) <= 0)
        {
            current->next = left;
            left->prev = current;
            left = left->next;
        }
        else
        {
            current->next = right;
            right->prev = current;
            right = right->next;
        }
        current = current->next;
    }

    // Attach the remaining list
    if (left)
    {
        current->next = left;
        left->prev = current;
    }
    else if (right)
    {
        current->next = right;
        right->prev = current;
    }
    else
    {
        current->next = nullptr;
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
static DListNode* dll_merge_sort(DListNode *head, const cmp_func compare)
{
    if (!head || !head->next)
    {
        return head;
    }

    // Split the list into two halves
    DListNode *right = dll_split(head);

    // Recursively sort both halves
    DListNode *left_sorted = dll_merge_sort(head, compare);
    DListNode *right_sorted = dll_merge_sort(right, compare);

    // Merge the sorted halves
    return dll_sort_helper_merge(left_sorted, right_sorted, compare);
}

//==============================================================================
// Iterator state and helper functions
//==============================================================================

typedef struct ListIteratorState
{
    DListNode *current;      // Current node
    DListNode *start;        // Starting position (head or tail)
    DLinkedList *list;       // The list being iterated
} ListIteratorState;

/**
 * Check if iterator has more elements.
 */
static int dll_iterator_has_next(const Iterator *it)
{
    if (!it || !it->data_state)
    {
        return 0;
    }

    const ListIteratorState *state = it->data_state;
    return state->current != nullptr;
}

/**
 * Get next element and advance iterator.
 */
static void *dll_iterator_next(const Iterator *it)
{
    if (!it || !it->data_state)
    {
        return nullptr;
    }

    ListIteratorState *state = it->data_state;
    if (!state->current)
    {
        return nullptr;
    }

    void *data = state->current->data;
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
static int dll_iterator_has_prev(const Iterator *it)
{
    if (!it || !it->data_state)
        return 0;

    const ListIteratorState *state = it->data_state;
    if (!state->current)
        return 0;

    // Check if there's a node in the opposite direction of iteration
    if (state->start == state->list->head)
    {
        // For forward iterator, check if we can go backwards
        return state->current->prev != nullptr;
    }
    else
    {
        // For reverse iterator, check if we can go forwards
        return state->current->next != nullptr;
    }
}

/**
 * Get previous element and move iterator backwards.
 */
static void *dll_iterator_prev(const Iterator *it)
{
    if (!it || !it->data_state)
    {
        return nullptr;
    }

    ListIteratorState *state = it->data_state;
    if (!state->current)
    {
        return nullptr;
    }

    void *data = state->current->data;
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
static void dll_iterator_reset(const Iterator *it)
{
    if (!it || !it->data_state)
    {
        return;
    }

    ListIteratorState *state = it->data_state;
    state->current = state->start;
}

/**
 * Check if iterator is valid.
 */
static int dll_iterator_is_valid(const Iterator *it)
{
    if (!it || !it->data_state)
    {
        return 0;
    }

    const ListIteratorState *state = it->data_state;
    return state->list != nullptr;
}

/**
 * Free resources used by iterator.
 */
static void dll_iterator_destroy(Iterator *it)
{
    if (!it || !it->data_state)
    {
        return;
    }

    free(it->data_state);
    it->data_state = nullptr;
}

/**
 * Get current element without advancing iterator.
 */
static void *dll_iterator_get(const Iterator *it)
{
    if (!it || !it->data_state)
    {
        return nullptr;
    }

    const ListIteratorState *state = it->data_state;
    if (!state->current)
    {
        return nullptr;
    }

    return state->current->data;
}

//==============================================================================
// Creation and destruction functions
//==============================================================================

DLinkedList *dll_create(void)
{
    return dll_create_custom(malloc, free);
}

DLinkedList *dll_create_custom(alloc_func alloc, dealloc_func dealloc)
{
    if (!alloc)
    {
        alloc = malloc;
    }

    if (!dealloc)
    {
        dealloc = free;
    }

    DLinkedList *list = alloc(sizeof(DLinkedList));
    if (!list)
    {
        return nullptr;
    }

    list->head = nullptr;
    list->tail = nullptr;
    list->size = 0;
    list->alloc = alloc;
    list->dealloc = dealloc;

    return list;
}

void dll_destroy(DLinkedList *list, const free_func data_free)
{
    if (!list)
    {
        return;
    }

    DListNode *node = list->head;
    while (node)
    {
        DListNode *next = node->next;
        if (data_free && node->data)
        {
            data_free(node->data);
        }

        list->dealloc(node);
        node = next;
    }
    list->dealloc(list);
}

void dll_clear(DLinkedList *list, const free_func data_free)
{
    if (!list)
    {
        return;
    }

    DListNode *node = list->head;
    while (node)
    {
        DListNode *next = node->next;
        if (data_free && node->data)
        {
            data_free(node->data);
        }

        list->dealloc(node);
        node = next;
    }

    list->head = nullptr;
    list->tail = nullptr;
    list->size = 0;
}

//==============================================================================
// Information functions
//==============================================================================

size_t dll_size(const DLinkedList *list)
{
    if (!list)
    {
        return 0;
    }

    return list->size;
}

int dll_is_empty(const DLinkedList *list)
{
    return !list || list->size == 0;
}

DListNode *dll_find(DLinkedList *list, const void *data, const cmp_func compare)
{
    if (!list || !compare)
    {
        return nullptr;
    }

    DListNode *curr = list->head;
    while (curr)
    {
        if (compare(curr->data, data) == 0)
        {
            return curr;
        }

        curr = curr->next;
    }

    return nullptr;
}

int dll_equals(const DLinkedList *list1, const DLinkedList *list2, const cmp_func compare)
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
    const DListNode *node1 = list1->head;
    const DListNode *node2 = list2->head;

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

int dll_insert_front(DLinkedList *list, void *data)
{
    if (!list)
    {
        return -1;
    }

    DListNode *node = list->alloc(sizeof(DListNode));
    if (!node)
    {
        return -1;
    }

    node->data = data;
    node->prev = nullptr;
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

int dll_insert_back(DLinkedList *list, void *data)
{
    if (!list)
    {
        return -1;
    }

    DListNode *node = list->alloc(sizeof(DListNode));
    if (!node)
    {
        return -1;
    }

    node->data = data;
    node->next = nullptr;
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

int dll_insert_at(DLinkedList *list, const size_t pos, void *data)
{
    if (!list || pos > list->size)
    {
        return -1;
    }

    if (pos == 0)
    {
        return dll_insert_front(list, data);
    }

    if (pos == list->size)
    {
        return dll_insert_back(list, data);
    }

    DListNode *node = list->alloc(sizeof(DListNode));
    if (!node)
    {
        return -1;
    }

    node->data = data;

    // Determine if it's more efficient to start from head or tail
    if (pos <= list->size / 2)
    {
        // Start from head and move forward
        DListNode *curr = list->head;
        for (size_t i = 0; i < pos - 1; ++i)
        {
            curr = curr->next;
        }

        node->next = curr->next;
        node->prev = curr;
        curr->next->prev = node;
        curr->next = node;
    }
    else
    {
        // Start from tail and move backward
        DListNode *curr = list->tail;
        for (size_t i = list->size - 1; i > pos; --i)
        {
            curr = curr->prev;
        }

        node->prev = curr->prev;
        node->next = curr;
        curr->prev->next = node;
        curr->prev = node;
    }

    list->size++;

    return 0;
}

//==============================================================================
// Removal functions
//==============================================================================

int dll_remove(DLinkedList *list, const void *data, const cmp_func compare, const free_func remove)
{
    if (!list || !compare || list->size == 0)
    {
        return -1;
    }

    DListNode *curr = list->head;
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

            if (remove && curr->data)
            {
                remove(curr->data);
            }
            list->dealloc(curr);
            list->size--;

            return 0;
        }
        curr = curr->next;
    }

    return -1;  // Node not found
}

int dll_remove_at(DLinkedList *list, const size_t pos, const free_func remove)
{
    if (!list || pos >= list->size)
    {
        return -1;
    }

    DListNode *node_to_remove;

    if (pos == 0)
    {
        // Remove head
        node_to_remove = list->head;
        list->head = node_to_remove->next;

        if (list->head)
        {
            list->head->prev = nullptr;
        }
        else
        {
            // Removed the only node
            list->tail = nullptr;
        }
    }
    else if (pos == list->size - 1)
    {
        // Remove tail
        node_to_remove = list->tail;
        list->tail = node_to_remove->prev;
        list->tail->next = nullptr;
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

    if (remove && node_to_remove->data)
    {
        remove(node_to_remove->data);
    }
    list->dealloc(node_to_remove);
    list->size--;

    return 0;
}

int dll_remove_front(DLinkedList *list, const free_func remove)
{
    if (!list || !list->head)
    {
        return -1;
    }

    DListNode *node_to_remove = list->head;
    list->head = node_to_remove->next;

    if (list->head)
    {
        list->head->prev = nullptr;
    }
    else
    {
        // Removed the only node
        list->tail = nullptr;
    }

    if (remove && node_to_remove->data)
    {
        remove(node_to_remove->data);
    }

    list->dealloc(node_to_remove);
    list->size--;

    return 0;
}

int dll_remove_back(DLinkedList *list, const free_func remove)
{
    if (!list || !list->tail)
    {
        return -1;
    }

    DListNode *node_to_remove = list->tail;
    list->tail = node_to_remove->prev;

    if (list->tail)
    {
        list->tail->next = nullptr;
    }
    else
    {
        // Removed the only node
        list->head = nullptr;
    }

    if (remove && node_to_remove->data)
    {
        remove(node_to_remove->data);
    }

    list->dealloc(node_to_remove);
    list->size--;
    return 0;
}

//==============================================================================
// List manipulation functions
//==============================================================================

int dll_sort(DLinkedList *list, const cmp_func compare)
{
    if (!list || !compare || list->size <= 1)
    {
        return !list || !compare ? -1 : 0;
    }

    // Perform the merge sort
    list->head = dll_merge_sort(list->head, compare);

    // Update the tail pointer
    DListNode *current = list->head;
    while (current && current->next)
    {
        current = current->next;
    }
    list->tail = current;

    return 0;
}

int dll_reverse(DLinkedList *list)
{
    if (!list)
    {
        return -1;
    }

    if (list->size <= 1)
    {
        return 0; // Empty or single-element list is already reversed
    }

    DListNode *current = list->head;
    DListNode *temp;

    // Swap next and prev pointers for all nodes
    while (current)
    {
        // Swap next and prev pointers
        temp = current->prev;
        current->prev = current->next;
        current->next = temp;

        // Move to the next node (which is now current->prev because we swapped)
        current = current->prev;
    }

    // Swap head and tail pointers
    temp = list->head;
    list->head = list->tail;
    list->tail = temp;

    return 0;
}

int dll_merge(DLinkedList *dest, DLinkedList *src)
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
        src->head->prev = dest->tail;

        // Update dest's tail
        dest->tail = src->tail;

        // Update size
        dest->size += src->size;
    }

    // Clear src list without destroying nodes
    src->head = nullptr;
    src->tail = nullptr;
    src->size = 0;

    return 0;
}

int dll_splice(DLinkedList *dest, DLinkedList *src, const size_t pos)
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
            src->tail->next = dest->head;
            dest->head->prev = src->tail;
            dest->head = src->head;
        }
    }
    // If inserting at the end
    else if (pos == dest->size)
    {
        dest->tail->next = src->head;
        src->head->prev = dest->tail;
        dest->tail = src->tail;
    }
    // If inserting in the middle
    else
    {
        DListNode *curr;

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
        src->head->prev = curr->prev;

        // Connect src's last node to the node at position pos
        src->tail->next = curr;
        curr->prev = src->tail;
    }

    // Update size
    dest->size += src->size;

    // Clear src list without destroying nodes
    src->head = nullptr;
    src->tail = nullptr;
    src->size = 0;

    return 0;
}

//==============================================================================
// Higher-order functions
//==============================================================================

DLinkedList *dll_filter(DLinkedList *list, const pred_func pred)
{
    if (!list || !pred)
    {
        return nullptr;
    }

    DLinkedList *result = dll_create_custom(list->alloc, list->dealloc);
    if (!result)
    {
        return nullptr;
    }

    const DListNode *curr = list->head;
    while (curr)
    {
        if (pred(curr->data))
        {
            // Include elements that match the predicate
            if (dll_insert_back(result, curr->data) != 0)
            {
                dll_destroy(result, nullptr); // Don't free data; it's still in source list
                return nullptr;
            }
        }
        curr = curr->next;
    }

    return result;
}

DLinkedList *dll_transform(DLinkedList *list, const transform_func transform)
{
    if (!list || !transform)
    {
        return nullptr;
    }

    DLinkedList *result = dll_create_custom(list->alloc, list->dealloc);
    if (!result)
    {
        return nullptr;
    }

    const DListNode *curr = list->head;
    while (curr)
    {
        void *new_data = transform(curr->data);
        if (dll_insert_back(result, new_data) != 0)
        {
            if (new_data)
            {
                free(new_data);
            }
            dll_destroy(result, free); // Free transformed data
            return nullptr;
        }
        curr = curr->next;
    }

    return result;
}

void dll_for_each(DLinkedList *list, const action_func action)
{
    if (!list || !action)
    {
        return;
    }

    const DListNode *curr = list->head;
    while (curr)
    {
        action(curr->data);
        curr = curr->next;
    }
}

//==============================================================================
// List copying functions
//==============================================================================

DLinkedList *dll_copy(DLinkedList *list)
{
    if (!list)
    {
        return nullptr;
    }

    DLinkedList *clone = dll_create_custom(list->alloc, list->dealloc);
    if (!clone)
    {
        return nullptr;
    }

    // Empty list case
    if (list->size == 0)
    {
        return clone;
    }

    // Copy nodes, sharing data pointers
    const DListNode *curr = list->head;
    while (curr)
    {
        if (dll_insert_back(clone, curr->data) != 0)
        {
            dll_destroy(clone, nullptr);  // Don't free data - they're shared
            return nullptr;
        }
        curr = curr->next;
    }

    return clone;
}

DLinkedList *dll_copy_deep(DLinkedList *list, const copy_func copy_data)
{
    if (!list || !copy_data)
    {
        return nullptr;
    }

    DLinkedList *clone = dll_create_custom(list->alloc, list->dealloc);
    if (!clone)
    {
        return nullptr;
    }

    // Empty list case
    if (list->size == 0)
    {
        return clone;
    }

    // Copy nodes and clone data
    const DListNode *curr = list->head;
    while (curr)
    {
        void *data_copy = copy_data(curr->data);
        if (!data_copy)
        {
            dll_destroy(clone, free);  // Free cloned data if failure
            return nullptr;
        }

        if (dll_insert_back(clone, data_copy) != 0)
        {
            free(data_copy);
            dll_destroy(clone, free);  // Free cloned data if failure
            return nullptr;
        }
        curr = curr->next;
    }

    return clone;
}

DLinkedList *dll_from_iterator(Iterator *it, const copy_func copy)
{
    if (!it || !it->is_valid(it))
    {
        return nullptr;
    }

    DLinkedList *list = dll_create();
    if (!list)
    {
        return nullptr;
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
                dll_destroy(list, free);
                return nullptr;
            }
        }

        if (dll_insert_back(list, data_to_insert) != 0)
        {
            if (copy)
            {
                free(data_to_insert);  // Free the copy we just made
            }
            dll_destroy(list, copy ? free : nullptr);
            return nullptr;
        }
    }

    return list;
}

//==============================================================================
// Iterator functions
//==============================================================================

Iterator dll_iterator(DLinkedList *list)
{
    Iterator it = {};  // Initialize all fields to nullptr/0

    it.get = dll_iterator_get;
    it.next = dll_iterator_next;
    it.has_next = dll_iterator_has_next;
    it.prev = dll_iterator_prev;
    it.has_prev = dll_iterator_has_prev;
    it.reset = dll_iterator_reset;
    it.is_valid = dll_iterator_is_valid;
    it.destroy = dll_iterator_destroy;

    if (!list)
    {
        return it;
    }

    ListIteratorState *state = calloc(1, sizeof(ListIteratorState));
    if (!state)
    {
        return it;
    }

    state->current = list->head;
    state->start = list->head;
    state->list = list;

    it.data_state = state;

    return it;
}

Iterator dll_iterator_reverse(DLinkedList *list)
{
    Iterator it = {};  // Initialize all fields to nullptr/0

    it.get = dll_iterator_get;
    it.next = dll_iterator_next;
    it.has_next = dll_iterator_has_next;
    it.prev = dll_iterator_prev;
    it.has_prev = dll_iterator_has_prev;
    it.reset = dll_iterator_reset;
    it.is_valid = dll_iterator_is_valid;
    it.destroy = dll_iterator_destroy;

    if (!list)
    {
        return it;
    }

    ListIteratorState *state = calloc(1, sizeof(ListIteratorState));
    if (!state)
    {
        return it;
    }

    state->current = list->tail;
    state->start = list->tail;
    state->list = list;

    it.data_state = state;

    return it;
}