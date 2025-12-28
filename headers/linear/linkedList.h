#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "common.h"
#include "result.h"

/**
 * @brief Doubly-linked list node (internal)
 */
typedef struct DLNode {
    struct DLNode *prev;
    struct DLNode *next;
    unsigned char data[];
} DLNode;

/**
 * @brief Doubly-linked list with O(1) operations at both ends
 */
typedef struct zzLinkedList {
    DLNode *head;        /**< First node */
    DLNode *tail;        /**< Last node */
    size_t elSize;       /**< Size of each element */
    size_t size;         /**< Number of elements */
    zzFreeFn elemFree;   /**< Optional free callback */
} zzLinkedList;

/**
 * @brief Initialize a LinkedList
 * @return zzOpResult - SUCCESS or error message
 */
zzOpResult zzLinkedListInit(zzLinkedList *ll, size_t elSize, zzFreeFn elemFree);

/**
 * @brief Free all memory associated with LinkedList
 */
void zzLinkedListFree(zzLinkedList *ll);

/**
 * @brief Push element to front of list
 * @return zzOpResult - SUCCESS or error message
 */
zzOpResult zzLinkedListPushFront(zzLinkedList *ll, const void *elem);

/**
 * @brief Push element to back of list
 * @return zzOpResult - SUCCESS or error message
 */
zzOpResult zzLinkedListPushBack(zzLinkedList *ll, const void *elem);

/**
 * @brief Pop element from front of list
 * @return zzOpResult - SUCCESS or error message
 */
zzOpResult zzLinkedListPopFront(zzLinkedList *ll, void *out);

/**
 * @brief Pop element from back of list
 * @return zzOpResult - SUCCESS or error message
 */
zzOpResult zzLinkedListPopBack(zzLinkedList *ll, void *out);

/**
 * @brief Get element at index
 * @return zzOpResult - SUCCESS or error message
 */
zzOpResult zzLinkedListGet(const zzLinkedList *ll, size_t idx, void *out);

/**
 * @brief Insert element at index
 * @return zzOpResult - SUCCESS or error message
 */
zzOpResult zzLinkedListInsert(zzLinkedList *ll, size_t idx, const void *elem);

/**
 * @brief Remove element at index
 * @return zzOpResult - SUCCESS or error message
 */
zzOpResult zzLinkedListRemove(zzLinkedList *ll, size_t idx);

/**
 * @brief Clear all elements from LinkedList
 */
void zzLinkedListClear(zzLinkedList *ll);

#endif
