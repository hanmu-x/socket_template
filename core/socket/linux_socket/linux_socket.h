
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
    static bool tcpServer(int port);
};


#endif  // SOCKET_H

