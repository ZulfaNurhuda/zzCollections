/**
 * @file arrayDeque.h
 * @brief Double-ended queue using circular buffer
 *
 * ArrayDeque provides O(1) operations at both ends using a circular buffer.
 * Automatically grows when capacity is reached.
 */

#ifndef ARRAY_DEQUE_H
#define ARRAY_DEQUE_H

#include "types.h"
#include "utils.h"
#include "result.h"

/**
 * @brief Double-ended queue with O(1) operations at both ends
 *
 * ArrayDeque uses a circular buffer to efficiently support insertions
 * and deletions at both front and back. Provides O(1) indexed access.
 */
typedef struct zzArrayDeque {
    void *buffer;        /**< Circular buffer storage */
    size_t elSize;       /**< Size of each element in bytes */
    size_t capacity;     /**< Current capacity */
    size_t front;        /**< Index of front element */
    size_t size;         /**< Current number of elements */
    zzFreeFn elemFree;   /**< Optional callback to free elements */
} zzArrayDeque;

/**
 * @brief Initialize an ArrayDeque
 *
 * @param[out] ad Pointer to ArrayDeque to initialize
 * @param[in] elSize Size of each element in bytes (must be > 0)
 * @param[in] capacity Initial capacity (use 0 for default of 4)
 * @param[in] elemFree Optional callback to free elements (can be NULL)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: ArrayDeque initialized successfully
 *         - ERROR: "ArrayDeque pointer is NULL" / "Element size cannot be zero" / "Failed to allocate buffer memory"
 *
 * @see zzArrayDequeFree
 */
zzOpResult zzArrayDequeInit(zzArrayDeque *ad, size_t elSize, size_t capacity, zzFreeFn elemFree);

/**
 * @brief Free all memory associated with ArrayDeque
 *
 * @param[in,out] ad Pointer to ArrayDeque to free
 *
 * @note Safe to call with NULL pointer (no-op)
 */
void zzArrayDequeFree(zzArrayDeque *ad);

/**
 * @brief Push element to front of deque
 *
 * @param[in,out] ad Pointer to ArrayDeque
 * @param[in] elem Pointer to element data to push (will be copied)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element pushed successfully
 *         - ERROR: "ArrayDeque pointer is NULL" / "Element pointer is NULL" / "Failed to grow buffer (realloc failed)"
 *
 * @see zzArrayDequePopFront
 */
zzOpResult zzArrayDequePushFront(zzArrayDeque *ad, const void *elem);

/**
 * @brief Push element to back of deque
 *
 * @param[in,out] ad Pointer to ArrayDeque
 * @param[in] elem Pointer to element data to push (will be copied)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element pushed successfully
 *         - ERROR: "ArrayDeque pointer is NULL" / "Element pointer is NULL" / "Failed to grow buffer (realloc failed)"
 *
 * @see zzArrayDequePopBack
 */
zzOpResult zzArrayDequePushBack(zzArrayDeque *ad, const void *elem);

/**
 * @brief Pop element from front of deque
 *
 * @param[in,out] ad Pointer to ArrayDeque
 * @param[out] out Buffer where element will be copied (must be elSize bytes)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element popped successfully
 *         - ERROR: "ArrayDeque pointer is NULL" / "Output buffer is NULL" / "Deque is empty"
 *
 * @see zzArrayDequePushFront
 */
zzOpResult zzArrayDequePopFront(zzArrayDeque *ad, void *out);

/**
 * @brief Pop element from back of deque
 *
 * @param[in,out] ad Pointer to ArrayDeque
 * @param[out] out Buffer where element will be copied (must be elSize bytes)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element popped successfully
 *         - ERROR: "ArrayDeque pointer is NULL" / "Output buffer is NULL" / "Deque is empty"
 *
 * @see zzArrayDequePushBack
 */
zzOpResult zzArrayDequePopBack(zzArrayDeque *ad, void *out);

/**
 * @brief Peek at front element without removing
 *
 * @param[in] ad Pointer to ArrayDeque
 * @param[out] out Buffer where element will be copied (must be elSize bytes)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element retrieved successfully
 *         - ERROR: "ArrayDeque pointer is NULL" / "Output buffer is NULL" / "Deque is empty"
 *
 * @see zzArrayDequePeekBack
 */
zzOpResult zzArrayDequePeekFront(const zzArrayDeque *ad, void *out);

/**
 * @brief Peek at back element without removing
 *
 * @param[in] ad Pointer to ArrayDeque
 * @param[out] out Buffer where element will be copied (must be elSize bytes)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element retrieved successfully
 *         - ERROR: "ArrayDeque pointer is NULL" / "Output buffer is NULL" / "Deque is empty"
 *
 * @see zzArrayDequePeekFront
 */
zzOpResult zzArrayDequePeekBack(const zzArrayDeque *ad, void *out);

/**
 * @brief Get element at specified index
 *
 * @param[in] ad Pointer to ArrayDeque
 * @param[in] idx Index of element to retrieve (0-based, 0 = front)
 * @param[out] out Buffer where element will be copied (must be elSize bytes)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element retrieved successfully
 *         - ERROR: "ArrayDeque pointer is NULL" / "Output buffer is NULL" / "Index out of bounds"
 */
zzOpResult zzArrayDequeGet(const zzArrayDeque *ad, size_t idx, void *out);

/**
 * @brief Clear all elements from ArrayDeque
 *
 * @param[in,out] ad Pointer to ArrayDeque
 *
 * @post ad->size is 0, capacity unchanged
 */
void zzArrayDequeClear(zzArrayDeque *ad);

#endif
