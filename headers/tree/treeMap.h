/**
 * @file treeMap.h
 * @brief Sorted key-value map using Red-Black tree
 *
 * TreeMap provides O(log n) operations with keys kept in sorted order.
 * Self-balancing binary search tree implementation.
 */

#ifndef TREE_MAP_H
#define TREE_MAP_H

#include "types.h"
#include "utils.h"
#include "result.h"

typedef enum { RB_RED, RB_BLACK } RBColor;

typedef struct TreeMapNode {
    struct TreeMapNode *left;
    struct TreeMapNode *right;
    struct TreeMapNode *parent;
    RBColor color;
    unsigned char data[];
} TreeMapNode;

/**
 * @brief TreeMap provides sorted key-value mapping using Red-Black tree
 *
 * Self-balancing binary search tree with O(log n) operations.
 * Keys are kept in sorted order as defined by comparison function.
 */
typedef struct zzTreeMap {
    TreeMapNode *root;
    size_t keySize;
    size_t valueSize;
    size_t size;
    zzCompareFn compareFn;
    zzFreeFn keyFree;
    zzFreeFn valueFree;
} zzTreeMap;

/**
 * @brief Initialize TreeMap with specified sizes and comparison function
 *
 * Sets up Red-Black tree structure. Comparison function is required.
 * Must be freed with zzTreeMapFree when done.
 *
 * @param[out] tm Pointer to TreeMap to initialize
 * @param[in] keySize Size of each key in bytes (must be > 0)
 * @param[in] valueSize Size of each value in bytes (must be > 0)
 * @param[in] compareFn Comparison function (required, cannot be NULL)
 * @param[in] keyFree Optional key cleanup function (can be NULL)
 * @param[in] valueFree Optional value cleanup function (can be NULL)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: TreeMap initialized successfully
 *         - ERROR: Possible errors:
 *           * "TreeMap pointer is NULL"
 *           * "Key size cannot be zero"
 *           * "Value size cannot be zero"
 *           * "Comparison function is NULL"
 *
 * @see zzTreeMapFree
 */
zzOpResult zzTreeMapInit(zzTreeMap *tm, size_t keySize, size_t valueSize,
                 zzCompareFn compareFn, zzFreeFn keyFree, zzFreeFn valueFree);

/**
 * @brief Free all memory associated with TreeMap
 *
 * Recursively frees all nodes, calling keyFree and valueFree on entries.
 * The TreeMap struct itself is not freed.
 *
 * @param[in,out] tm Pointer to TreeMap to free
 *
 * @note Safe to call with NULL pointer (no-op)
 */
void zzTreeMapFree(zzTreeMap *tm);

/**
 * @brief Put key-value pair into map
 *
 * Inserts new entry or updates existing one. Maintains tree balance.
 * Keys are stored in sorted order according to comparison function.
 *
 * @param[in,out] tm Pointer to TreeMap
 * @param[in] key Pointer to key data (will be copied)
 * @param[in] value Pointer to value data (will be copied)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Entry added or updated successfully
 *         - ERROR: Possible errors:
 *           * "TreeMap pointer is NULL"
 *           * "Key pointer is NULL"
 *           * "Value pointer is NULL"
 *           * "Failed to allocate node"
 */
zzOpResult zzTreeMapPut(zzTreeMap *tm, const void *key, const void *value);

/**
 * @brief Get value by key
 *
 * Binary search for key in tree (O(log n)).
 *
 * @param[in] tm Pointer to TreeMap
 * @param[in] key Pointer to key to search for
 * @param[out] valueOut Buffer where value will be copied (must be valueSize bytes)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Value retrieved successfully
 *         - ERROR: Possible errors:
 *           * "TreeMap pointer is NULL"
 *           * "Key pointer is NULL"
 *           * "Value output pointer is NULL"
 *           * "Key not found"
 */
zzOpResult zzTreeMapGet(const zzTreeMap *tm, const void *key, void *valueOut);

/**
 * @brief Check if key exists in map
 *
 * @param[in] tm Pointer to TreeMap
 * @param[in] key Pointer to key to check
 *
 * @return true if key exists, false otherwise
 */
bool zzTreeMapContains(const zzTreeMap *tm, const void *key);

/**
 * @brief Remove key-value pair from map
 *
 * Binary search for key, then removes node and rebalances tree.
 * Calls keyFree and valueFree if provided.
 *
 * @param[in,out] tm Pointer to TreeMap
 * @param[in] key Pointer to key to remove
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Entry removed successfully
 *         - ERROR: Possible errors:
 *           * "TreeMap pointer is NULL"
 *           * "Key pointer is NULL"
 *           * "Key not found"
 */
zzOpResult zzTreeMapRemove(zzTreeMap *tm, const void *key);

/**
 * @brief Clear all entries from map
 *
 * Recursively removes all nodes (calls keyFree and valueFree if provided).
 * Size becomes 0.
 *
 * @param[in,out] tm Pointer to TreeMap
 */
void zzTreeMapClear(zzTreeMap *tm);

/**
 * @brief Get minimum key-value pair
 *
 * Retrieves leftmost node in tree (smallest key).
 *
 * @param[in] tm Pointer to TreeMap
 * @param[out] keyOut Buffer for key (can be NULL to skip)
 * @param[out] valueOut Buffer for value (can be NULL to skip)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Minimum entry retrieved successfully
 *         - ERROR: Possible errors:
 *           * "TreeMap pointer is NULL"
 *           * "Tree is empty"
 */
zzOpResult zzTreeMapGetMin(const zzTreeMap *tm, void *keyOut, void *valueOut);

/**
 * @brief Get maximum key-value pair
 *
 * Retrieves rightmost node in tree (largest key).
 *
 * @param[in] tm Pointer to TreeMap
 * @param[out] keyOut Buffer for key (can be NULL to skip)
 * @param[out] valueOut Buffer for value (can be NULL to skip)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Maximum entry retrieved successfully
 *         - ERROR: Possible errors:
 *           * "TreeMap pointer is NULL"
 *           * "Tree is empty"
 */
zzOpResult zzTreeMapGetMax(const zzTreeMap *tm, void *keyOut, void *valueOut);

#endif
