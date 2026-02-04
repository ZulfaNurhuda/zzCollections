#include "treeSet.h"
#include <string.h>
#include <stdlib.h>

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
zzOpResult zzTreeSetInit(zzTreeSet *ts, size_t keySize, zzCompareFn compareFn, zzFreeFn keyFree) {
    if (!ts) return ZZ_ERR("TreeSet pointer is NULL");
    if (keySize == 0) return ZZ_ERR("Key size cannot be zero");
    if (!compareFn) return ZZ_ERR("Comparison function is NULL");

    ts->root = NULL;
    ts->size = 0;
    ts->keySize = keySize;
    ts->compareFn = compareFn;
    ts->keyFree = keyFree;
    return ZZ_OK();
}

static void zzTreeSetFreeNode(zzTreeSet *ts, TreeSetNode *node) {
    if (!node) return;
    zzTreeSetFreeNode(ts, node->left);
    zzTreeSetFreeNode(ts, node->right);
    if (ts->keyFree) ts->keyFree(node->key);
    free(node);
}

/**
 * @brief Frees all resources associated with the TreeSet.
 *
 * This function releases all memory used by the TreeSet, including calling
 * the custom free function for each key if provided. After this function
 * returns, the TreeSet structure should not be used until reinitialized.
 *
 * @param[in,out] ts Pointer to the TreeSet to free
 */
void zzTreeSetFree(zzTreeSet *ts) {
    if (!ts) return;
    zzTreeSetFreeNode(ts, ts->root);
    ts->root = NULL;
    ts->size = 0;
}

static void tsRotateLeft(zzTreeSet *ts, TreeSetNode *x) {
    TreeSetNode *y = x->right;
    x->right = y->left;
    if (y->left) y->left->parent = x;
    y->parent = x->parent;
    
    if (!x->parent) ts->root = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;
    
    y->left = x;
    x->parent = y;
}

static void tsRotateRight(zzTreeSet *ts, TreeSetNode *y) {
    TreeSetNode *x = y->left;
    y->left = x->right;
    if (x->right) x->right->parent = y;
    x->parent = y->parent;
    
    if (!y->parent) ts->root = x;
    else if (y == y->parent->right) y->parent->right = x;
    else y->parent->left = x;
    
    x->right = y;
    y->parent = x;
}

static void tsInsertFixup(zzTreeSet *ts, TreeSetNode *z) {
    while (z->parent && z->parent->color == ZZ_RED) {
        if (z->parent == z->parent->parent->left) {
            TreeSetNode *y = z->parent->parent->right;
            if (y && y->color == ZZ_RED) {
                z->parent->color = ZZ_BLACK;
                y->color = ZZ_BLACK;
                z->parent->parent->color = ZZ_RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    tsRotateLeft(ts, z);
                }
                z->parent->color = ZZ_BLACK;
                z->parent->parent->color = ZZ_RED;
                tsRotateRight(ts, z->parent->parent);
            }
        } else {
            TreeSetNode *y = z->parent->parent->left;
            if (y && y->color == ZZ_RED) {
                z->parent->color = ZZ_BLACK;
                y->color = ZZ_BLACK;
                z->parent->parent->color = ZZ_RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    tsRotateRight(ts, z);
                }
                z->parent->color = ZZ_BLACK;
                z->parent->parent->color = ZZ_RED;
                tsRotateLeft(ts, z->parent->parent);
            }
        }
    }
    ts->root->color = ZZ_BLACK;
}

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
zzOpResult zzTreeSetInsert(zzTreeSet *ts, const void *key) {
    if (!ts) return ZZ_ERR("TreeSet pointer is NULL");
    if (!key) return ZZ_ERR("Key pointer is NULL");

    TreeSetNode *parent = NULL;
    TreeSetNode *cur = ts->root;

    while (cur) {
        parent = cur;
        int cmp = ts->compareFn(key, cur->key);
        if (cmp == 0) return ZZ_ERR("Key already exists");
        cur = (cmp < 0) ? cur->left : cur->right;
    }

    TreeSetNode *node = malloc(sizeof(TreeSetNode) + ts->keySize);
    if (!node) return ZZ_ERR("Failed to allocate node");

    memcpy(node->key, key, ts->keySize);
    node->left = node->right = NULL;
    node->parent = parent;
    node->color = ZZ_RED;

    if (!parent) ts->root = node;
    else if (ts->compareFn(key, parent->key) < 0) parent->left = node;
    else parent->right = node;

    ts->size++;
    tsInsertFixup(ts, node);
    return ZZ_OK();
}

