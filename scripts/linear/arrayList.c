/**
 * @file arrayList.c
 * @brief Implementation of the dynamic array (ArrayList) data structure.
 *
 * This module provides the implementation for the ArrayList data structure,
 * which offers dynamic array functionality with automatic resizing capabilities.
 * The implementation handles memory management and provides efficient operations
 * for adding, removing, and accessing elements.
 */

#include "arrayList.h"
#include <string.h>
#include <stdlib.h>

/**
 * @brief Initializes a new ArrayList with the specified element size and capacity.
 *
 * This function initializes an ArrayList structure with the given element size
 * and initial capacity. The capacity will be automatically increased as needed
 * when elements are added to the list.
 *
 * @param[out] al Pointer to the ArrayList structure to initialize
 * @param[in] elSize Size in bytes of each element that will be stored in the list
 * @param[in] capacity Initial capacity of the list (will be adjusted to at least 4)
 * @param[in] elemFree Function to free individual elements when they are removed or the list is freed, or NULL if not needed
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzArrayListInit(zzArrayList *al, size_t elSize, size_t capacity, zzFreeFn elemFree) {
    if (!al) return ZZ_ERR("ArrayList pointer is NULL");
    if (elSize == 0) return ZZ_ERR("Element size cannot be zero");
    if (capacity == 0) capacity = 4;

    al->elSize = elSize;
    al->size = 0;
    al->capacity = capacity;
    al->elemFree = elemFree;
    al->buffer = malloc(elSize * capacity);

    if (!al->buffer) return ZZ_ERR("Failed to allocate buffer memory");
    return ZZ_OK();
}

/**
 * @brief Frees all resources associated with the ArrayList.
 *
 * This function releases all memory used by the ArrayList, including calling
 * the custom free function for each element if provided. After this function
 * returns, the ArrayList structure should not be used until reinitialized.
 *
 * @param[in,out] al Pointer to the ArrayList to free
 */
void zzArrayListFree(zzArrayList *al) {
    if (!al || !al->buffer) return;

    if (al->elemFree) {
        for (size_t i = 0; i < al->size; i++) {
            void *elem = (char*)al->buffer + i * al->elSize;
            al->elemFree(elem);
        }
    }
    free(al->buffer);
    al->buffer = NULL;
    al->size = 0;
}

