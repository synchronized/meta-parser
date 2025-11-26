#cmake_policy(SET CMP0074 OLD)


set(TARGET_NAME MetaParser)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(LLVM REQUIRED)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADERS} ${SOURCES})

#if (MSVC)
#    set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} /O2 /Ob2")
#else()
#    set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O3")
#endif()

file(GLOB_RECURSE HEADERS "parser/*.h" "parser/*.hpp")
file(GLOB_RECURSE SOURCES "parser/*.cpp")

# header files are superflous, but some IDEs (Visual Studio) don't include
# them in the solution explorer without them being added to the list of sources
add_executable(${TARGET_NAME} ${HEADERS} ${SOURCES})

# add LLVM includes
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/parser)

target_link_libraries(${TARGET_NAME} PRIVATE LLVM::clang)
target_link_libraries(${TARGET_NAME} PRIVATE mustache::mustache)
target_link_libraries(${TARGET_NAME} PRIVATE argparse::argparse)
set_target_properties(${TARGET_NAME} PROPERTIES 
    CXX_STANDARD ${MAX_CXX_STANDARD}
    DEBUG_POSTFIX ${CPP_REFLECTION_DEBUG_POSTFIX}
    FOLDER "Core"
    )


get_target_property(LLVM_SHARED_LIBRARY_DIR LLVM::clang IMPORTED_LOCATION)
get_filename_component(LLVM_SHARED_LIBRARY_DIR "${LLVM_SHARED_LIBRARY_DIR}" PATH)
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
	# mustache templates directory
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${LLVM_SHARED_LIBRARY_DIR}"
		$<TARGET_FILE_DIR:${TARGET_NAME}>
)

