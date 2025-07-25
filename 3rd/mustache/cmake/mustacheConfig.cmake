
get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_DIR}" PATH)

add_library(mustache INTERFACE IMPORTED)

set_target_properties(mustache PROPERTIES
  INTERFACE_COMPILE_FEATURES "cxx_std_11"
  INTERFACE_COMPILE_OPTIONS "\$<\$<AND:\$<COMPILE_LANGUAGE:CXX>,\$<CXX_COMPILER_ID:MSVC>>:/utf-8>"
  INTERFACE_INCLUDE_DIRECTORIES "${_IMPORT_PREFIX}/include"
)

add_library(mustache::mustache ALIAS mustache)

set(_IMPORT_PREFIX)

