cmake_minimum_required(VERSION 3.10)
project(dynamic_connectivity)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pthread")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(BST src/bst/bst_interface.h)

add_library(bst ${BST})

set_target_properties(bst PROPERTIES LINKER_LANGUAGE CXX)
