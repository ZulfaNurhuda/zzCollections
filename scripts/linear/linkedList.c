/**
 * @file linkedList.c
 * @brief Implementation of the doubly-linked list data structure.
 *
 * This module provides the implementation for the LinkedList data structure,
 * which offers efficient insertion and removal operations at both ends of the list.
 * The implementation handles memory management and provides O(1) operations for
 * adding and removing elements from the front and back of the list.
 */

#include "linkedList.h"
#include <string.h>
#include <stdlib.h>

/**
 * @brief Initializes a new LinkedList with the specified element size.
 *
 * This function initializes a LinkedList structure with the given element size
 * and custom free function. The list will be empty after initialization.
 *
 * @param[out] ll Pointer to the LinkedList structure to initialize
 * @param[in] elSize Size in bytes of each element that will be stored in the list
 * @param[in] elemFree Function to free individual elements when they are removed or the list is freed, or NULL if not needed
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
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
 * @brief Frees all resources associated with the LinkedList.
 *
 * This function releases all memory used by the LinkedList, including calling
 * the custom free function for each element if provided. After this function
 * returns, the LinkedList structure should not be used until reinitialized.
 *
 * @param[in,out] ll Pointer to the LinkedList to free
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
 * @brief Adds an element to the front of the LinkedList.
 *
 * This function adds the specified element to the front of the list. The element
 * is copied into a newly allocated node. The list size is increased by one.
 *
 * @param[in,out] ll Pointer to the LinkedList to add to
 * @param[in] elem Pointer to the element to add to the front (contents will be copied)
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
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
 * @brief Adds an element to the back of the LinkedList.
 *
 * This function adds the specified element to the back of the list. The element
 * is copied into a newly allocated node. The list size is increased by one.
 *
 * @param[in,out] ll Pointer to the LinkedList to add to
 * @param[in] elem Pointer to the element to add to the back (contents will be copied)
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
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
 * @brief Removes an element from the front of the LinkedList.
 *
 * This function removes the element from the front of the list and copies it
 * to the output buffer. The list size is decreased by one. If a custom free
 * function was provided, it will not be called since the element is returned.
 *
 * @param[in,out] ll Pointer to the LinkedList to remove from
 * @param[out] out Pointer to a buffer where the removed element will be copied
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
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
 * @brief Removes an element from the back of the LinkedList.
 *
 * This function removes the element from the back of the list and copies it
 * to the output buffer. The list size is decreased by one. If a custom free
 * function was provided, it will not be called since the element is returned.
 *
 * @param[in,out] ll Pointer to the LinkedList to remove from
 * @param[out] out Pointer to a buffer where the removed element will be copied
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
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
 * @brief Gets an element at the specified index in the LinkedList.
 *
 * This function retrieves the element at the given index in the list and copies
 * it to the output buffer. The index is relative to the logical order of elements
 * in the list, starting from the head. The implementation optimizes traversal
 * by choosing to start from either head or tail depending on which is closer.
 *
 * @param[in] ll Pointer to the LinkedList to retrieve from
 * @param[in] idx Index of the element to retrieve (0-based, from head)
 * @param[out] out Pointer to a buffer where the element will be copied
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
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
 * @brief Inserts an element at the specified index in the LinkedList.
 *
 * This function inserts the specified element at the given index in the list.
 * The element is copied into a newly allocated node. All elements at and after
 * the specified index will be shifted one position to the right. Special cases
 * for inserting at the beginning or end of the list are handled efficiently.
 *
 * @param[in,out] ll Pointer to the LinkedList to insert into
 * @param[in] idx Index at which to insert the element (0-based, from head)
 * @param[in] elem Pointer to the element to insert (contents will be copied)
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
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
 * @brief Removes an element at the specified index from the LinkedList.
 *
 * This function removes the element at the given index from the list and frees
 * the corresponding node. If a custom free function was provided, it will be
 * called on the removed element. All elements after the specified index will
 * be shifted one position to the left. Special cases for removing from the
 * beginning or end of the list are handled efficiently.
 *
 * @param[in,out] ll Pointer to the LinkedList to remove from
 * @param[in] idx Index of the element to remove (0-based, from head)
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzLinkedListRemove(zzLinkedList *ll, size_t idx) {
    if (!ll) return ZZ_ERR("LinkedList pointer is NULL");
    if (idx >= ll->size) return ZZ_ERR("Index out of bounds");

    if (idx == 0) {
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
 * @brief Clears all elements from the LinkedList.
 *
 * This function removes all elements from the list by calling the custom free
 * function on each element (if provided) and deallocating all nodes. The list
 * becomes empty after this operation.
 *
 * @param[in,out] ll Pointer to the LinkedList to clear
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