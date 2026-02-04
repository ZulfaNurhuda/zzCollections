/**
 * @file treeMap.h
 * @brief Red-black tree-based map implementation for ordered key-value storage.
 *
 * This module implements a balanced binary search tree (red-black tree) that maps
 * keys to values in sorted order. The implementation provides O(log n) time complexity
 * for insertion, lookup, and deletion operations. The tree maintains balance through
 * rotations and color flips to ensure logarithmic height. The implementation supports
 * generic key and value types with customizable comparison and memory management functions.
 */

#ifndef TREE_MAP_H
#define TREE_MAP_H

#include "types.h"
#include "utils.h"
#include "result.h"
#include "iterator.h"

/**
 * @brief Structure representing a node in the red-black tree.
 *
 * Each node contains pointers to its left child, right child, and parent,
 * a color indicator for red-black tree balancing, and a flexible array member
 * to store both key and value data.
 */
typedef struct TreeMapNode {
    struct TreeMapNode *left;   /**< Pointer to the left child node */
    struct TreeMapNode *right;  /**< Pointer to the right child node */
    struct TreeMapNode *parent; /**< Pointer to the parent node */
    zzRBColor color;            /**< Color of the node (RED or BLACK) for red-black tree balancing */
    unsigned char data[];       /**< Flexible array member to store key and value data */
} TreeMapNode;

/**
 * @brief Structure representing a red-black tree map.
 *
 * This structure maintains the root of the tree, tracks the current size,
 * and provides function pointers for comparison and memory management.
 */
typedef struct zzTreeMap {
    TreeMapNode *root;      /**< Pointer to the root node of the red-black tree */
    size_t size;            /**< Current number of key-value pairs in the tree */
    size_t keySize;         /**< Size in bytes of each key */
    size_t valueSize;       /**< Size in bytes of each value */
    zzCompareFn compareFn;  /**< Function to compare keys for ordering */
    zzFreeFn keyFree;       /**< Function to free key memory, or NULL if not needed */
    zzFreeFn valueFree;     /**< Function to free value memory, or NULL if not needed */
} zzTreeMap;

/**
 * @brief Structure representing an iterator for TreeMap.
 *
 * This structure provides in-order iteration through a TreeMap,
 * maintaining a stack of nodes to traverse the tree in sorted order.
 */
typedef struct zzTreeMapIterator {
    zzTreeMap *map;                 /**< Pointer to the TreeMap being iterated */
    TreeMapNode **stack;            /**< Stack of nodes for in-order traversal */
    TreeMapNode *lastReturned;      /**< Pointer to the last returned node */
    size_t stackSize;               /**< Current size of the stack */
    size_t stackCapacity;           /**< Maximum capacity of the stack */
    zzIteratorState state;          /**< Current state of the iterator */
} zzTreeMapIterator;

/**
 * @brief Initializes a new TreeMap with the specified key and value sizes.
 *
 * This function initializes a TreeMap structure with the given key and value sizes
 * and custom functions for comparison and memory management. The tree map will be
 * empty after initialization.
 *
 * @param[out] tm Pointer to the TreeMap structure to initialize
 * @param[in] keySize Size in bytes of each key that will be stored in the map
 * @param[in] valueSize Size in bytes of each value that will be stored in the map
 * @param[in] compareFn Function to compare keys for ordering (returns negative if a<b, 0 if a==b, positive if a>b)
 * @param[in] keyFree Function to free key memory when entries are removed or the map is freed, or NULL if not needed
 * @param[in] valueFree Function to free value memory when entries are removed or the map is freed, or NULL if not needed
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzTreeMapInit(zzTreeMap *tm, size_t keySize, size_t valueSize, zzCompareFn compareFn, zzFreeFn keyFree, zzFreeFn valueFree);

/**
 * @brief Frees all resources associated with the TreeMap.
 *
 * This function releases all memory used by the TreeMap, including calling
 * the custom free functions for each key and value if provided. After this
 * function returns, the TreeMap structure should not be used until reinitialized.
 *
 * @param[in,out] tm Pointer to the TreeMap to free
 */
void zzTreeMapFree(zzTreeMap *tm);

/**
 * @brief Inserts or updates a key-value pair in the TreeMap.
 *
 * This function inserts a new key-value pair into the tree map, or updates the
 * value if the key already exists. Both the key and value are copied into
 * the tree's internal storage. The tree is automatically rebalanced after
 * the insertion to maintain red-black tree properties.
 *
 * @param[in,out] tm Pointer to the TreeMap to insert/update in
 * @param[in] key Pointer to the key to insert/update (contents will be copied)
 * @param[in] value Pointer to the value to insert/update (contents will be copied)
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzTreeMapPut(zzTreeMap *tm, const void *key, const void *value);

/**
 * @brief Retrieves the value associated with the given key from the TreeMap.
 *
 * This function looks up the specified key in the tree map and copies the
 * associated value to the output buffer if found. The function returns an error
 * if the key is not present in the map.
 *
 * @param[in] tm Pointer to the TreeMap to retrieve from
 * @param[in] key Pointer to the key to look up
 * @param[out] valueOut Pointer to a buffer where the value will be copied if the key is found
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzTreeMapGet(const zzTreeMap *tm, const void *key, void *valueOut);

/**
 * @brief Checks if the TreeMap contains the specified key.
 *
 * This function checks whether the given key exists in the tree map.
 *
 * @param[in] tm Pointer to the TreeMap to check
 * @param[in] key Pointer to the key to look for
 * @return true if the key exists in the map, false otherwise
 */
