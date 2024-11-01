cmake_minimum_required (VERSION 3.15)

project(socket_tool CXX)

# 检测平台
if(WIN32)
    # Windows 平台
    set(SOCKET_DIR "win_socket")
elseif(UNIX)
    # Linux 平台
    set(SOCKET_DIR "linux_socket")
endif()


foreach(dirname ${CMAKE_CURRENT_SOURCE_DIR})
    file(GLOB_RECURSE SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
            "${SOCKET_DIR}/*.h"
            "${SOCKET_DIR}/*.hpp"
            "${SOCKET_DIR}/*.c"
            "${SOCKET_DIR}/*.cpp"
            )
endforeach()

add_library(${PROJECT_NAME} STATIC ${SRC})

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC ws2_32)
endif()


#find_package(asio CONFIG REQUIRED)
#target_link_libraries(${PROJECT_NAME} STATIC asio::asio)