/**
 * @file treeSet.h
 * @brief Red-black tree-based set implementation for ordered key storage.
 *
 * This module implements a balanced binary search tree (red-black tree) that stores
 * unique keys in sorted order. The implementation provides O(log n) time complexity
 * for insertion, lookup, and deletion operations. The tree maintains balance through
 * rotations and color flips to ensure logarithmic height. The implementation supports
 * generic key types with customizable comparison and memory management functions.
 */

#ifndef TREE_SET_H
#define TREE_SET_H

#include "types.h"
#include "utils.h"
#include "result.h"
#include "iterator.h"

/**
 * @brief Structure representing a node in the red-black tree.
 *
 * Each node contains pointers to its left child, right child, and parent,
 * a color indicator for red-black tree balancing, and a flexible array member
 * to store the key data.
 */
typedef struct TreeSetNode {
    struct TreeSetNode *left;   /**< Pointer to the left child node */
    struct TreeSetNode *right;  /**< Pointer to the right child node */
    struct TreeSetNode *parent; /**< Pointer to the parent node */
    zzRBColor color;            /**< Color of the node (RED or BLACK) for red-black tree balancing */
    unsigned char key[];        /**< Flexible array member to store the key data */
} TreeSetNode;

/**
 * @brief Structure representing a red-black tree set.
 *
 * This structure maintains the root of the tree, tracks the current size,
 * and provides function pointers for comparison and memory management.
 */
typedef struct zzTreeSet {
    TreeSetNode *root;      /**< Pointer to the root node of the red-black tree */
    size_t size;            /**< Current number of keys in the set */
    size_t keySize;         /**< Size in bytes of each key */
    zzCompareFn compareFn;  /**< Function to compare keys for ordering */
    zzFreeFn keyFree;       /**< Function to free key memory, or NULL if not needed */
} zzTreeSet;

/**
 * @brief Structure representing an iterator for TreeSet.
 *
 * This structure provides in-order iteration through a TreeSet,
 * using parent pointers to traverse the tree in sorted order.
 */
typedef struct zzTreeSetIterator {
    zzTreeSet *set;                 /**< Pointer to the TreeSet being iterated */
    TreeSetNode *currentNode;       /**< Current node in the iteration */
    TreeSetNode *lastReturned;      /**< Pointer to the last returned node */
    zzIteratorState state;          /**< Current state of the iterator */
} zzTreeSetIterator;

/**
 * @brief Initializes a new TreeSet with the specified key size.
 *
 * This function initializes a TreeSet structure with the given key size
 * and custom functions for comparison and memory management. The tree set will be
 * empty after initialization.
 *
 * @param[out] ts Pointer to the TreeSet structure to initialize
 * @param[in] keySize Size in bytes of each key that will be stored in the set
 * @param[in] compareFn Function to compare keys for ordering (returns negative if a<b, 0 if a==b, positive if a>b)
 * @param[in] keyFree Function to free key memory when entries are removed or the set is freed, or NULL if not needed
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzTreeSetInit(zzTreeSet *ts, size_t keySize, zzCompareFn compareFn, zzFreeFn keyFree);

/**
 * @brief Frees all resources associated with the TreeSet.
 *
 * This function releases all memory used by the TreeSet, including calling
 * the custom free function for each key if provided. After this function
 * returns, the TreeSet structure should not be used until reinitialized.
 *
 * @param[in,out] ts Pointer to the TreeSet to free
 */
void zzTreeSetFree(zzTreeSet *ts);

/**
 * @brief Inserts a key into the TreeSet.
 *
 * This function inserts a new key into the tree set if it doesn't already exist.
 * The key is copied into the tree's internal storage. The tree is automatically
 * rebalanced after the insertion to maintain red-black tree properties.
 *
 * @param[in,out] ts Pointer to the TreeSet to insert into
 * @param[in] key Pointer to the key to insert (contents will be copied)
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzTreeSetInsert(zzTreeSet *ts, const void *key);

/**
 * @brief Checks if the TreeSet contains the specified key.
 *
 * This function checks whether the given key exists in the tree set.
 *
 * @param[in] ts Pointer to the TreeSet to check
 * @param[in] key Pointer to the key to look for
 * @return true if the key exists in the set, false otherwise
 */
