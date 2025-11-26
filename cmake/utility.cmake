####################################################################################
#                                                                                  #
#  Copyright (c) 2014 - 2018 Axel Menzel <info@rttr.org>                           #
#                                                                                  #
#  This file is part of RTTR (Run Time Type Reflection)                            #
#  License: MIT License                                                            #
#                                                                                  #
#  Permission is hereby granted, free of charge, to any person obtaining           #
#  a copy of this software and associated documentation files (the "Software"),    #
#  to deal in the Software without restriction, including without limitation       #
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,        #
#  and/or sell copies of the Software, and to permit persons to whom the           #
#  Software is furnished to do so, subject to the following conditions:            #
#                                                                                  #
#  The above copyright notice and this permission notice shall be included in      #
#  all copies or substantial portions of the Software.                             #
#                                                                                  #
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      #
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        #
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     #
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          #
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   #
#  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   #
#  SOFTWARE.                                                                       #
#                                                                                  #
####################################################################################

####################################################################################
# Adds warnings compiler options to the target depending on the category
# target Target name
####################################################################################
function( set_compiler_warnings target)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(WARNINGS "-Werror"
                 "-Wall")
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(WARNINGS "-Werror"
                 "-Wall")
  elseif(MSVC)
    set(WARNINGS "/WX"
                 "/W4")
  endif()

  target_compile_options(${target} PRIVATE ${WARNINGS}) 
endfunction()

####################################################################################
# Get environment variable, define it as ENV_$var and make sure backslashes are converted to forward slashes
# _COMPILER_NAME
####################################################################################
macro(getenv_path VAR)
   set(ENV_${VAR} $ENV{${VAR}})
   # replace won't work if var is blank
   if (ENV_${VAR})
     string( REGEX REPLACE "\\\\" "/" ENV_${VAR} ${ENV_${VAR}} )
   endif ()
endmacro()

#收集目标头文件(在配置时)
function(collect_dependency_target_recursive_impl target result visited)
    list(APPEND ${visited} ${target})
    
    # 递归处理依赖
    get_target_property(link_libs ${target} INTERFACE_LINK_LIBRARIES)
    if(NOT link_libs)
        get_target_property(link_libs ${target} LINK_LIBRARIES)
    endif()
    
    foreach(lib ${link_libs})
        if(TARGET ${lib})
            if(NOT "${lib}" IN_LIST "${visited}")
                list(APPEND ${result} ${lib})
                collect_dependency_target_recursive_impl(${lib} ${result} "${visited}")
            endif()
        endif()
    endforeach()
    
    set(${result} "${${result}}" PARENT_SCOPE)
endfunction()

function(collect_dependency_target_recursive target result)
    set(visited "")
    collect_dependency_target_recursive_impl(${target} ${result} visited)
    list(REMOVE_DUPLICATES ${result})
    set(${result} "${${result}}" PARENT_SCOPE)
endfunction()

#收集目标头文件(在配置时)
function(collect_includes_recursive target result)

    collect_dependency_target_recursive(${target} OUT_LIBS)
    list(APPEND OUT_LIBS ${target})
    
    # 获取接口包含目录
    foreach(target_item ${OUT_LIBS})
        get_target_property(includes ${target_item} INCLUDE_DIRECTORIES)
        get_target_property(interface_includes ${target_item} INTERFACE_INCLUDE_DIRECTORIES)
        # 获取系统包含目录
        get_target_property(system_includes ${target_item} INTERFACE_SYSTEM_INCLUDE_DIRECTORIES)
        if(includes)
            list(APPEND ${result} ${includes})
        endif()
        if(interface_includes)
            list(APPEND ${result} ${interface_includes})
        endif()
        if(system_includes)
            list(APPEND ${result} ${system_includes})
        endif()
    endforeach()
    
    list(REMOVE_DUPLICATES ${result})
    set(${result} "${${result}}" PARENT_SCOPE)
endfunction()