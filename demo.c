#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arrayList.h"
#include "arrayDeque.h"
#include "linkedList.h"
#include "hashMap.h"
#include "hashSet.h"
#include "linkedHashMap.h"
#include "linkedHashSet.h"
#include "treeMap.h"
#include "treeSet.h"
#include "arrayStack.h"
#include "arrayQueue.h"
#include "linkedStack.h"
#include "linkedQueue.h"
#include "priorityQueue.h"
#include "circularBuffer.h"

int intCmp(const void *a, const void *b) {
    return *(int*)a - *(int*)b;
}

int main() {
    printf("╔════════════════════════════════════════════╗\n");
    printf("║  COMPLETE COLLECTIONS LIBRARY v4           ║\n");
    printf("║  15 Data Structures + Wrappers             ║\n");
    printf("╚════════════════════════════════════════════╝\n\n");
    
    printf("═══════════════════════════════════════════\n");
    printf(" 1. ARRAYLIST (Dynamic Array)\n");
    printf("═══════════════════════════════════════════\n");
    {
        zzArrayList al;
        zzArrayListInit(&al, sizeof(int), 4, NULL);
        
        int vals[] = {10, 20, 30, 40, 50};
        for (int i = 0; i < 5; i++) zzArrayListAdd(&al, &vals[i]);
        
        printf("Added: ");
        for (size_t i = 0; i < al.size; i++) {
            int v;
            zzArrayListGet(&al, i, &v);
            printf("%d ", v);
        }
        printf("\nSize: %zu, Capacity: %zu\n", al.size, al.capacity);
        
        zzArrayListInsert(&al, 2, &(int){99});
        printf("After insert 99 at idx 2: ");
        for (size_t i = 0; i < al.size; i++) {
            int v;
            zzArrayListGet(&al, i, &v);
            printf("%d ", v);
        }
        printf("\n");
        
        zzArrayListFree(&al);
    }
    
    printf("\n═══════════════════════════════════════════\n");
    printf(" 2. ARRAYDEQUE (Circular Buffer Deque)\n");
    printf("═══════════════════════════════════════════\n");
    {
        zzArrayDeque ad;
        zzArrayDequeInit(&ad, sizeof(int), 4, NULL);
        
        zzArrayDequePushBack(&ad, &(int){1});
        zzArrayDequePushBack(&ad, &(int){2});
        zzArrayDequePushFront(&ad, &(int){0});
        
        printf("Deque: ");
        for (size_t i = 0; i < ad.size; i++) {
            int v;
            zzArrayDequeGet(&ad, i, &v);
            printf("%d ", v);
        }
        printf("\n");
        
        int front, back;
        zzArrayDequePeekFront(&ad, &front);
        zzArrayDequePeekBack(&ad, &back);
        printf("Front: %d, Back: %d\n", front, back);
        
        zzArrayDequePopFront(&ad, &front);
        printf("Popped front: %d\n", front);
        
        zzArrayDequeFree(&ad);
    }
    
    printf("\n═══════════════════════════════════════════\n");
    printf(" 3. LINKEDLIST (Doubly Linked)\n");
    printf("═══════════════════════════════════════════\n");
    {
        zzLinkedList ll;
        zzLinkedListInit(&ll, sizeof(int), NULL);
        
        zzLinkedListPushBack(&ll, &(int){1});
        zzLinkedListPushBack(&ll, &(int){2});
        zzLinkedListPushBack(&ll, &(int){3});
        zzLinkedListPushFront(&ll, &(int){0});
        
        printf("zzLinkedList: ");
        for (size_t i = 0; i < ll.size; i++) {
            int v;
            zzLinkedListGet(&ll, i, &v);
            printf("%d ", v);
        }
        printf("\n");
        
        zzLinkedListInsert(&ll, 2, &(int){99});
        printf("After insert 99 at idx 2: ");
        for (size_t i = 0; i < ll.size; i++) {
            int v;
            zzLinkedListGet(&ll, i, &v);
            printf("%d ", v);
        }
        printf("\n");
        
        int popped;
        zzLinkedListPopFront(&ll, &popped);
        printf("Popped front: %d\n", popped);
        zzLinkedListPopBack(&ll, &popped);
        printf("Popped back: %d\n", popped);
        
        zzLinkedListFree(&ll);
    }
    
    printf("\n═══════════════════════════════════════════\n");
    printf(" 4. HASHMAP (Key-Value Store)\n");
    printf("═══════════════════════════════════════════\n");
    {
        zzHashMap hm;
        zzHashMapInit(&hm, sizeof(int), sizeof(int), 8, zzIntHash, zzIntEquals, NULL, NULL);
        
        zzHashMapPut(&hm, &(int){1}, &(int){100});
        zzHashMapPut(&hm, &(int){2}, &(int){200});
        zzHashMapPut(&hm, &(int){3}, &(int){300});
        
        int value;
        if (zzHashMapGet(&hm, &(int){2}, &value)) {
            printf("Key 2 -> Value %d\n", value);
        }
        
        printf("Contains key 3? %s\n", zzHashMapContains(&hm, &(int){3}) ? "Yes" : "No");
        printf("Contains key 99? %s\n", zzHashMapContains(&hm, &(int){99}) ? "Yes" : "No");
        
        zzHashMapPut(&hm, &(int){2}, &(int){999});
        zzHashMapGet(&hm, &(int){2}, &value);
        printf("Updated key 2 -> Value %d\n", value);
        
        printf("Size: %zu\n", hm.size);
        
        zzHashMapFree(&hm);
    }
    
    printf("\n═══════════════════════════════════════════\n");
    printf(" 5. HASHSET (Unique Keys Only)\n");
    printf("═══════════════════════════════════════════\n");
    {
        zzHashSet hs;
        setInit(&hs, sizeof(int), 8, zzIntHash, zzIntEquals, NULL);
        
        int nums[] = {5, 3, 8, 3, 1, 5, 9};
        for (int i = 0; i < 7; i++) {
            if (setInsert(&hs, &nums[i])) {
                printf("Added: %d\n", nums[i]);
            } else {
                printf("Duplicate: %d\n", nums[i]);
            }
        }
        
        printf("Size: %zu (unique elements)\n", hs.size);
        printf("Contains 3? %s\n", setContains(&hs, &(int){3}) ? "Yes" : "No");
        printf("Contains 99? %s\n", setContains(&hs, &(int){99}) ? "Yes" : "No");
        
        setFree(&hs);
    }
    
    printf("\n═══════════════════════════════════════════\n");
    printf(" 6. LINKEDHASHMAP (Insertion Order Map)\n");
    printf("═══════════════════════════════════════════\n");
    {
        zzLinkedHashMap lhm;
        zzLinkedHashMapInit(&lhm, sizeof(int), sizeof(int), 8, zzIntHash, zzIntEquals, NULL, NULL);
        
        zzLinkedHashMapPut(&lhm, &(int){5}, &(int){50});
        zzLinkedHashMapPut(&lhm, &(int){3}, &(int){30});
        zzLinkedHashMapPut(&lhm, &(int){8}, &(int){80});
        zzLinkedHashMapPut(&lhm, &(int){1}, &(int){10});
        
        int k, v;
        printf("Insertion order maintained:\n");
        if (zzLinkedHashMapGetFirst(&lhm, &k, &v)) {
            printf("First: %d -> %d\n", k, v);
        }
        if (zzLinkedHashMapGetLast(&lhm, &k, &v)) {
            printf("Last: %d -> %d\n", k, v);
        }
        
        int val;
        if (zzLinkedHashMapGet(&lhm, &(int){3}, &val)) {
            printf("Key 3 -> Value %d\n", val);
        }
        
        printf("Size: %zu\n", lhm.size);
        
        zzLinkedHashMapFree(&lhm);
    }
    
    printf("\n═══════════════════════════════════════════\n");
    printf(" 7. LINKEDHASHSET (Insertion Order Set)\n");
    printf("═══════════════════════════════════════════\n");
    {
        zzLinkedHashSet lhs;
        zzLinkedHashSetInit(&lhs, sizeof(int), 8, zzIntHash, zzIntEquals, NULL);
        
        int nums[] = {5, 3, 8, 3, 1, 5, 9};
        printf("Insertion order: ");
        for (int i = 0; i < 7; i++) {
            if (zzLinkedHashSetInsert(&lhs, &nums[i])) {
                printf("%d ", nums[i]);
            }
        }
        printf("\n");
        
        int first, last;
        if (zzLinkedHashSetGetFirst(&lhs, &first)) {
            printf("First: %d\n", first);
        }
        if (zzLinkedHashSetGetLast(&lhs, &last)) {
            printf("Last: %d\n", last);
        }
        
        printf("Size: %zu\n", lhs.size);
        
        zzLinkedHashSetFree(&lhs);
    }
    
    printf("\n═══════════════════════════════════════════\n");
    printf(" 8. TREEMAP (Sorted Key-Value, Red-Black)\n");
    printf("═══════════════════════════════════════════\n");
    {
        zzTreeMap tm;
        zzTreeMapInit(&tm, sizeof(int), sizeof(int), intCmp, NULL, NULL);
        
        zzTreeMapPut(&tm, &(int){5}, &(int){50});
        zzTreeMapPut(&tm, &(int){3}, &(int){30});
        zzTreeMapPut(&tm, &(int){8}, &(int){80});
        zzTreeMapPut(&tm, &(int){1}, &(int){10});
        zzTreeMapPut(&tm, &(int){9}, &(int){90});
        
        int k, v;
        if (zzTreeMapGetMin(&tm, &k, &v)) {
            printf("Min: %d -> %d\n", k, v);
        }
        if (zzTreeMapGetMax(&tm, &k, &v)) {
            printf("Max: %d -> %d\n", k, v);
        }
        
        int val;
        if (zzTreeMapGet(&tm, &(int){3}, &val)) {
            printf("Key 3 -> Value %d\n", val);
        }
        
        printf("Contains key 8? %s\n", zzTreeMapContains(&tm, &(int){8}) ? "Yes" : "No");
        printf("Size: %zu\n", tm.size);
        
        zzTreeMapFree(&tm);
    }
    
    printf("\n═══════════════════════════════════════════\n");
    printf(" 9. TREESET (Sorted Unique Keys, Red-Black)\n");
    printf("═══════════════════════════════════════════\n");
    {
        zzTreeSet ts;
        zzTreeSetInit(&ts, sizeof(int), intCmp, NULL);
        
        int nums[] = {5, 3, 8, 3, 1, 5, 9, 2, 7};
        printf("Inserting: ");
        for (int i = 0; i < 9; i++) {
            if (zzTreeSetInsert(&ts, &nums[i])) {
                printf("%d ", nums[i]);
            } else {
                printf("(%d) ", nums[i]);
            }
        }
        printf("\n");
        
        int min, max;
        if (zzTreeSetGetMin(&ts, &min)) {
            printf("Min: %d\n", min);
        }
        if (zzTreeSetGetMax(&ts, &max)) {
            printf("Max: %d\n", max);
        }
        
        printf("Contains 7? %s\n", zzTreeSetContains(&ts, &(int){7}) ? "Yes" : "No");
        printf("Contains 99? %s\n", zzTreeSetContains(&ts, &(int){99}) ? "Yes" : "No");
        printf("Size: %zu\n", ts.size);
        
        zzTreeSetRemove(&ts, &(int){5});
        printf("After removing 5, size: %zu\n", ts.size);
        
        zzTreeSetFree(&ts);
    }
    
    printf("\n═══════════════════════════════════════════\n");
    printf(" 10. ARRAYSTACK (Stack via zzArrayDeque)\n");
    printf("═══════════════════════════════════════════\n");
    {
        zzArrayStack stack;
        zzArrayStackInit(&stack, sizeof(int), 4, NULL);
        
        for (int i = 1; i <= 5; i++) {
            zzArrayStackPush(&stack, &i);
        }
        
        int top;
        if (zzArrayStackPeek(&stack, &top)) {
            printf("Top: %d\n", top);
        }
        
        printf("Popping: ");
        while (!zzArrayStackIsEmpty(&stack)) {
            int val;
            zzArrayStackPop(&stack, &val);
            printf("%d ", val);
        }
        printf("\n");
        
        zzArrayStackFree(&stack);
    }
    
    printf("\n═══════════════════════════════════════════\n");
    printf(" 11. ARRAYQUEUE (Queue via zzArrayDeque)\n");
    printf("═══════════════════════════════════════════\n");
    {
        zzArrayQueue queue;
        zzArrayQueueInit(&queue, sizeof(int), 4, NULL);
        
        for (int i = 1; i <= 5; i++) {
            zzArrayQueueEnqueue(&queue, &i);
        }
        
        int front;
        if (zzArrayQueuePeek(&queue, &front)) {
            printf("Front: %d\n", front);
        }
        
        printf("Dequeuing: ");
        while (!zzArrayQueueIsEmpty(&queue)) {
            int val;
            zzArrayQueueDequeue(&queue, &val);
            printf("%d ", val);
        }
        printf("\n");
        
        zzArrayQueueFree(&queue);
    }
    
    printf("\n═══════════════════════════════════════════\n");
    printf(" 12. LINKEDSTACK (Stack via zzLinkedList)\n");
    printf("═══════════════════════════════════════════\n");
    {
        zzLinkedStack stack;
        zzLinkedStackInit(&stack, sizeof(int), NULL);
        
        for (int i = 1; i <= 5; i++) {
            zzLinkedStackPush(&stack, &i);
        }
        
        int top;
        if (zzLinkedStackPeek(&stack, &top)) {
            printf("Top: %d\n", top);
        }
        
        printf("Popping: ");
        while (!zzLinkedStackIsEmpty(&stack)) {
            int val;
            zzLinkedStackPop(&stack, &val);
            printf("%d ", val);
        }
        printf("\n");
        
        zzLinkedStackFree(&stack);
    }
    
    printf("\n═══════════════════════════════════════════\n");
    printf(" 13. LINKEDQUEUE (Queue via zzLinkedList)\n");
    printf("═══════════════════════════════════════════\n");
    {
        zzLinkedQueue queue;
        zzLinkedQueueInit(&queue, sizeof(int), NULL);
        
        for (int i = 1; i <= 5; i++) {
            zzLinkedQueueEnqueue(&queue, &i);
        }
        
        int front;
        if (zzLinkedQueuePeek(&queue, &front)) {
            printf("Front: %d\n", front);
        }
        
        printf("Dequeuing: ");
        while (!zzLinkedQueueIsEmpty(&queue)) {
            int val;
            zzLinkedQueueDequeue(&queue, &val);
            printf("%d ", val);
        }
        printf("\n");
        
        zzLinkedQueueFree(&queue);
    }
    
    printf("\n═══════════════════════════════════════════\n");
    printf(" 14. PRIORITYQUEUE (Min-Heap)\n");
    printf("═══════════════════════════════════════════\n");
    {
        zzPriorityQueue pq;
        zzPriorityQueueInit(&pq, sizeof(int), 8, intCmp, NULL);
        
        int nums[] = {5, 3, 8, 1, 9, 2, 7};
        printf("Pushing: ");
        for (int i = 0; i < 7; i++) {
            zzPriorityQueuePush(&pq, &nums[i]);
            printf("%d ", nums[i]);
        }
        printf("\n");
        
        int min;
        if (zzPriorityQueuePeek(&pq, &min)) {
            printf("Min (peek): %d\n", min);
        }
        
        printf("Popping (sorted): ");
        while (pq.size > 0) {
            int val;
            zzPriorityQueuePop(&pq, &val);
            printf("%d ", val);
        }
        printf("\n");
        
        zzPriorityQueueFree(&pq);
    }
    
    printf("\n═══════════════════════════════════════════\n");
    printf(" 15. CIRCULARBUFFER (Fixed-Size Ring)\n");
    printf("═══════════════════════════════════════════\n");
    {
        zzCircularBuffer cb;
        zzCircularBufferInit(&cb, sizeof(int), 5, NULL);
        
        printf("Capacity: %zu (overwrites oldest)\n", cb.capacity);
        
        for (int i = 1; i <= 7; i++) {
            zzCircularBufferPush(&cb, &i);
            printf("Pushed %d, size: %zu\n", i, cb.size);
        }
        
        printf("Buffer (last 5): ");
        for (size_t i = 0; i < cb.size; i++) {
            int v;
            zzCircularBufferGet(&cb, i, &v);
            printf("%d ", v);
        }
        printf("\n");
        
        int front, back;
        if (zzCircularBufferPeekFront(&cb, &front) && 
            zzCircularBufferPeekBack(&cb, &back)) {
            printf("Front: %d, Back: %d\n", front, back);
        }
        
        zzCircularBufferFree(&cb);
    }
    
    printf("\n╔════════════════════════════════════════════╗\n");
    printf("║  ✨ ALL 15 COLLECTIONS TESTED! ✨           ║\n");
    printf("╚════════════════════════════════════════════╝\n");
    
    return 0;
}
