/**
 * @file linkedList.c
 * @brief Doubly linked list implementation
 *
 * Source implementation for the LinkedList data structure that provides
 * O(1) insertion and deletion at both ends, and O(n) access to arbitrary elements.
 */

#include "linkedList.h"
#include <string.h>
#include <stdlib.h>

/**
 * @brief Initialize a LinkedList with specified element size
 *
 * Sets up the internal structure of the linked list by initializing all fields.
 * The LinkedList must be freed with zzLinkedListFree when done.
 *
 * @param[out] ll Pointer to LinkedList to initialize
 * @param[in] elSize Size of each element in bytes (must be > 0)
 * @param[in] elemFree Optional callback to free elements (can be NULL)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: LinkedList initialized successfully
 *         - ERROR: Possible errors:
 *           * "LinkedList pointer is NULL"
 *           * "Element size cannot be zero"
 *
 * @see zzLinkedListFree
 */
zzOpResult zzLinkedListInit(zzLinkedList *ll, size_t elSize, zzFreeFn elemFree) {
    if (!ll) return ZZ_ERR("LinkedList pointer is NULL");
    if (elSize == 0) return ZZ_ERR("Element size cannot be zero");

    ll->head = NULL;
    ll->tail = NULL;
    ll->elSize = elSize;
    ll->size = 0;
    ll->elemFree = elemFree;
    return ZZ_OK();
}

/**
 * @brief Free all memory associated with LinkedList
 *
 * Iterates through all nodes, calling the elemFree callback if provided during
 * initialization, then frees all allocated memory for nodes. The LinkedList struct
 * itself is not freed (caller owns it).
 *
 * @param[in,out] ll Pointer to LinkedList to free
 *
 * @note Safe to call with NULL pointer (no-op)
 * @note Does not free the LinkedList struct itself
 */
void zzLinkedListFree(zzLinkedList *ll) {
    if (!ll) return;

    DLNode *cur = ll->head;
    while (cur) {
        DLNode *next = cur->next;
        if (ll->elemFree) ll->elemFree(cur->data);
        free(cur);
        cur = next;
    }
    ll->head = ll->tail = NULL;
    ll->size = 0;
}

/**
 * @brief Add an element to the front of the LinkedList
 *
 * Inserts a copy of the element at the beginning of the list.
 * This operation is O(1).
 *
 * @param[in,out] ll Pointer to initialized LinkedList
 * @param[in] elem Pointer to element data to add (will be copied)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element added successfully
 *         - ERROR: Possible errors:
 *           * "LinkedList pointer is NULL"
 *           * "Element pointer is NULL"
 *           * "Failed to allocate node"
 *
 * @pre ll must be initialized with zzLinkedListInit
 * @pre elem must point to valid data of size elSize
 * @post If successful, al->size increases by 1
 * @post elem data is copied into LinkedList (caller retains ownership)
 *
 * @see zzLinkedListPushBack
 * @see zzLinkedListPopFront
 */
zzOpResult zzLinkedListPushFront(zzLinkedList *ll, const void *elem) {
    if (!ll) return ZZ_ERR("LinkedList pointer is NULL");
    if (!elem) return ZZ_ERR("Element pointer is NULL");

    DLNode *n = malloc(sizeof(DLNode) + ll->elSize);
    if (!n) return ZZ_ERR("Failed to allocate node");

    memcpy(n->data, elem, ll->elSize);
    n->prev = NULL;
    n->next = ll->head;

    if (ll->head) ll->head->prev = n;
    ll->head = n;
    if (!ll->tail) ll->tail = n;

    ll->size++;
    return ZZ_OK();
}

/**
 * @brief Add an element to the back of the LinkedList
 *
 * Inserts a copy of the element at the end of the list.
 * This operation is O(1).
 *
 * @param[in,out] ll Pointer to initialized LinkedList
 * @param[in] elem Pointer to element data to add (will be copied)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element added successfully
 *         - ERROR: Possible errors:
 *           * "LinkedList pointer is NULL"
 *           * "Element pointer is NULL"
 *           * "Failed to allocate node"
 *
 * @pre ll must be initialized with zzLinkedListInit
 * @pre elem must point to valid data of size elSize
 * @post If successful, al->size increases by 1
 * @post elem data is copied into LinkedList (caller retains ownership)
 *
 * @see zzLinkedListPushFront
 * @see zzLinkedListPopBack
 */
zzOpResult zzLinkedListPushBack(zzLinkedList *ll, const void *elem) {
    if (!ll) return ZZ_ERR("LinkedList pointer is NULL");
    if (!elem) return ZZ_ERR("Element pointer is NULL");

    DLNode *n = malloc(sizeof(DLNode) + ll->elSize);
    if (!n) return ZZ_ERR("Failed to allocate node");

    memcpy(n->data, elem, ll->elSize);
    n->prev = ll->tail;
    n->next = NULL;

    if (ll->tail) ll->tail->next = n;
    ll->tail = n;
    if (!ll->head) ll->head = n;

    ll->size++;
    return ZZ_OK();
}

