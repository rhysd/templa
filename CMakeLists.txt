project(templa)

set(TEMPLA_VERSION_MAJOR 0)
set(TEMPLA_VERSION_MINOR 0)
set(TEMPLA_VERSION_TEENY 1)
set(PACKAGE_VERSION "${TEMPLA_VERSION_MAJOR}.${TEMPLA_VERSION_MINOR}.${TEMPLA_VERSION_TEENY}")

cmake_minimum_required( VERSION 2.8 )

if(EXISTS "/usr/local/bin/clang++-3.5")
    set(CMAKE_CXX_COMPILER "/usr/local/bin/clang++-3.5")
else()
    set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
endif()

include_directories( "${PROJECT_SOURCE_DIR}/src" )

add_definitions(-std=c++1y -g -Wall -Wextra)

find_package(Boost 1.55.0)
include_directories(${Boost_INCLUDE_DIRS})

file(GLOB_RECURSE CPPFILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(templa ${CPPFILES})

install(TARGETS templa DESTINATION bin)
