
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
    /// <param name="port"></param>
    /// <returns></returns>
    static bool tcpServer(const int& port);

    /// <summary>
    /// 同步 TCP socket 客户端
    /// </summary>
    /// <param name="ip"></param>
    /// <param name="port"></param>
    /// <returns></returns>
    static bool tcpClientSync(const std::string& ip, const int& port);

    /// <summary>
    /// 异步 TCP socket 客户端
    /// </summary>
    /// <param name="ip"></param>
    /// <param name="port"></param>
    /// <returns></returns>
    static bool tcpClientAsyn(const std::string& ip, const int& port);

    ///// <summary>
    ///// UDP 服务端
    ///// </summary>
    ///// <param name="ip"></param>
    ///// <param name="port"></param>
    ///// <returns></returns>
    //static bool udpServer(std::string ip, int port);

    /// <summary>
    /// UDP socket 发送数据
    /// </summary>
    /// <param name="ip"></param>
    /// <param name="port"></param>
    /// <returns></returns>
    static bool updSend(const std::string& ip, const int& port);

    static bool udpReceive(const std::string& ip, const int& port);

};


#endif  // SOCKET_H

