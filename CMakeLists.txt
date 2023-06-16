cmake_minimum_required(VERSION 3.00)
project(LMVM C)

set(CMAKE_C_STANDARD 99)

# scan source files
file(GLOB_RECURSE ASM_SOURCES ${PROJECT_SOURCE_DIR}/src/assembler/*.c)
file(GLOB_RECURSE VM_SOURCES ${PROJECT_SOURCE_DIR}/src/vm/*.c)
file(GLOB_RECURSE COMMON_SOURCES ${PROJECT_SOURCE_DIR}/src/common/*.c)

# add icon resource if windows
IF (WIN32)
    set(ASM_SOURCES ${ASM_SOURCES} ${PROJECT_SOURCE_DIR}/src/assembler/icon.rc)
    set(VM_SOURCES ${VM_SOURCES} ${PROJECT_SOURCE_DIR}/src/vm/icon.rc)
ENDIF ()

# include header files
include_directories(${PROJECT_SOURCE_DIR}/include)

# set version here
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

MESSAGE(STATUS "Version v${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")


# add LMASM executable
add_executable(LMASM ${ASM_SOURCES} ${COMMON_SOURCES})

# add regex library for windows
IF (WIN32)
    target_link_libraries(LMASM regex)
ENDIF ()

# add LMVM executable
add_executable(LMVM ${VM_SOURCES} ${COMMON_SOURCES})

# add version info to build definitions
target_compile_definitions(LMASM PRIVATE -DVERSION_MAJOR=${VERSION_MAJOR} -DVERSION_MINOR=${VERSION_MINOR} -DVERSION_PATCH=${VERSION_PATCH})
target_compile_definitions(LMVM PRIVATE -DVERSION_MAJOR=${VERSION_MAJOR} -DVERSION_MINOR=${VERSION_MINOR} -DVERSION_PATCH=${VERSION_PATCH})

# use harsh flags
if (MSVC)
    MESSAGE(STATUS "MSVC is not a supported compiler and may fail!")
    target_compile_options(LMASM PRIVATE /W4 /WX)
    target_compile_options(LMVM PRIVATE /W4 /WX)
else ()
    target_compile_options(LMASM PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(LMVM PRIVATE -Wall -Wextra -pedantic -Werror)
endif ()

# check if installers are enabled
if (INSTALLER STREQUAL OFF)
    MESSAGE(STATUS "Installer disabled, skipping installer generation")
else ()
    # make installer
    include(scripts/make_installer.cmake)
endif ()