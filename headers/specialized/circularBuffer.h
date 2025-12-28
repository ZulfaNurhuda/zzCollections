#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include "common.h"
#include "result.h"

/**
 * @brief CircularBuffer provides fixed-size ring buffer with overwrite semantics
 *
 * When full, pushing new elements overwrites the oldest element.
 * Efficient O(1) push and pop operations.
 */
typedef struct zzCircularBuffer {
    void *buffer;
    size_t elSize;
    size_t capacity;
    size_t head;
    size_t tail;
    size_t size;
    zzFreeFn elemFree;
} zzCircularBuffer;

/**
 * @brief Initialize CircularBuffer with fixed capacity
 *
 * Allocates fixed-size buffer. Capacity cannot change after initialization.
 * Must be freed with zzCircularBufferFree when done.
 *
 * @param[out] cb Pointer to CircularBuffer to initialize
 * @param[in] elSize Size of each element in bytes (must be > 0)
 * @param[in] capacity Fixed capacity (must be > 0)
 * @param[in] elemFree Optional element cleanup function (can be NULL)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: CircularBuffer initialized successfully
 *         - ERROR: Possible errors:
 *           * "CircularBuffer pointer is NULL"
 *           * "Element size cannot be zero"
 *           * "Capacity cannot be zero"
 *           * "Failed to allocate buffer memory"
 *
 * @see zzCircularBufferFree
 */
zzOpResult zzCircularBufferInit(zzCircularBuffer *cb, size_t elSize, size_t capacity, zzFreeFn elemFree);

/**
 * @brief Free all memory associated with CircularBuffer
 *
 * Calls elemFree on all elements (if provided), then frees buffer.
 * The CircularBuffer struct itself is not freed.
 *
 * @param[in,out] cb Pointer to CircularBuffer to free
 *
 * @note Safe to call with NULL pointer (no-op)
 */
void zzCircularBufferFree(zzCircularBuffer *cb);

/**
 * @brief Push element into buffer
 *
 * If buffer is full, overwrites oldest element.
 * Always succeeds (never fails due to lack of space).
 *
 * @param[in,out] cb Pointer to CircularBuffer
 * @param[in] elem Pointer to element data (will be copied)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element pushed successfully
 *         - ERROR: Possible errors:
 *           * "CircularBuffer pointer is NULL"
 *           * "Element pointer is NULL"
 */
zzOpResult zzCircularBufferPush(zzCircularBuffer *cb, const void *elem);

/**
 * @brief Pop element from front of buffer
 *
 * Removes and returns oldest element.
 *
 * @param[in,out] cb Pointer to CircularBuffer
 * @param[out] out Buffer where element will be copied (must be elSize bytes)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element popped successfully
 *         - ERROR: Possible errors:
 *           * "CircularBuffer pointer is NULL"
 *           * "Output buffer is NULL"
 *           * "Buffer is empty"
 */
zzOpResult zzCircularBufferPop(zzCircularBuffer *cb, void *out);

/**
 * @brief Get element at index
 *
 * Access element by logical index (0 = oldest, size-1 = newest).
 *
 * @param[in] cb Pointer to CircularBuffer
 * @param[in] idx Logical index (0-based)
 * @param[out] out Buffer where element will be copied (must be elSize bytes)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element retrieved successfully
 *         - ERROR: Possible errors:
 *           * "CircularBuffer pointer is NULL"
 *           * "Output buffer is NULL"
 *           * "Index out of bounds"
 */
zzOpResult zzCircularBufferGet(const zzCircularBuffer *cb, size_t idx, void *out);

/**
 * @brief Peek at front element without removing
 *
 * Returns oldest element without modifying buffer.
 *
 * @param[in] cb Pointer to CircularBuffer
 * @param[out] out Buffer where element will be copied (must be elSize bytes)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element peeked successfully
 *         - ERROR: Possible errors:
 *           * "CircularBuffer pointer is NULL"
 *           * "Output buffer is NULL"
 *           * "Buffer is empty"
 */
zzOpResult zzCircularBufferPeekFront(const zzCircularBuffer *cb, void *out);

/**
 * @brief Peek at back element without removing
 *
 * Returns newest element without modifying buffer.
 *
 * @param[in] cb Pointer to CircularBuffer
 * @param[out] out Buffer where element will be copied (must be elSize bytes)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element peeked successfully
 *         - ERROR: Possible errors:
 *           * "CircularBuffer pointer is NULL"
 *           * "Output buffer is NULL"
 *           * "Buffer is empty"
 */
zzOpResult zzCircularBufferPeekBack(const zzCircularBuffer *cb, void *out);

/**
 * @brief Clear all elements from buffer
 *
 * Removes all elements (calls elemFree if provided).
 * Resets head and tail. Size becomes 0, capacity unchanged.
 *
 * @param[in,out] cb Pointer to CircularBuffer
 */
void zzCircularBufferClear(zzCircularBuffer *cb);

#endif
