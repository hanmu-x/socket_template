cmake_minimum_required (VERSION 3.15)

project(socket_test CXX)


add_executable(${PROJECT_NAME}  "main.cpp" )
target_link_libraries(${PROJECT_NAME} PUBLIC socket_tool)






