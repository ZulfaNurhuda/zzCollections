/**
 * @file arrayList.h
 * @brief Dynamic array implementation providing list-like functionality.
 *
 * This module implements a dynamic array (ArrayList) that provides O(1) average
 * time complexity for append operations and O(n) for insertions/deletions at arbitrary positions.
 * The ArrayList automatically resizes when needed and supports generic element types.
 */

#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

#include "types.h"
#include "utils.h"
#include "result.h"

/**
 * @brief Structure representing a dynamic array (ArrayList).
 *
 * This structure stores elements in a contiguous memory block that can grow
 * dynamically as elements are added. It tracks the current size, capacity,
 * element size, and provides a custom free function for element cleanup.
 */
typedef struct zzArrayList {
    void *buffer;      /**< Pointer to the underlying buffer storing elements */
    size_t size;       /**< Current number of elements in the list */
    size_t capacity;   /**< Maximum number of elements the buffer can hold */
    size_t elSize;     /**< Size in bytes of each individual element */
    zzFreeFn elemFree; /**< Function to free individual elements, or NULL if not needed */
} zzArrayList;

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
zzOpResult zzArrayListInit(zzArrayList *al, size_t elSize, size_t capacity, zzFreeFn elemFree);

/**
 * @brief Frees all resources associated with the ArrayList.
 *
 * This function releases all memory used by the ArrayList, including calling
 * the custom free function for each element if provided. After this function
 * returns, the ArrayList structure should not be used until reinitialized.
 *
 * @param[in,out] al Pointer to the ArrayList to free
 */
void zzArrayListFree(zzArrayList *al);

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
zzOpResult zzArrayListAdd(zzArrayList *al, const void *elem);

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
zzOpResult zzArrayListGet(const zzArrayList *al, size_t idx, void *out);

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
zzOpResult zzArrayListSet(zzArrayList *al, size_t idx, const void *elem);

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
zzOpResult zzArrayListRemove(zzArrayList *al, size_t idx);

/**
 * @brief Clears all elements from the ArrayList.
 *
 * This function removes all elements from the list by calling the custom free
 * function on each element (if provided) and resetting the size to zero.
 * The underlying buffer remains allocated with the same capacity.
 *
 * @param[in,out] al Pointer to the ArrayList to clear
 */
void zzArrayListClear(zzArrayList *al);

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
zzOpResult zzArrayListInsert(zzArrayList *al, size_t idx, const void *elem);

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
zzOpResult zzArrayListIndexOf(const zzArrayList *al, const void *elem, zzCompareFn cmp, int *indexOut);

#endif