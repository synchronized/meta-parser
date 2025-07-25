
file(GLOB_RECURSE CUSTOM_PROJECT_RUNTIME_HEADS "runtime/*.h" "runtime/*.hpp")

# 生成代码
set(CODEGEN_TARGET "PreCompile")
set(PARSER_PROGRAM $<TARGET_FILE:MetaParser>)
set(PRECOMPILE_TOOLS_PATH "${PROJECT_SOURCE_DIR}/bin")
set(PRECOMPILE_FILE_IN_PATH "${CMAKE_CURRENT_SOURCE_DIR}/precompile/precompile.json.in")
set(PRECOMPILE_FILE_PATH "${PRECOMPILE_TOOLS_PATH}/precompile.json")
configure_file(${PRECOMPILE_FILE_IN_PATH} ${PRECOMPILE_FILE_PATH})
message("PRECOMPILE_FILE_PATH: ${PRECOMPILE_FILE_PATH}")

set(sys_include "*")
set(GENCODE_OUTPUT ${CMAKE_BINARY_DIR}/parser_header.h)
set(GENCODE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(GENCODE_TEMPLATE ${GENCODE_ROOT}/template)
# Called first time when building target 
add_custom_target(${CODEGEN_TARGET} ALL
# COMMAND # (DEBUG: DON'T USE )
#     this will make configure_file() is called on each compile
#   ${CMAKE_COMMAND} -E touch ${PRECOMPILE_PARAM_IN_PATH}a
# If more than one COMMAND is specified they will be executed in order...
COMMAND
  ${CMAKE_COMMAND} -E echo "************************************************************* "
COMMAND
  ${CMAKE_COMMAND} -E echo "**** [Precompile] BEGIN "
COMMAND
  ${CMAKE_COMMAND} -E echo "************************************************************* "

COMMAND
  ${CMAKE_COMMAND} -E echo ${PARSER_PROGRAM} "--project_file_name" "${PRECOMPILE_FILE_PATH}"  "--output_file_name" "${GENCODE_OUTPUT}"  "--project_root" "${GENCODE_ROOT}" "--template_root" "${GENCODE_TEMPLATE}" "--sys_include" "${sys_include}" "--module_name" "Sandbox" "--show_errors" "false" 
COMMAND
    ${PARSER_PROGRAM} "--project_file_name" "${PRECOMPILE_FILE_PATH}"  "--output_file_name" "${GENCODE_OUTPUT}"  "--project_root" "${GENCODE_ROOT}" "--template_root" "${GENCODE_TEMPLATE}" "--sys_include" "${sys_include}" "--module_name" "Sandbox" "--show_errors" "false" 
### BUILDING ====================================================================================
COMMAND
    ${CMAKE_COMMAND} -E echo "+++ Precompile finished +++"
)


set(SANDBOX_TARGET sandbox)
file(GLOB_RECURSE SOURCES "*.cpp")

# header files are superflous, but some IDEs (Visual Studio) don't include
# them in the solution explorer without them being added to the list of sources
add_executable(${SANDBOX_TARGET} ${SOURCES})
target_include_directories(${SANDBOX_TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


add_dependencies(sandbox "${CODEGEN_TARGET}")
add_dependencies("${CODEGEN_TARGET}" "MetaParser")


