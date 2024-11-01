

#ifdef _WIN32
#include "socket/win_socket/win_socket.h"
#elif defined(__linux__)
#include "socket/linux_socket/linux_socket.h"
#endif

#include <filesystem>
#include <iostream>

int main()
{
    std::string ip = "192.168.0.214";  // 服务器IP地址
    int port = 8080;                   // 服务器端口号


    //// tcp server test
    //Socket::tcpServer(port);
    //return 0;

     //if (Socket::tcpClientSync(ip, port))  // 同步
    Socket so;
    if (so.tcpClientAsyn(ip, port))  // 异步
    {
        std::cout << "TCP client executed successfully" << std::endl;
    }
    else
    {
        std::cout << "TCP client failed" << std::endl;
    }

    //// udp 发送
    //if (Socket::updSend(ip, port))  // 同步
    //{
    //    std::cout << "TCP client executed successfully" << std::endl;
    //}
    //else
    //{
    //    std::cout << "TCP client failed" << std::endl;
    //}

    //// udp 接收
    //if (Socket::receiveData(ip, port))  // 同步
    //{
    //    std::cout << "TCP client executed successfully" << std::endl;
    //}
    //else
    //{
    //    std::cout << "TCP client failed" << std::endl;
    //}




    return 0;
}
