/**
 * @file arrayList.c
 * @brief Dynamic array implementation with automatic resizing
 *
 * Source implementation for the ArrayList data structure that provides
 * O(1) random access and amortized O(1) append operations.
 * Automatically grows when capacity is reached.
 */

#include "arrayList.h"
#include <string.h>
#include <stdlib.h>

/**
 * @brief Initialize an ArrayList with specified element size and capacity
 *
 * Allocates internal buffer and sets up the data structure for use.
 * The ArrayList must be freed with zzArrayListFree when done.
 *
 * @param[out] al Pointer to ArrayList to initialize
 * @param[in] elSize Size of each element in bytes (must be > 0)
 * @param[in] capacity Initial capacity (use 0 for default of 4)
 * @param[in] elemFree Optional callback to free elements (can be NULL)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: ArrayList initialized successfully
 *         - ERROR: Possible errors:
 *           * "ArrayList pointer is NULL"
 *           * "Element size cannot be zero"
 *           * "Failed to allocate buffer memory"
 *
 * @see zzArrayListFree
 *
 * @par Example:
 * @code
 * zzArrayList list;
 * zzOpResult result = zzArrayListInit(&list, sizeof(int), 10, NULL);
 * if (ZZ_IS_OK(result)) {
 *     // Use list...
 *     zzArrayListFree(&list);
 * } else {
 *     fprintf(stderr, "Init failed: %s\n", result.error);
 * }
 * @endcode
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
 * @brief Free all memory associated with ArrayList
 *
 * Calls elemFree callback on all elements (if provided), then frees
 * internal buffer. The ArrayList struct itself is not freed (caller owns it).
 * After calling, the ArrayList is in undefined state and must be
 * re-initialized before reuse.
 *
 * @param[in,out] al Pointer to ArrayList to free
 *
 * @pre al must be initialized with zzArrayListInit
 * @post All internal memory is freed
 * @post If elemFree was provided, it has been called on all elements
 * @post al must not be used until re-initialized
 *
 * @note Safe to call with NULL pointer (no-op)
 * @note Does not free the ArrayList struct itself
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
 * @brief Internal function to grow the ArrayList buffer when capacity is reached
 *
 * Doubles the capacity of the internal buffer to accommodate more elements.
 * This operation may fail if sufficient memory is not available.
 *
 * @param[in,out] al Pointer to ArrayList to grow
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Buffer grown successfully
 *         - ERROR: "Failed to grow buffer (realloc failed)" if allocation fails
 *
 * @note This is an internal function and should not be called directly
 */
static zzOpResult zzArrayListGrow(zzArrayList *al) {
    size_t newCap = al->capacity * 2;
    void *newBuf = realloc(al->buffer, al->elSize * newCap);
    if (!newBuf) return ZZ_ERR("Failed to grow buffer (realloc failed)");
    al->buffer = newBuf;
    al->capacity = newCap;
    return ZZ_OK();
}

/**
 * @brief Add an element to the end of the ArrayList
 *
 * Inserts a copy of the element data at the end. Automatically grows
 * the buffer if at capacity (may fail if out of memory).
 *
 * @param[in,out] al Pointer to initialized ArrayList
 * @param[in] elem Pointer to element data to add (will be copied)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element added successfully
 *         - ERROR: Possible errors:
 *           * "ArrayList pointer is NULL"
 *           * "Element pointer is NULL"
 *           * "Failed to grow buffer (realloc failed)"
 *
 * @pre al must be initialized with zzArrayListInit
 * @pre elem must point to valid data of size elSize
 * @post If successful, al->size increases by 1
 * @post elem data is copied into ArrayList (caller retains ownership)
 *
 * @see zzArrayListRemove
 * @see zzArrayListInsert
 */
zzOpResult zzArrayListAdd(zzArrayList *al, const void *elem) {
    if (!al) return ZZ_ERR("ArrayList pointer is NULL");
    if (!elem) return ZZ_ERR("Element pointer is NULL");

    if (al->size == al->capacity) {
        zzOpResult growResult = zzArrayListGrow(al);
        if (ZZ_IS_ERR(growResult)) {
            return growResult;  // Propagate error
        }
    }

    memcpy((char*)al->buffer + al->size * al->elSize, elem, al->elSize);
    al->size++;
    return ZZ_OK();
}

/**
 * @brief Get element at specified index
 *
 * Retrieves a copy of the element at the specified index.
 * The element data is copied to the output buffer.
 *
 * @param[in] al Pointer to ArrayList
 * @param[in] idx Index of element to retrieve (0-based)
 * @param[out] out Buffer where element will be copied (must be elSize bytes)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element retrieved successfully
 *         - ERROR: Possible errors:
 *           * "ArrayList pointer is NULL"
 *           * "Output buffer is NULL"
 *           * "Index out of bounds"
 *
 * @pre al must be initialized
 * @pre out must point to buffer of at least elSize bytes
 * @pre idx must be < al->size
 * @post out contains copy of element data
 *
 * @see zzArrayListSet
 */
