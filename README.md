# **🚀・zzCollections - Production-Ready C11 Collections Library**

### **Ever Wished C Had Data Structures as Powerful as Modern Languages? ✨**

Welcome to **zzCollections**! This comprehensive library brings you **15 production-ready data structures** that make C programming feel modern and expressive. From dynamic arrays to red-black trees, from hash maps to priority queues – we've got everything you need! All with zero hidden allocations, complete error handling, and a beautiful `zz` namespace to keep your code clean and collision-free. Let's make C development _awesome_ again! 🎉

---

### **📋・Table of Contents**

- **✨・<a href="#what-is-zzcollections" style="text-decoration: none;">What is zzCollections?</a>**
- **🎯・<a href="#features" style="text-decoration: none;">Features</a>**
- **📚・<a href="#data-structures" style="text-decoration: none;">Data Structures (15 Total)</a>**
- **🚀・<a href="#quick-start" style="text-decoration: none;">Quick Start</a>**
- **💻・<a href="#usage-examples" style="text-decoration: none;">Usage Examples</a>**
- **📁・<a href="#project-structure" style="text-decoration: none;">Project Structure</a>**
- **🎨・<a href="#naming-conventions" style="text-decoration: none;">Naming Conventions</a>**
- **📊・<a href="#performance" style="text-decoration: none;">Performance Characteristics</a>**
- **⚙️・<a href="#requirements" style="text-decoration: none;">Requirements</a>**
- **📖・<a href="#documentation" style="text-decoration: none;">Documentation</a>**
- **💖・<a href="#contributing" style="text-decoration: none;">Contributing</a>**
- **📜・<a href="#license" style="text-decoration: none;">License</a>**
- **👋・<a href="#about" style="text-decoration: none;">About</a>**

---

### <div id="what-is-zzcollections">**✨・What is zzCollections?**</div>

**zzCollections** is a comprehensive C11 collections library designed to bring modern data structure capabilities to C programming! 🚀 Think of it as your personal toolkit of battle-tested algorithms and structures – from ArrayLists to TreeMaps, from PriorityQueues to CircularBuffers. Every structure follows the **zero-malloc convention** for getters (no hidden allocations!), includes complete error handling, and uses memory-safe patterns. Plus, everything is prefixed with `zz` to ensure zero namespace conflicts. Ready to level up your C projects? Let's go! 💪

---

### <div id="features">**🎯・Features (What Makes Us Special! ✨)**</div>

- **Zero-Malloc Convention** 🎯
  All getter functions return `bool` with output parameters – absolutely no hidden memory allocations! You're always in control.

- **Production-Ready Algorithms** 🏆
  Implements industry-standard algorithms: Red-Black trees for balanced operations, Min-heap for priority queues, Circular buffers with automatic overwrite, and efficient Hash tables with collision handling.

- **Complete Error Handling** 🛡️
  Every function includes null checks, bounds checking, and allocation failure handling. Your code stays robust and safe!

- **Memory Safe** 💾
  RAII-style initialization patterns, cleanup callbacks for custom types, and guaranteed no memory leaks when used correctly.

- **Optimized Operations** ⚡
  Custom memory copy implementation with aligned transfers for maximum performance!

- **Namespace Safety** 🔒
  All public symbols use the `zz` prefix – say goodbye to naming conflicts forever!

---

### <div id="data-structures">**📚・Data Structures (15 Total)**</div>

#### **Linear Collections (3)**
- **zzArrayList** - Dynamic array with O(1) random access and automatic resizing
- **zzArrayDeque** - Circular buffer deque with O(1) operations at both ends
- **zzLinkedList** - Doubly linked list with O(1) front/back insertions and deletions

#### **Hash Collections (2)**
- **zzHashMap** - Hash table for key-value pairs with O(1) average operations
- **zzHashSet** - Hash table for unique keys with O(1) average lookups

#### **Ordered Hash Collections (2)**
- **zzLinkedHashMap** - HashMap with insertion order preservation via linked list
- **zzLinkedHashSet** - HashSet with insertion order preservation

#### **Tree Collections (2)**
- **zzTreeMap** - Red-Black tree with key-value pairs and O(log n) sorted operations
- **zzTreeSet** - Red-Black tree for unique sorted keys with O(log n) operations

#### **Specialized Collections (2)**
- **zzPriorityQueue** - Min-heap priority queue with O(log n) push/pop operations
- **zzCircularBuffer** - Fixed-size ring buffer with automatic overwrite (perfect for streaming data!)

