cmake_minimum_required(VERSION 3.16)
project(ds C)

# Set default build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Detect platform
if(WIN32)
  set(DS_PLATFORM_WINDOWS TRUE)
elseif(APPLE)
  set(DS_PLATFORM_MACOS TRUE)
elseif(UNIX)
  set(DS_PLATFORM_LINUX TRUE)
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Setup compiler flags based on compiler type
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -g")

  # Enable sanitizers in Debug builds for GCC/Clang
  if(CMAKE_BUILD_TYPE MATCHES "Debug")
    message(STATUS "Configuring sanitizers for Debug build")

    # Add sanitizers directly without checking
    set(SANITIZER_FLAGS "-fsanitize=address,undefined -fno-omit-frame-pointer")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${SANITIZER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${SANITIZER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} ${SANITIZER_FLAGS}")

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
add_library(ds SHARED
        src/DynamicString.c
        src/SinglyLinkedList.c
        src/DoublyLinkedList.c
        src/Iterator.c
        include/DoublyLinkedList.h
        include/DynamicString.h
        include/Iterator.h
        include/PlatformDefs.h
        include/SinglyLinkedList.h
        include/CStandardCompatibility.h
        include/Alloc.h
)

# Create an OBJECT library for test helpers. This compiles the sources once
# and allows them to be linked into multiple test executables without creating
# a separate shared library.
add_library(test_helpers OBJECT tests/TestHelper.c)
target_include_directories(test_helpers PUBLIC tests)

target_include_directories(ds PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Define platform-specific compile definitions
if(DS_PLATFORM_WINDOWS)
  target_compile_definitions(ds PUBLIC DS_PLATFORM_WINDOWS)
elseif(DS_PLATFORM_MACOS)
  target_compile_definitions(ds PUBLIC DS_PLATFORM_MACOS)
elseif(DS_PLATFORM_LINUX)
  target_compile_definitions(ds PUBLIC DS_PLATFORM_LINUX)
endif()

# Define DLL export macros for Windows
if(DS_PLATFORM_WINDOWS)
  target_compile_definitions(ds PRIVATE DS_BUILDING_DLL)
endif()

# Set output directory for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

enable_testing()

# Add new SLL test executables
add_executable(test_sll_crud tests/test_sll_crud.c)
target_link_libraries(test_sll_crud PRIVATE ds test_helpers)
add_test(NAME test_sll_crud COMMAND test_sll_crud
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_sll_algorithms tests/test_sll_algorithms.c)
target_link_libraries(test_sll_algorithms PRIVATE ds test_helpers)
add_test(NAME test_sll_algorithms COMMAND test_sll_algorithms
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_sll_memory tests/test_sll_memory.c)
target_link_libraries(test_sll_memory PRIVATE ds test_helpers)
add_test(NAME test_sll_memory COMMAND test_sll_memory
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_sll_iterator tests/test_sll_iterator.c)
target_link_libraries(test_sll_iterator PRIVATE ds test_helpers)
add_test(NAME test_sll_iterator COMMAND test_sll_iterator
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_sll_properties tests/test_sll_properties.c)
target_link_libraries(test_sll_properties PRIVATE ds test_helpers)
add_test(NAME test_sll_properties
        COMMAND test_sll_properties
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)


# Add new DLL test executables
add_executable(test_dll_crud tests/test_dll_crud.c)
target_link_libraries(test_dll_crud PRIVATE ds test_helpers)
add_test(NAME test_dll_crud COMMAND test_dll_crud
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_dll_algorithms tests/test_dll_algorithms.c)
target_link_libraries(test_dll_algorithms PRIVATE ds test_helpers)
add_test(NAME test_dll_algorithms COMMAND test_dll_algorithms
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_dll_memory tests/test_dll_memory.c)
target_link_libraries(test_dll_memory PRIVATE ds test_helpers)
add_test(NAME test_dll_memory COMMAND test_dll_memory
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_dll_iterator tests/test_dll_iterator.c)
target_link_libraries(test_dll_iterator PRIVATE ds test_helpers)
add_test(NAME test_dll_iterator
        COMMAND test_dll_iterator
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_dll_properties tests/test_dll_properties.c)
target_link_libraries(test_dll_properties PRIVATE ds test_helpers)
add_test(NAME test_dll_properties
        COMMAND test_dll_properties
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

# Add new performance test executables
add_executable(test_sll_performance tests/test_sll_performance.c)
target_link_libraries(test_sll_performance PRIVATE ds test_helpers)
add_test(NAME test_sll_performance
        COMMAND test_sll_performance
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_dll_performance tests/test_dll_performance.c)
target_link_libraries(test_dll_performance PRIVATE ds test_helpers)
add_test(NAME test_dll_performance
        COMMAND test_dll_performance
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

# Add threaded test, requires C11 threads
find_package(Threads REQUIRED)
add_executable(test_dll_threaded tests/test_dll_threaded.c)
target_link_libraries(test_dll_threaded PRIVATE ds test_helpers Threads::Threads)
add_test(NAME test_dll_threaded
        COMMAND test_dll_threaded
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

# Add new iterator test executables
add_executable(test_iterator_range tests/test_iterator_range.c)
target_link_libraries(test_iterator_range PRIVATE ds test_helpers)
add_test(NAME test_iterator_range
        COMMAND test_iterator_range
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_iterator_transform tests/test_iterator_transform.c)
target_link_libraries(test_iterator_transform PRIVATE ds test_helpers)
add_test(NAME test_iterator_transform
        COMMAND test_iterator_transform
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_iterator_filter tests/test_iterator_filter.c)
target_link_libraries(test_iterator_filter PRIVATE ds test_helpers)
add_test(NAME test_iterator_filter
        COMMAND test_iterator_filter
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_iterator_chains tests/test_iterator_chains.c)
target_link_libraries(test_iterator_chains PRIVATE ds test_helpers)
add_test(NAME test_iterator_chains
        COMMAND test_iterator_chains
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

# Add tests for DString
add_executable(test_string_crud tests/test_string_crud.c)
target_link_libraries(test_string_crud PRIVATE ds)
add_test(NAME test_string_crud
        COMMAND test_string_crud
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_string_algorithms tests/test_string_algorithms.c)
target_link_libraries(test_string_algorithms PRIVATE ds)
add_test(NAME test_string_algorithms
        COMMAND test_string_algorithms
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_string_memory tests/test_string_memory.c)
target_link_libraries(test_string_memory PRIVATE ds)
add_test(NAME test_string_memory
        COMMAND test_string_memory
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_string_fuzz tests/test_string_fuzz.c)
target_link_libraries(test_string_fuzz PRIVATE ds)
add_test(NAME test_string_fuzz
        COMMAND test_string_fuzz
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

add_executable(test_string_properties tests/test_string_properties.c)
target_link_libraries(test_string_properties PRIVATE ds)
add_test(NAME test_string_properties
        COMMAND test_string_properties
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)