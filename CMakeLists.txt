set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

cmake_minimum_required(VERSION 3.0)     # minimum version of CMake

set(CMAKE_CXX_STANDARD 20)
find_package(LLVM REQUIRED CONFIG)

project(LoggerCC)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in ${LLVM_DIR}")

add_definitions(${LLVM_DEFINITIONS})    
include_directories(${LLVM_INCLUDE_DIRS}) 

add_subdirectory(DFAPass)
add_subdirectory(DotPatcher)
