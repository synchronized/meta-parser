
file(GLOB_RECURSE CUSTOM_PROJECT_RUNTIME_HEADS "runtime/*.h")

# 生成代码
set(PARSER_PROGRAM $<TARGET_FILE:MetaParser>)

set(GENCODE_OUTPUT_DIR "_generated")
set(GENCODE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(GENCODE_TEMPLATE ${GENCODE_ROOT}/template)


set(SANDBOX_TARGET sandbox)
file(GLOB_RECURSE SOURCES "*.cpp")

# header files are superflous, but some IDEs (Visual Studio) don't include
# them in the solution explorer without them being added to the list of sources
add_executable(${SANDBOX_TARGET} ${SOURCES})
set_target_properties(${SANDBOX_TARGET} PROPERTIES CXX_STANDARD 20)
target_include_directories(${SANDBOX_TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${SANDBOX_TARGET} PUBLIC json11)
set_target_properties(${SANDBOX_TARGET} PROPERTIES 
	CXX_STANDARD ${MAX_CXX_STANDARD}
    DEBUG_POSTFIX ${CPP_REFLECTION_DEBUG_POSTFIX}
    FOLDER "Example"
    )

meta_parser_build(
    PARSER_EXECUTABLE ${PARSER_PROGRAM}
    TNAMESPACE_PREFIX sandbox
    TARGET ${SANDBOX_TARGET}
    SOURCE_ROOT ${GENCODE_ROOT}
    SOURCE_HEADERS ${CUSTOM_PROJECT_RUNTIME_HEADS}
    GENERATED_DIR "_generated"
    TEMPLATE_DIR ${GENCODE_TEMPLATE}
    )
