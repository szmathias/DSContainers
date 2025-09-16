cmake_minimum_required(VERSION 3.16)
project(DSContainers
        LANGUAGES C
        VERSION 0.7.6)

# Set default build type to Debug if not specified
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif ()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Modern C settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Use file globbing to automatically find source files.
# This is often more convenient than listing every file.
file(GLOB_RECURSE SOURCES "src/*.c")

# Configure library
add_library(DSContainers SHARED ${SOURCES})

if (WIN32)
    add_custom_command(TARGET DSContainers POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:DSContainers>
            ${CMAKE_BINARY_DIR}/tests/
    )
endif ()

# Use target_include_directories for better encapsulation.
# This applies include paths to a specific target.
target_include_directories(DSContainers PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Use target_compile_options to apply compiler flags to a specific target.
if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(DSContainers PRIVATE -Wall -Wextra -Wpedantic -g)
elseif (MSVC)
    target_compile_options(DSContainers PRIVATE /W4 /wd4996)
endif ()

# Platform-specific compile definitions applied to the target
if (WIN32)
    target_compile_definitions(DSContainers PUBLIC DSCONTAINERS_PLATFORM_WINDOWS)
    target_compile_definitions(DSContainers PRIVATE DSCONTAINERS_BUILDING_DLL)
elseif (APPLE)
    target_compile_definitions(DSContainers PUBLIC DSCONTAINERS_PLATFORM_MACOS)
elseif (UNIX AND NOT APPLE)
    target_compile_definitions(DSContainers PUBLIC DSCONTAINERS_PLATFORM_LINUX)
endif ()

# Sanitizers option
option(ENABLE_SANITIZERS "Enable Address/Undefined sanitizers in Debug builds" ON)
if (ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE MATCHES "Debug" AND (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang"))
    message(STATUS "Configuring sanitizers for Debug build")
    target_compile_options(DSContainers PUBLIC -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_libraries(DSContainers PUBLIC -fsanitize=address,undefined)
    target_compile_definitions(DSContainers PUBLIC DSC_SANITIZERS_ENABLED)
endif ()

configure_file(
        "${CMAKE_SOURCE_DIR}/README.md.in"
        "${CMAKE_SOURCE_DIR}/README.md"
        @ONLY
)

enable_testing()

# Add the tests subdirectory
add_subdirectory(tests)