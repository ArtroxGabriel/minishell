# Tests CMakeLists for os-c-template
#
# This CMake file creates a simple CTest target for the project library (mylib).
# It will:
#  - create an example test source file if none exists
#  - build a test executable that links against the library target `mylib`
#  - register the test with CTest so `ctest` and `cmake --build` workflows work
#
# Usage:
#  - From the top-level build directory:
#      cmake -S .. -B build
#      cmake --build build --target test
#      ctest --test-dir build
#
# Notes:
#  - If you prefer a unit-test framework (cmocka, Unity, Check), set USE_TEST_FRAMEWORK
#    to a supported value and adjust the test source accordingly.
cmake_minimum_required(VERSION 3.14)

# This file is intended to be included from the top-level; ensure we have a project
if (NOT PROJECT_NAME)
    project(os-c-template LANGUAGES C)
endif()

include(CTest)

# Option to enable/disable building tests from this subdir (respects top-level BUILD_TESTING)
option(ENABLE_UNIT_TESTS "Enable building unit tests in tests/ (this CMakeLists)" ON)

if (NOT ENABLE_UNIT_TESTS)
    message(STATUS "Unit tests disabled in tests/CMakeLists.txt")
    return()
endif()

# Basic test executable name and source file
set(TEST_EXE_NAME test_mylib)
set(TEST_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/test_mylib.c)

# If no test source exists, create a minimal example that exercises mylib
# This helps first-time users to run tests without adding files.
if (NOT EXISTS ${TEST_SOURCE})
    message(STATUS "No test_mylib.c found; generating an example test source at ${TEST_SOURCE}")
    file(WRITE ${TEST_SOURCE} "/* Auto-generated minimal test for os-c-template */\n#include <stdio.h>\n#include <stdlib.h>\n#include <assert.h>\n#include \"mylib.h\"\n\nint main(void) {\n    /* initialize library if needed */\n    if (mylib_init() != MYLIB_OK) {\n        fprintf(stderr, \"mylib_init failed\\n\");\n        return EXIT_FAILURE;\n    }\n\n    /* simple functional checks */\n    assert(mylib_add(2, 3) == 5);\n\n    int32_t out = 0;\n    if (mylib_subtract_checked(10, 4, &out) != MYLIB_OK) {\n        fprintf(stderr, \"mylib_subtract_checked failed\\n\");\n        return EXIT_FAILURE;\n    }\n    assert(out == 6);\n\n    char *s = mylib_dup_string(\"hello\");\n    if (!s) { fprintf(stderr, \"mylib_dup_string failed\\n\"); return EXIT_FAILURE; }\n    puts(s);\n    mylib_free_string(s);\n\n    mylib_shutdown();\n    puts(\"[OK] basic tests passed\");\n    return EXIT_SUCCESS;\n}\n")
endif()

# Create the test executable and link to the library target defined in src/
# It is assumed the top-level CMake created a target named `mylib`.
add_executable(${TEST_EXE_NAME} ${TEST_SOURCE})

# Ensure include directory is available for tests
target_include_directories(${TEST_EXE_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link to the project library. If `mylib` target is not found (user uses different name),
# prefer a best-effort attempt to link a library by name.
if (TARGET mylib)
    target_link_libraries(${TEST_EXE_NAME} PRIVATE mylib)
else()
    message(WARNING "Target 'mylib' not found. If you named your library differently, update tests/CMakeLists.txt to link the correct target.")
endif()

# Use the same compile options as the project (inherit)
set_target_properties(${TEST_EXE_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests_bin
)

# Register with CTest so `ctest` discovers it
add_test(NAME ${TEST_EXE_NAME} COMMAND ${TEST_EXE_NAME})

# Convenience: add an alias target `test` that builds and runs tests via `ctest -R ^test_mylib$ --output-on-failure`
add_custom_target(test_all
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -R ^${TEST_EXE_NAME}$
    DEPENDS ${TEST_EXE_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Build and run unit tests"
)

# Optional: code-coverage helper (only works if coverage instrumentation is enabled at top-level)
option(ENABLE_TEST_COVERAGE "Collect coverage from unit tests (requires coverage flags enabled in top-level)" OFF)
if (ENABLE_TEST_COVERAGE)
    find_program(LCOV_BIN lcov)
    find_program(GENHTML_BIN genhtml)
    if (LCOV_BIN AND GENHTML_BIN)
        set(COVERAGE_INFO "${CMAKE_BINARY_DIR}/coverage.info")
        set(COVERAGE_DIR "${CMAKE_BINARY_DIR}/coverage_report")
        add_custom_target(coverage
            COMMAND ${LCOV_BIN} --directory . --capture --output-file ${COVERAGE_INFO}
            COMMAND ${LCOV_BIN} --remove ${COVERAGE_INFO} '/usr/*' --output-file ${COVERAGE_INFO}.filtered
            COMMAND ${GENHTML_BIN} ${COVERAGE_INFO}.filtered --output-directory ${COVERAGE_DIR}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generate code coverage report (lcov/genhtml)"
        )
    else()
        message(WARNING "Coverage requested but lcov/genhtml not found. Install them to enable coverage target.")
    endif()
endif()

# Helpful summary message
message(STATUS "Configured tests: ${TEST_EXE_NAME}")
