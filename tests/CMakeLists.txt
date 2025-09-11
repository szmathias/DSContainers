# Automatically find all test source files
file(GLOB TEST_SOURCES "test_*.c")

# Create an OBJECT library for test helpers
add_library(test_helpers OBJECT TestHelper.c)
target_include_directories(test_helpers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Loop through all found test sources and create an executable and test for each
foreach(TEST_SOURCE ${TEST_SOURCES})
    # Get the base name of the file to use as the test name
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)

    # Add the executable for the test
    add_executable(${TEST_NAME} ${TEST_SOURCE})

    # Link the test executable against the main library and helpers
    target_link_libraries(${TEST_NAME} PRIVATE DSContainers $<TARGET_OBJECTS:test_helpers>)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()