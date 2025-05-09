cmake_minimum_required(VERSION 3.10.0)
project(thesis VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 23)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# TODO: add another build option to build tests

find_package(Catch2 REQUIRED)

add_executable(bench main.cpp benchmarks/benches.cpp)
target_include_directories(bench PUBLIC benchmarks/)
target_include_directories(bench PUBLIC hopscotch_shadow/)
target_include_directories(bench PUBLIC hopscotch_bitmaps/)

add_executable(test tests/test.cpp)
target_include_directories(test PUBLIC hopscotch_shadow/)
target_include_directories(test PUBLIC hopscotch_bitmaps/)
target_link_libraries(test PRIVATE Catch2::Catch2WithMain)
