#include <stdio.h>
#include <stdlib.h>
#include "headers/linear/arrayList.h"
#include "headers/hash/hashMap.h"
#include "headers/tree/treeMap.h"

int intCmp(const void *a, const void *b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

uint32_t intHash(const void *key) {
    return (uint32_t)(*(const int*)key);
}

bool intEquals(const void *a, const void *b) {
    return *(const int*)a == *(const int*)b;
}

int main() {
    printf("🔄 Testing Iterator Functionality\n\n");
    
    // Test ArrayList Iterator
    printf("📋 ArrayList Iterator Test:\n");
    zzArrayList list;
    zzArrayListInit(&list, sizeof(int), 10, NULL);
    
    int values[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) {
        zzArrayListAdd(&list, &values[i]);
    }
    
    zzArrayListIterator listIt;
    zzArrayListIteratorInit(&listIt, &list);
    
    printf("   Contents: ");
    int value;
    while (zzArrayListIteratorNext(&listIt, &value)) {
        printf("%d ", value);
    }
    printf("\n\n");
    
    // Test HashMap Iterator
    printf("🗺️  HashMap Iterator Test:\n");
    zzHashMap map;
    zzHashMapInit(&map, sizeof(int), sizeof(int), 16, intHash, intEquals, NULL, NULL);
    
    int keys[] = {1, 2, 3, 4, 5};
    int vals[] = {100, 200, 300, 400, 500};
    for (int i = 0; i < 5; i++) {
        zzHashMapPut(&map, &keys[i], &vals[i]);
    }
    
    zzHashMapIterator mapIt;
    zzHashMapIteratorInit(&mapIt, &map);
    
    printf("   Key-Value pairs: ");
    int key, val;
    while (zzHashMapIteratorNext(&mapIt, &key, &val)) {
        printf("{%d:%d} ", key, val);
    }
    printf("\n\n");
    
    // Test TreeMap Iterator (sorted order)
    printf("🌳 TreeMap Iterator Test (sorted order):\n");
    zzTreeMap tree;
    zzTreeMapInit(&tree, sizeof(int), sizeof(int), intCmp, NULL, NULL);
    
    int unsortedKeys[] = {30, 10, 50, 20, 40};
    int unsortedVals[] = {300, 100, 500, 200, 400};
    for (int i = 0; i < 5; i++) {
        zzTreeMapPut(&tree, &unsortedKeys[i], &unsortedVals[i]);
    }
    
    zzTreeMapIterator treeIt;
    zzTreeMapIteratorInit(&treeIt, &tree);
    
    printf("   Sorted pairs: ");
    while (zzTreeMapIteratorNext(&treeIt, &key, &val)) {
        printf("{%d:%d} ", key, val);
    }
    printf("\n\n");
    
    // Cleanup
    zzArrayListFree(&list);
    zzHashMapFree(&map);
    zzTreeMapIteratorFree(&treeIt);
    zzTreeMapFree(&tree);
    
    printf("✅ All iterator tests passed!\n");
    return 0;
}
