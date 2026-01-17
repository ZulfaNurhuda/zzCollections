/**
 * @file arrayStack.h
 * @brief Stack implementation using ArrayDeque as the underlying data structure.
 *
 * This module provides a stack (LIFO - Last In, First Out) data structure implemented
 * using ArrayDeque as the underlying storage. The stack provides O(1) time complexity
 * for push, pop, and peek operations. The implementation supports generic element types
 * and includes memory management through customizable free functions.
 */

#ifndef ARRAY_STACK_H
#define ARRAY_STACK_H

#include "arrayDeque.h"

/**
 * @brief Type alias for the ArrayStack, which is implemented using ArrayDeque.
 */
typedef zzArrayDeque zzArrayStack;

/**
 * @brief Initializes a new ArrayStack with the specified element size and capacity.
 *
 * This function initializes an ArrayStack structure with the given element size
 * and initial capacity. The capacity will be automatically increased as needed
 * when elements are pushed onto the stack.
 *
 * @param[out] s Pointer to the ArrayStack structure to initialize
 * @param[in] elSize Size in bytes of each element that will be stored in the stack
 * @param[in] capacity Initial capacity of the stack (will be adjusted to at least 4)
 * @param[in] elemFree Function to free individual elements when they are popped or the stack is freed, or NULL if not needed
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
static inline zzOpResult zzArrayStackInit(zzArrayStack *s, size_t elSize, size_t capacity, zzFreeFn elemFree) {
    return zzArrayDequeInit(s, elSize, capacity, elemFree);
}

/**
 * @brief Frees all resources associated with the ArrayStack.
 *
 * This function releases all memory used by the ArrayStack, including calling
 * the custom free function for each element if provided. After this function
 * returns, the ArrayStack structure should not be used until reinitialized.
 *
 * @param[in,out] s Pointer to the ArrayStack to free
 */
static inline void zzArrayStackFree(zzArrayStack *s) {
    zzArrayDequeFree(s);
}

/**
 * @brief Pushes an element onto the top of the ArrayStack.
 *
 * This function adds the specified element to the top of the stack. If the
 * stack has reached its capacity, it will be automatically resized to accommodate
 * the new element. The element is copied into the stack's internal buffer.
 *
 * @param[in,out] s Pointer to the ArrayStack to push to
 * @param[in] elem Pointer to the element to push onto the stack (contents will be copied)
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
static inline zzOpResult zzArrayStackPush(zzArrayStack *s, const void *elem) {
    return zzArrayDequePushBack(s, elem);
}

/**
 * @brief Pops an element from the top of the ArrayStack.
 *
 * This function removes the element from the top of the stack and copies it
 * to the output buffer. The stack size is decreased by one. If a custom free
 * function was provided, it will not be called since the element is returned.
 *
 * @param[in,out] s Pointer to the ArrayStack to pop from
 * @param[out] out Pointer to a buffer where the popped element will be copied
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
static inline zzOpResult zzArrayStackPop(zzArrayStack *s, void *out) {
    return zzArrayDequePopBack(s, out);
}

/**
 * @brief Peeks at the element at the top of the ArrayStack without removing it.
 *
 * This function copies the element at the top of the stack to the output buffer
 * without removing it from the stack. The stack size remains unchanged.
 *
 * @param[in] s Pointer to the ArrayStack to peek into
 * @param[out] out Pointer to a buffer where the top element will be copied
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
static inline zzOpResult zzArrayStackPeek(const zzArrayStack *s, void *out) {
    return zzArrayDequePeekBack(s, out);
}

/**
 * @brief Checks if the ArrayStack is empty.
 *
 * This function checks whether the stack contains any elements.
 *
 * @param[in] s Pointer to the ArrayStack to check
 * @return true if the stack is empty (size is 0), false otherwise
 */
static inline bool zzArrayStackIsEmpty(const zzArrayStack *s) {
    return s->size == 0;
}

/**
 * @brief Gets the current size of the ArrayStack.
 *
 * This function returns the number of elements currently in the stack.
 *
 * @param[in] s Pointer to the ArrayStack to get the size of
 * @return The number of elements in the stack
 */
static inline size_t zzArrayStackSize(const zzArrayStack *s) {
    return s->size;
}

/**
 * @brief Clears all elements from the ArrayStack.
 *
 * This function removes all elements from the stack by calling the custom free
 * function on each element (if provided) and resetting the size to zero.
 * The underlying buffer remains allocated with the same capacity.
 *
 * @param[in,out] s Pointer to the ArrayStack to clear
 */
static inline void zzArrayStackClear(zzArrayStack *s) {
    zzArrayDequeClear(s);
}

#endif