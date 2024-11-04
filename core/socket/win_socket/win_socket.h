
#ifndef SOCKET_H
#define SOCKET_H

#include <iostream>
#include <string>
#include <mutex>
#include <thread>


#include <winsock2.h>

class Socket
{
  public:
    /// <summary>
    /// TCP 服务端
    /// </summary>
    /// <param name="port"></param>
    /// <returns></returns>
    static bool tcpServer( int port);

    /// <summary>
    /// 同步 TCP socket 客户端
    /// </summary>
    /// <param name="ip"></param>
    /// <param name="port"></param>
    /// <returns></returns>
    static bool tcpClientSync(std::string ip, int port);

    /// <summary>
    /// 异步 TCP socket 客户端
    /// </summary>
    /// <param name="ip"></param>
    /// <param name="port"></param>
    /// <returns></returns>
    bool tcpClientAsyn(std::string ip, int port);


    /// <summary>
    /// UDP socket 发送数据
    /// </summary>
    /// <param name="ip">UPD服务端IP</param>
    /// <param name="port">UPD服务端端口号</param>
    /// <returns></returns>
    static bool updSend(std::string ip, int port);

    /// <summary>
    /// UDP socket 接收数据
    /// </summary>
    /// <param name="ip">本地IP</param>
    /// <param name="port">本地端口号</param>
    /// <returns></returns>
    static bool udpReceive(std::string ip, int port);


    /// <summary>
    /// 同步 UDP socket 客户端
    /// </summary>
    /// <param name="ip"></param>
    /// <param name="port"></param>
    /// <returns></returns>
    static bool updClientSync(std::string ip, int port);

    /// <summary>
    /// 异步 UDP socket 客户端
    /// </summary>
    /// <param name="ip"></param>
    /// <param name="port"></param>
    /// <returns></returns>
    static bool updClientAsyn(std::string ip, int port);

    Socket() = default;
    ~Socket() = default;

  private:

    // 异步socket接收数据的函数
    void DataReceiver(SOCKET clientSocket);
    std::thread receiverThread;

  private:
    bool is_Running = false;
    std::mutex m_lock;  // 子线程锁
};




#endif // SOCKET_H

