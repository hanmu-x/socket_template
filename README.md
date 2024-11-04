# socket_template


socket 通信模板: TCP UDP

区分linux和windows平台的socket

windows: core/socket/win_socket
linux: core/socket/linux_socket

core/socket/CMakeLists.txt 会根据平台自动链接需要编译的文件

TCP:
	服务端
	客户端
		同步
		异步

UDP:
	发送数据
	接收数据


使用 把core目录下的socket文件复制到你的项目里

CMakeLists.txt 文件中 添加 `add_subdirectory(socket)`

然后调用这个socket的程序中连接一下 `target_link_libraries(${PROJECT_NAME} PUBLIC socket_tool)`


