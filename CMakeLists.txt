cmake_minimum_required(VERSION 3.16)
project(DSContainers C)

# Set default build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Detect platform
if(WIN32)
  set(DSCONTAINERS_PLATFORM_WINDOWS TRUE)
elseif(APPLE)
  set(DSCONTAINERS_PLATFORM_MACOS TRUE)
elseif(UNIX)
  set(DSCONTAINERS_PLATFORM_LINUX TRUE)
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(ENABLE_SANITIZERS "Enable Address/Undefined sanitizers in Debug builds" ON)

# Setup compiler flags based on compiler type
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -g")

  # Enable sanitizers in Debug builds for GCC/Clang when requested
  if(CMAKE_BUILD_TYPE MATCHES "Debug" AND ENABLE_SANITIZERS)
    message(STATUS "Configuring sanitizers for Debug build")

    # Add sanitizers directly without checking
    set(SANITIZER_FLAGS "-fsanitize=address,undefined -fno-omit-frame-pointer")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${SANITIZER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${SANITIZER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} ${SANITIZER_FLAGS}")

    # Define a compile-time macro so tests can detect sanitizer-enabled builds
    add_compile_definitions(DSC_SANITIZERS_ENABLED)

    message(STATUS "Added sanitizer flags: ${SANITIZER_FLAGS}")
    message(STATUS "Final C flags for Debug: ${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_DEBUG}")
  endif()
elseif(MSVC)
  # MSVC-specific flags
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
  # Disable specific warnings
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4996") # Disable deprecation warnings
endif()

# Configure library
add_library(DSContainers SHARED
        src/DynamicString.c
        src/SinglyLinkedList.c
        src/DoublyLinkedList.c
        src/Iterator.c
        src/Mutex.c
        src/Threads.c
        include/DoublyLinkedList.h
        include/DynamicString.h
        include/Iterator.h
        include/PlatformDefs.h
        include/SinglyLinkedList.h
        include/CStandardCompatibility.h
        include/Alloc.h
        include/Threads.h
        include/Mutex.h
        include/ArrayList.h
        src/ArrayList.c
)

# Create an OBJECT library for test helpers. This compiles the sources once
# and allows them to be linked into multiple test executables without creating
# a separate shared library.
add_library(test_helpers OBJECT tests/TestHelper.c)
target_include_directories(test_helpers PUBLIC tests)

target_include_directories(DSContainers PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Define platform-specific compile definitions
if(DSCONTAINERS_PLATFORM_WINDOWS)
  target_compile_definitions(DSContainers PUBLIC DSCONTAINERS_PLATFORM_WINDOWS)
elseif(DSCONTAINERS_PLATFORM_MACOS)
  target_compile_definitions(DSContainers PUBLIC DSCONTAINERS_PLATFORM_MACOS)
elseif(DSCONTAINERS_PLATFORM_LINUX)
  target_compile_definitions(DSContainers PUBLIC DSCONTAINERS_PLATFORM_LINUX)
endif()

# Define DLL export macros for Windows
if(DSCONTAINERS_PLATFORM_WINDOWS)
  target_compile_definitions(DSContainers PRIVATE DSCONTAINERS_BUILDING_DLL)
endif()

# Set output directory for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

enable_testing()

# Add new SLL test executables
add_executable(test_sll_crud tests/test_sll_crud.c)
target_link_libraries(test_sll_crud PRIVATE DSContainers test_helpers)
add_test(NAME test_sll_crud COMMAND test_sll_crud
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_sll_algorithms tests/test_sll_algorithms.c)
target_link_libraries(test_sll_algorithms PRIVATE DSContainers test_helpers)
add_test(NAME test_sll_algorithms COMMAND test_sll_algorithms
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_sll_memory tests/test_sll_memory.c)
target_link_libraries(test_sll_memory PRIVATE DSContainers test_helpers)
add_test(NAME test_sll_memory COMMAND test_sll_memory
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_sll_iterator tests/test_sll_iterator.c)
target_link_libraries(test_sll_iterator PRIVATE DSContainers test_helpers)
add_test(NAME test_sll_iterator COMMAND test_sll_iterator
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_sll_properties tests/test_sll_properties.c)
target_link_libraries(test_sll_properties PRIVATE DSContainers test_helpers)
add_test(NAME test_sll_properties
        COMMAND test_sll_properties
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

# Add new DLL test executables
add_executable(test_dll_crud tests/test_dll_crud.c)
target_link_libraries(test_dll_crud PRIVATE DSContainers test_helpers)
add_test(NAME test_dll_crud COMMAND test_dll_crud
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_dll_algorithms tests/test_dll_algorithms.c)
target_link_libraries(test_dll_algorithms PRIVATE DSContainers test_helpers)
add_test(NAME test_dll_algorithms COMMAND test_dll_algorithms
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_dll_memory tests/test_dll_memory.c)
target_link_libraries(test_dll_memory PRIVATE DSContainers test_helpers)
add_test(NAME test_dll_memory COMMAND test_dll_memory
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_dll_iterator tests/test_dll_iterator.c)
target_link_libraries(test_dll_iterator PRIVATE DSContainers test_helpers)
add_test(NAME test_dll_iterator
        COMMAND test_dll_iterator
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_dll_properties tests/test_dll_properties.c)
target_link_libraries(test_dll_properties PRIVATE DSContainers test_helpers)
add_test(NAME test_dll_properties
        COMMAND test_dll_properties
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

# Add new performance test executables
add_executable(test_sll_performance tests/test_sll_performance.c)
target_link_libraries(test_sll_performance PRIVATE DSContainers test_helpers)
add_test(NAME test_sll_performance
        COMMAND test_sll_performance
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_dll_performance tests/test_dll_performance.c)
target_link_libraries(test_dll_performance PRIVATE DSContainers test_helpers)
add_test(NAME test_dll_performance
        COMMAND test_dll_performance
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

# Add threaded test, requires C11 threads
if (NOT ENABLE_SANITIZERS)
  find_package(Threads REQUIRED)
  add_executable(test_dll_threaded tests/test_dll_threaded.c)

  # Build the threaded test without sanitizers because it intentionally exercises race conditions
  # which can corrupt data structures and lead to AddressSanitizer reporting leaks.
  if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
      target_compile_options(test_dll_threaded PRIVATE -fno-sanitize=address,undefined)
      target_link_options(test_dll_threaded PRIVATE -fno-sanitize=address,undefined)
  endif()

  target_link_libraries(test_dll_threaded PRIVATE DSContainers test_helpers Threads::Threads)
  add_test(NAME test_dll_threaded
          COMMAND test_dll_threaded
          WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)
endif ()

# Add new iterator test executables
add_executable(test_iterator_range tests/test_iterator_range.c)
target_link_libraries(test_iterator_range PRIVATE DSContainers test_helpers)
add_test(NAME test_iterator_range
        COMMAND test_iterator_range
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_iterator_transform tests/test_iterator_transform.c)
target_link_libraries(test_iterator_transform PRIVATE DSContainers test_helpers)
add_test(NAME test_iterator_transform
        COMMAND test_iterator_transform
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_iterator_filter tests/test_iterator_filter.c)
target_link_libraries(test_iterator_filter PRIVATE DSContainers test_helpers)
add_test(NAME test_iterator_filter
        COMMAND test_iterator_filter
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_iterator_chains tests/test_iterator_chains.c)
target_link_libraries(test_iterator_chains PRIVATE DSContainers test_helpers)
add_test(NAME test_iterator_chains
        COMMAND test_iterator_chains
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

# Add tests for DString
add_executable(test_string_crud tests/test_string_crud.c)
target_link_libraries(test_string_crud PRIVATE DSContainers)
add_test(NAME test_string_crud
        COMMAND test_string_crud
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_string_algorithms tests/test_string_algorithms.c)
target_link_libraries(test_string_algorithms PRIVATE DSContainers)
add_test(NAME test_string_algorithms
        COMMAND test_string_algorithms
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_string_memory tests/test_string_memory.c)
target_link_libraries(test_string_memory PRIVATE DSContainers)
add_test(NAME test_string_memory
        COMMAND test_string_memory
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_string_fuzz tests/test_string_fuzz.c)
target_link_libraries(test_string_fuzz PRIVATE DSContainers)
add_test(NAME test_string_fuzz
        COMMAND test_string_fuzz
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_string_properties tests/test_string_properties.c)
target_link_libraries(test_string_properties PRIVATE DSContainers)
add_test(NAME test_string_properties
        COMMAND test_string_properties
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_mutex_basic tests/test_mutex_basic.c)
target_link_libraries(test_mutex_basic PRIVATE DSContainers)
add_test(NAME test_mutex_basic
        COMMAND test_mutex_basic
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_mutex_unit tests/test_mutex_unit.c)
target_link_libraries(test_mutex_unit PRIVATE DSContainers)
add_test(NAME test_mutex_unit
        COMMAND test_mutex_unit
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

# Add tests for threads and mutex extra cases
add_executable(test_threads_mutex_extra tests/test_threads_mutex_extra.c)
target_link_libraries(test_threads_mutex_extra PRIVATE DSContainers test_helpers)
add_test(NAME test_threads_mutex_extra
        COMMAND test_threads_mutex_extra
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

# Add ArrayList test executables
add_executable(test_arraylist_crud tests/test_arraylist_crud.c)
target_link_libraries(test_arraylist_crud PRIVATE DSContainers test_helpers)
add_test(NAME test_arraylist_crud
        COMMAND test_arraylist_crud
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_arraylist_algorithms tests/test_arraylist_algorithms.c)
target_link_libraries(test_arraylist_algorithms PRIVATE DSContainers test_helpers)
add_test(NAME test_arraylist_algorithms
        COMMAND test_arraylist_algorithms
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_arraylist_memory tests/test_arraylist_memory.c)
target_link_libraries(test_arraylist_memory PRIVATE DSContainers test_helpers)
add_test(NAME test_arraylist_memory
        COMMAND test_arraylist_memory
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_arraylist_iterator tests/test_arraylist_iterator.c)
target_link_libraries(test_arraylist_iterator PRIVATE DSContainers test_helpers)
add_test(NAME test_arraylist_iterator
        COMMAND test_arraylist_iterator
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_arraylist_properties tests/test_arraylist_properties.c)
target_link_libraries(test_arraylist_properties PRIVATE DSContainers test_helpers)
add_test(NAME test_arraylist_properties
        COMMAND test_arraylist_properties
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)
