cmake_minimum_required(VERSION 3.20)
project(LMVM C)

set(CMAKE_C_STANDARD 99)

# scan source files
file(GLOB_RECURSE ASM_SOURCES ${PROJECT_SOURCE_DIR}/src/assembler/*.c)
file(GLOB_RECURSE VM_SOURCES ${PROJECT_SOURCE_DIR}/src/vm/*.c)
file(GLOB_RECURSE COMMON_SOURCES ${PROJECT_SOURCE_DIR}/src/common/*.c)

# include header files
include_directories(${PROJECT_SOURCE_DIR}/include)

# get git info
#include(scripts/GetGitRevisionDescription.cmake)
#MESSAGE(STATUS "Git executable: " ${GIT_EXECUTABLE})
#get_git_head_revision(GIT_REFSPEC GIT_SHA1)
#git_get_exact_tag(GIT_TAG)
#execute_process(COMMAND ${GIT_EXECUTABLE} show -s --format=%cs HEAD
#        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#        OUTPUT_VARIABLE GIT_DATE
#        OUTPUT_STRIP_TRAILING_WHITESPACE)
#
#MESSAGE(STATUS "Git preprocessed tag: ${GIT_TAG}")
#
#string(SUBSTRING ${GIT_SHA1} 0 7 GIT_SHA1_SHORT)
#if (NOT GIT_TAG MATCHES "v[0-9]+.[0-9]+.[0-9]+")
#    set(GIT_TAG "DEV")
#endif()
#
## get date properties
#string(SUBSTRING ${GIT_DATE} 8 2 GIT_DATE_DAY)
#string(SUBSTRING ${GIT_DATE} 5 2 GIT_DATE_MONTH)
#string(SUBSTRING ${GIT_DATE} 0 4 GIT_DATE_YEAR)
#
#MESSAGE(STATUS "Git SHA1: ${GIT_SHA1}")
#MESSAGE(STATUS "Git SHA1 short: ${GIT_SHA1_SHORT}")
#MESSAGE(STATUS "Git tag: ${GIT_TAG}")
#MESSAGE(STATUS "Git date: ${GIT_DATE}")
#
##todo: confirm no file changes before adding commit hash to menu, perhaps put flag next to commit hash if there are changes
#
## determine version number from git tag
#string(REGEX REPLACE "v([0-9]+)\\.([0-9]+)\\.([0-9]+)" "\\1" VERSION_MAJOR ${GIT_TAG})
#string(REGEX REPLACE "v([0-9]+)\\.([0-9]+)\\.([0-9]+)" "\\2" VERSION_MINOR ${GIT_TAG})
#string(REGEX REPLACE "v([0-9]+)\\.([0-9]+)\\.([0-9]+)" "\\3" VERSION_PATCH ${GIT_TAG})
#
#MESSAGE(STATUS "Resolved version: ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
#
## add version info to build definitions. commit flags use hash to prevent intepretation as number with base
#target_compile_definitions(projectify PRIVATE -DBUILD_VERSION=${VERSION_MAJOR},${VERSION_MINOR},${VERSION_PATCH} -DBUILD_VERSION_STR="${GIT_TAG}" -DBUILD_VERSION_STR_LITERAL=${GIT_TAG} -DBUILD_COMMIT_SHORT_STR="${GIT_SHA1_SHORT}" -D
#        BUILD_COMMIT_SHORT_STR_LITERAL=${GIT_SHA1_SHORT} -DBUILD_COMMIT_STR="${GIT_SHA1}" -DBUILD_COMMIT_STR_LITERAL=${GIT_SHA1} -DBUILD_DATE_STR="${GIT_DATE}" -DBUILD_DATE_STR_LITERAL=${GIT_DATE} -DBUILD_DATE=${GIT_DATE_YEAR},${GIT_DATE_MO
#        NTH},${GIT_DATE_DATE})


# add LMASM executable
add_executable(LMASM ${ASM_SOURCES} ${COMMON_SOURCES})

# add regex library for windows
IF (WIN32)
    target_link_libraries(LMASM regex)
ENDIF()

# add LMVM executable
add_executable(LMVM ${VM_SOURCES} ${COMMON_SOURCES})

# use harsh flags
if(MSVC)
    MESSAGE(STATUS "MSVC is not a supported compiler and may fail!")
    target_compile_options(LMASM PRIVATE /W4 /WX)
    target_compile_options(LMVM PRIVATE /W4 /WX)
else()
    target_compile_options(LMASM PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(LMVM PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# make installer
include(scripts/make_installer.cmake)

# TODO: set VERSION in main based on git tag