set(third_party_folder "ThirdParty")
set(THIRD_PARTY_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

if(NOT TARGET json11)
    set(JSON11_DIR ${THIRD_PARTY_ROOT}/json11)
    add_library(json11 ${JSON11_DIR}/json11.cpp)
    target_include_directories(json11 PUBLIC ${JSON11_DIR})
    set_target_properties(json11 PROPERTIES
        FOLDER "${third_party_folder}/json11"
        )
endif()

# Find Mustache 
if(NOT TARGET mustache)
    set(MUSTACHE_DIR ${THIRD_PARTY_ROOT}/mustache)
    add_library(mustache INTERFACE)
    add_library(mustache::mustache ALIAS mustache)
    target_include_directories(mustache INTERFACE ${MUSTACHE_DIR}/include)
    set_target_properties(mustache PROPERTIES
        FOLDER "${third_party_folder}/mustache"
        )
endif()

if(NOT TARGET argparse::argparse)
    add_subdirectory(argparse-3.2)
    set_target_properties(argparse PROPERTIES 
        FOLDER "${third_party_folder}/argparse"
    )
endif()


set(LLVM_DIR "${THIRD_PARTY_ROOT}/LLVM-21.1.6/cmake" PARENT_SCOPE)
