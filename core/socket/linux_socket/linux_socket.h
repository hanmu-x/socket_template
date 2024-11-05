
#ifndef SOCKET_H
#define SOCKET_H

#include <iostream>
#include <string>
#include <mutex>
#include <thread>


#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

class Socket
{
  public:
    /// <summary>
    /// TCP 服务端
    /// </summary>
    /// <param name="port">本地端口号</param>
    /// <returns></returns>
    static bool tcpServer(const int& port);

    /// <summary>
    /// 同步 TCP socket 客户端
    /// </summary>
    /// <param name="ip">tcp服务端IP</param>
    /// <param name="port">tcp服务端端口号</param>
    /// <returns></returns>
    static bool tcpClientSync(const std::string& ip, const int& port);

    /// <summary>
    /// 异步 TCP socket 客户端
    /// </summary>
    /// <param name="ip">tcp服务端IP</param>
    /// <param name="port">tcp服务端端口号</param>
    /// <returns></returns>
    static bool tcpClientAsyn(const std::string& ip, const int& port);

    /// <summary>
    /// UDP socket 发送数据
    /// </summary>
    /// <param name="ip">UPD服务端IP</param>
    /// <param name="port">UPD服务端端口号</param>
    /// <returns></returns>
    static bool updSend(const std::string& ip, const int& port);

    /// <summary>
    /// UDP socket 接收数据
    /// </summary>
    /// <param name="ip">本地IP</param>
    /// <param name="port">本地端口号</param>
    /// <returns></returns>
    static bool udpReceiveSync(const std::string& ip, const int& port);

};


#endif  // SOCKET_H

