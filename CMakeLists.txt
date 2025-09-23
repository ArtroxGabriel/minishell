cmake_minimum_required(VERSION 3.14)
project(os-c-template VERSION 0.1.0 LANGUAGES C)

# ------------------------------------------------------------
# Project configuration options
# ------------------------------------------------------------
option(ENABLE_SANITIZERS "Enable Address/Undefined sanitizers when supported" ON)
option(BUILD_TESTING "Enable building of tests" ON)
option(BUILD_SHARED_LIBS "Build libraries as shared by default" OFF)
option(ENABLE_CODE_COVERAGE "Enable generation of code coverage (GCC/Clang only)" OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Default compile flags and warnings, trying to be strict but portable
if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wformat=2)
    # -fno-common helps catch ODR-like issues in C builds across compilers
    add_compile_options(-fno-common)
endif()

# ------------------------------------------------------------
# Sanitizers (only for GCC/Clang on non-MSVC platforms)
# ------------------------------------------------------------
if (ENABLE_SANITIZERS AND NOT MSVC)
    include(CheckCCompilerFlag)
    check_c_compiler_flag("-fsanitize=address" COMPILER_SUPPORTS_ASAN)
    check_c_compiler_flag("-fsanitize=undefined" COMPILER_SUPPORTS_UBSAN)
    if (COMPILER_SUPPORTS_ASAN AND COMPILER_SUPPORTS_UBSAN)
        message(STATUS "Enabling Address and Undefined sanitizers")
        add_compile_options(-fsanitize=address,undefined)
        add_link_options(-fsanitize=address,undefined)
    else()
        message(WARNING "Sanitizers requested but not available for this compiler; skipping")
    endif()
endif()

# ------------------------------------------------------------
# Coverage support (best-effort)
# ------------------------------------------------------------
if (ENABLE_CODE_COVERAGE AND NOT MSVC)
    include(CheckCCompilerFlag)
    check_c_compiler_flag("--coverage" COMPILER_SUPPORTS_COVERAGE)
    if (COMPILER_SUPPORTS_COVERAGE)
        message(STATUS "Enabling coverage instrumentation")
        add_compile_options(--coverage -O0)
        add_link_options(--coverage)
    else()
        message(WARNING "Coverage requested but not supported by this compiler")
    endif()
endif()

# ------------------------------------------------------------
# Project layout: source and include
# ------------------------------------------------------------
# The repository is expected to follow:
#  - src/        -> implementation (.c)
#  - include/    -> public headers (.h)
#  - tests/      -> unit tests
#  - tools/      -> helper scripts (formatters, linters, etc.)
#
# Subdirectories are added below; each subdirectory should add its targets
#
add_subdirectory(src)

# ------------------------------------------------------------
# Tests
# ------------------------------------------------------------
if (BUILD_TESTING)
    enable_testing()
    # The tests directory should contain its own CMakeLists that registers tests
    if (EXISTS "${CMAKE_SOURCE_DIR}/tests/CMakeLists.txt")
        add_subdirectory(tests)
    else()
        message(STATUS "No tests/CMakeLists.txt found; skipping tests subdirectory")
    endif()
endif()

# ------------------------------------------------------------
# Formatting and linting helper targets
# These are convenience targets that try to run clang-format and clang-tidy
# If the tools aren't present they are skipped gracefully.
# ------------------------------------------------------------
find_program(CLANG_FORMAT_BIN clang-format)
if (CLANG_FORMAT_BIN)
    file(GLOB_RECURSE ALL_C_FILES
        "${CMAKE_SOURCE_DIR}/src/*.c"
        "${CMAKE_SOURCE_DIR}/include/*.h"
        "${CMAKE_SOURCE_DIR}/tests/*.c"
        "${CMAKE_SOURCE_DIR}/*.c"
    )

    add_custom_target(format
        COMMAND ${CLANG_FORMAT_BIN} -i --style=file ${ALL_C_FILES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running clang-format on C sources (style taken from .clang-format if present)"
    )
else()
    message(STATUS "clang-format not found; `format` target will not be available")
endif()

find_program(CLANG_TIDY_BIN clang-tidy)
if (CLANG_TIDY_BIN AND NOT MSVC)
    # Note: clang-tidy can be noisy; this target demonstrates a basic invocation.
    add_custom_target(lint
        COMMAND ${CLANG_TIDY_BIN}
                -p ${CMAKE_BINARY_DIR}
                ${CMAKE_SOURCE_DIR}/src/*.c
                --
                -I${CMAKE_SOURCE_DIR}/include
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running clang-tidy on source files (requires compilation database)"
    )
else()
    message(STATUS "clang-tidy not found or running on MSVC; `lint` target will not be available")
endif()

# Provide a target to generate a compile_commands.json (useful for clang-tidy and editor integrations)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------------------------------------
# Installation and packaging
# ------------------------------------------------------------
include(GNUInstallDirs)
# Install rules are best defined by the targets in src/
# Keep a small helper target for packaging
include(CPack)
set(CPACK_PACKAGE_NAME "os-c-template")
set(CPACK_PACKAGE_VENDOR "University")
set(CPACK_PACKAGE_CONTACT "antgabriel.dev@gmail.com")
set(CPACK_PACKAGE_DESCRIPTION "Template project for Systems Operating course in C")
set(CPACK_GENERATOR "TGZ;ZIP")

# ------------------------------------------------------------
# Developer notes and summary
# ------------------------------------------------------------
message(STATUS "Project: ${PROJECT_NAME} ${PROJECT_VERSION}")
message(STATUS "C standard: ${CMAKE_C_STANDARD}")
message(STATUS "Generator: ${CMAKE_GENERATOR}")
message(STATUS "System: ${CMAKE_SYSTEM_NAME}")
