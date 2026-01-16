/**
 * @file circularBuffer.c
 * @brief Circular buffer implementation
 *
 * Source implementation for the CircularBuffer data structure that provides
 * O(1) insertion and deletion operations with fixed-size storage.
 * Useful for implementing queues with bounded capacity.
 */

#include "circularBuffer.h"
#include <string.h>
#include <stdlib.h>

/**
 * @brief Initialize a CircularBuffer with specified element size and capacity
 *
 * Allocates internal buffer and sets up the data structure for use.
 * The CircularBuffer must be freed with zzCircularBufferFree when done.
 *
 * @param[out] cb Pointer to CircularBuffer to initialize
 * @param[in] elSize Size of each element in bytes (must be > 0)
 * @param[in] capacity Maximum number of elements the buffer can hold (must be > 0)
 * @param[in] elemFree Optional callback to free elements (can be NULL)
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
zzOpResult zzCircularBufferInit(zzCircularBuffer *cb, size_t elSize, size_t capacity, zzFreeFn elemFree) {
    if (!cb) return ZZ_ERR("CircularBuffer pointer is NULL");
    if (elSize == 0) return ZZ_ERR("Element size cannot be zero");
    if (capacity == 0) return ZZ_ERR("Capacity cannot be zero");

    cb->buffer = malloc(elSize * capacity);
    if (!cb->buffer) return ZZ_ERR("Failed to allocate buffer memory");

    cb->elSize = elSize;
    cb->capacity = capacity;
    cb->head = 0;
    cb->tail = 0;
    cb->size = 0;
    cb->elemFree = elemFree;
    return ZZ_OK();
}

/**
 * @brief Free all memory associated with CircularBuffer
 *
 * Calls elemFree callback on all elements (if provided), then frees
 * internal buffer. The CircularBuffer struct itself is not freed (caller owns it).
 * After calling, the CircularBuffer is in undefined state and must be
 * re-initialized before reuse.
 *
 * @param[in,out] cb Pointer to CircularBuffer to free
 *
 * @pre cb must be initialized with zzCircularBufferInit
 * @post All internal memory is freed
 * @post If elemFree was provided, it has been called on all elements
 * @post cb must not be used until re-initialized
 *
 * @note Safe to call with NULL pointer (no-op)
 * @note Does not free the CircularBuffer struct itself
 */
void zzCircularBufferFree(zzCircularBuffer *cb) {
    if (!cb || !cb->buffer) return;

    if (cb->elemFree) {
        for (size_t i = 0; i < cb->size; i++) {
            size_t idx = (cb->head + i) % cb->capacity;
            void *elem = (char*)cb->buffer + idx * cb->elSize;
            cb->elemFree(elem);
        }
    }
    free(cb->buffer);
    cb->buffer = NULL;
}

/**
 * @brief Add an element to the back of the CircularBuffer
 *
 * Inserts a copy of the element at the tail position. If the buffer is full,
 * the oldest element (at head) is overwritten and potentially freed if elemFree
 * callback is provided. This operation is O(1).
 *
 * @param[in,out] cb Pointer to initialized CircularBuffer
 * @param[in] elem Pointer to element data to add (will be copied)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element added successfully
 *         - ERROR: Possible errors:
 *           * "CircularBuffer pointer is NULL"
 *           * "Element pointer is NULL"
 *
 * @pre cb must be initialized with zzCircularBufferInit
 * @pre elem must point to valid data of size elSize
 * @post If buffer was not full, cb->size increases by 1
 * @post If buffer was full, oldest element is overwritten
 * @post elem data is copied into CircularBuffer (caller retains ownership)
 *
 * @see zzCircularBufferPop
 */
zzOpResult zzCircularBufferPush(zzCircularBuffer *cb, const void *elem) {
    if (!cb) return ZZ_ERR("CircularBuffer pointer is NULL");
    if (!elem) return ZZ_ERR("Element pointer is NULL");

    void *target = (char*)cb->buffer + cb->tail * cb->elSize;

    if (cb->size == cb->capacity) {
        if (cb->elemFree) cb->elemFree(target);
        cb->head = (cb->head + 1) % cb->capacity;
    } else {
        cb->size++;
    }

    memcpy(target, elem, cb->elSize);
    cb->tail = (cb->tail + 1) % cb->capacity;
    return ZZ_OK();
}

/**
 * @brief Remove an element from the front of the CircularBuffer
 *
 * Copies the data from the element at the head position to the output buffer
 * and advances the head pointer. This operation is O(1).
 *
 * @param[in,out] cb Pointer to initialized CircularBuffer
 * @param[out] out Buffer where element data will be copied (must be elSize bytes)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element removed and copied successfully
 *         - ERROR: Possible errors:
 *           * "CircularBuffer pointer is NULL"
 *           * "Output buffer is NULL"
 *           * "Buffer is empty"
 *
 * @pre cb must be initialized with zzCircularBufferInit
 * @pre out must point to buffer of at least elSize bytes
 * @post cb->size decreases by 1
 * @post out contains copy of the removed element data
 *
 * @see zzCircularBufferPush
 */
