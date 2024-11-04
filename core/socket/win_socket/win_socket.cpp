
#include "win_socket.h"

#include <cstring>
#include <thread>
#include <chrono>

#define MAX_BUFFER_SIZE 1024
#define RECONNECT_TIME 1000
#define RE_READ_TIME 10

#include <ws2tcpip.h>
#include <winsock2.h>

// 接收客户端发过来的数据
void HandleClientConnection(SOCKET clientSocket)
{
    char buffer[MAX_BUFFER_SIZE];
    int bytesReceived;
    while ((bytesReceived = recv(clientSocket, buffer, MAX_BUFFER_SIZE - 1, 0)) > 0)
    {
        buffer[bytesReceived] = '\0';
        std::cout << "Received data from client: " << buffer << std::endl;
    }
    if (bytesReceived == 0)
    {
        std::cout << "Client disconnected." << std::endl;
    }
    else if (bytesReceived == SOCKET_ERROR)
    {
        std::cout << "recv() failed with error: " << WSAGetLastError() << std::endl;
    }
}

// 向客户端发送数据
void SendDataToClient(SOCKET clientSocket)
{
    //char message[] = "Hello from the server!";
    //send(clientSocket, message, static_cast<int>(strlen(message)), 0);
    //std::cout << "Sent data to client: " << message << std::endl;
    while (true)
    {
        send(clientSocket, ".", static_cast<int>(strlen(".")), 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(RECONNECT_TIME));
    }
}

// 接受连接并处理
void AcceptAndHandleConnection(SOCKET listenSocket)
{
    sockaddr_in clientAddr;
    int addrLen = sizeof(clientAddr);
    SOCKET clientSocket = accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cout << "accept() failed with error: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    std::cout << "Accepted connection from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << std::endl;

    // 处理客户端请求
    std::thread HandleClient, SendData;
    // 接收客户端数据
    HandleClient = std::thread(HandleClientConnection, clientSocket);
    SendData = std::thread(SendDataToClient, clientSocket);

    HandleClient.join();
    SendData.join();
}



