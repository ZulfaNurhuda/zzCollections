# zzCollections - Quick Reference

## Build Commands
```bash
make        # Build the demo executable
make demo   # Build and run the complete demo
make clean  # Clean build artifacts
make help   # Show available targets
```

## Core Types
```c
#include "result.h"
#include "types.h"

// Result handling
zzOpResult result = someFunction();
if (ZZ_IS_OK(result)) {
    // Success
} else if (ZZ_IS_ERR(result)) {
    printf("Error: %s\n", result.error);
}

// Common function types
typedef void (*zzFreeFn)(void *data);
typedef int (*zzCompareFn)(const void *a, const void *b);
typedef uint32_t (*zzHashFn)(const void *key);
typedef bool (*zzEqualsFn)(const void *a, const void *b);
```

## Universal Iterator Pattern
```c
// All collections follow this pattern:
zzCollectionIterator it;
zzOpResult initResult = zzCollectionIteratorInit(&it, &collection);
if (ZZ_IS_OK(initResult)) {
    ElementType element;
    while (zzCollectionIteratorNext(&it, &element)) {
        // Process element
    }
}
```

## Collections Quick Reference

| Collection | Best For | Add | Get | Remove | Sorted | Ordered |
|------------|----------|-----|-----|--------|--------|---------|
| ArrayList | Random access | O(1)* | O(1) | O(n) | ❌ | ❌ |
| ArrayDeque | Queue/Stack | O(1)* | O(1) | O(1) | ❌ | ❌ |
| LinkedList | Insert/Delete | O(1) | O(n) | O(1) | ❌ | ❌ |
| HashMap | Key-value lookup | O(1)** | O(1)** | O(1)** | ❌ | ❌ |
| HashSet | Membership test | O(1)** | O(1)** | O(1)** | ❌ | ❌ |
| LinkedHashMap | Ordered K-V | O(1)** | O(1)** | O(1)** | ❌ | ✅ |
| LinkedHashSet | Ordered set | O(1)** | O(1)** | O(1)** | ❌ | ✅ |
| TreeMap | Sorted K-V | O(log n) | O(log n) | O(log n) | ✅ | ✅ |
| TreeSet | Sorted set | O(log n) | O(log n) | O(log n) | ✅ | ✅ |
| PriorityQueue | Min/Max heap | O(log n) | O(1) | O(log n) | ❌ | ❌ |
| CircularBuffer | Ring buffer | O(1) | O(1) | O(1) | ❌ | ❌ |

*Amortized, **Average case

## Common Utility Functions
```c
#include "utils.h"

// Hash functions
uint32_t zzIntHash(const void *key);
uint32_t zzStringHash(const void *key);

// Comparison functions  
int zzIntCompare(const void *a, const void *b);
int zzStringCompare(const void *a, const void *b);

// Equality functions
bool zzIntEquals(const void *a, const void *b);
bool zzStringEquals(const void *a, const void *b);
```
