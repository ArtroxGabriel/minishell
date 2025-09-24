cmake_minimum_required(VERSION 3.10)
project(minishell)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB SRC_FILES CONFIGURE_DEPENDS
  src/*.c
)

add_executable(minishell
  ${SRC_FILES}
)
