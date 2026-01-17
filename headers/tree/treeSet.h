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

#endif