zzOpResult zzCircularBufferPop(zzCircularBuffer *cb, void *out) {
    if (!cb) return ZZ_ERR("CircularBuffer pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (cb->size == 0) return ZZ_ERR("Buffer is empty");

    void *elem = (char*)cb->buffer + cb->head * cb->elSize;
    memcpy(out, elem, cb->elSize);

    cb->head = (cb->head + 1) % cb->capacity;
    cb->size--;
    return ZZ_OK();
}

/**
 * @brief Get element at specified logical index from the CircularBuffer
 *
 * Retrieves a copy of the element at the specified logical index (where index 0
 * refers to the head element). This operation is O(1).
 *
 * @param[in] cb Pointer to initialized CircularBuffer
 * @param[in] idx Logical index of element to retrieve (0-based, relative to head)
 * @param[out] out Buffer where element will be copied (must be elSize bytes)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element retrieved successfully
 *         - ERROR: Possible errors:
 *           * "CircularBuffer pointer is NULL"
 *           * "Output buffer is NULL"
 *           * "Index out of bounds"
 *
 * @pre cb must be initialized with zzCircularBufferInit
 * @pre out must point to buffer of at least elSize bytes
 * @pre idx must be < cb->size
 * @post out contains copy of element data
 *
 * @see zzCircularBufferPeekFront
 * @see zzCircularBufferPeekBack
 */
zzOpResult zzCircularBufferGet(const zzCircularBuffer *cb, size_t idx, void *out) {
    if (!cb) return ZZ_ERR("CircularBuffer pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (idx >= cb->size) return ZZ_ERR("Index out of bounds");

    size_t realIdx = (cb->head + idx) % cb->capacity;
    memcpy(out, (char*)cb->buffer + realIdx * cb->elSize, cb->elSize);
    return ZZ_OK();
}

/**
 * @brief Peek at the front element of the CircularBuffer without removing it
 *
 * Copies the data from the element at the head position to the output buffer
 * without advancing the head pointer. This operation is O(1).
 *
 * @param[in] cb Pointer to initialized CircularBuffer
 * @param[out] out Buffer where element data will be copied (must be elSize bytes)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element peeked successfully
 *         - ERROR: Possible errors:
 *           * "CircularBuffer pointer is NULL"
 *           * "Output buffer is NULL"
 *           * "Buffer is empty"
 *
 * @pre cb must be initialized with zzCircularBufferInit
 * @pre out must point to buffer of at least elSize bytes
 * @post cb->size remains unchanged
 * @post out contains copy of the front element data
 *
 * @see zzCircularBufferPeekBack
 * @see zzCircularBufferPop
 */
zzOpResult zzCircularBufferPeekFront(const zzCircularBuffer *cb, void *out) {
    if (!cb) return ZZ_ERR("CircularBuffer pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (cb->size == 0) return ZZ_ERR("Buffer is empty");

    memcpy(out, (char*)cb->buffer + cb->head * cb->elSize, cb->elSize);
    return ZZ_OK();
}

/**
 * @brief Peek at the back element of the CircularBuffer without removing it
 *
 * Copies the data from the element at the tail position to the output buffer
 * without advancing the tail pointer. This operation is O(1).
 *
 * @param[in] cb Pointer to initialized CircularBuffer
 * @param[out] out Buffer where element data will be copied (must be elSize bytes)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element peeked successfully
 *         - ERROR: Possible errors:
 *           * "CircularBuffer pointer is NULL"
 *           * "Output buffer is NULL"
 *           * "Buffer is empty"
 *
 * @pre cb must be initialized with zzCircularBufferInit
 * @pre out must point to buffer of at least elSize bytes
 * @post cb->size remains unchanged
 * @post out contains copy of the back element data
 *
 * @see zzCircularBufferPeekFront
 * @see zzCircularBufferPush
 */
zzOpResult zzCircularBufferPeekBack(const zzCircularBuffer *cb, void *out) {
    if (!cb) return ZZ_ERR("CircularBuffer pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (cb->size == 0) return ZZ_ERR("Buffer is empty");

    size_t idx = (cb->tail + cb->capacity - 1) % cb->capacity;
    memcpy(out, (char*)cb->buffer + idx * cb->elSize, cb->elSize);
    return ZZ_OK();
}

/**
 * @brief Clear all elements from CircularBuffer
 *
 * Removes all elements (calls elemFree if provided) but keeps
 * allocated buffer. Size becomes 0, capacity unchanged.
 *
 * @param[in,out] cb Pointer to initialized CircularBuffer
 *
 * @pre cb must be initialized with zzCircularBufferInit
 * @post cb->size is 0
 * @post cb->head and cb->tail are reset to 0
 * @post If elemFree provided, it has been called on all elements
 *
 * @note Safe to call with NULL pointer (no-op)
 *
 * @see zzCircularBufferFree
 */
void zzCircularBufferClear(zzCircularBuffer *cb) {
    if (!cb) return;

    if (cb->elemFree) {
        for (size_t i = 0; i < cb->size; i++) {
            size_t idx = (cb->head + i) % cb->capacity;
            void *elem = (char*)cb->buffer + idx * cb->elSize;
            cb->elemFree(elem);
        }
    }
    cb->head = 0;
    cb->tail = 0;
    cb->size = 0;
}