bool Socket::tcpServer(int port)
{
    // 初始化Winsock, 初始化 Winsock 库，使用版本 2.2
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        std::cout << "WSAStartup failed with error: " << result << std::endl;
        return false;
    }
    // 创建监听套接字
    // AF_INET 表示 IPv4，SOCK_STREAM 表示流式套接字（TCP），IPPROTO_TCP 表示使用 TCP 协议
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET)
    {
        std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    sockaddr_in service; // 套接字地址
    service.sin_family = AF_INET; // IPv4 地址族
    service.sin_addr.s_addr = INADDR_ANY; // 使用 INADDR_ANY 允许接受任意客户端的连接
    service.sin_port = htons(port); // 将端口号转换为网络字节序

    // 绑定套接字到指定的地址和端口
    if (bind(listenSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
    {
        std::cout << "bind() failed with error: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return false;
    }
    // listen：开始监听传入的连接请求，SOMAXCONN 表示使用系统允许的最大连接队列长度
    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cout << "listen() failed with error: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return false;
    }

    while (true)  // 持续等待客户端连接
    {
        sockaddr_in clientAddr;
        int addrLen = sizeof(clientAddr);
        // accept：接受连接请求，返回一个新的套接字用于与客户端通信，clientAddr 存储客户端的地址信息
        SOCKET clientSocket = accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
        if (clientSocket == INVALID_SOCKET)
        {
            std::cout << "accept() failed with error: " << WSAGetLastError() << std::endl;
            continue;  // 继续等待新的连接
        }

        std::cout << "Accepted connection from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << std::endl;

        // 处理客户端请求
        std::thread HandleClient(HandleClientConnection, clientSocket);
        std::thread SendData(SendDataToClient, clientSocket);

        HandleClient.detach();  // 处理连接的线程分离，允许其独立运行
        SendData.detach();      // 同样分离发送数据的线程
    }

    // 关闭监听 socket
    closesocket(listenSocket);
    WSACleanup();
    return true;
}


bool Socket::tcpClientSync(std::string ip, int port)
{
    bool status = false;
    // WSAStartup() 函数初始化了Winsock库
    WSADATA wsaData;
    // 调用 WSAStartup 来初始化 Winsock 库，指定版本为 2.2
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cout << "Failed to initialize winsock" << std::endl;
        return false;
    }

    while (true)
    {
        // 创建套接字
        SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == INVALID_SOCKET)
        {
            std::cout << "Failed to create socket" << std::endl;
            WSACleanup();
            return false;
        }

        // 设置服务器信息
        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        if (inet_pton(AF_INET, ip.c_str(), &(serverAddress.sin_addr)) <= 0)
        {
            std::cout << "Invalid address/Address not supported" << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return false;
        }

        while (true)
        {
            // 连接到服务器
            int is_con = connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
            if (is_con == SOCKET_ERROR)
            {
                std::cout << "Connection failed" << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(RECONNECT_TIME));  // 等待1秒后重新连接

                continue;
            }
            while (true)
            {
                char buffer[1024];
                memset(buffer, 0, sizeof(buffer));

                // 接收数据 (都是 <0 出错 =0 连接关闭 >0 接收到数据大小)
                size_t is_rec = recv(clientSocket, buffer, sizeof(buffer), 0);
                if ((int)is_rec == 0)
                {
                    std::cout << "Server disconnection and reconnection " << std::endl;
                    break;
                }
                else if ((int)is_rec < 0)
                {
                    std::cout << "Failed to receive data" << std::endl;
                }
                else
                {
                    // 输出接收到的数据
                    std::cout << "TCP client sync received data from server: " << buffer << std::endl;
                    // 等待100毫秒后重新连接
                }
                std::cout << "---------- get once data -----------" << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(RE_READ_TIME));  // 等待1秒后重新连接
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(RE_READ_TIME));  // 等待1秒后重新连接
            // 跳出循环关闭socket 然后从新建立socket从新连接
            break;
        }

        closesocket(clientSocket);
        std::this_thread::sleep_for(std::chrono::milliseconds(RECONNECT_TIME));  // 等待1秒后重新连接
    }

    WSACleanup();
    return true;
}


void Socket::DataReceiver(SOCKET clientSocket)
{
    std::unique_lock lock_t(m_lock);
    char buffer[1024];
    while (is_Running)
    {
        memset(buffer, 0, sizeof(buffer));
        int is_rec = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (is_rec == 0)
        {
            std::cout << "Server disconnection and reconnection" << std::endl;
            is_Running = false;
            break;
        }
        else if (is_rec < 0)
        {
            std::cout << "Failed to receive data" << std::endl;
        }
        else
        {
            // 打印查看数据
            std::cout << "TCP client asny received data from server: " << buffer << std::endl;
        }
        std::cout << ".";
        std::this_thread::sleep_for(std::chrono::milliseconds(RE_READ_TIME));
    }
    closesocket(clientSocket);
    lock_t.unlock();
    return;
}

bool Socket::tcpClientAsyn(std::string ip, int port)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cout << "Failed to initialize winsock" << std::endl;
        return false;
    }
    while (true)
    {
        // 创建套接字
        SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == INVALID_SOCKET)
        {
            std::cout << "Failed to create socket" << std::endl;
            WSACleanup();
            return false;
        }

        // 设置服务器信息
        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        if (inet_pton(AF_INET, ip.c_str(), &(serverAddress.sin_addr)) <= 0)
        {
            std::cout << "Invalid address/Address not supported" << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return false;
        }

        is_Running = true;
        while (is_Running)
        {
            // 连接到服务器
            int is_con = connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
            if (is_con == SOCKET_ERROR)
            {
                std::cout << "Connection failed " << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(RECONNECT_TIME));  // 等待1秒后重新连接
                break;
            }
            else
            {
                std::cout << "Connected to server " << std::endl;
                // 异步接收数据
                receiverThread = std::thread(&Socket::DataReceiver, this, clientSocket);
                receiverThread.detach();
                std::cout << "asyn" << std::endl;

                int n = 0;
                while (is_Running)
                {
                    //std::cout << n++ << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(RECONNECT_TIME));  // 等待1秒后重新连接
                }
            }
        }

        closesocket(clientSocket);
        std::this_thread::sleep_for(std::chrono::milliseconds(RECONNECT_TIME));  // 等待1秒后重新连接
    }

    WSACleanup();
    return true;
}


