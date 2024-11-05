<!--
 * @Author: yingjie_wang 2778809626@qq.com
 * @Date: 2024-06-28 17:49:50
 * @LastEditors: yingjie_wang 2778809626@qq.com
 * @LastEditTime: 2024-11-05 09:28:35
 * @FilePath: \undefinedd:\1_wangyingjie\learn\github_project\11_socket\socket_template\README.md
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
-->
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


使用 把core目录下的socket目录文件复制到你的项目里

CMakeLists.txt 文件中 添加 `add_subdirectory(socket)`
在socket目录中有根据windows/linux环境编译所需的文件

然后调用这个socket的程序中连接一下 `target_link_libraries(${PROJECT_NAME} PUBLIC socket_tool)`


# 使用vcpkg的库的尝试

1. Boost.Asio
    描述：Boost.Asio 是一个跨平台的 C++ 库，用于网络和低级 I/O 编程。它提供了异步和同步的 TCP 和 UDP 通信功能。
    vcpkg install boost-asio

2. Poco
    描述：Poco 是一个开源的 C++ 类库集合，提供了一系列基础和高级功能，包括网络编程（TCP/UDP）、线程、文件系统访问等。
    vcpkg install poco

3. libuv
    描述：libuv 是一个多平台的异步 I/O 库，最初为 Node.js 开发。它提供了高性能的异步 I/O 功能，包括 TCP 和 UDP
    vcpkg install libuv

4. zmq (ZeroMQ)
    描述：ZeroMQ 是一个高性能的异步消息库，适用于多种传输协议，包括 TCP 和 UDP。它提供了丰富的消息传递模式。
    vcpkg install zmq

5. cpp-netlib
    描述：cpp-netlib 是一个基于 Boost.Asio 的高级网络库，提供了 HTTP 客户端和服务器的实现，同时也支持 TCP 和 UDP。
    vcpkg install cpp-netlib

6. enet
    描述：ENet 是一个可靠的 UDP 传输库，设计用于游戏和其他实时应用。它提供了可靠性和带宽管理功能。
	vcpkg install enet






