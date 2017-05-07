#include "UDPClient.h"
#include "CommonDefine.h"

UDPClient::UDPClient()
{
}

UDPClient::UDPClient(std::string strServerIP, int nServerPort)
{
	m_strServerIP = strServerIP;
	m_nServerPort = nServerPort;
	Poco::Net::SocketAddress sa(m_strServerIP, m_nServerPort);
	m_socketAddr = sa;
}

UDPClient::~UDPClient()
{
}

void UDPClient::Init(std::string strIP, int nPort)
{
	m_strServerIP = strIP;
	m_nServerPort = nPort;
	Poco::Net::SocketAddress sa(m_strServerIP, m_nServerPort);
	m_socketAddr = sa;
}

bool UDPClient::ConnectServer()
{
	
	m_udpSocket.connect(m_socketAddr);
	return true;
}

bool UDPClient::Bind()
{
	m_udpSocket.bind(m_socketAddr);
	return true;
}

bool UDPClient::SendProtocol(std::string strSend, std::string& strRecv)
{
	m_udpSocket.sendBytes(strSend.c_str(), strSend.length());
	return false;
}