/**
 * @brief Remove an element from the front of the LinkedList
 *
 * Copies the data from the first element to the output buffer and removes the node.
 * This operation is O(1).
 *
 * @param[in,out] ll Pointer to initialized LinkedList
 * @param[out] out Buffer where element data will be copied (must be elSize bytes)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element removed and copied successfully
 *         - ERROR: Possible errors:
 *           * "LinkedList pointer is NULL"
 *           * "Output buffer is NULL"
 *           * "List is empty"
 *
 * @pre ll must be initialized with zzLinkedListInit
 * @pre out must point to buffer of at least elSize bytes
 * @post If successful, al->size decreases by 1
 * @post out contains copy of the removed element data
 *
 * @see zzLinkedListPopBack
 * @see zzLinkedListPushFront
 */
zzOpResult zzLinkedListPopFront(zzLinkedList *ll, void *out) {
    if (!ll) return ZZ_ERR("LinkedList pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (!ll->head) return ZZ_ERR("List is empty");

    DLNode *tmp = ll->head;
    memcpy(out, tmp->data, ll->elSize);

    ll->head = ll->head->next;
    if (ll->head) ll->head->prev = NULL;
    else ll->tail = NULL;

    free(tmp);
    ll->size--;
    return ZZ_OK();
}

/**
 * @brief Remove an element from the back of the LinkedList
 *
 * Copies the data from the last element to the output buffer and removes the node.
 * This operation is O(1).
 *
 * @param[in,out] ll Pointer to initialized LinkedList
 * @param[out] out Buffer where element data will be copied (must be elSize bytes)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element removed and copied successfully
 *         - ERROR: Possible errors:
 *           * "LinkedList pointer is NULL"
 *           * "Output buffer is NULL"
 *           * "List is empty"
 *
 * @pre ll must be initialized with zzLinkedListInit
 * @pre out must point to buffer of at least elSize bytes
 * @post If successful, al->size decreases by 1
 * @post out contains copy of the removed element data
 *
 * @see zzLinkedListPopFront
 * @see zzLinkedListPushBack
 */
zzOpResult zzLinkedListPopBack(zzLinkedList *ll, void *out) {
    if (!ll) return ZZ_ERR("LinkedList pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (!ll->tail) return ZZ_ERR("List is empty");

    DLNode *tmp = ll->tail;
    memcpy(out, tmp->data, ll->elSize);

    ll->tail = ll->tail->prev;
    if (ll->tail) ll->tail->next = NULL;
    else ll->head = NULL;

    free(tmp);
    ll->size--;
    return ZZ_OK();
}

/**
 * @brief Get element at specified index from the LinkedList
 *
 * Retrieves a copy of the element at the specified index.
 * Optimizes access by choosing to traverse from head or tail based on index position.
 * This operation is O(n) in the worst case, but O(min(i, n-i)) where i is the index.
 *
 * @param[in] ll Pointer to initialized LinkedList
 * @param[in] idx Index of element to retrieve (0-based)
 * @param[out] out Buffer where element will be copied (must be elSize bytes)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element retrieved successfully
 *         - ERROR: Possible errors:
 *           * "LinkedList pointer is NULL"
 *           * "Output buffer is NULL"
 *           * "Index out of bounds"
 *
 * @pre ll must be initialized with zzLinkedListInit
 * @pre out must point to buffer of at least elSize bytes
 * @pre idx must be < ll->size
 * @post out contains copy of element data
 *
 * @see zzLinkedListInsert
 * @see zzLinkedListRemove
 */
zzOpResult zzLinkedListGet(const zzLinkedList *ll, size_t idx, void *out) {
    if (!ll) return ZZ_ERR("LinkedList pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (idx >= ll->size) return ZZ_ERR("Index out of bounds");

    DLNode *cur;
    if (idx < ll->size / 2) {
        cur = ll->head;
        for (size_t i = 0; i < idx; i++) cur = cur->next;
    } else {
        cur = ll->tail;
        for (size_t i = ll->size - 1; i > idx; i--) cur = cur->prev;
    }

    memcpy(out, cur->data, ll->elSize);
    return ZZ_OK();
}

/**
 * @brief Insert element at specified index in the LinkedList
 *
 * Inserts a copy of the element at the specified index, shifting existing elements right.
 * This operation is O(n) in the worst case as it may need to traverse to the insertion point.
 *
 * @param[in,out] ll Pointer to initialized LinkedList
 * @param[in] idx Index where element will be inserted (0-based, can be == size)
 * @param[in] elem Pointer to element data to insert (will be copied)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element inserted successfully
 *         - ERROR: Possible errors:
 *           * "LinkedList pointer is NULL"
 *           * "Element pointer is NULL"
 *           * "Index out of bounds"
 *           * "Failed to allocate node"
 *
 * @pre ll must be initialized with zzLinkedListInit
 * @pre elem must point to valid data of size elSize
 * @pre idx must be <= ll->size (can insert at end)
 * @post Element inserted at idx
 * @post Elements at idx and after are shifted right
 * @post ll->size increases by 1
 *
 * @see zzLinkedListPushFront
 * @see zzLinkedListPushBack
 * @see zzLinkedListRemove
 */
zzOpResult zzLinkedListInsert(zzLinkedList *ll, size_t idx, const void *elem) {
    if (!ll) return ZZ_ERR("LinkedList pointer is NULL");
    if (!elem) return ZZ_ERR("Element pointer is NULL");
    if (idx > ll->size) return ZZ_ERR("Index out of bounds");
    if (idx == 0) return zzLinkedListPushFront(ll, elem);
    if (idx == ll->size) return zzLinkedListPushBack(ll, elem);

    DLNode *n = malloc(sizeof(DLNode) + ll->elSize);
    if (!n) return ZZ_ERR("Failed to allocate node");
    memcpy(n->data, elem, ll->elSize);

    DLNode *cur = ll->head;
    for (size_t i = 0; i < idx; i++) cur = cur->next;

    n->next = cur;
    n->prev = cur->prev;
    cur->prev->next = n;
    cur->prev = n;

    ll->size++;
    return ZZ_OK();
}

/**
 * @brief Remove element at specified index from the LinkedList
 *
 * Removes the element at the specified index and shifts remaining elements left.
 * Calls elemFree on removed element if provided.
 * This operation is O(n) in the worst case as it may need to traverse to the removal point.
 *
 * @param[in,out] ll Pointer to initialized LinkedList
 * @param[in] idx Index of element to remove (0-based)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element removed successfully
 *         - ERROR: Possible errors:
 *           * "LinkedList pointer is NULL"
 *           * "Index out of bounds"
 *           * "List is empty"
 *
 * @pre ll must be initialized with zzLinkedListInit
 * @pre idx must be < ll->size
 * @post Element at idx is removed
 * @post Elements after idx are shifted left
 * @post ll->size decreases by 1
 * @post If elemFree provided, removed element has been freed
 *
 * @see zzLinkedListInsert
 * @see zzLinkedListClear
 */
zzOpResult zzLinkedListRemove(zzLinkedList *ll, size_t idx) {
    if (!ll) return ZZ_ERR("LinkedList pointer is NULL");
    if (idx >= ll->size) return ZZ_ERR("Index out of bounds");

    if (idx == 0) {
        // Special case for front
        if (!ll->head) return ZZ_ERR("List is empty");
        DLNode *tmp = ll->head;
        ll->head = ll->head->next;
        if (ll->head) ll->head->prev = NULL;
        else ll->tail = NULL;
        if (ll->elemFree) ll->elemFree(tmp->data);
        free(tmp);
        ll->size--;
        return ZZ_OK();
    }

    if (idx == ll->size - 1) {
        // Special case for back
        if (!ll->tail) return ZZ_ERR("List is empty");
        DLNode *tmp = ll->tail;
        ll->tail = ll->tail->prev;
        if (ll->tail) ll->tail->next = NULL;
        else ll->head = NULL;
        if (ll->elemFree) ll->elemFree(tmp->data);
        free(tmp);
        ll->size--;
        return ZZ_OK();
    }

    DLNode *cur = ll->head;
    for (size_t i = 0; i < idx; i++) cur = cur->next;

    cur->prev->next = cur->next;
    cur->next->prev = cur->prev;

    if (ll->elemFree) ll->elemFree(cur->data);
    free(cur);
    ll->size--;
    return ZZ_OK();
}

/**
 * @brief Clear all elements from LinkedList
 *
 * Removes all elements (calls elemFree if provided) and frees all node memory.
 * The list structure remains but is empty.
 *
 * @param[in,out] ll Pointer to initialized LinkedList
 *
 * @pre ll must be initialized with zzLinkedListInit
 * @post ll->size is 0
 * @post ll->head and ll->tail are NULL
 * @post If elemFree provided, it has been called on all elements
 *
 * @note Safe to call with NULL pointer (no-op)
 *
 * @see zzLinkedListFree
 */
void zzLinkedListClear(zzLinkedList *ll) {
    if (!ll) return;

    DLNode *cur = ll->head;
    while (cur) {
        DLNode *next = cur->next;
        if (ll->elemFree) ll->elemFree(cur->data);
        free(cur);
        cur = next;
    }
    ll->head = ll->tail = NULL;
    ll->size = 0;
}
