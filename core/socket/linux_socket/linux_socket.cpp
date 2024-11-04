
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

bool Socket::tcpServer(const int& port)
{
    // 创建监听套接字
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0)
    {
        std::cout << "Error at socket(): " << strerror(errno) << std::endl;
        return false;
    }

    sockaddr_in service;                   // 套接字地址
    service.sin_family = AF_INET;          // IPv4 地址族
    service.sin_addr.s_addr = INADDR_ANY;  // 接受任意客户端的连接
    service.sin_port = htons(port);        // 转换为网络字节序

    // 绑定套接字到指定的地址和端口
    if (bind(listenSocket, (struct sockaddr*)&service, sizeof(service)) < 0)
    {
        std::cout << "bind() failed: " << strerror(errno) << std::endl;
        close(listenSocket);
        return false;
    }

    // 开始监听
    if (listen(listenSocket, SOMAXCONN) < 0)
    {
        std::cout << "listen() failed: " << strerror(errno) << std::endl;
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
            std::cout << "accept() failed: " << strerror(errno) << std::endl;
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


bool Socket::tcpClientSync(const std::string& ip, const int& port)
{
    bool status = false;

    while (true)
    {
        // 创建套接字
        int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket < 0)
        {
            std::cout << "Failed to create socket" << std::endl;
            return false;
        }

        // 设置服务器信息
        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        if (inet_pton(AF_INET, ip.c_str(), &(serverAddress.sin_addr)) <= 0)
        {
            std::cout << "Invalid address/Address not supported" << std::endl;
            close(clientSocket);
            return false;
        }

        while (true)
        {
            // 连接到服务器
            int is_con = connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
            if (is_con < 0)
            {
                std::cout << "Connection failed" << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(RECONNECT_TIME));  // 等待重新连接
                continue;
            }

            while (true)
            {
                char buffer[1024];
                memset(buffer, 0, sizeof(buffer));

                // 接收数据 (都是 <0 出错 =0 连接关闭 >0 接收到数据大小)
                ssize_t is_rec = recv(clientSocket, buffer, sizeof(buffer), 0);
                if (is_rec == 0)
                {
                    std::cout << "Server disconnection and reconnection " << std::endl;
                    break;
                }
                else if (is_rec < 0)
                {
                    std::cout << "Failed to receive data" << std::endl;
                }
                else
                {
                    // 输出接收到的数据
                    std::cout << "TCP client sync received data from server: " << buffer << std::endl;
                }
                std::cout << "---------- get once data -----------" << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(RE_READ_TIME));  // 等待重新读取
            }

            // 跳出循环关闭 socket 然后从新建立 socket 重新连接
            close(clientSocket);
            std::this_thread::sleep_for(std::chrono::milliseconds(RECONNECT_TIME));  // 等待重新连接
            break;
        }
    }

    return true;
}

bool Socket::tcpClientAsyn(const std::string& ip, const int& port)
{
    while (true)
    {
        // 创建套接字
        int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket < 0)
        {
            std::cout << "Failed to create socket" << std::endl;
            return false;
        }

        // 设置服务器信息
        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        if (inet_pton(AF_INET, ip.c_str(), &(serverAddress.sin_addr)) <= 0)
        {
            std::cout << "Invalid address/Address not supported" << std::endl;
            close(clientSocket);
            return false;
        }

        // 连接到服务器
        if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
        {
            std::cout << "Connection failed" << std::endl;
            close(clientSocket);
            std::this_thread::sleep_for(std::chrono::milliseconds(RECONNECT_TIME));
            continue;
        }

        fd_set readfds;
        while (true)
        {
            // 清空并设置文件描述符集合
            FD_ZERO(&readfds);
            FD_SET(clientSocket, &readfds);

            // 设置超时
            struct timeval timeout;
            timeout.tv_sec = 1;  // 1秒超时
            timeout.tv_usec = 0;

            // 监视套接字
            int activity = select(clientSocket + 1, &readfds, nullptr, nullptr, &timeout);
            if (activity < 0)
            {
                std::cout << "Select error" << std::endl;
                break;
            }
            else if (activity == 0)
            {
                // 超时，没有数据可读
                std::cout << "No data received, continue waiting..." << std::endl;
                continue;
            }

            // 有数据可读
            if (FD_ISSET(clientSocket, &readfds))
            {
                char buffer[1024];
                memset(buffer, 0, sizeof(buffer));

                // 接收数据
                ssize_t is_rec = recv(clientSocket, buffer, sizeof(buffer), 0);
                if (is_rec == 0)
                {
                    std::cout << "Server disconnection, reconnecting..." << std::endl;
                    break;
                }
                else if (is_rec < 0)
                {
                    std::cout << "Failed to receive data" << std::endl;
                }
                else
                {
                    // 输出接收到的数据
                    std::cout << "TCP client async received data from server: " << buffer << std::endl;
                }
                std::cout << "---------- get once data -----------" << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(RE_READ_TIME));
        }

        close(clientSocket);
        std::this_thread::sleep_for(std::chrono::milliseconds(RECONNECT_TIME));
    }

    return true;
}


bool Socket::updSend(const std::string& ip, const int& port)
{
    int sockfd;
    struct sockaddr_in server_addr;

    // 创建UDP Socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        return false;
    }

    // 服务器地址设置
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr) <= 0)
    {
        perror("inet_pton");
        close(sockfd);
        return false;
    }

    // 发送数据
    std::string message = "Hello, UDP server!\n";

    while (true)
    {
        ssize_t sent_bytes = sendto(sockfd, message.c_str(), message.size(), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
        if (sent_bytes < 0)
        {
            perror("sendto");
            close(sockfd);
            return false;
        }
        else
        {
            std::cout << "Sent " << sent_bytes << " bytes: " << message ;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(RECONNECT_TIME));

    }

    // 关闭Socket
    close(sockfd);
    return true;
}



bool Socket::udpReceive(const std::string& ip, const int& port)
{
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[1024];

    // 创建UDP Socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        return false;
    }

    // 设置 socket 选项
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt");
        close(sockfd);
        return false;
    }

    // 服务器地址设置
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // 将 IP 地址转换为网络字节序
    if (inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr) <= 0)
    {
        perror("inet_pton");
        close(sockfd);
        return false;
    }

    // 绑定Socket
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        std::cout << "bind error: " << ip << ":" << port << std::endl;
        perror("bind");
        close(sockfd);
        return false;
    }

    std::cout << "Listening for UDP messages on " << ip << ":" << port << "...\n";

    // 循环接收数据
    while (true)
    {
        ssize_t received_bytes = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&client_addr, &addr_len);
        if (received_bytes < 0)
        {
            perror("recvfrom");
            close(sockfd);
            return false;
        }

        buffer[received_bytes] = '\0';  // 添加字符串结束符
        std::cout << "Received " << received_bytes << " bytes: " << buffer << std::endl;
    }

    // 关闭Socket（这里实际上永远不会达到，除非发生错误）
    close(sockfd);
    return true;
}



















