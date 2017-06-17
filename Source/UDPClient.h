#pragma once

#include <string>
#include "Poco/Mutex.h"
#include "Poco/Net/Net.h"
#include "Poco/Net/DatagramSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/Socket.h"
#include "Poco/Foundation.h"

class UDPClient
{
public:
	UDPClient();
	UDPClient(std::string strServerIP, int nServerPort);
	virtual ~UDPClient();

public:
	void		Init(std::string strIP, int nPort);
	//连接服务器
	bool		ConnectServer();
	bool		Bind();
	bool		SendProtocol(std::string strSend, std::string& strRecv);
	bool		Close();

public:
	std::string		m_strServerIP;
	int				m_nServerPort;
	Poco::Net::SocketAddress	m_socketAddr;
	Poco::Net::DatagramSocket	m_udpSocket;
};

