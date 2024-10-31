

#include <iostream>
#include <string>
#include <mutex>
#include <winsock2.h>
#include <thread>



class Socket
{
public:

	/// <summary>
	/// TCP 服务端
	/// </summary>
	/// <param name="ip"></param>
	/// <param name="port"></param>
	/// <returns></returns>
	bool tcpServer(std::string ip, int port);

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
	/// UDP 服务端
	/// </summary>
	/// <param name="ip"></param>
	/// <param name="port"></param>
	/// <returns></returns>
    static bool udpServer(std::string ip, int port);

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

	void DataReceiver(SOCKET clientSocket);
	std::thread receiverThread;

private:
	
	bool is_Running = false;
	std::mutex m_lock;                  // 子线程锁

};



