bool zzTreeSetContains(const zzTreeSet *ts, const void *key);

/**
 * @brief Removes the specified key from the TreeSet.
 *
 * This function removes the given key from the tree set if it exists.
 * If a custom free function was provided for keys, it will be called
 * on the removed key. The tree is automatically rebalanced after
 * the removal to maintain red-black tree properties. The function returns an error
 * if the key is not present in the set.
 *
 * @param[in,out] ts Pointer to the TreeSet to remove from
 * @param[in] key Pointer to the key to remove
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzTreeSetRemove(zzTreeSet *ts, const void *key);

/**
 * @brief Clears all keys from the TreeSet.
 *
 * This function removes all keys from the tree set by calling the custom free
 * function for each key (if provided) and deallocating all nodes. The tree set
 * becomes empty after this operation.
 *
 * @param[in,out] ts Pointer to the TreeSet to clear
 */
void zzTreeSetClear(zzTreeSet *ts);

/**
 * @brief Gets the minimum key from the TreeSet.
 *
 * This function retrieves the smallest key in the tree set and copies it to the
 * output buffer. The minimum element is the leftmost node in the binary search tree.
 *
 * @param[in] ts Pointer to the TreeSet to retrieve from
 * @param[out] keyOut Pointer to a buffer where the minimum key will be copied
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzTreeSetGetMin(const zzTreeSet *ts, void *keyOut);

/**
 * @brief Gets the maximum key from the TreeSet.
 *
 * This function retrieves the largest key in the tree set and copies it to the
 * output buffer. The maximum element is the rightmost node in the binary search tree.
 *
 * @param[in] ts Pointer to the TreeSet to retrieve from
 * @param[out] keyOut Pointer to a buffer where the maximum key will be copied
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzTreeSetGetMax(const zzTreeSet *ts, void *keyOut);

/**
 * @brief Initializes an iterator for the TreeSet.
 *
 * This function initializes an iterator to traverse the TreeSet in sorted order
 * (in-order traversal). The iterator will visit keys from smallest to largest
 * according to the comparison function.
 *
 * @param[out] it Pointer to the iterator structure to initialize
 * @param[in] ts Pointer to the TreeSet to iterate over
 */
void zzTreeSetIteratorInit(zzTreeSetIterator *it, zzTreeSet *ts);

/**
 * @brief Advances the iterator to the next key.
 *
 * This function moves the iterator to the next key in sorted order
 * and copies the current key to the output buffer. Returns false when
 * the iterator reaches the end of the set.
 *
 * @param[in,out] it Pointer to the iterator to advance
 * @param[out] keyOut Pointer to a buffer where the current key will be copied
 * @return true if a key was retrieved, false if the iterator reached the end
 */
bool zzTreeSetIteratorNext(zzTreeSetIterator *it, void *keyOut);

/**
 * @brief Checks if the iterator has more elements.
 *
 * This function checks whether the iterator can advance to another key
 * without actually advancing it.
 *
 * @param[in] it Pointer to the iterator to check
 * @return true if there are more elements, false otherwise
 */
bool zzTreeSetIteratorHasNext(const zzTreeSetIterator *it);

/**
 * @brief Removes the last key returned by the iterator.
 *
 * This function removes the key that was most recently returned by
 * zzTreeSetIteratorNext. It safely handles the tree restructuring by rebuilding
 * the iterator stack to the correct next element.
 *
 * @param[in,out] it Pointer to the iterator
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzTreeSetIteratorRemove(zzTreeSetIterator *it);

#endif