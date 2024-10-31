
#include "socket/socket.h"

#include <filesystem>
#include <iostream>

int main()
{
    // tcp server 
    Socket::tcpServer(8081);

    return 0;

    std::string ip = "192.168.0.214";  // 服务器IP地址
    int port = 8080;                   // 服务器端口号

    // if (Socket::tcpClientSync(ip, port))  // 同步
    Socket so;
    if (so.tcpClientAsyn(ip, port))  // 异步
    {
        std::cout << "TCP client executed successfully" << std::endl;
    }
    else
    {
        std::cout << "TCP client failed" << std::endl;
    }

    // if (Socket::updClientSync(ip, port))  // 同步
    //{
    //     std::cout << "TCP client executed successfully" << std::endl;
    // }
    // else
    //{
    //     std::cout << "TCP client failed" << std::endl;
    // }

    return 0;
}
