#include "treeSet.h"
#include <string.h>
#include <stdlib.h>

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

void zzTreeSetClear(zzTreeSet *ts) {
    if (!ts) return;
    zzTreeSetFreeNode(ts, ts->root);
    ts->root = NULL;
    ts->size = 0;
}

zzOpResult zzTreeSetGetMin(const zzTreeSet *ts, void *keyOut) {
    if (!ts) return ZZ_ERR("TreeSet pointer is NULL");
    if (!keyOut) return ZZ_ERR("Key output pointer is NULL");
    if (!ts->root) return ZZ_ERR("Set is empty");

    TreeSetNode *min = zzTreeSetMin(ts->root);
    memcpy(keyOut, min->key, ts->keySize);
    return ZZ_OK();
}

zzOpResult zzTreeSetGetMax(const zzTreeSet *ts, void *keyOut) {
    if (!ts) return ZZ_ERR("TreeSet pointer is NULL");
    if (!keyOut) return ZZ_ERR("Key output pointer is NULL");
    if (!ts->root) return ZZ_ERR("Set is empty");

    TreeSetNode *max = zzTreeSetMax(ts->root);
    memcpy(keyOut, max->key, ts->keySize);
    return ZZ_OK();
}