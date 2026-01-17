/**
 * @file linkedStack.h
 * @brief Stack implementation using LinkedList as the underlying data structure.
 *
 * This module provides a stack (LIFO - Last In, First Out) data structure implemented
 * using LinkedList as the underlying storage. The stack provides O(1) time complexity
 * for push, pop, and peek operations. The implementation supports generic element types
 * and includes memory management through customizable free functions.
 */

#ifndef LINKED_STACK_H
#define LINKED_STACK_H

#include "linkedList.h"

/**
 * @brief Type alias for the LinkedStack, which is implemented using LinkedList.
 */
typedef zzLinkedList zzLinkedStack;

/**
 * @brief Initializes a new LinkedStack with the specified element size.
 *
 * This function initializes a LinkedStack structure with the given element size
 * and custom free function. The stack will be empty after initialization.
 *
 * @param[out] s Pointer to the LinkedStack structure to initialize
 * @param[in] elSize Size in bytes of each element that will be stored in the stack
 * @param[in] elemFree Function to free individual elements when they are popped or the stack is freed, or NULL if not needed
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
static inline zzOpResult zzLinkedStackInit(zzLinkedStack *s, size_t elSize, zzFreeFn elemFree) {
    return zzLinkedListInit(s, elSize, elemFree);
}

/**
 * @brief Frees all resources associated with the LinkedStack.
 *
 * This function releases all memory used by the LinkedStack, including calling
 * the custom free function for each element if provided. After this function
 * returns, the LinkedStack structure should not be used until reinitialized.
 *
 * @param[in,out] s Pointer to the LinkedStack to free
 */
static inline void zzLinkedStackFree(zzLinkedStack *s) {
    zzLinkedListFree(s);
}

/**
 * @brief Pushes an element onto the top of the LinkedStack.
 *
 * This function adds the specified element to the top of the stack. The element
 * is copied into a newly allocated node. The stack size is increased by one.
 *
 * @param[in,out] s Pointer to the LinkedStack to push to
 * @param[in] elem Pointer to the element to push onto the stack (contents will be copied)
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
static inline zzOpResult zzLinkedStackPush(zzLinkedStack *s, const void *elem) {
    return zzLinkedListPushBack(s, elem);
}

/**
 * @brief Pops an element from the top of the LinkedStack.
 *
 * This function removes the element from the top of the stack and copies it
 * to the output buffer. The stack size is decreased by one. If a custom free
 * function was provided, it will not be called since the element is returned.
 *
 * @param[in,out] s Pointer to the LinkedStack to pop from
 * @param[out] out Pointer to a buffer where the popped element will be copied
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
static inline zzOpResult zzLinkedStackPop(zzLinkedStack *s, void *out) {
    return zzLinkedListPopBack(s, out);
}

/**
 * @brief Peeks at the element at the top of the LinkedStack without removing it.
 *
 * This function copies the element at the top of the stack to the output buffer
 * without removing it from the stack. The stack size remains unchanged.
 *
 * @param[in] s Pointer to the LinkedStack to peek into
 * @param[out] out Pointer to a buffer where the top element will be copied
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
static inline zzOpResult zzLinkedStackPeek(const zzLinkedStack *s, void *out) {
    if (!s) return ZZ_ERR("LinkedList pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (!s->tail) return ZZ_ERR("List is empty");

    memcpy(out, s->tail->data, s->elSize);
    return ZZ_OK();
}

/**
 * @brief Checks if the LinkedStack is empty.
 *
 * This function checks whether the stack contains any elements.
 *
 * @param[in] s Pointer to the LinkedStack to check
 * @return true if the stack is empty (size is 0), false otherwise
 */
static inline bool zzLinkedStackIsEmpty(const zzLinkedStack *s) {
    return s->size == 0;
}

/**
 * @brief Gets the current size of the LinkedStack.
 *
 * This function returns the number of elements currently in the stack.
 *
 * @param[in] s Pointer to the LinkedStack to get the size of
 * @return The number of elements in the stack
 */
static inline size_t zzLinkedStackSize(const zzLinkedStack *s) {
    return s->size;
}

/**
 * @brief Clears all elements from the LinkedStack.
 *
 * This function removes all elements from the stack by calling the custom free
 * function on each element (if provided) and deallocating all nodes. The stack
 * becomes empty after this operation.
 *
 * @param[in,out] s Pointer to the LinkedStack to clear
 */
static inline void zzLinkedStackClear(zzLinkedStack *s) {
    zzLinkedListClear(s);
}

#endif