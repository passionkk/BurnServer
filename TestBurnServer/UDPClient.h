#pragma once

#include "Poco/Mutex.h"
#include "Poco/Net/Net.h"
#include "Poco/Net/DatagramSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/Socket.h"
#include "poco/Foundation.h"
#include "poco/Runnable.h"

class UDPClient //: public Poco::Runnable
{
public:
	UDPClient();
	UDPClient(std::string strServerIP, int nServerPort);
	virtual ~UDPClient();

public:
	void		TestUDPConnect(CString& strRecv);
	void		Init(std::string strIP, int nPort);
	//连接服务器
	bool		ConnectServer();
	bool		Bind();
	bool		SendProtocol(std::string strSend, std::string& strRecv);
	bool		Close();

	//virtual	void run();
public:
	std::string		m_strServerIP;
	int				m_nServerPort;
	Poco::Net::SocketAddress	m_socketAddr;
	Poco::Net::DatagramSocket	m_udpSocket;
};

