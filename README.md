# DSContainers - Advanced C Data Structures Library

A comprehensive, production-ready C library implementing essential data structures with a focus on memory safety, performance, and usability. Built with modern C11 standards and extensive testing.

## Features

### Core Data Structures
- **Singly Linked List** - Efficient forward-only list with O(1) insertions
- **Doubly Linked List** - Bidirectional list with O(1) insertions/deletions at both ends  
- **Dynamic String** - Growable string with small string optimization
- **ArrayList** - Dynamic array with O(1) amortized insertion and O(1) random access
- **Stack** - LIFO container with O(1) push/pop operations
- **Queue** - FIFO container with O(1) enqueue/dequeue operations
- **HashMap** - Hash table with O(1) average insert/lookup/delete operations
- **HashSet** - Set of unique elements built on HashMap foundation
- **Generic Iterator** - Unified iteration interface with filter/transform support

### Threading and Concurrency
- **Mutex** - Cross-platform mutex implementation
- **Threads** - Portable threading utilities with C11 threads support

### Key Capabilities
- ✅ Custom memory allocators support
- ✅ Generic design with function pointers
- ✅ Comprehensive error handling
- ✅ Memory leak protection with sanitizers
- ✅ Thread-safety considerations
- ✅ Cross-platform compatibility (Windows, macOS, Linux)
- ✅ Extensive test coverage (52 test suites, 17,000+ lines of tests)

## Quick Start

### Building

```bash
mkdir build && cd build
cmake ..
make
```

### Running Tests

```bash
cd build
ctest --output-on-failure
```

### Basic Usage

```c
#include "SinglyLinkedList.h"
#include "DynamicString.h"
#include "ArrayList.h"
#include "HashMap.h"
#include "Stack.h"
#include "Queue.h"

// Create a default allocator
DSCAlloc* alloc = dsc_alloc_create();

// ========== Linked List Example ==========
SinglyLinkedList *list = sll_create_custom(alloc);
int *value = alloc->malloc_func(sizeof(int));
*value = 42;
sll_insert_back(list, value);

// ========== Dynamic String Example ==========
String str = str_create("Hello, World!");
str_append(&str, " from DSContainers!");
printf("%s\n", str_data(&str));

// ========== ArrayList Example ==========
DSCArrayList *arr = dsc_arraylist_create(alloc);
int *arr_value = alloc->malloc_func(sizeof(int));
*arr_value = 100;
dsc_arraylist_add(arr, arr_value);
void *retrieved = dsc_arraylist_get(arr, 0);

// ========== HashMap Example ==========
DSCHashMap *map = dsc_hashmap_create(alloc, string_hash, string_equals, 0);
char *key = "example_key";
char *map_value = "example_value";
dsc_hashmap_put(map, key, map_value);
char *found = dsc_hashmap_get(map, key);

// ========== Stack Example ==========
DSCStack *stack = dsc_stack_create(alloc);
int *stack_value = alloc->malloc_func(sizeof(int));
*stack_value = 200;
dsc_stack_push(stack, stack_value);
void *popped = dsc_stack_pop(stack);

// ========== Queue Example ==========
DSCQueue *queue = dsc_queue_create(alloc);
int *queue_value = alloc->malloc_func(sizeof(int));
*queue_value = 300;
dsc_queue_enqueue(queue, queue_value);
void *dequeued = dsc_queue_dequeue(queue);

// Cleanup
sll_destroy(list, true);
str_destroy(&str);
dsc_arraylist_destroy(arr, true);
dsc_hashmap_destroy(map, false, false); // keys/values not owned
dsc_stack_destroy(stack, true);
dsc_queue_destroy(queue, true);
dsc_alloc_destroy(alloc);
```

## API Documentation

### Memory Management
All data structures support custom allocators and provide clear ownership semantics:

```c
// Create custom allocator
DSCAlloc* custom_alloc = dsc_alloc_create_custom(my_malloc, my_free, my_copy, my_data_free);

// Use custom allocator with any data structure
SinglyLinkedList *list = sll_create_custom(custom_alloc);
DSCHashMap *map = dsc_hashmap_create(custom_alloc, hash_func, equals_func, 16);

// Library handles node allocation, user manages data
sll_insert_back(list, user_data);
sll_destroy(list, true); // true = free user data with custom allocator
```

### Error Handling
Functions return consistent error codes:
- Pointer-returning functions return `NULL` on failure
- Status functions return `-1` on error, `0` on success
- Boolean functions return `false` on failure

### Iterator Support
Unified iteration with functional programming support across all containers:

```c
// Create iterator from any container
Iterator it = sll_iterator(list);           // Singly linked list
Iterator arr_it = dsc_arraylist_iterator(arraylist);  // ArrayList
Iterator map_it = dsc_hashmap_key_iterator(hashmap);  // HashMap keys

// Chain functional operations
Iterator filtered = iterator_filter(&it, is_even_predicate);
Iterator transformed = iterator_transform(&filtered, square_function);

while (transformed.has_next(&transformed)) {
    int *value = transformed.next(&transformed);
    printf("%d ", *value);
}
transformed.destroy(&transformed);
```

