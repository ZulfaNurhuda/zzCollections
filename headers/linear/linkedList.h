/**
 * @file linkedList.h
 * @brief Doubly-linked list implementation
 *
 * LinkedList provides O(1) insertions and deletions at both ends.
 * Efficient for frequent insertions/deletions at arbitrary positions.
 */

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "types.h"
#include "utils.h"
#include "result.h"

/**
 * @brief Doubly-linked list node (internal structure)
 *
 * Internal node structure for the doubly-linked list implementation.
 * Contains pointers to previous and next nodes, with flexible array
 * member for element data storage.
 */
typedef struct DLNode {
    struct DLNode *prev;    /**< Pointer to previous node */
    struct DLNode *next;    /**< Pointer to next node */
    unsigned char data[];   /**< Flexible array member for element data */
} DLNode;

/**
 * @brief Doubly-linked list with O(1) operations at both ends
 *
 * LinkedList provides efficient insertions and deletions at both ends
 * and at arbitrary positions. Uses O(n) memory with good cache locality
 * for sequential access patterns.
 */
typedef struct zzLinkedList {
    DLNode *head;        /**< Pointer to first node (NULL if empty) */
    DLNode *tail;        /**< Pointer to last node (NULL if empty) */
    size_t elSize;       /**< Size of each element in bytes */
    size_t size;         /**< Current number of elements */
    zzFreeFn elemFree;   /**< Optional callback to free elements */
} zzLinkedList;

/**
 * @brief Initialize a LinkedList
 *
 * @param[out] ll Pointer to LinkedList to initialize
 * @param[in] elSize Size of each element in bytes (must be > 0)
 * @param[in] elemFree Optional callback to free elements (can be NULL)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: LinkedList initialized successfully
 *         - ERROR: "LinkedList pointer is NULL" / "Element size cannot be zero"
 *
 * @see zzLinkedListFree
 */
zzOpResult zzLinkedListInit(zzLinkedList *ll, size_t elSize, zzFreeFn elemFree);

/**
 * @brief Free all memory associated with LinkedList
 *
 * @param[in,out] ll Pointer to LinkedList to free
 *
 * @note Safe to call with NULL pointer (no-op)
 */
void zzLinkedListFree(zzLinkedList *ll);

/**
 * @brief Push element to front of list
 *
 * @param[in,out] ll Pointer to LinkedList
 * @param[in] elem Pointer to element data to push (will be copied)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element pushed successfully
 *         - ERROR: "LinkedList pointer is NULL" / "Element pointer is NULL" / "Failed to allocate node memory"
 *
 * @see zzLinkedListPopFront
 */
zzOpResult zzLinkedListPushFront(zzLinkedList *ll, const void *elem);

/**
 * @brief Push element to back of list
 *
 * @param[in,out] ll Pointer to LinkedList
 * @param[in] elem Pointer to element data to push (will be copied)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element pushed successfully
 *         - ERROR: "LinkedList pointer is NULL" / "Element pointer is NULL" / "Failed to allocate node memory"
 *
 * @see zzLinkedListPopBack
 */
zzOpResult zzLinkedListPushBack(zzLinkedList *ll, const void *elem);

/**
 * @brief Pop element from front of list
 *
 * @param[in,out] ll Pointer to LinkedList
 * @param[out] out Buffer where element will be copied (must be elSize bytes)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element popped successfully
 *         - ERROR: "LinkedList pointer is NULL" / "Output buffer is NULL" / "List is empty"
 *
 * @see zzLinkedListPushFront
 */
zzOpResult zzLinkedListPopFront(zzLinkedList *ll, void *out);

/**
 * @brief Pop element from back of list
 *
 * @param[in,out] ll Pointer to LinkedList
 * @param[out] out Buffer where element will be copied (must be elSize bytes)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element popped successfully
 *         - ERROR: "LinkedList pointer is NULL" / "Output buffer is NULL" / "List is empty"
 *
 * @see zzLinkedListPushBack
 */
zzOpResult zzLinkedListPopBack(zzLinkedList *ll, void *out);

/**
 * @brief Get element at specified index
 *
 * @param[in] ll Pointer to LinkedList
 * @param[in] idx Index of element to retrieve (0-based)
 * @param[out] out Buffer where element will be copied (must be elSize bytes)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element retrieved successfully
 *         - ERROR: "LinkedList pointer is NULL" / "Output buffer is NULL" / "Index out of bounds"
 *
 * @note Time complexity: O(n) - traverses from head or tail depending on index
 */
zzOpResult zzLinkedListGet(const zzLinkedList *ll, size_t idx, void *out);

/**
 * @brief Insert element at specified index
 *
 * @param[in,out] ll Pointer to LinkedList
 * @param[in] idx Index where element should be inserted (0-based)
 * @param[in] elem Pointer to element data to insert (will be copied)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element inserted successfully
 *         - ERROR: "LinkedList pointer is NULL" / "Element pointer is NULL" / "Index out of bounds" / "Failed to allocate node memory"
 *
 * @note Time complexity: O(n) for middle insertions, O(1) for front/back
 */
zzOpResult zzLinkedListInsert(zzLinkedList *ll, size_t idx, const void *elem);

/**
 * @brief Remove element at specified index
 *
 * @param[in,out] ll Pointer to LinkedList
 * @param[in] idx Index of element to remove (0-based)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element removed successfully
 *         - ERROR: "LinkedList pointer is NULL" / "Index out of bounds"
 *
 * @note Time complexity: O(n) for middle removals, O(1) for front/back
 */
zzOpResult zzLinkedListRemove(zzLinkedList *ll, size_t idx);

/**
 * @brief Clear all elements from LinkedList
 *
 * @param[in,out] ll Pointer to LinkedList
 *
 * @post ll->size is 0, head and tail are NULL
 */
void zzLinkedListClear(zzLinkedList *ll);

#endif /* LINKED_LIST_H */