#### **Wrapper Collections (4)**
- **zzArrayStack** - LIFO stack wrapper around ArrayDeque
- **zzArrayQueue** - FIFO queue wrapper around ArrayDeque
- **zzLinkedStack** - LIFO stack wrapper around LinkedList
- **zzLinkedQueue** - FIFO queue wrapper around LinkedList

---

### <div id="quick-start">**🚀・Quick Start (Let's Build Something! 🎉)**</div>

#### **Building the Demo**

Ready to see zzCollections in action? Building is super easy!

```bash
make        # Build the demo executable
make demo   # Build and run the demo
make clean  # Clean build artifacts
```

The demo showcases **all 15 data structures** with practical examples. Run it and see the magic happen! ✨

---

### <div id="usage-examples">**💻・Usage Examples (Time to Code! 🚀)**</div>

#### **ArrayList - Your Go-To Dynamic Array**

```c
#include "arrayList.h"

// Create and initialize
zzArrayList list;
zzArrayListInit(&list, sizeof(int), 10, NULL);

// Add elements
int value = 42;
zzArrayListAdd(&list, &value);

// Get elements (zero-malloc convention!)
int retrieved;
if (zzArrayListGet(&list, 0, &retrieved)) {
    printf("Got value: %d\n", retrieved);  // Prints: Got value: 42
}

// Cleanup (always remember!)
zzArrayListFree(&list);
```

#### **HashMap - Fast Key-Value Storage**

```c
#include "hashMap.h"

// Initialize with hash and equality functions
zzHashMap map;
zzHashMapInit(&map, sizeof(int), sizeof(char*), 16,
              zzIntHash, zzIntEquals, NULL, NULL);

// Add key-value pairs
int key = 1;
char* value = "Hello, zzCollections!";
zzHashMapPut(&map, &key, &value);

// Retrieve values
char* result;
if (zzHashMapGet(&map, &key, &result)) {
    printf("Found: %s\n", result);  // Prints: Found: Hello, zzCollections!
}

// Cleanup
zzHashMapFree(&map);
```

#### **PriorityQueue - Always Get the Minimum**

```c
#include "priorityQueue.h"

// Initialize with comparator function
zzPriorityQueue pq;
zzPriorityQueueInit(&pq, sizeof(int), 16, intCmp, NULL);

// Add elements in any order
int values[] = {5, 3, 8, 1, 9};
for (int i = 0; i < 5; i++) {
    zzPriorityQueuePush(&pq, &values[i]);
}

// Pop gets the smallest element!
int min;
if (zzPriorityQueuePop(&pq, &min)) {
    printf("Minimum value: %d\n", min);  // Prints: Minimum value: 1
}

// Cleanup
zzPriorityQueueFree(&pq);
```

#### **TreeMap - Sorted Key-Value Pairs**

```c
#include "treeMap.h"

// Initialize with comparator (Red-Black tree under the hood!)
zzTreeMap tree;
zzTreeMapInit(&tree, sizeof(int), sizeof(char*), intCmp, NULL, NULL);

// Add elements - they'll be kept sorted automatically!
int keys[] = {30, 10, 50, 20, 40};
char* names[] = {"Thirty", "Ten", "Fifty", "Twenty", "Forty"};

for (int i = 0; i < 5; i++) {
    zzTreeMapPut(&tree, &keys[i], &names[i]);
}

// Iterate in sorted order
zzTreeMapIterator it;
zzTreeMapIteratorInit(&it, &tree);
int k;
char* v;
while (zzTreeMapIteratorNext(&it, &k, &v)) {
    printf("%d: %s\n", k, v);
    // Prints in sorted order: 10, 20, 30, 40, 50!
}

// Cleanup
zzTreeMapFree(&tree);
```

---

### <div id="project-structure">**📁・Project Structure (How We're Organized! 📂)**</div>

```
zzCollections/
├── headers/              # All header files (.h)
│   ├── core/            # Common utilities, types, and memory operations
│   │   ├── types.h      # Core type definitions
│   │   ├── utils.h      # Utility functions
│   │   └── result.h     # Result/error handling
│   ├── linear/          # ArrayList, ArrayDeque, LinkedList
│   ├── hash/            # HashMap, HashSet
│   ├── orderedhash/     # LinkedHashMap, LinkedHashSet
│   ├── tree/            # TreeMap, TreeSet (Red-Black trees)
│   ├── specialized/     # PriorityQueue, CircularBuffer
│   └── wrapper/         # Stack and Queue wrappers
├── scripts/             # Implementation files (.c)
│   └── [same structure as headers]
├── demo.c               # Complete demo of all 15 structures
├── Makefile             # Build system
└── README.md            # You are here! 👋
```

