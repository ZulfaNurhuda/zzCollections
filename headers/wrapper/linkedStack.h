/**
 * @file linkedStack.h
 * @brief LIFO stack wrapper around LinkedList
 *
 * Provides stack semantics (push/pop) using LinkedList as underlying storage.
 * All operations have O(1) time complexity.
 */

#ifndef LINKED_STACK_H
#define LINKED_STACK_H

#include "linkedList.h"
#include <string.h>

typedef zzLinkedList zzLinkedStack;

/**
 * @brief Initialize LinkedStack
 * @see zzLinkedListInit for detailed documentation
 */
static inline zzOpResult zzLinkedStackInit(zzLinkedStack *s, size_t elSize, zzFreeFn elemFree) {
    return zzLinkedListInit(s, elSize, elemFree);
}

/**
 * @brief Free LinkedStack
 * @see zzLinkedListFree for detailed documentation
 */
static inline void zzLinkedStackFree(zzLinkedStack *s) {
    zzLinkedListFree(s);
}

/**
 * @brief Push element onto stack (LIFO)
 * @see zzLinkedListPushBack for detailed documentation
 */
static inline zzOpResult zzLinkedStackPush(zzLinkedStack *s, const void *elem) {
    return zzLinkedListPushBack(s, elem);
}

/**
 * @brief Pop element from stack
 * @see zzLinkedListPopBack for detailed documentation
 */
static inline zzOpResult zzLinkedStackPop(zzLinkedStack *s, void *out) {
    return zzLinkedListPopBack(s, out);
}

/**
 * @brief Peek at top element without removing
 *
 * @param[in] s Pointer to LinkedStack
 * @param[out] out Buffer where element will be copied
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element peeked successfully
 *         - ERROR: Possible errors:
 *           * "LinkedList pointer is NULL"
 *           * "Output buffer is NULL"
 *           * "List is empty"
 */
static inline zzOpResult zzLinkedStackPeek(const zzLinkedStack *s, void *out) {
    if (!s) return ZZ_ERR("LinkedList pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (!s->tail) return ZZ_ERR("List is empty");

    memcpy(out, s->tail->data, s->elSize);
    return ZZ_OK();
}

/**
 * @brief Check if stack is empty
 * @return true if empty, false otherwise
 */
static inline bool zzLinkedStackIsEmpty(const zzLinkedStack *s) {
    return s->size == 0;
}

/**
 * @brief Get stack size
 * @return Number of elements in stack
 */
static inline size_t zzLinkedStackSize(const zzLinkedStack *s) {
    return s->size;
}

/**
 * @brief Clear all elements from stack
 * @see zzLinkedListClear for detailed documentation
 */
static inline void zzLinkedStackClear(zzLinkedStack *s) {
    zzLinkedListClear(s);
}

#endif
