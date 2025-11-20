cmake_minimum_required(VERSION 3.12.4)

if(NOT DEFINED PROJECT_NAME)
  set(ARGPARSE_IS_TOP_LEVEL ON)
else()
  set(ARGPARSE_IS_TOP_LEVEL OFF)
endif()

project(argparse
        VERSION 3.2.0 
        DESCRIPTION "A single header argument parser for C++17"
        HOMEPAGE_URL "https://github.com/p-ranav/argparse"
        LANGUAGES CXX
)

option(ARGPARSE_INSTALL "Include an install target" ${ARGPARSE_IS_TOP_LEVEL})
option(ARGPARSE_BUILD_TESTS "Build tests" ${ARGPARSE_IS_TOP_LEVEL})
option(ARGPARSE_BUILD_SAMPLES "Build samples" OFF)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(argparse INTERFACE)
add_library(argparse::argparse ALIAS argparse)

target_compile_features(argparse INTERFACE cxx_std_17)
target_include_directories(argparse INTERFACE
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)
