#include "treeMap.h"
#include <string.h>
#include <stdlib.h>

#define KEY_PTR(node) ((node)->data)
#define VAL_PTR(node, keySize) ((node)->data + (keySize))

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
zzOpResult zzTreeMapInit(zzTreeMap *tm, size_t keySize, size_t valueSize, zzCompareFn compareFn, zzFreeFn keyFree, zzFreeFn valueFree) {
    if (!tm) return ZZ_ERR("TreeMap pointer is NULL");
    if (keySize == 0) return ZZ_ERR("Key size cannot be zero");
    if (valueSize == 0) return ZZ_ERR("Value size cannot be zero");
    if (!compareFn) return ZZ_ERR("Comparison function is NULL");

    tm->root = NULL;
    tm->size = 0;
    tm->keySize = keySize;
    tm->valueSize = valueSize;
    tm->compareFn = compareFn;
    tm->keyFree = keyFree;
    tm->valueFree = valueFree;
    return ZZ_OK();
}

static void zzTreeMapFreeNode(zzTreeMap *tm, TreeMapNode *node) {
    if (!node) return;
    zzTreeMapFreeNode(tm, node->left);
    zzTreeMapFreeNode(tm, node->right);
    if (tm->keyFree) tm->keyFree(KEY_PTR(node));
    if (tm->valueFree) tm->valueFree(VAL_PTR(node, tm->keySize));
    free(node);
}

/**
 * @brief Frees all resources associated with the TreeMap.
 *
 * This function releases all memory used by the TreeMap, including calling
 * the custom free functions for each key and value if provided. After this
 * function returns, the TreeMap structure should not be used until reinitialized.
 *
 * @param[in,out] tm Pointer to the TreeMap to free
 */
void zzTreeMapFree(zzTreeMap *tm) {
    if (!tm) return;
    zzTreeMapFreeNode(tm, tm->root);
    tm->root = NULL;
    tm->size = 0;
}

static void rotateLeft(zzTreeMap *tm, TreeMapNode *x) {
    TreeMapNode *y = x->right;
    x->right = y->left;
    if (y->left) y->left->parent = x;
    y->parent = x->parent;
    
    if (!x->parent) tm->root = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;
    
    y->left = x;
    x->parent = y;
}

static void rotateRight(zzTreeMap *tm, TreeMapNode *y) {
    TreeMapNode *x = y->left;
    y->left = x->right;
    if (x->right) x->right->parent = y;
    x->parent = y->parent;
    
    if (!y->parent) tm->root = x;
    else if (y == y->parent->right) y->parent->right = x;
    else y->parent->left = x;
    
    x->right = y;
    y->parent = x;
}

