#pragma once

#include "Poco/Mutex.h"
#include "Poco/Net/Net.h"
#include "Poco/Net/DatagramSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/Socket.h"
#include "poco/Foundation.h"

class UDPClient
{
public:
	UDPClient();
	UDPClient(std::string strServerIP, int nServerPort);
	virtual ~UDPClient();

public:
	//连接服务器
	bool		ConnectServer();
	bool		Bind();
	bool		SendProtocol(std::string strSend, std::string& strRecv);

public:
	std::string		m_strServerIP;
	int				m_nServerPort;
	Poco::Net::SocketAddress	m_socketAddr;
	Poco::Net::DatagramSocket	m_udpSocket;
};

