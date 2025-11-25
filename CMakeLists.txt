cmake_minimum_required(VERSION 3.18)

project(meta-parser)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG 	${CMAKE_CURRENT_SOURCE_DIR}/bin/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE 	${CMAKE_CURRENT_SOURCE_DIR}/bin/release)

option(META_PARSER_SANDBOX "build sandbox" OFF)

if (PROJECT_IS_TOP_LEVEL)
    set(META_PARSER_SANDBOX ON)
endif()

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_LIST_DIR}/cmake")

include(utility)
include(config)
include(MetaParser)

add_subdirectory(3rd)
add_subdirectory(src)

if(META_PARSER_SANDBOX)
    add_subdirectory(sandbox)
endif()
