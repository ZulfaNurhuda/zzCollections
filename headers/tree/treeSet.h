#ifndef TREE_SET_H
#define TREE_SET_H

#include "common.h"

typedef enum { TS_RED, TS_BLACK } TSColor;

typedef struct TreeSetNode {
    struct TreeSetNode *left;
    struct TreeSetNode *right;
    struct TreeSetNode *parent;
    TSColor color;
    unsigned char key[];
} TreeSetNode;

typedef struct {
    TreeSetNode *root;
    size_t keySize;
    size_t size;
    zzCompareFn compareFn;
    zzFreeFn keyFree;
} zzTreeSet;

bool zzTreeSetInit(zzTreeSet *ts, size_t keySize, zzCompareFn compareFn, zzFreeFn keyFree);
void zzTreeSetFree(zzTreeSet *ts);
bool zzTreeSetInsert(zzTreeSet *ts, const void *key);
bool zzTreeSetContains(const zzTreeSet *ts, const void *key);
bool zzTreeSetRemove(zzTreeSet *ts, const void *key);
void zzTreeSetClear(zzTreeSet *ts);
bool zzTreeSetGetMin(const zzTreeSet *ts, void *keyOut);
bool zzTreeSetGetMax(const zzTreeSet *ts, void *keyOut);

#endif