bool Socket::updSend(std::string ip, int port)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cout << "Failed to initialize winsock" << std::endl;
        return false;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cout << "Failed to create socket" << std::endl;
        WSACleanup();
        return false;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    int is_conn = inet_pton(AF_INET, ip.c_str(), &(serverAddress.sin_addr));
    if (is_conn <= 0)
    {
        std::cout << "Invalid address/Address not supported" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return false;
    }

    while (true)
    {
        // Send data to the server
        std::string message = "Hello, UDP Server!";
        if (sendto(clientSocket, message.c_str(), message.length(), 0, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
        {
            std::cout << "Failed to send data" << std::endl;
        }
        else
        {
            std::cout << "Sent data to server: " << message << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(RECONNECT_TIME));
    }

    closesocket(clientSocket);
    WSACleanup();
    return true;
}




bool Socket::updClientSync(std::string ip, int port)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cout << "Failed to initialize winsock" << std::endl;
        return false;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cout << "Failed to create socket" << std::endl;
        WSACleanup();
        return false;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    int is_conn = inet_pton(AF_INET, ip.c_str(), &(serverAddress.sin_addr));
    if (is_conn <= 0)
    {
        std::cout << "Invalid address/Address not supported" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return false;
    }

    while (true)
    {
        // Send data to the server
        std::string message = "Hello, UDP Server!";
        if (sendto(clientSocket, message.c_str(), message.length(), 0, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
        {
            std::cout << "Failed to send data" << std::endl;
        }
        else
        {
            std::cout << "Sent data to server: " << message << std::endl;
        }

        while (true)
        {
            char buffer[1024];
            memset(buffer, 0, sizeof(buffer));
            // Receive data from the server
            int serverAddrSize = sizeof(serverAddress);
            //MSG_WAITALL：尝试接收全部请求的数据。函数可能会阻塞，直到收到所有数据。
            //MSG_PEEK：查看即将接收的数据，但不从套接字缓冲区中删除它【1】。
            int flags = 0;  // MSG_DONTWAIT 表示非阻塞接收, 0 表示
            int recvBytes = recvfrom(clientSocket, buffer, sizeof(buffer), flags, (sockaddr*)&serverAddress, &serverAddrSize);
            if (recvBytes == SOCKET_ERROR)
            {
                //std::cout << "Failed to receive data" << std::endl;
                break;
            }
            else
            {
                buffer[recvBytes] = '\0';
                std::cout << "UDP client received data from server: " << buffer << std::endl;
            }
            std::cout << ".";
            std::this_thread::sleep_for(std::chrono::milliseconds(RE_READ_TIME));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(RECONNECT_TIME));
    }

    closesocket(clientSocket);
    WSACleanup();
    return true;
}




//bool Socket::receiveData(std::string ip, int port)
//{
//    WSADATA wsaData;
//    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
//    {
//        std::cout << "Failed to initialize winsock" << std::endl;
//        return false;
//    }
//
//    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
//    if (clientSocket == INVALID_SOCKET)
//    {
//        std::cout << "Failed to create socket" << std::endl;
//        WSACleanup();
//        return false;
//    }
//
//    sockaddr_in serverAddress;
//    serverAddress.sin_family = AF_INET;
//    serverAddress.sin_port = htons(port);
//    int is_conn = inet_pton(AF_INET, ip.c_str(), &(serverAddress.sin_addr));
//    if (is_conn <= 0)
//    {
//        std::cout << "Invalid address/Address not supported" << std::endl;
//        closesocket(clientSocket);
//        WSACleanup();
//        return false;
//    }
//
//    while (true)
//    {
//        char buffer[1024];
//        memset(buffer, 0, sizeof(buffer));
//        // Receive data from the server
//        int serverAddrSize = sizeof(serverAddress);
//        // MSG_WAITALL：尝试接收全部请求的数据。函数可能会阻塞，直到收到所有数据。
//        // MSG_PEEK：查看即将接收的数据，但不从套接字缓冲区中删除它【1】。
//        int flags = 0;  // MSG_DONTWAIT 表示非阻塞接收, 0 表示
//        int recvBytes = recvfrom(clientSocket, buffer, sizeof(buffer), flags, (sockaddr*)&serverAddress, &serverAddrSize);
//        if (recvBytes == SOCKET_ERROR)
//        {
//            // std::cout << "Failed to receive data" << std::endl;
//            continue;
//        }
//        else
//        {
//            buffer[recvBytes] = '\0';
//            std::cout << "UDP client received data from server: " << buffer << std::endl;
//        }
//        std::cout << ".";
//        std::this_thread::sleep_for(std::chrono::milliseconds(RE_READ_TIME));
//    }
//
//    closesocket(clientSocket);
//    WSACleanup();
//    return true;
//}

bool Socket::udpReceive(std::string ip, int port)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cout << "Failed to initialize winsock" << std::endl;
        return false;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cout << "Failed to create socket" << std::endl;
        WSACleanup();
        return false;
    }

    sockaddr_in localAddress;
    localAddress.sin_family = AF_INET;
    localAddress.sin_port = htons(port);        // 绑定到接收的端口
    localAddress.sin_addr.s_addr = INADDR_ANY;  // 监听所有可用接口

    // 绑定套接字到本地地址和端口
    if (bind(clientSocket, (sockaddr*)&localAddress, sizeof(localAddress)) == SOCKET_ERROR)
    {
        std::cout << "Bind failed with error: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return false;
    }

    std::cout << "Listening for UDP messages on " << ip << ":" << port << "...\n";

    while (true)
    {
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));

        sockaddr_in senderAddress;
        int senderAddrSize = sizeof(senderAddress);

        int recvBytes = recvfrom(clientSocket, buffer, sizeof(buffer), 0, (sockaddr*)&senderAddress, &senderAddrSize);
        if (recvBytes == SOCKET_ERROR)
        {
            int error = WSAGetLastError();
            std::cout << "Failed to receive data, error: " << error << std::endl;
            continue;
        }
        else
        {
            buffer[recvBytes] = '\0';
            std::cout << "UDP client received data from " << inet_ntoa(senderAddress.sin_addr) << ":" << ntohs(senderAddress.sin_port) << ": " << buffer << std::endl;
        }

    }

    closesocket(clientSocket);
    WSACleanup();
    return true;
}







