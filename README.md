# zzCollections

A comprehensive C11 collections library with 15 production-ready data structures. All names are prefixed with `zz` signature for namespace safety.

## ✨ Features

- **Zero-Malloc Convention**: All getters return `bool` with output parameters - no hidden allocations
- **Production-Ready Algorithms**: Red-Black trees, Min-heap, Circular buffers, Hash tables
- **Complete Error Handling**: Null checks, bounds checking, allocation failure handling
- **Memory Safe**: RAII-style initialization, cleanup callbacks, no memory leaks
- **Optimized Operations**: Custom memory copy with aligned transfers
- **Namespace Safety**: All symbols prefixed with `zz` to prevent naming conflicts

## 📚 Data Structures (15 Total)

### Linear Collections (3)
- **zzArrayList** - Dynamic array with O(1) random access
- **zzArrayDeque** - Circular buffer deque with O(1) operations at both ends
- **zzLinkedList** - Doubly linked list with O(1) front/back operations

### Hash Collections (2)
- **zzHashMap** - Hash table for key-value pairs, O(1) average operations
- **zzHashSet** - Hash table for unique keys, O(1) average operations

### Ordered Hash Collections (2)
- **zzLinkedHashMap** - HashMap with insertion order preservation
- **zzLinkedHashSet** - HashSet with insertion order preservation

### Tree Collections (2)
- **zzTreeMap** - Red-Black tree with key-value pairs, O(log n) sorted operations
- **zzTreeSet** - Red-Black tree for unique sorted keys, O(log n) operations

### Specialized Collections (2)
- **zzPriorityQueue** - Min-heap priority queue, O(log n) push/pop
- **zzCircularBuffer** - Fixed-size ring buffer with automatic overwrite

### Wrapper Collections (4)
- **zzArrayStack** - LIFO stack wrapper around ArrayDeque
- **zzArrayQueue** - FIFO queue wrapper around ArrayDeque
- **zzLinkedStack** - LIFO stack wrapper around LinkedList
- **zzLinkedQueue** - FIFO queue wrapper around LinkedList

## 🚀 Quick Start

### Building

```bash
make        # Build demo executable
make demo   # Build and run demo
make clean  # Clean build artifacts
```

### Basic Usage

```c
#include "arrayList.h"

// Create and initialize
zzArrayList list;
zzArrayListInit(&list, sizeof(int), 10, NULL);

// Add elements
int value = 42;
zzArrayListAdd(&list, &value);

// Get elements
int retrieved;
zzArrayListGet(&list, 0, &retrieved);

// Cleanup
zzArrayListFree(&list);
```

### HashMap Example

```c
#include "hashMap.h"

zzHashMap map;
zzHashMapInit(&map, sizeof(int), sizeof(char*), 16,
              zzIntHash, zzIntEquals, NULL, NULL);

int key = 1;
char* value = "Hello";
zzHashMapPut(&map, &key, &value);

char* result;
zzHashMapGet(&map, &key, &result);

zzHashMapFree(&map);
```

### Priority Queue Example

```c
#include "priorityQueue.h"

zzPriorityQueue pq;
zzPriorityQueueInit(&pq, sizeof(int), 16, intCmp, NULL);

int values[] = {5, 3, 8, 1, 9};
for (int i = 0; i < 5; i++) {
    zzPriorityQueuePush(&pq, &values[i]);
}

int min;
zzPriorityQueuePop(&pq, &min);  // Gets smallest: 1

zzPriorityQueueFree(&pq);
```

## 📁 Project Structure

```
zzCollections/
├── headers/              # Header files (.h)
│   ├── core/            # Common utilities and memory operations
│   ├── linear/          # ArrayList, ArrayDeque, LinkedList
│   ├── hash/            # HashMap, HashSet
│   ├── orderedhash/     # LinkedHashMap, LinkedHashSet
│   ├── tree/            # TreeMap, TreeSet
│   ├── specialized/     # PriorityQueue, CircularBuffer
│   └── wrapper/         # Stack and Queue wrappers
├── scripts/             # Implementation files (.c)
│   └── [same structure as headers]
├── demo.c               # Complete demonstration of all 15 structures
├── Makefile             # Build system
└── README.md            # This file
```

## 🎯 Naming Conventions

All public symbols use the `zz` prefix for namespace safety:

- **Files**: camelCase without prefix (e.g., `arrayList.h`, `hashMap.c`)
- **Structs**: `zz` + PascalCase (e.g., `zzArrayList`, `zzHashMap`)
- **Functions**: `zz` + camelCase (e.g., `zzArrayListInit`, `zzHashMapPut`)
- **Typedefs**: `zz` + PascalCase (e.g., `zzFreeFn`, `zzCompareFn`)

## 📊 Performance Characteristics

| Collection        | Add      | Get      | Remove   | Memory   |
|-------------------|----------|----------|----------|----------|
| zzArrayList       | O(1)*    | O(1)     | O(n)     | Compact  |
| zzArrayDeque      | O(1)*    | O(1)     | O(1)     | Compact  |
| zzLinkedList      | O(1)     | O(n)     | O(1)     | Higher   |
| zzHashMap         | O(1)**   | O(1)**   | O(1)**   | Medium   |
| zzHashSet         | O(1)**   | O(1)**   | O(1)**   | Lower    |
| zzLinkedHashMap   | O(1)**   | O(1)**   | O(1)**   | Higher   |
| zzLinkedHashSet   | O(1)**   | O(1)**   | O(1)**   | Medium   |
| zzTreeMap         | O(log n) | O(log n) | O(log n) | Higher   |
| zzTreeSet         | O(log n) | O(log n) | O(log n) | Lower    |
| zzPriorityQueue   | O(log n) | O(1)     | O(log n) | Compact  |
| zzCircularBuffer  | O(1)     | O(1)     | O(1)     | Fixed    |

\* Amortized due to dynamic resizing
\** Average case (worst case O(n) for hash collisions)

## 🔧 Requirements

- **Compiler**: GCC with C11 support
- **Make**: GNU Make
- **OS**: Cross-platform (tested on Windows/MSYS, Linux, macOS)

## 📖 Documentation

Each header file contains comprehensive documentation with:
- Function descriptions
- Parameter documentation
- Return value specifications
- Usage examples
- Complexity guarantees

See `demo.c` for complete working examples of all 15 data structures.

## 🤝 Contributing

Feel free to submit issues and enhancement requests!

## 📄 License

See [LICENSE](LICENSE) file for details.

---

**Built with C11 • Zero-malloc convention • Production-ready algorithms**
