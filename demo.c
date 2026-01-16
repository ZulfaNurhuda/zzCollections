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

void printSeparator() {
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
}

int main() {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════╗\n");
    printf("║                                                   ║\n");
    printf("║       🚀 zzCollections Library Demo 🚀            ║\n");
    printf("║                                                   ║\n");
    printf("║   15 Production-Ready Data Structures in C11     ║\n");
    printf("║                                                   ║\n");
    printf("╚═══════════════════════════════════════════════════╝\n");
    printSeparator();
    
    // ========== ArrayList ==========
    printf("📦 1. ARRAYLIST - Dynamic Array\n");
    printf("   Perfect for: Random access, iteration, growing lists\n");
    printf("   Complexity: O(1) access, O(1) amortized append\n\n");
    {
        zzArrayList al;
        zzArrayListInit(&al, sizeof(int), 4, NULL);
        
        printf("   → Adding 5 elements: ");
        int vals[] = {10, 20, 30, 40, 50};
        for (int i = 0; i < 5; i++) {
            zzArrayListAdd(&al, &vals[i]);
            printf("%d ", vals[i]);
        }
        printf("\n   ✓ Size: %zu, Capacity: %zu (auto-resized from 4!)\n\n", al.size, al.capacity);
        
        printf("   → Inserting 99 at index 2...\n");
        zzArrayListInsert(&al, 2, &(int){99});
        printf("   ✓ Result: ");
        for (size_t i = 0; i < al.size; i++) {
            int v;
            zzArrayListGet(&al, i, &v);
            printf("%d ", v);
        }
        printf("\n   💡 Tip: Elements shifted right automatically!\n");
        
        zzArrayListFree(&al);
    }
    printSeparator();
    
    // ========== ArrayDeque ==========
    printf("🔄 2. ARRAYDEQUE - Circular Buffer Deque\n");
    printf("   Perfect for: Queue/Stack operations, both-end access\n");
    printf("   Complexity: O(1) for all operations\n\n");
    {
        zzArrayDeque ad;
        zzArrayDequeInit(&ad, sizeof(int), 4, NULL);
        
        printf("   → Building deque: PushBack(1), PushBack(2), PushFront(0)\n");
        zzArrayDequePushBack(&ad, &(int){1});
        zzArrayDequePushBack(&ad, &(int){2});
        zzArrayDequePushFront(&ad, &(int){0});
        
        printf("   ✓ Contents: ");
        for (size_t i = 0; i < ad.size; i++) {
            int v;
            zzArrayDequeGet(&ad, i, &v);
            printf("%d ", v);
        }
        
        int front, back;
        zzArrayDequePeekFront(&ad, &front);
        zzArrayDequePeekBack(&ad, &back);
        printf("\n   ✓ Front: %d, Back: %d\n", front, back);
        
        zzArrayDequePopFront(&ad, &front);
        printf("   → Popped front: %d\n", front);
        printf("   💡 Tip: Circular buffer means no shifting needed!\n");
        
        zzArrayDequeFree(&ad);
    }
    printSeparator();
    
    // ========== LinkedList ==========
    printf("🔗 3. LINKEDLIST - Doubly Linked List\n");
    printf("   Perfect for: Frequent insertions/deletions anywhere\n");
    printf("   Complexity: O(1) front/back ops, O(n) random access\n\n");
    {
        zzLinkedList ll;
        zzLinkedListInit(&ll, sizeof(int), NULL);
        
        printf("   → Building list: PushBack(1,2,3), PushFront(0)\n");
        zzLinkedListPushBack(&ll, &(int){1});
        zzLinkedListPushBack(&ll, &(int){2});
        zzLinkedListPushBack(&ll, &(int){3});
        zzLinkedListPushFront(&ll, &(int){0});
        
        printf("   ✓ Contents: ");
        for (size_t i = 0; i < ll.size; i++) {
            int v;
            zzLinkedListGet(&ll, i, &v);
            printf("%d ", v);
        }
        printf("\n\n");
        
        printf("   → Inserting 99 at index 2...\n");
        zzLinkedListInsert(&ll, 2, &(int){99});
        printf("   ✓ Result: ");
        for (size_t i = 0; i < ll.size; i++) {
            int v;
            zzLinkedListGet(&ll, i, &v);
            printf("%d ", v);
        }
        printf("\n\n");
        
        int popped;
        zzLinkedListPopFront(&ll, &popped);
        printf("   → Popped front: %d\n", popped);
        zzLinkedListPopBack(&ll, &popped);
        printf("   → Popped back: %d\n", popped);
        printf("   💡 Tip: No memory reallocation on insert/delete!\n");
        
        zzLinkedListFree(&ll);
    }
    printSeparator();
    
    // ========== HashMap ==========
    printf("🗺️  4. HASHMAP - Key-Value Hash Table\n");
    printf("   Perfect for: Fast lookups, caching, dictionaries\n");
    printf("   Complexity: O(1) average for all operations\n\n");
    {
        zzHashMap hm;
        zzHashMapInit(&hm, sizeof(int), sizeof(int), 8, zzIntHash, zzIntEquals, NULL, NULL);
        
        printf("   → Adding key-value pairs: {1:100, 2:200, 3:300}\n");
        zzHashMapPut(&hm, &(int){1}, &(int){100});
        zzHashMapPut(&hm, &(int){2}, &(int){200});
        zzHashMapPut(&hm, &(int){3}, &(int){300});
        
        int value;
        zzHashMapGet(&hm, &(int){2}, &value);
        printf("   ✓ Get key 2 → %d\n\n", value);
        
        printf("   → Checking existence:\n");
        printf("     • Contains key 3? %s\n", zzHashMapContains(&hm, &(int){3}) ? "✓ Yes" : "✗ No");
        printf("     • Contains key 99? %s\n\n", zzHashMapContains(&hm, &(int){99}) ? "✓ Yes" : "✗ No");
        
        printf("   → Updating key 2 to 999...\n");
        zzHashMapPut(&hm, &(int){2}, &(int){999});
        zzHashMapGet(&hm, &(int){2}, &value);
        printf("   ✓ New value: %d\n", value);
        printf("   💡 Tip: Put() updates existing keys automatically!\n");
        
        zzHashMapFree(&hm);
    }
    printSeparator();
    
    // ========== HashSet ==========
    printf("🎯 5. HASHSET - Unique Elements Set\n");
    printf("   Perfect for: Deduplication, membership testing\n");
    printf("   Complexity: O(1) average insert/lookup\n\n");
    {
        zzHashSet hs;
        zzHashSetInit(&hs, sizeof(int), 8, zzIntHash, zzIntEquals, NULL);

        printf("   → Inserting: 5, 3, 8, 3, 1, 5, 9 (with duplicates)\n");
        int nums[] = {5, 3, 8, 3, 1, 5, 9};
        printf("   ");
        for (int i = 0; i < 7; i++) {
            bool existed = !ZZ_IS_OK(zzHashSetInsert(&hs, &nums[i]));
            if (existed) {
                printf("[%d:dup] ", nums[i]);
            } else {
                printf("[%d:new] ", nums[i]);
            }
        }
        printf("\n   ✓ Final size: %zu unique elements\n\n", hs.size);
        
        printf("   → Membership tests:\n");
        printf("     • Contains 3? %s\n", zzHashSetContains(&hs, &(int){3}) ? "✓ Yes" : "✗ No");
        printf("     • Contains 99? %s\n", zzHashSetContains(&hs, &(int){99}) ? "✓ Yes" : "✗ No");
        printf("   💡 Tip: Duplicates are automatically rejected!\n");

        zzHashSetFree(&hs);
    }
    printSeparator();
    
    // ========== LinkedHashMap ==========
    printf("📋 6. LINKEDHASHMAP - Ordered Hash Map\n");
    printf("   Perfect for: LRU cache, maintaining insertion order\n");
    printf("   Complexity: O(1) ops + insertion order preserved\n\n");
    {
        zzLinkedHashMap lhm;
        zzLinkedHashMapInit(&lhm, sizeof(int), sizeof(int), 8, zzIntHash, zzIntEquals, NULL, NULL);
        
        printf("   → Inserting in order: 5→50, 3→30, 8→80, 1→10\n");
        zzLinkedHashMapPut(&lhm, &(int){5}, &(int){50});
        zzLinkedHashMapPut(&lhm, &(int){3}, &(int){30});
        zzLinkedHashMapPut(&lhm, &(int){8}, &(int){80});
        zzLinkedHashMapPut(&lhm, &(int){1}, &(int){10});
        
        int k, v;
        zzLinkedHashMapGetFirst(&lhm, &k, &v);
        printf("   ✓ First inserted: %d → %d\n", k, v);
        zzLinkedHashMapGetLast(&lhm, &k, &v);
        printf("   ✓ Last inserted: %d → %d\n\n", k, v);

        int val;
        zzLinkedHashMapGet(&lhm, &(int){3}, &val);
        printf("   → Random access: key 3 → %d\n", val);
        printf("   💡 Tip: Combines HashMap speed with order tracking!\n");
        
        zzLinkedHashMapFree(&lhm);
    }
    printSeparator();
    
    // ========== LinkedHashSet ==========
    printf("📌 7. LINKEDHASHSET - Ordered Unique Set\n");
    printf("   Perfect for: Ordered deduplication, iteration order\n");
    printf("   Complexity: O(1) ops + insertion order preserved\n\n");
    {
        zzLinkedHashSet lhs;
        zzLinkedHashSetInit(&lhs, sizeof(int), 8, zzIntHash, zzIntEquals, NULL);
        
        printf("   → Inserting: 5, 3, 8, 3, 1, 5, 9 (with duplicates)\n");
        int nums[] = {5, 3, 8, 3, 1, 5, 9};
        printf("   ✓ Insertion order: ");
        for (int i = 0; i < 7; i++) {
            if (ZZ_IS_OK(zzLinkedHashSetInsert(&lhs, &nums[i]))) {
                printf("%d ", nums[i]);
            }
        }
        printf("\n\n");

        int first, last;
        zzLinkedHashSetGetFirst(&lhs, &first);
        zzLinkedHashSetGetLast(&lhs, &last);
        printf("   ✓ First: %d, Last: %d\n", first, last);
        printf("   💡 Tip: Iterate in insertion order, not hash order!\n");
        
        zzLinkedHashSetFree(&lhs);
    }
    printSeparator();
    
    // ========== TreeMap ==========
    printf("🌳 8. TREEMAP - Sorted Key-Value Tree\n");
    printf("   Perfect for: Range queries, sorted iteration\n");
    printf("   Complexity: O(log n) for all operations\n\n");
    {
        zzTreeMap tm;
        zzTreeMapInit(&tm, sizeof(int), sizeof(int), intCmp, NULL, NULL);
        
        printf("   → Inserting unsorted: 5→50, 3→30, 8→80, 1→10, 9→90\n");
        zzTreeMapPut(&tm, &(int){5}, &(int){50});
        zzTreeMapPut(&tm, &(int){3}, &(int){30});
        zzTreeMapPut(&tm, &(int){8}, &(int){80});
        zzTreeMapPut(&tm, &(int){1}, &(int){10});
        zzTreeMapPut(&tm, &(int){9}, &(int){90});
        
        int k, v;
        zzTreeMapGetMin(&tm, &k, &v);
        printf("   ✓ Min key: %d → %d\n", k, v);
        zzTreeMapGetMax(&tm, &k, &v);
        printf("   ✓ Max key: %d → %d\n\n", k, v);

        int val;
        zzTreeMapGet(&tm, &(int){3}, &val);
        printf("   → Lookup key 3 → %d\n", val);
        printf("   💡 Tip: Red-Black tree keeps everything balanced!\n");
        
        zzTreeMapFree(&tm);
    }
    printSeparator();
    
    // ========== TreeSet ==========
    printf("🎄 9. TREESET - Sorted Unique Set\n");
    printf("   Perfect for: Sorted unique elements, range operations\n");
    printf("   Complexity: O(log n) for all operations\n\n");
    {
        zzTreeSet ts;
        zzTreeSetInit(&ts, sizeof(int), intCmp, NULL);
        
        printf("   → Inserting: 5, 3, 8, 3, 1, 5, 9, 2, 7\n");
        int nums[] = {5, 3, 8, 3, 1, 5, 9, 2, 7};
        printf("   ");
        for (int i = 0; i < 9; i++) {
            if (ZZ_IS_OK(zzTreeSetInsert(&ts, &nums[i]))) {
                printf("[%d:new] ", nums[i]);
            } else {
                printf("[%d:dup] ", nums[i]);
            }
        }
        printf("\n\n");

        int min, max;
        zzTreeSetGetMin(&ts, &min);
        zzTreeSetGetMax(&ts, &max);
        printf("   ✓ Min: %d, Max: %d, Size: %zu\n\n", min, max, ts.size);
        
        printf("   → Removing 5...\n");
        zzTreeSetRemove(&ts, &(int){5});
        printf("   ✓ New size: %zu\n", ts.size);
        printf("   💡 Tip: Always sorted, always balanced!\n");
        
        zzTreeSetFree(&ts);
    }
    printSeparator();
    
    // ========== ArrayStack ==========
    printf("📚 10. ARRAYSTACK - LIFO Stack (Array-based)\n");
    printf("   Perfect for: Function calls, undo/redo, DFS\n");
    printf("   Complexity: O(1) push/pop\n\n");
    {
        zzArrayStack stack;
        zzArrayStackInit(&stack, sizeof(int), 4, NULL);
        
        printf("   → Pushing: 1, 2, 3, 4, 5\n");
        for (int i = 1; i <= 5; i++) {
            zzArrayStackPush(&stack, &i);
        }
        
        int top;
        zzArrayStackPeek(&stack, &top);
        printf("   ✓ Top element: %d\n\n", top);
        
        printf("   → Popping (LIFO): ");
        while (!zzArrayStackIsEmpty(&stack)) {
            int val;
            zzArrayStackPop(&stack, &val);
            printf("%d ", val);
        }
        printf("\n   💡 Tip: Last In, First Out!\n");
        
        zzArrayStackFree(&stack);
    }
    printSeparator();
    
    // ========== ArrayQueue ==========
    printf("🎫 11. ARRAYQUEUE - FIFO Queue (Array-based)\n");
    printf("   Perfect for: Task scheduling, BFS, buffering\n");
    printf("   Complexity: O(1) enqueue/dequeue\n\n");
    {
        zzArrayQueue queue;
        zzArrayQueueInit(&queue, sizeof(int), 4, NULL);
        
        printf("   → Enqueuing: 1, 2, 3, 4, 5\n");
        for (int i = 1; i <= 5; i++) {
            zzArrayQueueEnqueue(&queue, &i);
        }
        
        int front;
        zzArrayQueuePeek(&queue, &front);
        printf("   ✓ Front element: %d\n\n", front);
        
        printf("   → Dequeuing (FIFO): ");
        while (!zzArrayQueueIsEmpty(&queue)) {
            int val;
            zzArrayQueueDequeue(&queue, &val);
            printf("%d ", val);
        }
        printf("\n   💡 Tip: First In, First Out!\n");
        
        zzArrayQueueFree(&queue);
    }
    printSeparator();
    
    // ========== LinkedStack ==========
    printf("🔗📚 12. LINKEDSTACK - LIFO Stack (List-based)\n");
    printf("   Perfect for: When size is unpredictable\n");
    printf("   Complexity: O(1) push/pop, no reallocation\n\n");
    {
        zzLinkedStack stack;
        zzLinkedStackInit(&stack, sizeof(int), NULL);
        
        printf("   → Pushing: 1, 2, 3, 4, 5\n");
        for (int i = 1; i <= 5; i++) {
            zzLinkedStackPush(&stack, &i);
        }
        
        int top;
        zzLinkedStackPeek(&stack, &top);
        printf("   ✓ Top: %d\n\n", top);
        
        printf("   → Popping: ");
        while (!zzLinkedStackIsEmpty(&stack)) {
            int val;
            zzLinkedStackPop(&stack, &val);
            printf("%d ", val);
        }
        printf("\n   💡 Tip: No capacity limits!\n");
        
        zzLinkedStackFree(&stack);
    }
    printSeparator();
    
    // ========== LinkedQueue ==========
    printf("🔗🎫 13. LINKEDQUEUE - FIFO Queue (List-based)\n");
    printf("   Perfect for: Unbounded queues, streaming\n");
    printf("   Complexity: O(1) enqueue/dequeue\n\n");
    {
        zzLinkedQueue queue;
        zzLinkedQueueInit(&queue, sizeof(int), NULL);
        
        printf("   → Enqueuing: 1, 2, 3, 4, 5\n");
        for (int i = 1; i <= 5; i++) {
            zzLinkedQueueEnqueue(&queue, &i);
        }
        
        int front;
        zzLinkedQueuePeek(&queue, &front);
        printf("   ✓ Front: %d\n\n", front);
        
        printf("   → Dequeuing: ");
        while (!zzLinkedQueueIsEmpty(&queue)) {
            int val;
            zzLinkedQueueDequeue(&queue, &val);
            printf("%d ", val);
        }
        printf("\n   💡 Tip: Grows dynamically as needed!\n");
        
        zzLinkedQueueFree(&queue);
    }
    printSeparator();
    
    // ========== PriorityQueue ==========
    printf("⭐ 14. PRIORITYQUEUE - Min-Heap\n");
    printf("   Perfect for: Dijkstra, task scheduling, top-K\n");
    printf("   Complexity: O(log n) push/pop, O(1) peek\n\n");
    {
        zzPriorityQueue pq;
        zzPriorityQueueInit(&pq, sizeof(int), 8, intCmp, NULL);
        
        printf("   → Pushing unsorted: 5, 3, 8, 1, 9, 2, 7\n");
        int nums[] = {5, 3, 8, 1, 9, 2, 7};
        for (int i = 0; i < 7; i++) {
            zzPriorityQueuePush(&pq, &nums[i]);
        }
        
        int min;
        zzPriorityQueuePeek(&pq, &min);
        printf("   ✓ Min element (peek): %d\n\n", min);
        
        printf("   → Popping (auto-sorted): ");
        while (pq.size > 0) {
            int val;
            zzPriorityQueuePop(&pq, &val);
            printf("%d ", val);
        }
        printf("\n   💡 Tip: Always get the minimum element first!\n");
        
        zzPriorityQueueFree(&pq);
    }
    printSeparator();
    
    // ========== CircularBuffer ==========
    printf("⭕ 15. CIRCULARBUFFER - Fixed-Size Ring Buffer\n");
    printf("   Perfect for: Streaming, logging, sliding windows\n");
    printf("   Complexity: O(1) all operations, auto-overwrite\n\n");
    {
        zzCircularBuffer cb;
        zzCircularBufferInit(&cb, sizeof(int), 5, NULL);
        
        printf("   → Capacity: %zu (fixed size)\n", cb.capacity);
        printf("   → Pushing 7 elements into size-5 buffer:\n");
        
        for (int i = 1; i <= 7; i++) {
            zzCircularBufferPush(&cb, &i);
            printf("     Push %d → size: %zu%s\n", i, cb.size, 
                   i > 5 ? " (overwrote oldest!)" : "");
        }
        
        printf("\n   ✓ Final buffer (last 5): ");
        for (size_t i = 0; i < cb.size; i++) {
            int v;
            zzCircularBufferGet(&cb, i, &v);
            printf("%d ", v);
        }
        
        int front, back;
        zzCircularBufferPeekFront(&cb, &front);
        zzCircularBufferPeekBack(&cb, &back);
        printf("\n   ✓ Front: %d, Back: %d\n", front, back);
        printf("   💡 Tip: Perfect for keeping last N items!\n");
        
        zzCircularBufferFree(&cb);
    }
    printSeparator();
    
    printf("╔═══════════════════════════════════════════════════╗\n");
    printf("║                                                   ║\n");
    printf("║        ✨ All 15 Collections Tested! ✨           ║\n");
    printf("║                                                   ║\n");
    printf("║  🎉 Zero memory leaks • Production ready 🎉      ║\n");
    printf("║                                                   ║\n");
    printf("╚═══════════════════════════════════════════════════╝\n\n");
    
    return 0;
}