//{
//    // Initialize Winsock
//    WSADATA wsaData;
//    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
//    {
//        std::cout << "WSAStartup failed." << std::endl;
//        return false;
//    }
//
//    // Create a UDP socket
//    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
//    if (clientSocket == INVALID_SOCKET)
//    {
//        std::cout << "Error creating socket: " << WSAGetLastError() << std::endl;
//        WSACleanup();
//        return false;
//    }
//
//    // Server address information
//    sockaddr_in serverAddr;
//    serverAddr.sin_family = AF_INET;
//    serverAddr.sin_port = htons(port);
//    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());
//
//    // Send data to the server
//    std::string message = "Hello, UDP Server!";
//    if (sendto(clientSocket, message.c_str(), message.length(), 0, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
//    {
//        std::cout << "Error sending data: " << WSAGetLastError() << std::endl;
//        closesocket(clientSocket);
//        WSACleanup();
//        return false;
//    }
//
//    // Receive response from the server
//    char buffer[1024];
//    int recvBytes = recvfrom(clientSocket, buffer, sizeof(buffer), 0, nullptr, nullptr);
//    if (recvBytes == SOCKET_ERROR)
//    {
//        std::cout << "Error receiving data: " << WSAGetLastError() << std::endl;
//    }
//    else
//    {
//        buffer[recvBytes] = '\0';
//        std::cout << "Received from server: " << buffer << std::endl;
//    }
//
//    // Cleanup
//    closesocket(clientSocket);
//    WSACleanup();
//
//    return true;
//}

// 异步
bool Socket::updClientAsyn(std::string ip, int port)
{
    return false;
}


