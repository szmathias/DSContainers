cmake_minimum_required(VERSION 4.0)
project(ds C)


set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
# Enable sanitizers in Debug builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(SANITIZER_FLAGS "-fsanitize=address,undefined -fno-omit-frame-pointer")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SANITIZER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SANITIZER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${SANITIZER_FLAGS}")
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)
add_library(ds SHARED
        include/DString.h
        src/DString.c)

enable_testing()

# Add tests for DString
add_executable(test_string tests/test_string.c tests/TestAssert.h)
target_link_libraries(test_string ds)
add_test(NAME test_string
        COMMAND test_string
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