bool zzTreeMapContains(const zzTreeMap *tm, const void *key);

/**
 * @brief Removes the key-value pair associated with the given key from the TreeMap.
 *
 * This function removes the entry with the specified key from the tree map.
 * If a custom free function was provided for keys or values, it will be called
 * on the removed key and value. The tree is automatically rebalanced after
 * the removal to maintain red-black tree properties. The function returns an error
 * if the key is not present in the map.
 *
 * @param[in,out] tm Pointer to the TreeMap to remove from
 * @param[in] key Pointer to the key to remove
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzTreeMapRemove(zzTreeMap *tm, const void *key);

/**
 * @brief Clears all entries from the TreeMap.
 *
 * This function removes all key-value pairs from the tree map by calling the
 * custom free functions for each key and value (if provided) and deallocating
 * all nodes. The tree map becomes empty after this operation.
 *
 * @param[in,out] tm Pointer to the TreeMap to clear
 */
void zzTreeMapClear(zzTreeMap *tm);

/**
 * @brief Gets the minimum key-value pair from the TreeMap.
 *
 * This function retrieves the key-value pair with the smallest key in the tree map
 * and copies both the key and value to the output buffers. The minimum element
 * is the leftmost node in the binary search tree.
 *
 * @param[in] tm Pointer to the TreeMap to retrieve from
 * @param[out] keyOut Pointer to a buffer where the minimum key will be copied
 * @param[out] valueOut Pointer to a buffer where the value associated with the minimum key will be copied
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzTreeMapGetMin(const zzTreeMap *tm, void *keyOut, void *valueOut);

/**
 * @brief Gets the maximum key-value pair from the TreeMap.
 *
 * This function retrieves the key-value pair with the largest key in the tree map
 * and copies both the key and value to the output buffers. The maximum element
 * is the rightmost node in the binary search tree.
 *
 * @param[in] tm Pointer to the TreeMap to retrieve from
 * @param[out] keyOut Pointer to a buffer where the maximum key will be copied
 * @param[out] valueOut Pointer to a buffer where the value associated with the maximum key will be copied
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzTreeMapGetMax(const zzTreeMap *tm, void *keyOut, void *valueOut);

/**
 * @brief Initializes an iterator for the TreeMap.
 *
 * This function initializes an iterator to traverse the TreeMap in sorted order
 * (in-order traversal). The iterator will visit key-value pairs from smallest
 * to largest key according to the comparison function.
 *
 * @param[out] it Pointer to the iterator structure to initialize
 * @param[in] tm Pointer to the TreeMap to iterate over
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzTreeMapIteratorInit(zzTreeMapIterator *it, zzTreeMap *tm);

/**
 * @brief Frees resources associated with the TreeMap iterator.
 *
 * This function releases the memory used by the iterator's internal stack.
 * The iterator should not be used after calling this function.
 *
 * @param[in,out] it Pointer to the iterator to free
 */
void zzTreeMapIteratorFree(zzTreeMapIterator *it);

/**
 * @brief Advances the iterator to the next key-value pair.
 *
 * This function moves the iterator to the next key-value pair in sorted order
 * and copies the current key and value to the output buffers. Returns false when
 * the iterator reaches the end of the map.
 *
 * @param[in,out] it Pointer to the iterator to advance
 * @param[out] keyOut Pointer to a buffer where the current key will be copied
 * @param[out] valueOut Pointer to a buffer where the current value will be copied
 * @return true if a key-value pair was retrieved, false if the iterator reached the end
 */
bool zzTreeMapIteratorNext(zzTreeMapIterator *it, void *keyOut, void *valueOut);

/**
 * @brief Checks if the iterator has more elements.
 *
 * This function checks whether the iterator can advance to another key-value pair
 * without actually advancing it.
 *
 * @param[in] it Pointer to the iterator to check
 * @return true if there are more elements, false otherwise
 */
bool zzTreeMapIteratorHasNext(const zzTreeMapIterator *it);

/**
 * @brief Removes the last key-value pair returned by the iterator.
 *
 * This function removes the key-value pair that was most recently returned by
 * zzTreeMapIteratorNext. It safely handles the tree restructuring by rebuilding
 * the iterator stack to the correct next element.
 *
 * @param[in,out] it Pointer to the iterator
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzTreeMapIteratorRemove(zzTreeMapIterator *it);

#endif