include(CMakeParseArguments)

function(meta_parser_build)
    set(ONE_VALUE_ARGS
        PARSER_EXECUTABLE
        TNAMESPACE_PREFIX
        TARGET
        SOURCE_ROOT
        GENERATED_DIR
        TEMPLATE_DIR
        VERBOSE
    )

    set(MULTI_VALUE_ARGS 
        SOURCE_HEADERS
        DEFINES
        INCLUDES
    )

    cmake_parse_arguments(BUILD_META "" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})

    if (IS_ABSOLUTE ${BUILD_META_GENERATED_DIR})
        message(FATAL_ERROR "生成目录必须相对与源文件路径 GENERATED_DIR: ${BUILD_META_GENERATED_DIR}")
    endif ()
    
    set(GENERATE_ENTRY_FILE "${CMAKE_CURRENT_BINARY_DIR}/generated/Module.${BUILD_META_TARGET}.entry.h")

    set(HEADERS_FILE "${CMAKE_CURRENT_BINARY_DIR}/generated/Module.${BUILD_META_TARGET}.header_list.txt")
    file(GENERATE OUTPUT "${HEADERS_FILE}"
        CONTENT "$<JOIN:${BUILD_META_SOURCE_HEADERS},\n>"
    )

    # 处理全局头文件路径和传入参数的头文件路径
    set(RAW_INCLUDES ${GLOBAL_META_INCLUDES} ${BUILD_META_INCLUDES})
    set(RAW_INCLUDES_CONTENT "")
    # 构建固定路径的字符串
    if(RAW_INCLUDES)
        foreach(include ${RAW_INCLUDES})
            string(APPEND RAW_INCLUDES_CONTENT "${include}\n")
        endforeach()
    endif()

    set(INCLUDES_FILE "${CMAKE_CURRENT_BINARY_DIR}/generated/Module.${BUILD_META_TARGET}.Includes.txt")

    # 获取目标依赖库的头文件路径 + 自身定义的头文件路径
    set(include_genex "$<TARGET_PROPERTY:${BUILD_META_TARGET},INCLUDE_DIRECTORIES>")
    file(GENERATE OUTPUT "${INCLUDES_FILE}"
        CONTENT "${RAW_INCLUDES_CONTENT}$<JOIN:${include_genex},\n>"
    )

    set(DEFINES ${GLOBAL_META_DEFINES} ${BUILD_META_DEFINES})

    string(REPLACE " " "" DEFINES_TRIMMED "${DEFINES}")

    if ("${DEFINES_TRIMMED}" STREQUAL "")
        set(DEFINES_SWITCH )
    else ()
        set(DEFINES_SWITCH --defines "${DEFINES}")
    endif ()

    # empty source files need to include the precompiled header
    if (NOT "${BUILD_META_PCH_NAME}" STREQUAL "")
        set(EMPTY_SOURCE_CONTENTS "#include \"${BUILD_META_PCH_NAME}.h\"")
        set(PCH_SWITCH --pch \"${BUILD_META_PCH_NAME}.h\")
    else ()
        set(EMPTY_SOURCE_CONTENTS "")
        set(PCH_SWITCH )
    endif ()
    get_target_property(TARGET_FOLDER ${BUILD_META_TARGET} FOLDER)

    set(OPTIONS_VERBOSE "")
    if(BUILD_META_VERBOSE)
        set(OPTIONS_VERBOSE "--verbose")
    endif()

    # add the command that generates the header and source files
    set(TARGET_NAME "${BUILD_META_TARGET}_Pregenerate")
    add_custom_target(${TARGET_NAME}
        DEPENDS ${INCLUDES_FILE}
        COMMAND ${BUILD_META_PARSER_EXECUTABLE}
        --target-name "${BUILD_META_TARGET}"
        --remove-namespace-prefix "${BUILD_META_TNAMESPACE_PREFIX}"
        --source-root "${BUILD_META_SOURCE_ROOT}"
        --output-dir "${BUILD_META_GENERATED_DIR}"
        --template-dir "${BUILD_META_TEMPLATE_DIR}"
        --header-list-file "${HEADERS_FILE}"
        --generate-entry-file "${GENERATE_ENTRY_FILE}"
        --includes-file "${INCLUDES_FILE}"
        ${OPTIONS_VERBOSE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    if(TARGET_FOLDER)
        set_target_properties(${TARGET_NAME} PROPERTIES 
            FOLDER ${TARGET_FOLDER})
    endif()
    add_dependencies(${BUILD_META_TARGET} ${TARGET_NAME})
endfunction ()