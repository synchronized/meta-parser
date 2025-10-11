cmake_minimum_required(VERSION 3.18)

project(meta-parser)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG 	${CMAKE_CURRENT_SOURCE_DIR}/bin/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE 	${CMAKE_CURRENT_SOURCE_DIR}/bin/release)

option(META_PARSER_SANDBOX "build sandbox" OFF)

if (PROJECT_IS_TOP_LEVEL)
    set(META_PARSER_SANDBOX ON)
endif()

add_subdirectory(3rd)
add_subdirectory(src)

if(META_PARSER_SANDBOX)
    add_subdirectory(sandbox)
endif()
