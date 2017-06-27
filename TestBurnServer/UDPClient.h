#pragma once

#include "Poco/Mutex.h"
#include "Poco/Net/Net.h"
#include "Poco/Net/DatagramSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/Socket.h"
#include "poco/Foundation.h"
#include "DataStructDefine.h"

class UDPClient
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
	int			SendProtocol(std::string strSend, std::string& strRecv);
	bool		Close();

	int			SendGetCDRomListProtocol(std::string & strSend, std::string& strRecv);
	int			SendStartBurnProtocol(std::string strBurnType, std::string strBurnMode, int nAlarmSize,
									  const std::vector<FileInfo>& vecFileInfo, const BurnStateFeedbcak feedback, std::string & strSend, std::string& strRecv);
	int			SendPauseBurnProtocol(std::string & strSend, std::string& strRecv);
	int			SendRemuseBurnProtocol(std::string & strSend, std::string& strRecv);
	int			SendStopBurnProtocol(std::string & strSend, std::string& strRecv);
	int			SendGetCDRomInfoProtocol(std::string & strSend, std::string& strRecv);
	int			SendAddBurnFileProtocol(std::string & strSend, std::string& strRecv);

public:
	std::string		m_strServerIP;
	int				m_nServerPort;
	Poco::Net::SocketAddress	m_socketAddr;
	Poco::Net::DatagramSocket	m_udpSocket;
};