static void insertFixup(zzTreeMap *tm, TreeMapNode *z) {
    while (z->parent && z->parent->color == ZZ_RED) {
        if (z->parent == z->parent->parent->left) {
            TreeMapNode *y = z->parent->parent->right;
            if (y && y->color == ZZ_RED) {
                z->parent->color = ZZ_BLACK;
                y->color = ZZ_BLACK;
                z->parent->parent->color = ZZ_RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    rotateLeft(tm, z);
                }
                z->parent->color = ZZ_BLACK;
                z->parent->parent->color = ZZ_RED;
                rotateRight(tm, z->parent->parent);
            }
        } else {
            TreeMapNode *y = z->parent->parent->left;
            if (y && y->color == ZZ_RED) {
                z->parent->color = ZZ_BLACK;
                y->color = ZZ_BLACK;
                z->parent->parent->color = ZZ_RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rotateRight(tm, z);
                }
                z->parent->color = ZZ_BLACK;
                z->parent->parent->color = ZZ_RED;
                rotateLeft(tm, z->parent->parent);
            }
        }
    }
    tm->root->color = ZZ_BLACK;
}

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
zzOpResult zzTreeMapPut(zzTreeMap *tm, const void *key, const void *value) {
    if (!tm) return ZZ_ERR("TreeMap pointer is NULL");
    if (!key) return ZZ_ERR("Key pointer is NULL");
    if (!value) return ZZ_ERR("Value pointer is NULL");

    TreeMapNode *parent = NULL;
    TreeMapNode *cur = tm->root;

    while (cur) {
        parent = cur;
        int cmp = tm->compareFn(key, KEY_PTR(cur));
        if (cmp == 0) {
            if (tm->valueFree) tm->valueFree(VAL_PTR(cur, tm->keySize));
            memcpy(VAL_PTR(cur, tm->keySize), value, tm->valueSize);
            return ZZ_OK();
        }
        cur = (cmp < 0) ? cur->left : cur->right;
    }

    TreeMapNode *node = malloc(sizeof(TreeMapNode) + tm->keySize + tm->valueSize);
    if (!node) return ZZ_ERR("Failed to allocate node");

    memcpy(KEY_PTR(node), key, tm->keySize);
    memcpy(VAL_PTR(node, tm->keySize), value, tm->valueSize);
    node->left = node->right = NULL;
    node->parent = parent;
    node->color = ZZ_RED;

    if (!parent) tm->root = node;
    else if (tm->compareFn(key, KEY_PTR(parent)) < 0) parent->left = node;
    else parent->right = node;

    tm->size++;
    insertFixup(tm, node);
    return ZZ_OK();
}

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
zzOpResult zzTreeMapGet(const zzTreeMap *tm, const void *key, void *valueOut) {
    if (!tm) return ZZ_ERR("TreeMap pointer is NULL");
    if (!key) return ZZ_ERR("Key pointer is NULL");
    if (!valueOut) return ZZ_ERR("Value output pointer is NULL");

    TreeMapNode *cur = tm->root;
    while (cur) {
        int cmp = tm->compareFn(key, KEY_PTR(cur));
        if (cmp == 0) {
            memcpy(valueOut, VAL_PTR(cur, tm->keySize), tm->valueSize);
            return ZZ_OK();
        }
        cur = (cmp < 0) ? cur->left : cur->right;
    }
    return ZZ_ERR("Key not found");
}

/**
 * @brief Checks if the TreeMap contains the specified key.
 *
 * This function checks whether the given key exists in the tree map.
 *
 * @param[in] tm Pointer to the TreeMap to check
 * @param[in] key Pointer to the key to look for
 * @return true if the key exists in the map, false otherwise
 */
bool zzTreeMapContains(const zzTreeMap *tm, const void *key) {
    if (!tm || !key) return false;
    
    TreeMapNode *cur = tm->root;
    while (cur) {
        int cmp = tm->compareFn(key, KEY_PTR(cur));
        if (cmp == 0) return true;
        cur = (cmp < 0) ? cur->left : cur->right;
    }
    return false;
}

static TreeMapNode *zzTreeMapMin(TreeMapNode *node) {
    while (node && node->left) node = node->left;
    return node;
}

static TreeMapNode *zzTreeMapMax(TreeMapNode *node) {
    while (node && node->right) node = node->right;
    return node;
}

static void transplant(zzTreeMap *tm, TreeMapNode *u, TreeMapNode *v) {
    if (!u->parent) tm->root = v;
    else if (u == u->parent->left) u->parent->left = v;
    else u->parent->right = v;
    if (v) v->parent = u->parent;
}

