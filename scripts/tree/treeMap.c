#include "treeMap.h"
#include <string.h>
#include <stdlib.h>

#define KEY_PTR(node) ((node)->data)
#define VAL_PTR(node, keySize) ((node)->data + (keySize))

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

void zzTreeMapClear(zzTreeMap *tm) {
    if (!tm) return;
    zzTreeMapFreeNode(tm, tm->root);
    tm->root = NULL;
    tm->size = 0;
}

zzOpResult zzTreeMapGetMin(const zzTreeMap *tm, void *keyOut, void *valueOut) {
    if (!tm) return ZZ_ERR("TreeMap pointer is NULL");
    if (!tm->root) return ZZ_ERR("Tree is empty");

    TreeMapNode *min = zzTreeMapMin(tm->root);
    if (keyOut) memcpy(keyOut, KEY_PTR(min), tm->keySize);
    if (valueOut) memcpy(valueOut, VAL_PTR(min, tm->keySize), tm->valueSize);
    return ZZ_OK();
}

zzOpResult zzTreeMapGetMax(const zzTreeMap *tm, void *keyOut, void *valueOut) {
    if (!tm) return ZZ_ERR("TreeMap pointer is NULL");
    if (!tm->root) return ZZ_ERR("Tree is empty");

    TreeMapNode *max = zzTreeMapMax(tm->root);
    if (keyOut) memcpy(keyOut, KEY_PTR(max), tm->keySize);
    if (valueOut) memcpy(valueOut, VAL_PTR(max, tm->keySize), tm->valueSize);
    return ZZ_OK();
}