/**
 * @brief Internal function to grow the ArrayList buffer when capacity is exceeded.
 *
 * This helper function increases the capacity of the ArrayList by approximately 50%
 * (or by at least 8 elements if the current capacity is small). It reallocates the
 * buffer to the new size, preserving existing elements.
 *
 * @param[in,out] al Pointer to the ArrayList to grow
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
static zzOpResult zzArrayListGrow(zzArrayList *al) {
    size_t newCap = al->capacity + (al->capacity >> 1);
    if (newCap < al->capacity + 8) newCap = al->capacity + 8;
    void *newBuf = realloc(al->buffer, al->elSize * newCap);
    if (!newBuf) return ZZ_ERR("Failed to grow buffer (realloc failed)");
    al->buffer = newBuf;
    al->capacity = newCap;
    return ZZ_OK();
}

/**
 * @brief Adds an element to the end of the ArrayList.
 *
 * This function appends the specified element to the end of the list. If the
 * list has reached its capacity, it will be automatically resized to accommodate
 * the new element. The element is copied into the list's internal buffer.
 *
 * @param[in,out] al Pointer to the ArrayList to add to
 * @param[in] elem Pointer to the element to add (contents will be copied)
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzArrayListAdd(zzArrayList *al, const void *elem) {
    if (!al) return ZZ_ERR("ArrayList pointer is NULL");
    if (!elem) return ZZ_ERR("Element pointer is NULL");

    if (al->size == al->capacity) {
        zzOpResult growResult = zzArrayListGrow(al);
        if (ZZ_IS_ERR(growResult)) {
            return growResult;
        }
    }

    memcpy((char*)al->buffer + al->size * al->elSize, elem, al->elSize);
    al->size++;
    return ZZ_OK();
}

/**
 * @brief Retrieves an element at the specified index.
 *
 * This function copies the element at the given index into the output buffer.
 * The index must be within the valid range [0, size).
 *
 * @param[in] al Pointer to the ArrayList to retrieve from
 * @param[in] idx Index of the element to retrieve (0-based)
 * @param[out] out Pointer to a buffer where the element will be copied
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzArrayListGet(const zzArrayList *al, size_t idx, void *out) {
    if (!al) return ZZ_ERR("ArrayList pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (idx >= al->size) return ZZ_ERR("Index out of bounds");

    memcpy(out, (char*)al->buffer + idx * al->elSize, al->elSize);
    return ZZ_OK();
}

/**
 * @brief Sets the element at the specified index.
 *
 * This function replaces the element at the given index with a new value.
 * If a custom free function was provided and the old element exists, it will
 * be called on the old element before replacing it. The new element is copied
 * into the list's internal buffer.
 *
 * @param[in,out] al Pointer to the ArrayList to modify
 * @param[in] idx Index of the element to set (0-based)
 * @param[in] elem Pointer to the new element to store (contents will be copied)
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzArrayListSet(zzArrayList *al, size_t idx, const void *elem) {
    if (!al) return ZZ_ERR("ArrayList pointer is NULL");
    if (!elem) return ZZ_ERR("Element pointer is NULL");
    if (idx >= al->size) return ZZ_ERR("Index out of bounds");

    void *target = (char*)al->buffer + idx * al->elSize;
    if (al->elemFree) {
        al->elemFree(target);
    }
    memcpy(target, elem, al->elSize);
    return ZZ_OK();
}

/**
 * @brief Removes the element at the specified index.
 *
 * This function removes the element at the given index and shifts all subsequent
 * elements one position to the left. If a custom free function was provided,
 * it will be called on the removed element. The list size is decreased by one.
 *
 * @param[in,out] al Pointer to the ArrayList to remove from
 * @param[in] idx Index of the element to remove (0-based)
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzArrayListRemove(zzArrayList *al, size_t idx) {
    if (!al) return ZZ_ERR("ArrayList pointer is NULL");
    if (idx >= al->size) return ZZ_ERR("Index out of bounds");

    void *target = (char*)al->buffer + idx * al->elSize;
    if (al->elemFree) {
        al->elemFree(target);
    }

    if (idx < al->size - 1) {
        memmove(target, (char*)target + al->elSize,
                     (al->size - idx - 1) * al->elSize);
    }
    al->size--;
    return ZZ_OK();
}

/**
 * @brief Clears all elements from the ArrayList.
 *
 * This function removes all elements from the list by calling the custom free
 * function on each element (if provided) and resetting the size to zero.
 * The underlying buffer remains allocated with the same capacity.
 *
 * @param[in,out] al Pointer to the ArrayList to clear
 */
void zzArrayListClear(zzArrayList *al) {
    if (!al) return;

    if (al->elemFree) {
        for (size_t i = 0; i < al->size; i++) {
            void *elem = (char*)al->buffer + i * al->elSize;
            al->elemFree(elem);
        }
    }
    al->size = 0;
}

