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





