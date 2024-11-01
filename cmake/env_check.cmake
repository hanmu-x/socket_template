cmake_minimum_required(VERSION 3.15)

if(CMP0074)
  cmake_policy(SET CMP0074 NEW)
endif()
if(CMP0167)
  cmake_policy(SET CMP0167 NEW)
  set(NEW_BOOST_FIND ON)
endif()

include(CheckIncludeFileCXX)

set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
  set(CMAKE_BUILD_TYPE "Debug")
  message("AUTO SET CMAKE_BUILD_TYPE : Debug")
endif()

string(TOUPPER "${CMAKE_BUILD_TYPE}" UPPER_CMAKE_BUILD_TYPE)
if("DEBUG" STREQUAL "${UPPER_CMAKE_BUILD_TYPE}")
set(DEBUG_MODE 1)
  message(STATUS "Debug 模式")
endif()

# 系统信息
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
  message(STATUS "This is Linux")
  set(IS_LINUX TRUE)
  set(PLATFORM_NAME "Linux")
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Android")
  message(STATUS "This is Android")
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
  message(STATUS "This is Windows")
  set(PLATFORM_NAME "Windows")
  set(IS_WINDOWS TRUE)
endif()

if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "aarch64")
  message(STATUS "this is aarch64 cpu")
elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86_64")
  message(STATUS "this is x86_64 cpu")
endif()

# 检查系统位数
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(ARCH_NAME "x64")
  message(STATUS "Target is 64 bits")
else()
  set(ARCH_NAME "x32")
  message(STATUS "Target is 32 bits")
  message(FATAL_ERROR "Not support on 32 bit platform")
endif()

# message("当前C++编译器支持的C++ 特性:")
foreach(feature ${CMAKE_CXX_COMPILE_FEATURES})
  if("${feature}" STREQUAL "cxx_std_11")
    add_definitions("-DSU_SUPPORT_CXX11=1")
    set(SU_SUPPORT_CXX11 TRUE)
  endif()
  if("${feature}" STREQUAL "cxx_std_17")
    add_definitions("-DSU_SUPPORT_CXX17=1")
    set(SU_SUPPORT_CXX17 TRUE)
  endif()
  if("${feature}" STREQUAL "cxx_std_20")
    # add_definitions("-DSU_SUPPORT_CXX20=1") set(SU_SUPPORT_CXX20 TRUE)
  endif()

endforeach()

# c++ 支持版本
if(SU_SUPPORT_CXX20)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  # c++ 17 已经包含一些类型定义了,可能会导致重复定义
  add_definitions("-D_HAS_STD_BYTE=0")
  message("使用C++20标准")
elseif(SU_SUPPORT_CXX17)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  # c++ 17 已经包含一些类型定义了,可能会导致重复定义
  add_definitions("-D_HAS_STD_BYTE=0")
  message("使用C++17标准")
elseif(SU_SUPPORT_CXX11)
  set(CMAKE_CXX_STANDARD 11)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  message("使用C++11标准")
endif()

add_definitions("-DENABLE_SILLY_LOG")
# 命令行解析库里面的std::max
add_definitions("-DNOMINMAX")
# 指定项目编码类型 unicode 不指定默认utf8 ???
add_definitions("-DUNICODE")
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")

  set(CMAKE_THREAD_LIBS_INIT "-lpthread")
  set(CMAKE_HAVE_THREADS_LIBRARY 1)
  set(CMAKE_USE_WIN32_THREADS_INIT 0)
  set(CMAKE_USE_PTHREADS_INIT 1)
  set(THREADS_PREFER_PTHREAD_FLAG ON)

  set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} -std=c++17 -lpthread -fPIC -Wall -fopenmp -Wno-unused-function -Wno-unused-variable -lodbc"
  )
  set(STATIC_LIB_SUFFIX "a")
  set(DYNAMIC_LIB_SUFFIX "so")
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")

  add_definitions("-DIS_WIN32")
  add_definitions("-DWIN32_LEAN_AND_MEAN")
  add_compile_options(/wd4819 /wd4005 /wd4834 /wd4996 /utf-8 /openmp)
  set(STATIC_LIB_SUFFIX "lib")
  set(DYNAMIC_LIB_SUFFIX "dll")

  if(MSVC)
    # SET cmake cxx flags.
    set(CMAKE_CXX_FLAGS_DEBUG "")
    set(CMAKE_CXX_FLAGS_DEBUG
        "${CMAKE_CXX_FLAGS_DEBUG} /D_DEBUG /MDd /Zi /Ob0  /Od /RTC1 /Gy /EHsc")

    set(CMAKE_CXX_FLAGS_MINSIZEREL "")
    set(CMAKE_CXX_FLAGS_MINSIZEREL
        "${CMAKE_CXX_FLAGS_MINSIZEREL} /MD /Zi /O1 /Ob2 /Oi /Os /D NDEBUG /GS- "
    )

    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO
        "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MD /Zi /O2 /Ob1 /D NDEBUG ")

    set(CMAKE_CXX_FLAGS_RELEASE "")
    set(CMAKE_CXX_FLAGS_RELEASE
        "${CMAKE_CXX_FLAGS_RELEASE} /MD /Zi /O2 /Ob1 /D NDEBUG ")
  endif(MSVC)

endif()

if (${CMAKE_VERSION} VERSION_LESS 3.30)
  find_package(Boost COMPONENTS program_options date_time)
else ()
  find_package(Boost COMPONENTS program_options date_time)
endif ()

if(Boost_FOUND)
  # 如果找到了 Boost 库
  include_directories(${Boost_INCLUDE_DIRS})
  add_definitions("-DSILLY_UTILS_BOOST_ENABLE")
  message("Found boost  ${Boost_INCLUDE_DIRS}")
else()
  message(WARNING "Boost library not found. Some features might be disabled.")
endif()

# ##############################################################################
# 设置全局的可执行程序和链接库的生成路径.
# ##############################################################################
set(EXECUTABLE_OUTPUT_PATH
    "${CMAKE_SOURCE_DIR}/Bin/${PLATFORM_NAME}/${ARCH_NAME}/${CMAKE_BUILD_TYPE}")
set(LIBRARY_OUTPUT_PATH "${EXECUTABLE_OUTPUT_PATH}")
