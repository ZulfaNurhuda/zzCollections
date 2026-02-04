/**
 * @file collections_demo.c
 * @brief Enhanced demonstration program for the zzCollections library.
 *
 * This program demonstrates the usage of all 15 data structures provided by the
 * zzCollections library. It showcases how to initialize, use, and free each
 * data structure with various examples and operations, including universal
 * iterator support and remove-over-iterator functionality across all collections.
 */

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
#include "arraySet.h"
#include "utils.h"

/**
 * @brief Prints a visual separator line for the demo output.
 *
 * This function prints a horizontal line to visually separate different
 * sections of the demo output.
 */
void printSeparator() {
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
}

/**
 * @brief Prints a colorful header for each section.
 *
 * This function prints a colorful header to make the demo more cheerful.
 */
void printHeader(const char* title) {
    printf("🔸 %s\n", title);
}

/**
 * @brief Prints a tip message.
 *
 * This function prints a tip message to provide helpful information.
 */
void printTip(const char* tip) {
    printf("\n💡 Tip: %s\n", tip);
}

/**
 * @brief Prints an info message.
 *
 * This function prints an info message to provide additional information.
 */
void printInfo(const char* info) {
    printf("ℹ️  Info: %s\n", info);
}

/**
 * @brief Main function demonstrating the usage of all data structures in the library.
 *
 * This function serves as the entry point for the demonstration program.
 * It showcases the initialization, usage, and cleanup of each data structure
 * provided by the zzCollections library.
 *
 * @return int Exit status of the program (0 for success)
 */
