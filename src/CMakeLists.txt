cmake_minimum_required (VERSION 3.15)

project(socket_test CXX)


add_subdirectory(socket)

link_libraries(socket_tool)

add_executable(${PROJECT_NAME}  "main.cpp" )

#target_link_libraries(${PROJECT_NAME} PUBLIC ws2_32)


#target_link_libraries(${PROJECT_NAME} PUBLIC sutils)




