/**
 * @file treeSet.h
 * @brief Sorted unique keys using Red-Black tree
 *
 * TreeSet provides O(log n) operations with keys kept in sorted order.
 * Self-balancing binary search tree for unique elements.
 */

#ifndef TREE_SET_H
#define TREE_SET_H

#include "types.h"
#include "utils.h"
#include "result.h"

typedef enum { TS_RED, TS_BLACK } TSColor;

typedef struct TreeSetNode {
    struct TreeSetNode *left;
    struct TreeSetNode *right;
    struct TreeSetNode *parent;
    TSColor color;
    unsigned char key[];
} TreeSetNode;

/**
 * @brief TreeSet provides sorted unique keys using Red-Black tree
 *
 * Self-balancing binary search tree with O(log n) operations.
 * Keys are kept in sorted order as defined by comparison function.
 */
typedef struct zzTreeSet {
    TreeSetNode *root;
    size_t keySize;
    size_t size;
    zzCompareFn compareFn;
    zzFreeFn keyFree;
} zzTreeSet;

/**
 * @brief Initialize TreeSet with specified key size and comparison function
 *
 * Sets up Red-Black tree structure. Comparison function is required.
 * Must be freed with zzTreeSetFree when done.
 *
 * @param[out] ts Pointer to TreeSet to initialize
 * @param[in] keySize Size of each key in bytes (must be > 0)
 * @param[in] compareFn Comparison function (required, cannot be NULL)
 * @param[in] keyFree Optional key cleanup function (can be NULL)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: TreeSet initialized successfully
 *         - ERROR: Possible errors:
 *           * "TreeSet pointer is NULL"
 *           * "Key size cannot be zero"
 *           * "Comparison function is NULL"
 *
 * @see zzTreeSetFree
 */
zzOpResult zzTreeSetInit(zzTreeSet *ts, size_t keySize, zzCompareFn compareFn, zzFreeFn keyFree);

/**
 * @brief Free all memory associated with TreeSet
 *
 * Recursively frees all nodes, calling keyFree on keys.
 * The TreeSet struct itself is not freed.
 *
 * @param[in,out] ts Pointer to TreeSet to free
 *
 * @note Safe to call with NULL pointer (no-op)
 */
void zzTreeSetFree(zzTreeSet *ts);

/**
 * @brief Insert key into set
 *
 * Adds new key to set and maintains tree balance.
 * Keys are stored in sorted order. Duplicates are not allowed.
 *
 * @param[in,out] ts Pointer to TreeSet
 * @param[in] key Pointer to key data (will be copied)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Key inserted successfully
 *         - ERROR: Possible errors:
 *           * "TreeSet pointer is NULL"
 *           * "Key pointer is NULL"
 *           * "Key already exists"
 *           * "Failed to allocate node"
 */
zzOpResult zzTreeSetInsert(zzTreeSet *ts, const void *key);

/**
 * @brief Check if key exists in set
 *
 * @param[in] ts Pointer to TreeSet
 * @param[in] key Pointer to key to check
 *
 * @return true if key exists, false otherwise
 */
bool zzTreeSetContains(const zzTreeSet *ts, const void *key);

/**
 * @brief Remove key from set
 *
 * Binary search for key, then removes node and rebalances tree.
 * Calls keyFree if provided.
 *
 * @param[in,out] ts Pointer to TreeSet
 * @param[in] key Pointer to key to remove
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Key removed successfully
 *         - ERROR: Possible errors:
 *           * "TreeSet pointer is NULL"
 *           * "Key pointer is NULL"
 *           * "Key not found"
 */
zzOpResult zzTreeSetRemove(zzTreeSet *ts, const void *key);

/**
 * @brief Clear all keys from set
 *
 * Recursively removes all nodes (calls keyFree if provided).
 * Size becomes 0.
 *
 * @param[in,out] ts Pointer to TreeSet
 */
void zzTreeSetClear(zzTreeSet *ts);

/**
 * @brief Get minimum key
 *
 * Retrieves leftmost node in tree (smallest key).
 *
 * @param[in] ts Pointer to TreeSet
 * @param[out] keyOut Buffer where key will be copied (must be keySize bytes)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Minimum key retrieved successfully
 *         - ERROR: Possible errors:
 *           * "TreeSet pointer is NULL"
 *           * "Key output pointer is NULL"
 *           * "Set is empty"
 */
zzOpResult zzTreeSetGetMin(const zzTreeSet *ts, void *keyOut);

/**
 * @brief Get maximum key
 *
 * Retrieves rightmost node in tree (largest key).
 *
 * @param[in] ts Pointer to TreeSet
 * @param[out] keyOut Buffer where key will be copied (must be keySize bytes)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Maximum key retrieved successfully
 *         - ERROR: Possible errors:
 *           * "TreeSet pointer is NULL"
 *           * "Key output pointer is NULL"
 *           * "Set is empty"
 */
zzOpResult zzTreeSetGetMax(const zzTreeSet *ts, void *keyOut);

#endif