---

### <div id="naming-conventions">**🎨・Naming Conventions (Keep It Clean! ✨)**</div>

We follow a consistent naming pattern with the `zz` prefix for all public symbols:

- **Files**: camelCase without prefix
  Examples: `arrayList.h`, `hashMap.c`, `priorityQueue.h`

- **Structs**: `zz` + PascalCase
  Examples: `zzArrayList`, `zzHashMap`, `zzTreeSet`

- **Functions**: `zz` + PascalCase + camelCase
  Examples: `zzArrayListInit`, `zzHashMapPut`, `zzTreeSetContains`

- **Typedefs**: `zz` + PascalCase
  Examples: `zzFreeFn`, `zzCompareFn`, `zzHashFn`

This consistent naming ensures **zero namespace conflicts** and makes your code super readable! 📖

---

### <div id="performance">**📊・Performance Characteristics (Speed Matters! ⚡)**</div>

| Collection        | Add      | Get      | Remove   | Memory   | Best Use Case                    |
|-------------------|----------|----------|----------|----------|----------------------------------|
| zzArrayList       | O(1)*    | O(1)     | O(n)     | Compact  | Random access, iteration         |
| zzArrayDeque      | O(1)*    | O(1)     | O(1)     | Compact  | Queue/Stack, both-end operations |
| zzLinkedList      | O(1)     | O(n)     | O(1)     | Higher   | Frequent insertions/deletions    |
| zzHashMap         | O(1)**   | O(1)**   | O(1)**   | Medium   | Fast key-value lookups           |
| zzHashSet         | O(1)**   | O(1)**   | O(1)**   | Lower    | Fast membership testing          |
| zzLinkedHashMap   | O(1)**   | O(1)**   | O(1)**   | Higher   | Ordered key-value pairs          |
| zzLinkedHashSet   | O(1)**   | O(1)**   | O(1)**   | Medium   | Ordered unique elements          |
| zzTreeMap         | O(log n) | O(log n) | O(log n) | Higher   | Sorted key-value pairs           |
| zzTreeSet         | O(log n) | O(log n) | O(log n) | Lower    | Sorted unique elements           |
| zzPriorityQueue   | O(log n) | O(1)     | O(log n) | Compact  | Min/Max heap operations          |
| zzCircularBuffer  | O(1)     | O(1)     | O(1)     | Fixed    | Streaming data, ring buffers     |

**Notes:**
- `*` Amortized complexity due to dynamic resizing
- `**` Average case (worst case O(n) for hash collisions)

---

### <div id="requirements">**⚙️・Requirements (What You Need! 🛠️)**</div>

- **Compiler**: GCC with C11 support (or any modern C11-compatible compiler)
- **Make**: GNU Make
- **OS**: Cross-platform support! ✨
  - Windows (tested with MSYS2/MinGW)
  - Linux (all major distributions)
  - macOS

---

### <div id="documentation">**📖・Documentation (Learn More! 📚)**</div>

Each header file in the `headers/` directory contains comprehensive documentation including:

- **Function descriptions** - What each function does
- **Parameter documentation** - Detailed explanation of each parameter
- **Return value specifications** - What to expect back
- **Usage examples** - Quick code snippets to get you started
- **Complexity guarantees** - Big-O notation for performance

Want to see everything in action? Check out `demo.c` for complete working examples of **all 15 data structures**! It's like a interactive tutorial. 🎓

---

### <div id="contributing">**💖・Contributing (Join the Adventure! 🎉)**</div>

Found a bug? Have an awesome idea for a new feature? Want to improve the documentation? We'd love your contributions! Feel free to:

- Open an issue for bugs or feature requests
- Submit a pull request with improvements
- Improve documentation or add examples
- Share your use cases and success stories!

Every contribution makes zzCollections better for everyone. Let's build something amazing together! 🚀

---

### <div id="license">**📜・License (The Legal Stuff! 🤝)**</div>

This project is open-source and distributed under the **MIT License**. Feel free to use it in your personal or commercial projects, modify it, share it – whatever you need! Check out the `LICENSE` file for all the details. Happy coding! 💻

---

### <div id="about">**👋・About**</div>

**Muhammad Zulfa Fauzan Nurhuda**
Just a passionate developer who loves building robust, production-ready tools! 🚀
Studying at Bandung Institute of Technology (ITB) 🎓

---

**Built with ❤️ using C11 • Zero-malloc convention • Production-ready algorithms**
