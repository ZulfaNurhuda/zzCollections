#ifndef TREE_MAP_H
#define TREE_MAP_H

#include "common.h"

typedef enum { RB_RED, RB_BLACK } RBColor;

typedef struct TreeMapNode {
    struct TreeMapNode *left;
    struct TreeMapNode *right;
    struct TreeMapNode *parent;
    RBColor color;
    unsigned char data[];
} TreeMapNode;

typedef struct {
    TreeMapNode *root;
    size_t keySize;
    size_t valueSize;
    size_t size;
    zzCompareFn compareFn;
    zzFreeFn keyFree;
    zzFreeFn valueFree;
} zzTreeMap;

bool zzTreeMapInit(zzTreeMap *tm, size_t keySize, size_t valueSize,
                 zzCompareFn compareFn, zzFreeFn keyFree, zzFreeFn valueFree);
void zzTreeMapFree(zzTreeMap *tm);
bool zzTreeMapPut(zzTreeMap *tm, const void *key, const void *value);
bool zzTreeMapGet(const zzTreeMap *tm, const void *key, void *valueOut);
bool zzTreeMapContains(const zzTreeMap *tm, const void *key);
bool zzTreeMapRemove(zzTreeMap *tm, const void *key);
void zzTreeMapClear(zzTreeMap *tm);
bool zzTreeMapGetMin(const zzTreeMap *tm, void *keyOut, void *valueOut);
bool zzTreeMapGetMax(const zzTreeMap *tm, void *keyOut, void *valueOut);

#endif
