cmake_minimum_required(VERSION 3.11)
include(FetchContent)

FetchContent_Declare(
  check
  GIT_REPOSITORY https://github.com/libcheck/check.git
  GIT_TAG        0.15.2
)
FetchContent_MakeAvailable(check)

add_executable(tests
  test_main.c
)

target_include_directories(tests PRIVATE ${check_SOURCE_DIR}/include)
target_link_libraries(tests PRIVATE check pthread)

enable_testing()
add_test(NAME minishell_tests COMMAND tests)
