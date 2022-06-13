cmake_minimum_required(VERSION 3.10)
project(dynamic_connectivity)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pthread")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(BST src/bst/bst_interface.h
        src/bst/cartesian_bst.h)

set(FOREST src/forest/simple_forest.cpp)

add_library(bst ${BST})
add_library(forest ${FOREST})

set_target_properties(bst PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(forest PROPERTIES LINKER_LANGUAGE CXX)

add_executable(run_bst_tests tests/bst/bst_test.cpp)
add_executable(run_forest_tests tests/forest/forest_test.cpp)
