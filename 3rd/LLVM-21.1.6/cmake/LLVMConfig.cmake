# LLVMConfig.cmake
get_filename_component(LLVM_ROOT "${CMAKE_CURRENT_LIST_DIR}" PATH)

# 版本信息
set(LLVM_VERSION 21.1.6)
set(LLVM_VERSION_MAJOR 21)

# 平台特定配置
if(WIN32)
    set(LLVM_LIB_DIR "${LLVM_ROOT}/lib/x64")
    set(LLVM_BIN_DIR "${LLVM_ROOT}/bin/x64")
    
    # Clang
    add_library(LLVM::clang SHARED IMPORTED)
    set_target_properties(LLVM::clang PROPERTIES
      IMPORTED_IMPLIB "${LLVM_LIB_DIR}/libclang.lib"
      IMPORTED_LOCATION "${LLVM_BIN_DIR}/libclang.dll"
      INTERFACE_INCLUDE_DIRECTORIES "${LLVM_ROOT}/include"
    )
    
elseif(APPLE)
    set(LLVM_LIB_DIR "${LLVM_ROOT}/lib/macOS")
    set(LLVM_BIN_DIR "${LLVM_ROOT}/bin/macOS")
    
    add_library(LLVM::clang SHARED IMPORTED)
    set_target_properties(LLVM::clang PROPERTIES
      IMPORTED_LOCATION "${LLVM_BIN_DIR}/libclang.dylib"
      INTERFACE_INCLUDE_DIRECTORIES "${LLVM_ROOT}/include"
    )
    
else()  # Linux
    set(LLVM_LIB_DIR "${LLVM_ROOT}/lib/Linux")
    set(LLVM_BIN_DIR "${LLVM_ROOT}/bin/Linux")
    
    add_library(LLVM::clang SHARED IMPORTED)
    set_target_properties(LLVM::clang PROPERTIES
      IMPORTED_LOCATION "${LLVM_BIN_DIR}/libclang.so.${LLVM_VERSION_MAJOR}"
      INTERFACE_INCLUDE_DIRECTORIES "${LLVM_ROOT}/include"
    )
endif()

# 可选：添加其他LLVM组件
# add_library(LLVM::LLVM SHARED IMPORTED)...