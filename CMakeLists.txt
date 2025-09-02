cmake_minimum_required(VERSION 3.18)

project(rtti-meta-parser)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG 	${CMAKE_CURRENT_SOURCE_DIR}/bin/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE 	${CMAKE_CURRENT_SOURCE_DIR}/bin/release)

add_subdirectory(3rd)
add_subdirectory(src)

if(META_PARSER_SANDBOX)
    add_subdirectory(sandbox)
endif()
