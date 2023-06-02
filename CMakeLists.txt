cmake_minimum_required(VERSION 3.23)
project(LMVM C)

set(CMAKE_C_STANDARD 99)

# scan source files
file(GLOB_RECURSE ASM_SOURCES ${PROJECT_SOURCE_DIR}/src/assembler/*.c)
file(GLOB_RECURSE VM_SOURCES ${PROJECT_SOURCE_DIR}/src/vm/*.c)
file(GLOB_RECURSE COMMON_SOURCES ${PROJECT_SOURCE_DIR}/src/common/*.c)

# include header files
include_directories(${PROJECT_SOURCE_DIR}/include)

# add LMASM executable and link libraries
add_executable(LMASM ${ASM_SOURCES} ${COMMON_SOURCES})
target_link_libraries(LMASM regex)

# add LMVM executable
add_executable(LMVM ${VM_SOURCES} ${COMMON_SOURCES})
