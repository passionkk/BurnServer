#include "UDPClient.h"


UDPClient::UDPClient()
{
}

UDPClient::UDPClient(std::string strServerIP, int nServerPort)
{
	m_strServerIP = strServerIP;
	m_nServerPort = nServerPort;
}

UDPClient::~UDPClient()
{
}

bool UDPClient::ConnectServer()
{
	return false;
}

bool UDPClient::SendProtocol(std::string strSend, std::string& strRecv)
{
	return false;
}
