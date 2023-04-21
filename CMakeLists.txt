cmake_minimum_required(VERSION 3.23)
project(LMVM C)

set(CMAKE_C_STANDARD 99)

# scan source files
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.c)

# include header files
include_directories(${PROJECT_SOURCE_DIR}/include)

# add executable and link libraries
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} regex)