### Advanced HashMap Usage
```c
// String hash function example
size_t string_hash(const void* key) {
    const char* str = (const char*)key;
    size_t hash = 5381;
    while (*str) {
        hash = ((hash << 5) + hash) + *str++;
    }
    return hash;
}

// String equality function
int string_equals(const void* key1, const void* key2) {
    return strcmp((const char*)key1, (const char*)key2) == 0;
}

// Create and use HashMap
DSCHashMap* user_db = dsc_hashmap_create(alloc, string_hash, string_equals, 64);
dsc_hashmap_put(user_db, "user123", user_data);
User* user = dsc_hashmap_get(user_db, "user123");
```

## Architecture

### Design Principles
1. **Memory Safety** - No buffer overflows, proper allocation failure handling
2. **Generic Design** - Function pointers enable type-agnostic operations
3. **Zero-cost Abstractions** - Minimal overhead over manual implementation
4. **Clear Ownership** - Well-defined memory management responsibilities

### Thread Safety
- Data structures are **not** thread-safe by default for performance reasons
- Use external synchronization for concurrent access
- Built-in threading utilities available:

```c
// Cross-platform mutex
DSCMutex* mutex = dsc_mutex_create();
dsc_mutex_lock(mutex);
// ... access shared data structure ...
dsc_mutex_unlock(mutex);
dsc_mutex_destroy(mutex);

// Thread creation and management
DSCThread* worker = dsc_thread_create(worker_function, worker_data);
dsc_thread_join(worker);
dsc_thread_destroy(worker);
```

## Testing

The library includes comprehensive test suites with over 17,000 lines of test code:

- **52 Test Suites** - Covering all data structures and utilities
- **CRUD Operations** - Create, Read, Update, Delete functionality
- **Memory Management** - Allocation/deallocation correctness with sanitizers
- **Algorithm Testing** - Sorting, searching, transformation operations
- **Iterator Testing** - All iterator operations and edge cases
- **Property Testing** - Invariant verification across operations
- **Performance Testing** - Benchmarks for large dataset handling
- **Threading Tests** - Concurrent access patterns and thread safety
- **Memory Safety** - AddressSanitizer and UBSan integration

## Contributing

### Code Quality Standards
- C11 standard compliance
- AddressSanitizer/UBSan clean
- Comprehensive documentation
- 100% test coverage for new features

### Development Setup

```bash
# Clone the repository
git clone https://github.com/szmathias/DSContainers.git
cd DSContainers

# Enable sanitizers (automatic in Debug builds)
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)

# Run all tests
ctest --output-on-failure

# Run static analysis
clang-tidy src/*.c --

# Run specific test suite
./bin/test_hashmap_crud
./bin/test_arraylist_performance
```

### Integration

To integrate DSContainers into your project:

1. **Copy source files** to your project directory
2. **Include headers** in your build system
3. **Link the library** during compilation

```cmake
# CMake integration example
add_subdirectory(DSContainers)
target_link_libraries(your_target DSContainers)
```

## Performance Characteristics

| Operation    | Singly Linked List | Doubly Linked List | ArrayList | Dynamic String | Stack | Queue | HashMap | HashSet |
|--------------|--------------------|--------------------|-----------|----------------|-------|-------|---------|---------|
| Insert Front | O(1)               | O(1)               | O(n)      | O(n)           | O(1)  | N/A   | N/A     | N/A     |
| Insert Back  | O(n)               | O(1)               | O(1)*     | O(1)*          | N/A   | O(1)  | N/A     | N/A     |
| Remove Front | O(1)               | O(1)               | O(n)      | O(n)           | O(1)  | O(1)  | N/A     | N/A     |
| Remove Back  | O(n)               | O(1)               | O(1)      | O(1)           | N/A   | N/A   | N/A     | N/A     |
| Random Access| O(n)               | O(n)               | O(1)      | O(1)           | N/A   | N/A   | O(1)*   | N/A     |
| Search       | O(n)               | O(n)               | O(n)      | O(n)           | O(n)  | O(n)  | O(1)*   | O(1)*   |
| Sort         | O(n log n)         | O(n log n)         | O(n log n)| O(n log n)     | N/A   | N/A   | N/A     | N/A     |

*Amortized time complexity, **Average case for hash-based structures

## Planned Features

See [TODO.md](TODO.md) for upcoming features:
- **String View Library** - Lightweight string references for zero-copy operations
- **I/O Library** - Formatted printing, file operations, and stream utilities
- **Concurrent Data Structures** - Lock-free and thread-safe variants
- **Advanced Algorithms** - Graph algorithms, advanced sorting, pattern matching

## License

This project is released under an open source license. Please refer to the license terms in the repository.

## Changelog

### v1.0.0 (Current)
- ✅ Complete implementation of 8 core data structures
- ✅ Comprehensive test suite (52 test modules, 17,000+ lines)
- ✅ Cross-platform build system with CI/CD
- ✅ Memory safety with sanitizer integration
- ✅ Threading utilities and mutex support
- ✅ Generic iterator framework with functional programming support
- ✅ Custom allocator support across all containers
- ✅ Production-ready codebase with extensive documentation
