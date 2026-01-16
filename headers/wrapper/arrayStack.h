/**
 * @file arrayStack.h
 * @brief LIFO stack wrapper around ArrayDeque
 *
 * Provides stack semantics (push/pop) using ArrayDeque as underlying storage.
 * All operations have O(1) time complexity.
 */

#ifndef ARRAY_STACK_H
#define ARRAY_STACK_H

#include "arrayDeque.h"

typedef zzArrayDeque zzArrayStack;

/**
 * @brief Initialize ArrayStack
 * @see zzArrayDequeInit for detailed documentation
 */
static inline zzOpResult zzArrayStackInit(zzArrayStack *s, size_t elSize, size_t capacity, zzFreeFn elemFree) {
    return zzArrayDequeInit(s, elSize, capacity, elemFree);
}

/**
 * @brief Free ArrayStack
 * @see zzArrayDequeFree for detailed documentation
 */
static inline void zzArrayStackFree(zzArrayStack *s) {
    zzArrayDequeFree(s);
}

/**
 * @brief Push element onto stack (LIFO)
 * @see zzArrayDequePushBack for detailed documentation
 */
static inline zzOpResult zzArrayStackPush(zzArrayStack *s, const void *elem) {
    return zzArrayDequePushBack(s, elem);
}

/**
 * @brief Pop element from stack
 * @see zzArrayDequePopBack for detailed documentation
 */
static inline zzOpResult zzArrayStackPop(zzArrayStack *s, void *out) {
    return zzArrayDequePopBack(s, out);
}

/**
 * @brief Peek at top element without removing
 * @see zzArrayDequePeekBack for detailed documentation
 */
static inline zzOpResult zzArrayStackPeek(const zzArrayStack *s, void *out) {
    return zzArrayDequePeekBack(s, out);
}

/**
 * @brief Check if stack is empty
 * @return true if empty, false otherwise
 */
static inline bool zzArrayStackIsEmpty(const zzArrayStack *s) {
    return s->size == 0;
}

/**
 * @brief Get stack size
 * @return Number of elements in stack
 */
static inline size_t zzArrayStackSize(const zzArrayStack *s) {
    return s->size;
}

/**
 * @brief Clear all elements from stack
 * @see zzArrayDequeClear for detailed documentation
 */
static inline void zzArrayStackClear(zzArrayStack *s) {
    zzArrayDequeClear(s);
}

#endif
