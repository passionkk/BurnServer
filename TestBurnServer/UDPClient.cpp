#include "stdafx.h"
#include "UDPClient.h"
#include "Poco/JSON/Parser.h"
#include "Poco/Dynamic/Var.h"
using namespace Poco::JSON;
using namespace Poco::Dynamic;
using Poco::DynamicStruct;

#define UDP_BUFFER_SIZE (64 * 1024)

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
	//run();
}

void UDPClient::TestUDPConnect(CString& strRecv)
{
	try
	{
		Poco::Net::SocketAddress sa("192.168.1.102", 91);
		m_socketAddr = sa;
		m_udpSocket.connect(m_socketAddr);
		//m_udpSocket.bind(sa);
	
		Object::Ptr pObj = new Object(true);
		Object::Ptr pObj1 = new Object(true);
		pObj1->set("videoEncodeType", 8);
		pObj1->set("logicNo", 88);
		pObj->set("params", pObj1);
		pObj->set("method", "testProtocol");

		std::stringstream ss;
		pObj->stringify(ss);
		std::string sSend = ss.str();
		std::string sRecv = "";

		if (SendProtocol(sSend, sRecv) == 0)
		{
			CString str(sRecv.c_str());
			strRecv = str;//strRecv.Format(L"%s", sRecv.c_str());
		}
		m_udpSocket.close();
	}
	catch (...)
	{
		//g_NetLog.Debug("%s catched\n", __PRETTY_FUNCTION__);
	}
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
	try
	{
		char buffer[UDP_BUFFER_SIZE];
		memset(buffer, 0, UDP_BUFFER_SIZE);
		Poco::Timespan timeSpan(3 * 1000 * 1000);
		m_udpSocket.setReceiveTimeout(timeSpan);
		m_udpSocket.sendBytes(strSend.c_str(), strSend.length());
		int nRecvByte = m_udpSocket.receiveBytes(buffer, UDP_BUFFER_SIZE);
		AfxMessageBox(L"udp protocol return back");
		strRecv = buffer;
		return true;
	}
	catch (...)
	{
		AfxMessageBox(L"udp protocol time out");
		return false;
	}
}
