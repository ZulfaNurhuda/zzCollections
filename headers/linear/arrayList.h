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
#include "iterator.h"

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
 * @brief Structure representing an iterator for ArrayList.
 *
 * This structure provides forward iteration through an ArrayList,
 * maintaining the current position and reference to the list.
 */
typedef struct zzArrayListIterator {
    zzArrayList *list;       /**< Pointer to the ArrayList being iterated */
    size_t index;            /**< Current index position in the list */
    zzIteratorState state;   /**< Current state of the iterator */
} zzArrayListIterator;

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
void zzArrayListIteratorInit(zzArrayListIterator *it, zzArrayList *al);

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
bool zzArrayListIteratorNext(zzArrayListIterator *it, void *valueOut);

/**
 * @brief Checks if the iterator has more elements.
 *
 * This function checks whether the iterator can advance to another element
 * without actually advancing it.
 *
 * @param[in] it Pointer to the iterator to check
 * @return true if there are more elements, false otherwise
 */
bool zzArrayListIteratorHasNext(const zzArrayListIterator *it);

/**
 * @brief Removes the last element returned by the iterator.
 *
 * This function removes the element that was most recently returned by
 * zzArrayListIteratorNext. After removal, the iterator remains valid and
 * continues to the next element on the next call to Next.
 *
 * @param[in,out] it Pointer to the iterator
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzArrayListIteratorRemove(zzArrayListIterator *it);

#endif