static void deleteFixup(zzTreeMap *tm, TreeMapNode *x, TreeMapNode *xParent) {
    while (x != tm->root && (!x || x->color == ZZ_BLACK)) {
        if (x == (xParent ? xParent->left : NULL)) {
            TreeMapNode *w = xParent->right;
            if (w && w->color == ZZ_RED) {
                w->color = ZZ_BLACK;
                xParent->color = ZZ_RED;
                rotateLeft(tm, xParent);
                w = xParent->right;
            }
            if (w && (!w->left || w->left->color == ZZ_BLACK) &&
                (!w->right || w->right->color == ZZ_BLACK)) {
                w->color = ZZ_RED;
                x = xParent;
                xParent = x ? x->parent : NULL;
            } else {
                if (w && (!w->right || w->right->color == ZZ_BLACK)) {
                    if (w->left) w->left->color = ZZ_BLACK;
                    w->color = ZZ_RED;
                    rotateRight(tm, w);
                    w = xParent->right;
                }
                if (w) {
                    w->color = xParent->color;
                    if (w->right) w->right->color = ZZ_BLACK;
                }
                xParent->color = ZZ_BLACK;
                rotateLeft(tm, xParent);
                x = tm->root;
            }
        } else {
            TreeMapNode *w = xParent->left;
            if (w && w->color == ZZ_RED) {
                w->color = ZZ_BLACK;
                xParent->color = ZZ_RED;
                rotateRight(tm, xParent);
                w = xParent->left;
            }
            if (w && (!w->right || w->right->color == ZZ_BLACK) &&
                (!w->left || w->left->color == ZZ_BLACK)) {
                w->color = ZZ_RED;
                x = xParent;
                xParent = x ? x->parent : NULL;
            } else {
                if (w && (!w->left || w->left->color == ZZ_BLACK)) {
                    if (w->right) w->right->color = ZZ_BLACK;
                    w->color = ZZ_RED;
                    rotateLeft(tm, w);
                    w = xParent->left;
                }
                if (w) {
                    w->color = xParent->color;
                    if (w->left) w->left->color = ZZ_BLACK;
                }
                xParent->color = ZZ_BLACK;
                rotateRight(tm, xParent);
                x = tm->root;
            }
        }
    }
    if (x) x->color = ZZ_BLACK;
}

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
zzOpResult zzTreeMapRemove(zzTreeMap *tm, const void *key) {
    if (!tm) return ZZ_ERR("TreeMap pointer is NULL");
    if (!key) return ZZ_ERR("Key pointer is NULL");

    TreeMapNode *z = tm->root;
    while (z) {
        int cmp = tm->compareFn(key, KEY_PTR(z));
        if (cmp == 0) break;
        z = (cmp < 0) ? z->left : z->right;
    }
    if (!z) return ZZ_ERR("Key not found");

    TreeMapNode *y = z;
    TreeMapNode *x, *xParent;
    zzRBColor yOrigColor = y->color;

    if (!z->left) {
        x = z->right;
        xParent = z->parent;
        transplant(tm, z, z->right);
    } else if (!z->right) {
        x = z->left;
        xParent = z->parent;
        transplant(tm, z, z->left);
    } else {
        y = zzTreeMapMin(z->right);
        yOrigColor = y->color;
        x = y->right;
        xParent = y;

        if (y->parent == z) {
            if (x) x->parent = y;
            xParent = y;
        } else {
            xParent = y->parent;
            transplant(tm, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }

        transplant(tm, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    if (tm->keyFree) tm->keyFree(KEY_PTR(z));
    if (tm->valueFree) tm->valueFree(VAL_PTR(z, tm->keySize));
    free(z);
    tm->size--;

    if (yOrigColor == ZZ_BLACK) {
        deleteFixup(tm, x, xParent);
    }

    return ZZ_OK();
}

/**
 * @brief Clears all entries from the TreeMap.
 *
 * This function removes all key-value pairs from the tree map by calling the
 * custom free functions for each key and value (if provided) and deallocating
 * all nodes. The tree map becomes empty after this operation.
 *
 * @param[in,out] tm Pointer to the TreeMap to clear
 */
void zzTreeMapClear(zzTreeMap *tm) {
    if (!tm) return;
    zzTreeMapFreeNode(tm, tm->root);
    tm->root = NULL;
    tm->size = 0;
}

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
zzOpResult zzTreeMapGetMin(const zzTreeMap *tm, void *keyOut, void *valueOut) {
    if (!tm) return ZZ_ERR("TreeMap pointer is NULL");
    if (!tm->root) return ZZ_ERR("Tree is empty");

    TreeMapNode *min = zzTreeMapMin(tm->root);
    if (keyOut) memcpy(keyOut, KEY_PTR(min), tm->keySize);
    if (valueOut) memcpy(valueOut, VAL_PTR(min, tm->keySize), tm->valueSize);
    return ZZ_OK();
}

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
zzOpResult zzTreeMapGetMax(const zzTreeMap *tm, void *keyOut, void *valueOut) {
    if (!tm) return ZZ_ERR("TreeMap pointer is NULL");
    if (!tm->root) return ZZ_ERR("Tree is empty");

    TreeMapNode *max = zzTreeMapMax(tm->root);
    if (keyOut) memcpy(keyOut, KEY_PTR(max), tm->keySize);
    if (valueOut) memcpy(valueOut, VAL_PTR(max, tm->keySize), tm->valueSize);
    return ZZ_OK();
}

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
zzOpResult zzTreeMapIteratorInit(zzTreeMapIterator *it, zzTreeMap *tm) {
    if (!it) return ZZ_ERR("Iterator pointer is NULL");
    if (!tm) return ZZ_ERR("TreeMap pointer is NULL");
    
    it->map = tm;
    it->stackSize = 0;
    it->lastReturned = NULL;
    it->stackCapacity = tm->size + 1;
    it->state = ZZ_ITER_END;
    
    if (it->stackCapacity == 0) it->stackCapacity = 1;
    
    it->stack = malloc(sizeof(TreeMapNode*) * it->stackCapacity);
    if (!it->stack) return ZZ_ERR("Failed to allocate iterator stack");
    
    TreeMapNode *current = tm->root;
    while (current) {
        it->stack[it->stackSize++] = current;
        current = current->left;
    }
    
    if (it->stackSize > 0) {
        it->state = ZZ_ITER_VALID;
    }
    
    return ZZ_OK();
}

/**
 * @brief Frees resources associated with the TreeMap iterator.
 *
 * This function releases the memory used by the iterator's internal stack.
 * The iterator should not be used after calling this function.
 *
 * @param[in,out] it Pointer to the iterator to free
 */
void zzTreeMapIteratorFree(zzTreeMapIterator *it) {
    if (!it) return;
    
    if (it->stack) {
        free(it->stack);
        it->stack = NULL;
    }
    it->stackSize = 0;
    it->stackCapacity = 0;
    it->state = ZZ_ITER_END;
}

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
bool zzTreeMapIteratorNext(zzTreeMapIterator *it, void *keyOut, void *valueOut) {
    if (!it || !keyOut || !valueOut || it->state != ZZ_ITER_VALID || it->stackSize == 0) return false;
    
    TreeMapNode *current = it->stack[--it->stackSize];
    it->lastReturned = current;
    
    memcpy(keyOut, current->data, it->map->keySize);
    memcpy(valueOut, (char*)current->data + it->map->keySize, it->map->valueSize);
    
    current = current->right;
    while (current) {
        it->stack[it->stackSize++] = current;
        current = current->left;
    }
    
    return true;
}

/**
 * @brief Checks if the iterator has more elements.
 *
 * This function checks whether the iterator can advance to another key-value pair
 * without actually advancing it.
 *
 * @param[in] it Pointer to the iterator to check
 * @return true if there are more elements, false otherwise
 */
bool zzTreeMapIteratorHasNext(const zzTreeMapIterator *it) {
    return it && it->state == ZZ_ITER_VALID && it->stackSize > 0;
}

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
zzOpResult zzTreeMapIteratorRemove(zzTreeMapIterator *it) {
    if (!it || it->state != ZZ_ITER_VALID) return ZZ_ERR("Invalid iterator state");
    if (!it->lastReturned) return ZZ_ERR("No element to remove");

    void *nextKey = NULL;
    if (it->stackSize > 0) {
        TreeMapNode *next = it->stack[it->stackSize - 1];
        nextKey = malloc(it->map->keySize);
        if (!nextKey) return ZZ_ERR("Memory allocation failed");
        memcpy(nextKey, next->data, it->map->keySize);
    }

    void *removeKey = malloc(it->map->keySize);
    if (!removeKey) {
        if (nextKey) free(nextKey);
        return ZZ_ERR("Memory allocation failed");
    }
    memcpy(removeKey, it->lastReturned->data, it->map->keySize);

    zzOpResult res = zzTreeMapRemove(it->map, removeKey);
    free(removeKey);

    if (ZZ_IS_ERR(res)) {
        if (nextKey) free(nextKey);
        return res;
    }

    it->lastReturned = NULL;

    if (nextKey) {
        it->stackSize = 0;
        
        TreeMapNode *cur = it->map->root;
        while (cur) {
            int cmp = it->map->compareFn(nextKey, cur->data);
            if (cmp == 0) {
                it->stack[it->stackSize++] = cur;
                break;
            } else if (cmp < 0) {
                it->stack[it->stackSize++] = cur;
                cur = cur->left;
            } else {
                cur = cur->right;
            }
        }
        
        if (it->stackSize > 0) {
             it->state = ZZ_ITER_VALID;
        } else {
             it->state = ZZ_ITER_END;
        }
        
        free(nextKey);
    } else {
        it->stackSize = 0;
        it->state = ZZ_ITER_END;
    }

    return ZZ_OK();
}