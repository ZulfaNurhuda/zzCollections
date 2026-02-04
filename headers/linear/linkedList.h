/**
 * @file linkedList.h
 * @brief Doubly-linked list implementation for dynamic data storage.
 *
 * This module implements a doubly-linked list that allows efficient insertion
 * and removal of elements at both ends of the list in O(1) time. The list
 * supports generic element types and provides memory management through
 * customizable free functions.
 */

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "types.h"
#include "utils.h"
#include "result.h"
#include "iterator.h"

/**
 * @brief Structure representing a node in the doubly-linked list.
 *
 * Each node contains pointers to the previous and next nodes in the list,
 * as well as a flexible array member to store the actual element data.
 */
typedef struct DLNode {
    struct DLNode *prev; /**< Pointer to the previous node in the list */
    struct DLNode *next; /**< Pointer to the next node in the list */
    unsigned char data[]; /**< Flexible array member to store the element data */
} DLNode;

/**
 * @brief Structure representing a doubly-linked list.
 *
 * This structure maintains pointers to the head and tail nodes of the list,
 * tracks the current size, element size, and provides a custom free function
 * for element cleanup.
 */
typedef struct zzLinkedList {
    DLNode *head;    /**< Pointer to the first node in the list, or NULL if empty */
    DLNode *tail;    /**< Pointer to the last node in the list, or NULL if empty */
    size_t size;     /**< Current number of elements in the list */
    size_t elSize;   /**< Size in bytes of each individual element */
    zzFreeFn elemFree; /**< Function to free individual elements, or NULL if not needed */
} zzLinkedList;

/**
 * @brief Structure representing an iterator for LinkedList.
 *
 * This structure provides forward iteration through a LinkedList,
 * maintaining the current node and reference to the list.
 */
typedef struct zzLinkedListIterator {
    zzLinkedList *list;       /**< Pointer to the LinkedList being iterated */
    DLNode *current;          /**< Pointer to the current node */
    zzIteratorState state;    /**< Current state of the iterator */
} zzLinkedListIterator;

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
zzOpResult zzLinkedListInit(zzLinkedList *ll, size_t elSize, zzFreeFn elemFree);

/**
 * @brief Frees all resources associated with the LinkedList.
 *
 * This function releases all memory used by the LinkedList, including calling
 * the custom free function for each element if provided. After this function
 * returns, the LinkedList structure should not be used until reinitialized.
 *
 * @param[in,out] ll Pointer to the LinkedList to free
 */
void zzLinkedListFree(zzLinkedList *ll);

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
zzOpResult zzLinkedListPushFront(zzLinkedList *ll, const void *elem);

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
zzOpResult zzLinkedListPushBack(zzLinkedList *ll, const void *elem);

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
zzOpResult zzLinkedListPopFront(zzLinkedList *ll, void *out);

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
zzOpResult zzLinkedListPopBack(zzLinkedList *ll, void *out);

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
zzOpResult zzLinkedListGet(const zzLinkedList *ll, size_t idx, void *out);

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
zzOpResult zzLinkedListInsert(zzLinkedList *ll, size_t idx, const void *elem);

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
zzOpResult zzLinkedListRemove(zzLinkedList *ll, size_t idx);

/**
 * @brief Clears all elements from the LinkedList.
 *
 * This function removes all elements from the list by calling the custom free
 * function on each element (if provided) and deallocating all nodes. The list
 * becomes empty after this operation.
 *
 * @param[in,out] ll Pointer to the LinkedList to clear
 */
void zzLinkedListClear(zzLinkedList *ll);

/**
 * @brief Initializes an iterator for the LinkedList.
 *
 * This function initializes an iterator to traverse the LinkedList from
 * the head to the tail. The iterator will be positioned at the first
 * element if the list is not empty.
 *
 * @param[out] it Pointer to the iterator structure to initialize
 * @param[in] ll Pointer to the LinkedList to iterate over
 */
void zzLinkedListIteratorInit(zzLinkedListIterator *it, zzLinkedList *ll);

/**
 * @brief Advances the iterator to the next element.
 *
 * This function moves the iterator to the next element in the LinkedList
 * and copies the current element to the output buffer. Returns false when
 * the iterator reaches the end of the list.
 *
 * @param[in,out] it Pointer to the iterator to advance
 * @param[out] valueOut Pointer to a buffer where the current element will be copied
 * @return true if an element was retrieved, false if the iterator reached the end
 */
bool zzLinkedListIteratorNext(zzLinkedListIterator *it, void *valueOut);

/**
 * @brief Checks if the iterator has more elements.
 *
 * This function checks whether the iterator can advance to another element
 * without actually advancing it.
 *
 * @param[in] it Pointer to the iterator to check
 * @return true if there are more elements, false otherwise
 */
bool zzLinkedListIteratorHasNext(const zzLinkedListIterator *it);

/**
 * @brief Removes the last element returned by the iterator.
 *
 * This function removes the element that was most recently returned by
 * zzLinkedListIteratorNext. After removal, the iterator remains valid and
 * continues to the next element on the next call to Next.
 *
 * @param[in,out] it Pointer to the iterator
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzLinkedListIteratorRemove(zzLinkedListIterator *it);

#endif