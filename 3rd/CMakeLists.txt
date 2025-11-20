
set(METAPARSER_3RD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(LLVM_DIR "${METAPARSER_3RD_PARTY_DIR}/LLVM/cmake" PARENT_SCOPE)
set(mustache_DIR "${METAPARSER_3RD_PARTY_DIR}/mustache/cmake" PARENT_SCOPE)

add_subdirectory(argparse-3.2)

if(NOT TARGET json11)
    set(JSON11_DIR ${METAPARSER_3RD_PARTY_DIR}/json11)
    add_library(json11 ${JSON11_DIR}/json11.cpp)
    target_include_directories(json11 PUBLIC ${JSON11_DIR})
    set_target_properties(json11 PROPERTIES
        FOLDER "Rttr/3rdParty/json11"
        )
endif()
