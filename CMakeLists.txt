cmake_minimum_required(VERSION 3.23)
project(LMVM C)

set(CMAKE_C_STANDARD 99)

# scan source files
file(GLOB_RECURSE LMASM_SOURCES ${PROJECT_SOURCE_DIR}/src/assembler/*.c)
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/vm/*.c)

# include header files
include_directories(${PROJECT_SOURCE_DIR}/include)

# add LMASM executable and link libraries
add_executable(LMASM ${LMASM_SOURCES})
target_link_libraries(LMASM regex)

# add LMVM executable
add_executable(LMVM ${SOURCES})
