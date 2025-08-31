# Data Structures Library (ds)

A comprehensive C library implementing essential data structures with a focus on memory safety, performance, and usability.

## Features

### Core Data Structures
- **Singly Linked List** - Efficient forward-only list with O(1) insertions
- **Doubly Linked List** - Bidirectional list with O(1) insertions/deletions at both ends  
- **Dynamic String** - Growable string with small string optimization
- **Generic Iterator** - Unified iteration interface with filter/transform support

### Key Capabilities
- ✅ Custom memory allocators support
- ✅ Generic design with function pointers
- ✅ Comprehensive error handling
- ✅ Memory leak protection with sanitizers
- ✅ Thread-safety considerations
- ✅ Cross-platform compatibility (Windows, macOS, Linux)
- ✅ Extensive test coverage (1,072+ assertions)

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

// Create and use a linked list
SinglyLinkedList *list = sll_create();
int *value = malloc(sizeof(int));
*value = 42;
sll_insert_back(list, value);

// Create and use a dynamic string
String str = str_create("Hello, World!");
str_append(&str, " from ds library!");
printf("%s\n", str_data(&str));

// Cleanup
sll_destroy(list, free);
str_destroy(&str);
```

## API Documentation

### Memory Management
All data structures support custom allocators and provide clear ownership semantics:

```c
// Use custom allocator
SinglyLinkedList *list = sll_create_custom(my_alloc, my_free);

// Library handles node allocation, user manages data
sll_insert_back(list, user_data);
sll_destroy(list, user_data_free_function);
```

### Error Handling
Functions return consistent error codes:
- Pointer-returning functions return `NULL` on failure
- Status functions return `-1` on error, `0` on success
- Boolean functions return `false` on failure

### Iterator Support
Unified iteration with functional programming support:

```c
Iterator it = sll_iterator(list);
Iterator filtered = iterator_filter(&it, is_even_predicate);
Iterator transformed = iterator_transform(&filtered, square_function);

while (transformed.has_next(&transformed)) {
    int *value = transformed.next(&transformed);
    printf("%d ", *value);
}
transformed.destroy(&transformed);
```

## Architecture

### Design Principles
1. **Memory Safety** - No buffer overflows, proper allocation failure handling
2. **Generic Design** - Function pointers enable type-agnostic operations
3. **Zero-cost Abstractions** - Minimal overhead over manual implementation
4. **Clear Ownership** - Well-defined memory management responsibilities

### Thread Safety
- Data structures are **not** thread-safe by default
- Use external synchronization for concurrent access
- Thread-safe variants available for specific use cases

## Testing

The library includes comprehensive test suites:

- **CRUD Operations** - Create, Read, Update, Delete functionality
- **Memory Management** - Allocation/deallocation correctness
- **Algorithm Testing** - Sorting, searching, transformation operations
- **Iterator Testing** - All iterator operations and edge cases
- **Property Testing** - Invariant verification
- **Stress Testing** - Large dataset handling

## Contributing

### Code Quality Standards
- C11 standard compliance
- AddressSanitizer/UBSan clean
- Comprehensive documentation
- 100% test coverage for new features

### Development Setup
```bash
# Enable sanitizers (automatic in Debug builds)
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Run static analysis
clang-tidy src/*.c --
```

## Performance Characteristics

| Operation    | Singly Linked List | Doubly Linked List | Dynamic String |
|--------------|--------------------|--------------------|----------------|
| Insert Front | O(1)               | O(1)               | O(n)           |
| Insert Back  | O(n)               | O(1)               | O(1) amortized |
| Remove Front | O(1)               | O(1)               | O(n)           |
| Remove Back  | O(n)               | O(1)               | O(1)           |
| Search       | O(n)               | O(n)               | O(n)           |
| Sort         | O(n log n)         | O(n log n)         | O(n log n)     |

## Planned Features

See [TODO.md](TODO.md) for upcoming features:
- String View Library (lightweight string references)
- I/O Library (formatted printing, file operations)

## License

[Add license information here]

## Changelog

### v0.1.0 (Current)
- Initial implementation of core data structures
- Comprehensive test suite
- Cross-platform build system
- Memory safety improvements