/**
 * @brief Inserts an element at the specified index.
 *
 * This function inserts the specified element at the given index, shifting all
 * subsequent elements one position to the right. If the list has reached its
 * capacity, it will be automatically resized to accommodate the new element.
 *
 * @param[in,out] al Pointer to the ArrayList to insert into
 * @param[in] idx Index at which to insert the element (0-based)
 * @param[in] elem Pointer to the element to insert (contents will be copied)
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzArrayListInsert(zzArrayList *al, size_t idx, const void *elem) {
    if (!al) return ZZ_ERR("ArrayList pointer is NULL");
    if (!elem) return ZZ_ERR("Element pointer is NULL");
    if (idx > al->size) return ZZ_ERR("Index out of bounds");

    if (al->size == al->capacity) {
        zzOpResult growResult = zzArrayListGrow(al);
        if (ZZ_IS_ERR(growResult)) {
            return growResult;
        }
    }

    if (idx < al->size) {
        void *src = (char*)al->buffer + idx * al->elSize;
        void *dst = (char*)src + al->elSize;
        memmove(dst, src, (al->size - idx) * al->elSize);
    }

    memcpy((char*)al->buffer + idx * al->elSize, elem, al->elSize);
    al->size++;
    return ZZ_OK();
}

/**
 * @brief Finds the index of the first occurrence of an element.
 *
 * This function searches for the first element in the list that matches the
 * specified element using the provided comparison function. The search proceeds
 * from index 0 to the end of the list.
 *
 * @param[in] al Pointer to the ArrayList to search in
 * @param[in] elem Pointer to the element to search for
 * @param[in] cmp Comparison function to use for matching elements
 * @param[out] indexOut Pointer to an integer where the found index will be stored, or -1 if not found
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzArrayListIndexOf(const zzArrayList *al, const void *elem, zzCompareFn cmp, int *indexOut) {
    if (!al) return ZZ_ERR("ArrayList pointer is NULL");
    if (!elem) return ZZ_ERR("Element pointer is NULL");
    if (!cmp) return ZZ_ERR("Comparison function is NULL");
    if (!indexOut) return ZZ_ERR("Index output pointer is NULL");

    for (size_t i = 0; i < al->size; i++) {
        void *item = (char*)al->buffer + i * al->elSize;
        if (cmp(item, elem) == 0) {
            *indexOut = (int)i;
            return ZZ_OK();
        }
    }
    return ZZ_ERR("Element not found");
}

/**
 * @brief Initializes an iterator for the ArrayList.
 *
 * This function initializes an iterator to traverse the ArrayList from
 * the beginning to the end. The iterator will be positioned at the first
 * element if the list is not empty.
 *
 * @param[out] it Pointer to the iterator structure to initialize
 * @param[in] al Pointer to the ArrayList to iterate over
 */
void zzArrayListIteratorInit(zzArrayListIterator *it, const zzArrayList *al) {
    if (!it || !al) return;
    
    it->list = al;
    it->index = 0;
    it->state = (al->size > 0) ? ZZ_ITER_VALID : ZZ_ITER_END;
}

/**
 * @brief Advances the iterator to the next element.
 *
 * This function moves the iterator to the next element in the ArrayList
 * and copies the current element to the output buffer. Returns false when
 * the iterator reaches the end of the list.
 *
 * @param[in,out] it Pointer to the iterator to advance
 * @param[out] valueOut Pointer to a buffer where the current element will be copied
 * @return true if an element was retrieved, false if the iterator reached the end
 */
bool zzArrayListIteratorNext(zzArrayListIterator *it, void *valueOut) {
    if (!it || !valueOut || it->state != ZZ_ITER_VALID) return false;
    
    if (it->index >= it->list->size) {
        it->state = ZZ_ITER_END;
        return false;
    }
    
    void *elem = (char*)it->list->buffer + it->index * it->list->elSize;
    memcpy(valueOut, elem, it->list->elSize);
    
    it->index++;
    if (it->index >= it->list->size) {
        it->state = ZZ_ITER_END;
    }
    
    return true;
}

/**
 * @brief Checks if the iterator has more elements.
 *
 * This function checks whether the iterator can advance to another element
 * without actually advancing it.
 *
 * @param[in] it Pointer to the iterator to check
 * @return true if there are more elements, false otherwise
 */
bool zzArrayListIteratorHasNext(const zzArrayListIterator *it) {
    return it && it->state == ZZ_ITER_VALID && it->index < it->list->size;
}