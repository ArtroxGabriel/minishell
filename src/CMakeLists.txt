cmake_minimum_required(VERSION 3.14)

# This CMakeLists is intended to be included from the top-level CMakeLists.
# It creates:
#  - a library target `mylib` (static/shared depending on BUILD_SHARED_LIBS)
#  - an example executable `os_app` that links against `mylib`
#
# Expected layout:
#  - src/mylib.c
#  - src/main.c
#  - include/*.h
#
# Keep this file minimal; project-wide options (warnings, standards, sanitizers)
# are managed at the top-level.

# Collect source files (simple glob; adjust if you prefer explicit lists)
file(GLOB_RECURSE SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

# Separate library and example sources if you want different compilation units.
# By default assume any .c in src/ that is not main.c belongs to the library.
set(LIB_SOURCES "")
set(APP_SOURCES "")

foreach(_f IN LISTS SRC_FILES)
    get_filename_component(_name "${_f}" NAME)
    if (_name STREQUAL "main.c" OR _name STREQUAL "app.c")
        list(APPEND APP_SOURCES "${_f}")
    else()
        list(APPEND LIB_SOURCES "${_f}")
    endif()
endforeach()

# Leave a fallback in case glob didn't pick anything (helps users who create files later)
if (NOT LIB_SOURCES)
    # create a placeholder source name so CMake doesn't error; user should replace it
    list(APPEND LIB_SOURCES "${CMAKE_CURRENT_LIST_DIR}/mylib.c")
endif()

if (NOT APP_SOURCES)
    list(APPEND APP_SOURCES "${CMAKE_CURRENT_LIST_DIR}/main.c")
endif()

# Create the library. Respect BUILD_SHARED_LIBS set at top-level or by user.
add_library(mylib ${LIB_SOURCES})

# Public include path points to the top-level include/ directory
target_include_directories(mylib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Recommended compile features / properties (can be extended)
target_compile_features(mylib PUBLIC c_std_11)

# Example executable
add_executable(os_app ${APP_SOURCES})
target_link_libraries(os_app PRIVATE mylib)

# If you want to export symbols for shared builds on Windows, users can add properties here.
if (WIN32)
    # Example: add_compile_definitions for DLL export macro (left commented for template)
    # target_compile_definitions(mylib PUBLIC MYLIB_BUILD_DLL)
endif()

# Install rules for library, headers and executable
include(GNUInstallDirs)

# Install library (archive or shared)
install(TARGETS mylib
    EXPORT os_c_templateTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(TARGETS os_app
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers from the project's include/ directory (if present)
if (EXISTS "${CMAKE_SOURCE_DIR}/include")
    install(DIRECTORY "${CMAKE_SOURCE_DIR}/include/"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.h"
    )
endif()

# Export target config for downstream use
install(EXPORT os_c_templateTargets
    FILE os_c_templateTargets.cmake
    NAMESPACE os_c_template::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/os_c_template
)

# Provide a small test target helper (useful during development)
add_custom_target(run
    COMMAND os_app
    DEPENDS os_app
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Run the example application"
)

# Developer convenience: a target to list sources used by the library
add_custom_target(list-src
    COMMAND ${CMAKE_COMMAND} -E echo \"Library sources:\"
    COMMAND ${CMAKE_COMMAND} -E echo ${LIB_SOURCES}
    COMMENT "List collected library source files"
)

# End of src/CMakeLists.txt
