
#include "linux_socket.h"

#include <cstring>
#include <thread>
#include <chrono>

#define MAX_BUFFER_SIZE 1024
#define RECONNECT_TIME 1000
#define RE_READ_TIME 10




// 向客户端发送数据
void SendDataToClient(int clientSocket)
{
    // char message[] = "Hello from the server!";
    // send(clientSocket, message, static_cast<int>(strlen(message)), 0);
    // std::cout << "Sent data to client: " << message << std::endl;
    while (true)
    {
        send(clientSocket, ".", static_cast<int>(strlen(".")), 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(RECONNECT_TIME));
    }
}

void HandleClientConnection(int clientSocket)
{
    // 处理客户端连接的代码
    // 例如，接收数据、发送响应等
    char buffer[1024];
    ssize_t bytesReceived;

    while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0)) > 0)
    {
        buffer[bytesReceived] = '\0';  // 确保字符串以 NULL 结尾
        std::cout << "Received: " << buffer << std::endl;

        // 发送回客户端
        send(clientSocket, buffer, bytesReceived, 0);
    }

    close(clientSocket);  // 关闭客户端套接字
}

bool Socket::tcpServer(int port)
{
    // 创建监听套接字
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0)
    {
        std::cerr << "Error at socket(): " << strerror(errno) << std::endl;
        return false;
    }

    sockaddr_in service;                   // 套接字地址
    service.sin_family = AF_INET;          // IPv4 地址族
    service.sin_addr.s_addr = INADDR_ANY;  // 接受任意客户端的连接
    service.sin_port = htons(port);        // 转换为网络字节序

    // 绑定套接字到指定的地址和端口
    if (bind(listenSocket, (struct sockaddr*)&service, sizeof(service)) < 0)
    {
        std::cerr << "bind() failed: " << strerror(errno) << std::endl;
        close(listenSocket);
        return false;
    }

    // 开始监听
    if (listen(listenSocket, SOMAXCONN) < 0)
    {
        std::cerr << "listen() failed: " << strerror(errno) << std::endl;
        close(listenSocket);
        return false;
    }

    std::cout << "Server listening on port " << port << std::endl;

    while (true)
    {  // 持续等待客户端连接
        sockaddr_in clientAddr;
        socklen_t addrLen = sizeof(clientAddr);

        // 接受连接请求
        int clientSocket = accept(listenSocket, (struct sockaddr*)&clientAddr, &addrLen);
        if (clientSocket < 0)
        {
            std::cerr << "accept() failed: " << strerror(errno) << std::endl;
            continue;  // 继续等待新的连接
        }

        std::cout << "Accepted connection from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << std::endl;

        // 创建线程处理客户端连接
        std::thread clientThread1(HandleClientConnection, clientSocket);
        std::thread clientThread2(SendDataToClient, clientSocket);

        clientThread1.detach();  // 分离线程，允许其独立运行
        clientThread2.detach();  // 分离线程，允许其独立运行
    }

    // 关闭监听套接字
    close(listenSocket);
    return true;
}