zzOpResult zzArrayListGet(const zzArrayList *al, size_t idx, void *out) {
    if (!al) return ZZ_ERR("ArrayList pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (idx >= al->size) return ZZ_ERR("Index out of bounds");

    memcpy(out, (char*)al->buffer + idx * al->elSize, al->elSize);
    return ZZ_OK();
}

/**
 * @brief Set element at specified index
 *
 * Replaces the element at the specified index with new data.
 * Calls elemFree on old element if provided.
 *
 * @param[in,out] al Pointer to ArrayList
 * @param[in] idx Index of element to replace (0-based)
 * @param[in] elem Pointer to new element data (will be copied)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element replaced successfully
 *         - ERROR: Possible errors:
 *           * "ArrayList pointer is NULL"
 *           * "Element pointer is NULL"
 *           * "Index out of bounds"
 *
 * @pre al must be initialized
 * @pre elem must point to valid data of size elSize
 * @pre idx must be < al->size
 * @post Element at idx is replaced with new data
 * @post If elemFree provided, old element has been freed
 *
 * @see zzArrayListGet
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
 * @brief Remove element at specified index
 *
 * Removes element and shifts remaining elements left.
 * Calls elemFree on removed element if provided.
 *
 * @param[in,out] al Pointer to ArrayList
 * @param[in] idx Index of element to remove (0-based)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element removed successfully
 *         - ERROR: Possible errors:
 *           * "ArrayList pointer is NULL"
 *           * "Index out of bounds"
 *
 * @pre al must be initialized
 * @pre idx must be < al->size
 * @post Element at idx is removed
 * @post Elements after idx are shifted left
 * @post al->size decreases by 1
 * @post If elemFree provided, removed element has been freed
 *
 * @see zzArrayListAdd
 * @see zzArrayListClear
 */
zzOpResult zzArrayListRemove(zzArrayList *al, size_t idx) {
    if (!al) return ZZ_ERR("ArrayList pointer is NULL");
    if (idx >= al->size) return ZZ_ERR("Index out of bounds");

    void *target = (char*)al->buffer + idx * al->elSize;
    if (al->elemFree) {
        al->elemFree(target);
    }

    // Shift elements left (overlapping memory, use memmove)
    if (idx < al->size - 1) {
        memmove(target, (char*)target + al->elSize,
                     (al->size - idx - 1) * al->elSize);
    }
    al->size--;
    return ZZ_OK();
}

/**
 * @brief Clear all elements from ArrayList
 *
 * Removes all elements (calls elemFree if provided) but keeps
 * allocated buffer. Size becomes 0, capacity unchanged.
 *
 * @param[in,out] al Pointer to ArrayList
 *
 * @pre al must be initialized
 * @post al->size is 0
 * @post al->capacity unchanged
 * @post If elemFree provided, it has been called on all elements
 *
 * @note Does not free the internal buffer (capacity preserved)
 *
 * @see zzArrayListFree
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
 * @brief Insert element at specified index
 *
 * Inserts element and shifts existing elements right.
 * May grow buffer if at capacity.
 *
 * @param[in,out] al Pointer to ArrayList
 * @param[in] idx Index where element will be inserted (0-based, can be == size)
 * @param[in] elem Pointer to element data to insert (will be copied)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element inserted successfully
 *         - ERROR: Possible errors:
 *           * "ArrayList pointer is NULL"
 *           * "Element pointer is NULL"
 *           * "Index out of bounds"
 *           * "Failed to grow buffer (realloc failed)"
 *
 * @pre al must be initialized
 * @pre elem must point to valid data of size elSize
 * @pre idx must be <= al->size (can insert at end)
 * @post Element inserted at idx
 * @post Elements at idx and after are shifted right
 * @post al->size increases by 1
 *
 * @see zzArrayListAdd
 * @see zzArrayListRemove
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

    // Shift elements right (overlapping memory, use memmove)
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
 * @brief Find index of first matching element
 *
 * Uses provided comparison function to search for element.
 * Linear search from beginning to end.
 *
 * @param[in] al Pointer to ArrayList
 * @param[in] elem Pointer to element data to find
 * @param[in] cmp Comparison function (returns 0 for match)
 * @param[out] indexOut Pointer to int where index will be stored
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element found, index stored in indexOut
 *         - ERROR: "ArrayList pointer is NULL" / "Element pointer is NULL" / "Comparison function is NULL" / "Index output pointer is NULL" / "Element not found"
 *
 * @pre al must be initialized
 * @pre elem must point to valid data of size elSize
 * @pre cmp must be valid comparison function
 * @pre indexOut must point to valid int
 *
 * @note This is O(n) operation
 *
 * @see zzArrayListGet
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