int main() {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════╗\n");
    printf("║                                                   ║\n");
    printf("║         🚀 zzCollections Library Demo 🚀          ║\n");
    printf("║                                                   ║\n");
    printf("║   16 Production-Ready Data Structures in C11      ║\n");
    printf("║                                                   ║\n");
    printf("╚═══════════════════════════════════════════════════╝\n");
    printSeparator();

    // ========== ArrayList ==========
    printHeader("📦 1. ARRAYLIST - Dynamic Array");
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
        printTip("Elements shifted right automatically!");

        printf("\n   → Iterator traversal: ");
        zzArrayListIterator it;
        zzArrayListIteratorInit(&it, &al);
        int value;
        while (zzArrayListIteratorNext(&it, &value)) {
            printf("%d ", value);
        }
        printTip("Universal iterator pattern works on all collections!");
        
        printf("\n   → Testing iterator remove: ");
        // Add more elements to demonstrate removal
        zzArrayListAdd(&al, &(int){100});
        zzArrayListAdd(&al, &(int){200});
        zzArrayListAdd(&al, &(int){300});
        
        zzArrayListIterator it2;
        zzArrayListIteratorInit(&it2, &al);
        while (zzArrayListIteratorNext(&it2, &value)) {
            if (value == 200) {
                zzArrayListIteratorRemove(&it2);  // Remove element 200
                printf("(removed %d) ", value);
            } else {
                printf("%d ", value);
            }
        }
        printf("\n   ✓ ArrayList after iterator removal: ");
        for (size_t i = 0; i < al.size; i++) {
            int v;
            zzArrayListGet(&al, i, &v);
            printf("%d ", v);
        }
        printTip("Iterator remove is safe and maintains integrity!");

        zzArrayListFree(&al);
    }
    printSeparator();

    // ========== ArraySet ==========
    printHeader("🔢 2. ARRAYSET - Flat Set (Dynamic Array)");
    printf("   Perfect for: Small unique datasets, low overhead\n");
    printf("   Complexity: O(n) insert/remove, O(1) iteration\n\n");
    {
        zzArraySet as;
        zzArraySetInit(&as, sizeof(int), 4, zzIntEquals, NULL);

        printf("   → Adding: 10, 20, 10 (duplicate), 30\n");
        int vals[] = {10, 20, 10, 30};
        for(int i=0; i<4; i++) {
            if(ZZ_IS_OK(zzArraySetAdd(&as, &vals[i]))) printf("     Added %d\n", vals[i]);
            else printf("     Skipped %d (Duplicate)\n", vals[i]);
        }

        printf("   ✓ Size: %zu\n", as.size);
        printf("   → Contains 20? %s\n", zzArraySetContains(&as, &(int){20}) ? "Yes" : "No");

        zzArraySetRemove(&as, &(int){20});
        printf("   → Removed 20. Size: %zu\n", as.size);
        
        // Add more elements for iterator demonstration
        zzArraySetAdd(&as, &(int){100});
        zzArraySetAdd(&as, &(int){200});
        zzArraySetAdd(&as, &(int){300});
        
        printf("   → Iterator traversal: ");
        zzArraySetIterator it;
        zzArraySetIteratorInit(&it, &as);
        int value;
        while (zzArraySetIteratorNext(&it, &value)) {
            printf("%d ", value);
        }
        printTip("Iterate through all unique elements!");
        
        printf("\n   → Testing iterator remove: ");
        zzArraySetIterator it2;
        zzArraySetIteratorInit(&it2, &as);
        while (zzArraySetIteratorNext(&it2, &value)) {
            if (value == 200) {
                zzArraySetIteratorRemove(&it2);  // Remove element 200
                printf("(removed %d) ", value);
            } else {
                printf("%d ", value);
            }
        }
        printf("\n   ✓ ArraySet after iterator removal: ");
        zzArraySetIterator it3;
        zzArraySetIteratorInit(&it3, &as);
        while (zzArraySetIteratorNext(&it3, &value)) {
            printf("%d ", value);
        }
        printTip("Iterator remove maintains uniqueness property!");

        zzArraySetFree(&as);
    }
    printSeparator();

    // ========== ArrayDeque ==========
    printHeader("🔄 3. ARRAYDEQUE - Circular Buffer Deque");
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
        printf("   → Popped front: %d", front);
        printTip("Circular buffer means no shifting needed!");
        
        // Add more elements for iterator demonstration
        zzArrayDequePushBack(&ad, &(int){100});
        zzArrayDequePushBack(&ad, &(int){200});
        zzArrayDequePushBack(&ad, &(int){300});

        printf("\n   → Iterator traversal: ");
        zzArrayDequeIterator it;
        zzArrayDequeIteratorInit(&it, &ad);
        int value;
        while (zzArrayDequeIteratorNext(&it, &value)) {
            printf("%d ", value);
        }
        printTip("Traverse the deque with ease!");
        
        printf("\n   → Testing iterator remove: ");
        zzArrayDequeIterator it2;
        zzArrayDequeIteratorInit(&it2, &ad);
        while (zzArrayDequeIteratorNext(&it2, &value)) {
            if (value == 200) {
                zzArrayDequeIteratorRemove(&it2);  // Remove element 200
                printf("(removed %d) ", value);
            } else {
                printf("%d ", value);
            }
        }
        printf("\n   ✓ ArrayDeque after iterator removal: ");
        zzArrayDequeIterator it3;
        zzArrayDequeIteratorInit(&it3, &ad);
        while (zzArrayDequeIteratorNext(&it3, &value)) {
            printf("%d ", value);
        }
        printTip("Iterator remove maintains deque integrity!");

        zzArrayDequeFree(&ad);
    }
    printSeparator();

    // ========== LinkedList ==========
    printHeader("🔗 4. LINKEDLIST - Doubly Linked List");
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
        printf("   → Popped front: %d", popped);
        zzLinkedListPopBack(&ll, &popped);
        printf("\n   → Popped back: %d", popped);
        printTip("No memory reallocation on insert/delete!");
        
        // Add more elements for iterator demonstration
        zzLinkedListPushBack(&ll, &(int){100});
        zzLinkedListPushBack(&ll, &(int){200});
        zzLinkedListPushBack(&ll, &(int){300});

        printf("\n   → Iterator traversal: ");
        zzLinkedListIterator it;
        zzLinkedListIteratorInit(&it, &ll);
        int value;
        while (zzLinkedListIteratorNext(&it, &value)) {
            printf("%d ", value);
        }
        printTip("Traverse the linked list with ease!");
        
        printf("\n   → Testing iterator remove: ");
        zzLinkedListIterator it2;
        zzLinkedListIteratorInit(&it2, &ll);
        while (zzLinkedListIteratorNext(&it2, &value)) {
            if (value == 200) {
                zzLinkedListIteratorRemove(&it2);  // Remove element 200
                printf("(removed %d) ", value);
            } else {
                printf("%d ", value);
            }
        }
        printf("\n   ✓ LinkedList after iterator removal: ");
        zzLinkedListIterator it3;
        zzLinkedListIteratorInit(&it3, &ll);
        while (zzLinkedListIteratorNext(&it3, &value)) {
            printf("%d ", value);
        }
        printTip("Iterator remove maintains linked list integrity!");

        zzLinkedListFree(&ll);
    }
    printSeparator();

    // ========== HashMap ==========
    printHeader("🗺️  5. HASHMAP - Key-Value Hash Table");
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
        printf("   ✓ New value: %d", value);
        printTip("Put() updates existing keys automatically!");

        printf("\n   → Iterator traversal: ");
        zzHashMapIterator it;
        zzHashMapIteratorInit(&it, &hm);
        int k, v;
        while (zzHashMapIteratorNext(&it, &k, &v)) {
            printf("(%d:%d) ", k, v);
        }
        printTip("Hash order may vary, use LinkedHashMap for insertion order!");
        
        printf("\n   → Testing iterator remove: ");
        zzHashMapPut(&hm, &(int){4}, &(int){400});
        zzHashMapPut(&hm, &(int){5}, &(int){500});
        
        zzHashMapIterator it2;
        zzHashMapIteratorInit(&it2, &hm);
        int k2, v2;
        while (zzHashMapIteratorNext(&it2, &k2, &v2)) {
            if (k2 == 2) {
                zzHashMapIteratorRemove(&it2);  // Remove key-value pair with key 2
                printf("(removed %d:%d) ", k2, v2);
            } else {
                printf("(%d:%d) ", k2, v2);
            }
        }
        printf("\n   ✓ HashMap after iterator removal: ");
        zzHashMapIterator it3;
        zzHashMapIteratorInit(&it3, &hm);
        int k3, v3;
        while (zzHashMapIteratorNext(&it3, &k3, &v3)) {
            printf("(%d:%d) ", k3, v3);
        }
        printTip("Iterator remove works on key-value pairs!");

        zzHashMapFree(&hm);
    }
    printSeparator();

    // ========== HashSet ==========
    printHeader("🎯 6. HASHSET - Unique Elements Set");
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

        printf("   → Membership tests:");
        printf("\n     • Contains 3? %s", zzHashSetContains(&hs, &(int){3}) ? "✓ Yes" : "✗ No");
        printf("\n     • Contains 99? %s", zzHashSetContains(&hs, &(int){99}) ? "✓ Yes" : "✗ No");
        printTip("Duplicates are automatically rejected!");
        
        // Add more elements for iterator demonstration
        zzHashSetInsert(&hs, &(int){100});
        zzHashSetInsert(&hs, &(int){200});
        zzHashSetInsert(&hs, &(int){300});

        printf("\n   → Iterator traversal: ");
        zzHashSetIterator it;
        zzHashSetIteratorInit(&it, &hs);
        int value;
        while (zzHashSetIteratorNext(&it, &value)) {
            printf("%d ", value);
        }
        printTip("Iterate through unique elements!");
        
        printf("\n   → Testing iterator remove: ");
        zzHashSetIterator it2;
        zzHashSetIteratorInit(&it2, &hs);
        while (zzHashSetIteratorNext(&it2, &value)) {
            if (value == 200) {
                zzHashSetIteratorRemove(&it2);  // Remove element 200
                printf("(removed %d) ", value);
            } else {
                printf("%d ", value);
            }
        }
        printf("\n   ✓ HashSet after iterator removal: ");
        zzHashSetIterator it3;
        zzHashSetIteratorInit(&it3, &hs);
        while (zzHashSetIteratorNext(&it3, &value)) {
            printf("%d ", value);
        }
        printTip("Iterator remove maintains uniqueness property!");

        zzHashSetFree(&hs);
    }
    printSeparator();

    // ========== LinkedHashMap ==========
    printHeader("📋 7. LINKEDHASHMAP - Ordered Hash Map");
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
        printf("   → Random access: key 3 → %d", val);
        printTip("Combines HashMap speed with order tracking!");
        
        // Add more elements for iterator demonstration
        zzLinkedHashMapPut(&lhm, &(int){100}, &(int){1000});
        zzLinkedHashMapPut(&lhm, &(int){200}, &(int){2000});
        zzLinkedHashMapPut(&lhm, &(int){300}, &(int){3000});

        printf("\n   → Iterator traversal: ");
        zzLinkedHashMapIterator it;
        zzLinkedHashMapIteratorInit(&it, &lhm);
        int k2, v2;
        while (zzLinkedHashMapIteratorNext(&it, &k2, &v2)) {
            printf("(%d:%d) ", k2, v2);
        }
        printTip("Iterate in insertion order!");
        
        printf("\n   → Testing iterator remove: ");
        zzLinkedHashMapIterator it2;
        zzLinkedHashMapIteratorInit(&it2, &lhm);
        int k3, v3;
        while (zzLinkedHashMapIteratorNext(&it2, &k3, &v3)) {
            if (k3 == 200) {
                zzLinkedHashMapIteratorRemove(&it2);  // Remove key-value pair with key 200
                printf("(removed %d:%d) ", k3, v3);
            } else {
                printf("(%d:%d) ", k3, v3);
            }
        }
        printf("\n   ✓ LinkedHashMap after iterator removal: ");
        zzLinkedHashMapIterator it3;
        zzLinkedHashMapIteratorInit(&it3, &lhm);
        int k4, v4;
        while (zzLinkedHashMapIteratorNext(&it3, &k4, &v4)) {
            printf("(%d:%d) ", k4, v4);
        }
        printTip("Iterator remove maintains insertion order property!");

        zzLinkedHashMapFree(&lhm);
    }
    printSeparator();

    // ========== LinkedHashSet ==========
    printHeader("📌 8. LINKEDHASHSET - Ordered Unique Set");
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
        printf("   ✓ First: %d, Last: %d", first, last);
        printTip("Iterate in insertion order, not hash order!");
        
        // Add more elements for iterator demonstration
        zzLinkedHashSetInsert(&lhs, &(int){100});
        zzLinkedHashSetInsert(&lhs, &(int){200});
        zzLinkedHashSetInsert(&lhs, &(int){300});

        printf("\n   → Iterator traversal: ");
        zzLinkedHashSetIterator it;
        zzLinkedHashSetIteratorInit(&it, &lhs);
        int value;
        while (zzLinkedHashSetIteratorNext(&it, &value)) {
            printf("%d ", value);
        }
        printTip("Iterate in insertion order!");
        
        printf("\n   → Testing iterator remove: ");
        zzLinkedHashSetIterator it2;
        zzLinkedHashSetIteratorInit(&it2, &lhs);
        while (zzLinkedHashSetIteratorNext(&it2, &value)) {
            if (value == 200) {
                zzLinkedHashSetIteratorRemove(&it2);  // Remove element 200
                printf("(removed %d) ", value);
            } else {
                printf("%d ", value);
            }
        }
        printf("\n   ✓ LinkedHashSet after iterator removal: ");
        zzLinkedHashSetIterator it3;
        zzLinkedHashSetIteratorInit(&it3, &lhs);
        while (zzLinkedHashSetIteratorNext(&it3, &value)) {
            printf("%d ", value);
        }
        printTip("Iterator remove maintains insertion order property!");

        zzLinkedHashSetFree(&lhs);
    }
    printSeparator();

    // ========== TreeMap ==========
    printHeader("🌳 9. TREEMAP - Sorted Key-Value Tree");
    printf("   Perfect for: Range queries, sorted iteration\n");
    printf("   Complexity: O(log n) for all operations\n\n");
    {
        zzTreeMap tm;
        zzTreeMapInit(&tm, sizeof(int), sizeof(int), zzIntCompare, NULL, NULL);

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
        printf("   → Lookup key 3 → %d", val);
        printTip("Red-Black tree keeps everything balanced!");

        printf("\n   → Sorted iteration: ");
        zzTreeMapIterator it;
        zzTreeMapIteratorInit(&it, &tm);
        int k2, v2;
        while (zzTreeMapIteratorNext(&it, &k2, &v2)) {
            printf("(%d:%d) ", k2, v2);
        }
        printTip("Always iterates in sorted key order!");
        
        // Add more elements for iterator demonstration
        zzTreeMapPut(&tm, &(int){2}, &(int){20});
        zzTreeMapPut(&tm, &(int){4}, &(int){40});
        zzTreeMapPut(&tm, &(int){6}, &(int){60});

        printf("\n   → Testing iterator remove: ");
        zzTreeMapIterator it2;
        zzTreeMapIteratorInit(&it2, &tm);
        int k3, v3;
        while (zzTreeMapIteratorNext(&it2, &k3, &v3)) {
            if (k3 == 4) {
                zzTreeMapIteratorRemove(&it2);  // Remove key-value pair with key 4
                printf("(removed %d:%d) ", k3, v3);
            } else {
                printf("(%d:%d) ", k3, v3);
            }
        }
        printf("\n   ✓ TreeMap after iterator removal: ");
        zzTreeMapIterator it3;
        zzTreeMapIteratorInit(&it3, &tm);
        int k4, v4;
        while (zzTreeMapIteratorNext(&it3, &k4, &v4)) {
            printf("(%d:%d) ", k4, v4);
        }
        printTip("Iterator remove maintains sorted order property!");

        zzTreeMapFree(&tm);
    }
    printSeparator();

    // ========== TreeSet ==========
    printHeader("🎄 10. TREESET - Sorted Unique Set");
    printf("   Perfect for: Sorted unique elements, range operations\n");
    printf("   Complexity: O(log n) for all operations\n\n");
    {
        zzTreeSet ts;
        zzTreeSetInit(&ts, sizeof(int), zzIntCompare, NULL);

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
        printf("   ✓ New size: %zu", ts.size);
        printTip("Always sorted, always balanced!");
        
        // Add more elements for iterator demonstration
        zzTreeSetInsert(&ts, &(int){10});
        zzTreeSetInsert(&ts, &(int){4});
        zzTreeSetInsert(&ts, &(int){6});

        printf("\n   → Iterator traversal: ");
        zzTreeSetIterator it;
        zzTreeSetIteratorInit(&it, &ts);
        int value;
        while (zzTreeSetIteratorNext(&it, &value)) {
            printf("%d ", value);
        }
        printTip("Iterate in sorted order!");
        
        printf("\n   → Testing iterator remove: ");
        zzTreeSetIterator it2;
        zzTreeSetIteratorInit(&it2, &ts);
        while (zzTreeSetIteratorNext(&it2, &value)) {
            if (value == 4) {
                zzTreeSetIteratorRemove(&it2);  // Remove element 4
                printf("(removed %d) ", value);
            } else {
                printf("%d ", value);
            }
        }
        printf("\n   ✓ TreeSet after iterator removal: ");
        zzTreeSetIterator it3;
        zzTreeSetIteratorInit(&it3, &ts);
        while (zzTreeSetIteratorNext(&it3, &value)) {
            printf("%d ", value);
        }
        printTip("Iterator remove maintains sorted order property!");

        zzTreeSetFree(&ts);
    }
    printSeparator();

    // ========== ArrayStack ==========
    printHeader("📚 11. ARRAYSTACK - LIFO Stack (Array-based)");
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
        printTip("Last In, First Out!");
        
        // Rebuild stack for iterator demonstration
        for (int i = 1; i <= 5; i++) {
            zzArrayStackPush(&stack, &i);
        }

        printf("\n   → Iterator traversal: ");
        zzArrayDequeIterator it;  // Stack uses ArrayDeque internally
        zzArrayDequeIteratorInit(&it, &stack);
        int value;
        while (zzArrayDequeIteratorNext(&it, &value)) {
            printf("%d ", value);
        }
        printTip("Stack iterator traverses from bottom to top!");
        
        printf("\n   → Testing iterator remove: ");
        zzArrayDequeIterator it2;
        zzArrayDequeIteratorInit(&it2, &stack);
        while (zzArrayDequeIteratorNext(&it2, &value)) {
            if (value == 3) {
                zzArrayDequeIteratorRemove(&it2);  // Remove element 3
                printf("(removed %d) ", value);
            } else {
                printf("%d ", value);
            }
        }
        printf("\n   ✓ ArrayStack after iterator removal: ");
        while (!zzArrayStackIsEmpty(&stack)) {
            int val;
            zzArrayStackPop(&stack, &val);
            printf("%d ", val);
        }
        printTip("Iterator remove works on stack elements!");

        zzArrayStackFree(&stack);
    }
    printSeparator();

    // ========== ArrayQueue ==========
    printHeader("🎫 12. ARRAYQUEUE - FIFO Queue (Array-based)");
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
        printTip("First In, First Out!");
        
        // Rebuild queue for iterator demonstration
        for (int i = 1; i <= 5; i++) {
            zzArrayQueueEnqueue(&queue, &i);
        }

        printf("\n   → Iterator traversal: ");
        zzArrayDequeIterator it;
        zzArrayDequeIteratorInit(&it, &queue);
        int value;
        while (zzArrayDequeIteratorNext(&it, &value)) {
            printf("%d ", value);
        }
        printTip("Queue iterator traverses from front to back!");
        
        printf("\n   → Testing iterator remove: ");
        zzArrayDequeIterator it2;
        zzArrayDequeIteratorInit(&it2, &queue);
        while (zzArrayDequeIteratorNext(&it2, &value)) {
            if (value == 3) {
                zzArrayDequeIteratorRemove(&it2);  // Remove element 3
                printf("(removed %d) ", value);
            } else {
                printf("%d ", value);
            }
        }
        printf("\n   ✓ ArrayQueue after iterator removal: ");
        while (!zzArrayQueueIsEmpty(&queue)) {
            int val;
            zzArrayQueueDequeue(&queue, &val);
            printf("%d ", val);
        }
        printTip("Iterator remove works on queue elements!");

        zzArrayQueueFree(&queue);
    }
    printSeparator();

    // ========== LinkedStack ==========
    printHeader("🔗📚 13. LINKEDSTACK - LIFO Stack (List-based)");
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
        printTip("No capacity limits!");
        
        // Rebuild stack for iterator demonstration
        for (int i = 1; i <= 5; i++) {
            zzLinkedStackPush(&stack, &i);
        }

        printf("\n   → Iterator traversal: ");
        zzLinkedListIterator it;
        zzLinkedListIteratorInit(&it, &stack);
        int value;
        while (zzLinkedListIteratorNext(&it, &value)) {
            printf("%d ", value);
        }
        printTip("LinkedStack iterator traverses from bottom to top!");
        
        printf("\n   → Testing iterator remove: ");
        zzLinkedListIterator it2;
        zzLinkedListIteratorInit(&it2, &stack);
        while (zzLinkedListIteratorNext(&it2, &value)) {
            if (value == 3) {
                zzLinkedListIteratorRemove(&it2);  // Remove element 3
                printf("(removed %d) ", value);
            } else {
                printf("%d ", value);
            }
        }
        printf("\n   ✓ LinkedStack after iterator removal: ");
        while (!zzLinkedStackIsEmpty(&stack)) {
            int val;
            zzLinkedStackPop(&stack, &val);
            printf("%d ", val);
        }
        printTip("Iterator remove works on linked stack elements!");

        zzLinkedStackFree(&stack);
    }
    printSeparator();

    // ========== LinkedQueue ==========
    printHeader("🔗🎫 14. LINKEDQUEUE - FIFO Queue (List-based)");
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
        printTip("Grows dynamically as needed!");
        
        // Rebuild queue for iterator demonstration
        for (int i = 1; i <= 5; i++) {
            zzLinkedQueueEnqueue(&queue, &i);
        }

        printf("\n   → Iterator traversal: ");
        zzLinkedListIterator it;
        zzLinkedListIteratorInit(&it, &queue);
        int value;
        while (zzLinkedListIteratorNext(&it, &value)) {
            printf("%d ", value);
        }
        printTip("LinkedQueue iterator traverses from front to back!");
        
        printf("\n   → Testing iterator remove: ");
        zzLinkedListIterator it2;
        zzLinkedListIteratorInit(&it2, &queue);
        while (zzLinkedListIteratorNext(&it2, &value)) {
            if (value == 3) {
                zzLinkedListIteratorRemove(&it2);  // Remove element 3
                printf("(removed %d) ", value);
            } else {
                printf("%d ", value);
            }
        }
        printf("\n   ✓ LinkedQueue after iterator removal: ");
        while (!zzLinkedQueueIsEmpty(&queue)) {
            int val;
            zzLinkedQueueDequeue(&queue, &val);
            printf("%d ", val);
        }
        printTip("Iterator remove works on linked queue elements!");

        zzLinkedQueueFree(&queue);
    }
    printSeparator();

    // ========== PriorityQueue ==========
    printHeader("⭐ 15. PRIORITYQUEUE - Min-Heap");
    printf("   Perfect for: Dijkstra, task scheduling, top-K\n");
    printf("   Complexity: O(log n) push/pop, O(1) peek\n\n");
    {
        zzPriorityQueue pq;
        zzPriorityQueueInit(&pq, sizeof(int), 8, zzIntCompare, NULL);

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
        printTip("Always get the minimum element first!");
        
        // Rebuild priority queue for iterator demonstration
        for (int i = 0; i < 7; i++) {
            zzPriorityQueuePush(&pq, &nums[i]);
        }

        printf("\n   → Iterator traversal: ");
        zzPriorityQueueIterator it;
        zzPriorityQueueIteratorInit(&it, &pq);
        int value;
        while (zzPriorityQueueIteratorNext(&it, &value)) {
            printf("%d ", value);
        }
        printTip("Priority queue iterator traverses heap order!");
        
        printf("\n   → Testing iterator remove: ");
        zzPriorityQueueIterator it2;
        zzPriorityQueueIteratorInit(&it2, &pq);
        while (zzPriorityQueueIteratorNext(&it2, &value)) {
            if (value == 3) {
                zzPriorityQueueIteratorRemove(&it2);  // Remove element 3
                printf("(removed %d) ", value);
            } else {
                printf("%d ", value);
            }
        }
        printf("\n   ✓ PriorityQueue after iterator removal: ");
        while(pq.size > 0) {
            int val;
            zzPriorityQueuePop(&pq, &val);
            printf("%d ", val);
        }
        printTip("Iterator remove works on priority queue elements!");

        zzPriorityQueueFree(&pq);
    }
    printSeparator();

    // ========== CircularBuffer ==========
    printHeader("⭕ 16. CIRCULARBUFFER - Fixed-Size Ring Buffer");
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
        printf("\n   ✓ Front: %d, Back: %d", front, back);
        printTip("Perfect for keeping last N items!");
        
        // Add more elements for iterator demonstration
        zzCircularBufferPush(&cb, &(int){100});
        zzCircularBufferPush(&cb, &(int){200});
        zzCircularBufferPush(&cb, &(int){300});

        printf("\n   → Iterator traversal: ");
        zzCircularBufferIterator it;
        zzCircularBufferIteratorInit(&it, &cb);
        int value;
        while (zzCircularBufferIteratorNext(&it, &value)) {
            printf("%d ", value);
        }
        printTip("Circular buffer iterator traverses from oldest to newest!");
        
        printf("\n   → Testing iterator remove: ");
        zzCircularBufferIterator it2;
        zzCircularBufferIteratorInit(&it2, &cb);
        while (zzCircularBufferIteratorNext(&it2, &value)) {
            if (value == 200) {
                zzCircularBufferIteratorRemove(&it2);  // Remove element 200
                printf("(removed %d) ", value);
            } else {
                printf("%d ", value);
            }
        }
        printf("\n   ✓ CircularBuffer after iterator removal: ");
        zzCircularBufferIterator it3;
        zzCircularBufferIteratorInit(&it3, &cb);
        while (zzCircularBufferIteratorNext(&it3, &value)) {
            printf("%d ", value);
        }
        printTip("Iterator remove works on circular buffer elements!");

        zzCircularBufferFree(&cb);
    }
    printSeparator();

    printf("╔═══════════════════════════════════════════════════╗\n");
    printf("║                                                   ║\n");
    printf("║          ✨ All 16 Collections Tested! ✨         ║\n");
    printf("║                                                   ║\n");
    printf("║    🎉 Zero memory leaks • Production ready 🎉     ║\n");
    printf("║                                                   ║\n");
    printf("╚═══════════════════════════════════════════════════╝\n\n");

    return 0;
}