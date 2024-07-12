#include "socket.h"

#include <cstring>
#include <ws2tcpip.h>
#include <thread>
#include <chrono>

#define MAX_BUFFER_SIZE 1024
#define RECONNECT_TIME 1000
#define RE_READ_TIME 10

bool Socket::tcpServer(std::string ip, int port)
{
    return false;
}


bool Socket::tcpClientSync(std::string ip, int port)
{
    bool status = false;
    // WSAStartup() 函数初始化了Winsock库
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
                receiverThread = std::thread(&Socket::DataReceiver, this, clientSocket);
                receiverThread.detach();

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

bool Socket::udpServer(std::string ip, int port)
{
    return false;
}

// 同步
bool Socket::updClientSync(std::string ip, int port)
{
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cout << "Failed to initialize winsock" << std::endl;
        return false;
    }

    // Create a UDP socket
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
            int recvBytes = recvfrom(clientSocket, buffer, sizeof(buffer), 0, (sockaddr*)&serverAddress, &serverAddrSize);
            if (recvBytes == SOCKET_ERROR)
            {
                std::cout << "Failed to receive data" << std::endl;
            }
            else
            {
                buffer[recvBytes] = '\0';
                std::cout << "UDP client received data from server: " << buffer << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(RE_READ_TIME));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(RECONNECT_TIME));
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

// Socket::Socket()
//{
// }
//
// Socket::~Socket()
//{
// }