/**
 * @brief Checks if the TreeSet contains the specified key.
 *
 * This function checks whether the given key exists in the tree set.
 *
 * @param[in] ts Pointer to the TreeSet to check
 * @param[in] key Pointer to the key to look for
 * @return true if the key exists in the set, false otherwise
 */
bool zzTreeSetContains(const zzTreeSet *ts, const void *key) {
    if (!ts || !key) return false;
    
    TreeSetNode *cur = ts->root;
    while (cur) {
        int cmp = ts->compareFn(key, cur->key);
        if (cmp == 0) return true;
        cur = (cmp < 0) ? cur->left : cur->right;
    }
    return false;
}

static TreeSetNode *zzTreeSetMin(TreeSetNode *node) {
    while (node && node->left) node = node->left;
    return node;
}

static TreeSetNode *zzTreeSetMax(TreeSetNode *node) {
    while (node && node->right) node = node->right;
    return node;
}

static void tsTransplant(zzTreeSet *ts, TreeSetNode *u, TreeSetNode *v) {
    if (!u->parent) ts->root = v;
    else if (u == u->parent->left) u->parent->left = v;
    else u->parent->right = v;
    if (v) v->parent = u->parent;
}

static void tsDeleteFixup(zzTreeSet *ts, TreeSetNode *x, TreeSetNode *xParent) {
    while (x != ts->root && (!x || x->color == ZZ_BLACK)) {
        if (x == (xParent ? xParent->left : NULL)) {
            TreeSetNode *w = xParent->right;
            if (w && w->color == ZZ_RED) {
                w->color = ZZ_BLACK;
                xParent->color = ZZ_RED;
                tsRotateLeft(ts, xParent);
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
                    tsRotateRight(ts, w);
                    w = xParent->right;
                }
                if (w) {
                    w->color = xParent->color;
                    if (w->right) w->right->color = ZZ_BLACK;
                }
                xParent->color = ZZ_BLACK;
                tsRotateLeft(ts, xParent);
                x = ts->root;
            }
        } else {
            TreeSetNode *w = xParent->left;
            if (w && w->color == ZZ_RED) {
                w->color = ZZ_BLACK;
                xParent->color = ZZ_RED;
                tsRotateRight(ts, xParent);
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
                    tsRotateLeft(ts, w);
                    w = xParent->left;
                }
                if (w) {
                    w->color = xParent->color;
                    if (w->left) w->left->color = ZZ_BLACK;
                }
                xParent->color = ZZ_BLACK;
                tsRotateRight(ts, xParent);
                x = ts->root;
            }
        }
    }
    if (x) x->color = ZZ_BLACK;
}

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
zzOpResult zzTreeSetRemove(zzTreeSet *ts, const void *key) {
    if (!ts) return ZZ_ERR("TreeSet pointer is NULL");
    if (!key) return ZZ_ERR("Key pointer is NULL");

    TreeSetNode *z = ts->root;
    while (z) {
        int cmp = ts->compareFn(key, z->key);
        if (cmp == 0) break;
        z = (cmp < 0) ? z->left : z->right;
    }
    if (!z) return ZZ_ERR("Key not found");

    TreeSetNode *y = z;
    TreeSetNode *x, *xParent;
    zzRBColor yOrigColor = y->color;

    if (!z->left) {
        x = z->right;
        xParent = z->parent;
        tsTransplant(ts, z, z->right);
    } else if (!z->right) {
        x = z->left;
        xParent = z->parent;
        tsTransplant(ts, z, z->left);
    } else {
        y = zzTreeSetMin(z->right);
        yOrigColor = y->color;
        x = y->right;
        xParent = y;

        if (y->parent == z) {
            if (x) x->parent = y;
            xParent = y;
        } else {
            xParent = y->parent;
            tsTransplant(ts, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }

        tsTransplant(ts, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    if (ts->keyFree) ts->keyFree(z->key);
    free(z);
    ts->size--;

    if (yOrigColor == ZZ_BLACK) {
        tsDeleteFixup(ts, x, xParent);
    }

    return ZZ_OK();
}

/**
 * @brief Clears all keys from the TreeSet.
 *
 * This function removes all keys from the tree set by calling the custom free
 * function for each key (if provided) and deallocating all nodes. The tree set
 * becomes empty after this operation.
 *
 * @param[in,out] ts Pointer to the TreeSet to clear
 */
void zzTreeSetClear(zzTreeSet *ts) {
    if (!ts) return;
    zzTreeSetFreeNode(ts, ts->root);
    ts->root = NULL;
    ts->size = 0;
}

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
zzOpResult zzTreeSetGetMin(const zzTreeSet *ts, void *keyOut) {
    if (!ts) return ZZ_ERR("TreeSet pointer is NULL");
    if (!keyOut) return ZZ_ERR("Key output pointer is NULL");
    if (!ts->root) return ZZ_ERR("Set is empty");

    TreeSetNode *min = zzTreeSetMin(ts->root);
    memcpy(keyOut, min->key, ts->keySize);
    return ZZ_OK();
}

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
zzOpResult zzTreeSetGetMax(const zzTreeSet *ts, void *keyOut) {
    if (!ts) return ZZ_ERR("TreeSet pointer is NULL");
    if (!keyOut) return ZZ_ERR("Key output pointer is NULL");
    if (!ts->root) return ZZ_ERR("Set is empty");

    TreeSetNode *max = zzTreeSetMax(ts->root);
    memcpy(keyOut, max->key, ts->keySize);
    return ZZ_OK();
}

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
void zzTreeSetIteratorInit(zzTreeSetIterator *it, zzTreeSet *ts) {
    if (!it || !ts) return;

    it->set = ts;
    it->lastReturned = NULL;

    // Find the minimum node (leftmost node) to start iteration
    it->currentNode = ts->root;
    if (it->currentNode) {
        while (it->currentNode->left) {
            it->currentNode = it->currentNode->left;
        }
    }

    it->state = it->currentNode ? ZZ_ITER_VALID : ZZ_ITER_END;
}


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
bool zzTreeSetIteratorNext(zzTreeSetIterator *it, void *keyOut) {
    if (!it || !keyOut || it->state != ZZ_ITER_VALID || !it->currentNode) return false;

    TreeSetNode *current = it->currentNode;
    it->lastReturned = current;

    memcpy(keyOut, current->key, it->set->keySize);

    // Find the in-order successor
    if (current->right) {
        // If current node has a right child, go to right child, then all the way left
        current = current->right;
        while (current->left) {
            current = current->left;
        }
        it->currentNode = current;
    } else {
        // If current node has no right child, go up to parent until we come from a left child
        TreeSetNode *parent = current->parent;
        while (parent && current == parent->right) {
            current = parent;
            parent = parent->parent;
        }
        it->currentNode = parent;
    }

    if (!it->currentNode) {
        it->state = ZZ_ITER_END;
    }

    return true;
}

/**
 * @brief Checks if the iterator has more elements.
 *
 * This function checks whether the iterator can advance to another key
 * without actually advancing it.
 *
 * @param[in] it Pointer to the iterator to check
 * @return true if there are more elements, false otherwise
 */
bool zzTreeSetIteratorHasNext(const zzTreeSetIterator *it) {
    return it && it->state == ZZ_ITER_VALID && it->currentNode != NULL;
}

/**
 * @brief Removes the last key returned by the iterator.
 *
 * This function removes the key that was most recently returned by
 * zzTreeSetIteratorNext. It safely handles the tree restructuring by rebuilding
 * the iterator position to the correct next element.
 *
 * @param[in,out] it Pointer to the iterator
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzTreeSetIteratorRemove(zzTreeSetIterator *it) {
    if (!it || it->state != ZZ_ITER_VALID) return ZZ_ERR("Invalid iterator state");
    if (!it->lastReturned) return ZZ_ERR("No element to remove");

    // Save the key of the next node if there is one
    void *nextKey = NULL;
    if (it->currentNode) {
        nextKey = malloc(it->set->keySize);
        if (!nextKey) return ZZ_ERR("Memory allocation failed");
        memcpy(nextKey, it->currentNode->key, it->set->keySize);
    }

    // Save the key of the node to be removed
    void *removeKey = malloc(it->set->keySize);
    if (!removeKey) {
        if (nextKey) free(nextKey);
        return ZZ_ERR("Memory allocation failed");
    }
    memcpy(removeKey, it->lastReturned->key, it->set->keySize);

    zzOpResult res = zzTreeSetRemove(it->set, removeKey);
    free(removeKey);

    if (ZZ_IS_ERR(res)) {
        if (nextKey) free(nextKey);
        return res;
    }

    it->lastReturned = NULL;

    if (nextKey) {
        // Find the node corresponding to the saved next key
        TreeSetNode *cur = it->set->root;
        while (cur) {
            int cmp = it->set->compareFn(nextKey, cur->key);
            if (cmp == 0) {
                it->currentNode = cur;
                break;
            } else if (cmp < 0) {
                cur = cur->left;
            } else {
                cur = cur->right;
            }
        }

        if (it->currentNode) {
             it->state = ZZ_ITER_VALID;
        } else {
             it->state = ZZ_ITER_END;
        }

        free(nextKey);
    } else {
        it->currentNode = NULL;
        it->state = ZZ_ITER_END;
    }

    return ZZ_OK();
}