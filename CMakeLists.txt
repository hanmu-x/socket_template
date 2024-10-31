cmake_minimum_required (VERSION 3.15)  #最低版本

project(video_pro CXX)  #项目的名称以及使用的编程语言

include(CheckIncludeFileCXX)  # ？CMake模块，用于检查C++头文件的包含情况
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)  #允许使用宽松循环结构
set(CMAKE_VERBOSE_MAKEFILE ON)  #设置了生成的Makefile文件中的详细输出，可以显示构建过程的详细信息
set(CMAKE_INCLUDE_CURRENT_DIR ON)  #指定了在当前目录中搜索头文件


# 指定 c++ 最高版本
check_include_file_cxx(any HAS_ANY)  #check_include_file_cxx命令检查是否包含头文件any，将结果保存在HAS_ANY变量中。
check_include_file_cxx(string_view HAS_STRING_VIEW)
check_include_file_cxx(coroutine HAS_COROUTINE)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  #关闭了C++编译器的扩展

# c++ 17 已经包含一些类型定义了,可能会导致重复定义
add_definitions("-D_HAS_STD_BYTE=0")
# 命令行解析库里面的std::max
add_definitions("-DNOMINMAX")
# 指定项目编码类型 unicode 不指定默认utf8 ???
add_definitions("-DUNICODE")
if (CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    set(IS_LINUX TRUE)
    set(PLATFORM_NAME "Linux")
    set(CMAKE_THREAD_LIBS_INIT "-lpthread")
    set(CMAKE_HAVE_THREADS_LIBRARY 1)
    set(CMAKE_USE_WIN32_THREADS_INIT 0)
    set(CMAKE_USE_PTHREADS_INIT 1)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17  -lodbc -lpthread -fPIC -L. /usr/local/ssl/lib64/libssl.a /usr/local/ssl/lib64/libcrypto.a")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17  -lodbc -lpthread -fPIC -Wl,-Bstatic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17  -lodbc -lpthread -fPIC ")
elseif (CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    set(PLATFORM_NAME "Windows")
    set(IS_WINDOWS TRUE)
    add_definitions("-DTFF_INLINE_SPECIFIER=inline")
    add_definitions("-DIS_WIN32")
    add_definitions("-DWIN32_LEAN_AND_MEAN")
    add_compile_options(/wd4819 /wd4005 /wd4834)

    if(MSVC)
        # Set cmake cxx flags.
        set(CMAKE_CXX_FLAGS_DEBUG "")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D_DEBUG /MDd /Zi /Ob0  /Od /RTC1 /Gy /EHsc")

        set(CMAKE_CXX_FLAGS_MINSIZEREL "")
        set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /MD /Zi /O1 /Ob2 /Oi /Os /D NDEBUG /GS- ")

        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "")
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MD /Zi /O2 /Ob1 /D NDEBUG ")

        set(CMAKE_CXX_FLAGS_RELEASE "")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD /Zi /O2 /Ob1 /D NDEBUG ")
    endif(MSVC)

endif ()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_NAME "x64")
    message(STATUS "Target is 64 bits")
else ()
    set(ARCH_NAME "x32")
    message(STATUS "Target is 32 bits")
endif ()

# 设置默认程序根目录,程序中所有路径均参照此路径,方便调试和发布
if(IS_DEBUG)
    add_definitions("-DDEFAULT_ROOT_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"")
else()
    # add_definitions("-DDEFAULT_ROOT_DIR=\"\"")
    add_definitions("-DDEFAULT_ROOT_DIR=\"./\"")
endif()

##########################################
# global define.
##########################################
set(DEFAULT_CONFIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Config")
add_definitions("-DDEFAULT_CONFIG_DIR=\"${DEFAULT_CONFIG_DIR}\"")


##########################################
# set build target path.
##########################################
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/Bin/${PLATFORM_NAME}/${ARCH_NAME}/${CMAKE_BUILD_TYPE}")
set(LIBRARY_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/Bin/${PLATFORM_NAME}/${ARCH_NAME}/${CMAKE_BUILD_TYPE}")

# 
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
# 
add_definitions("-DDEFAULT_DATA_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/data\"")
message(STATUS "${DEFAULT_DATA_DIR}")

#set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
#set(SU_AS_SUB_PROJECT TRUE)
#include(silly_utils)
# 头文件
#include_directories("silly_utils/su")

# 添加core目录下的头文件到项目
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/core)

add_subdirectory(src)
add_subdirectory(core)