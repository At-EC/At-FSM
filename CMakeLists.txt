cmake_minimum_required(VERSION 3.20)

add_library(fsm_kernel STATIC)

set(KERNEL_PATH ${CMAKE_CURRENT_LIST_DIR})

include_directories(${CMAKE_CURRENT_LIST_DIR})

target_sources(fsm_kernel
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/build_version.h
)

include(${CMAKE_CURRENT_LIST_DIR}/include/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/source/CMakeLists.txt)


