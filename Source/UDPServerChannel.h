#ifndef UDPSERVERCHANNEL_INCLUDED
#define UDPSERVERCHANNEL_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "CommonDefine.h"

#include "Poco/Net/Net.h"
#include "Poco/Net/DatagramSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Thread.h"
#include "Poco/Event.h"
#include "Poco/Mutex.h"
using Poco::Mutex;
using Poco::Net::Socket;
using Poco::Net::DatagramSocket;
using Poco::Net::SocketAddress;
using Poco::Net::IPAddress;

#include <vector>

typedef int(*UdpServerCallBackFunc)(char *sRemoteIP, int nRemotePort, DatagramSocket &localSocket, char *sData, int nData, void *pContext);

typedef  void CmdListener;

class UDPServerChannel: public Poco::Runnable
{
public:
	UDPServerChannel(std::string strName);

	~UDPServerChannel();

	Poco::UInt16 port() const;
		/// Returns the port the  server is
		/// listening on.
		
	Poco::Net::SocketAddress address() const;
		/// Returns the address of the server.	
		
	void run();
		/// Does the work.
public:
	/// start the UDPServer
	int start(int iPort);
	/// stop the UDPServer.
	int stop();

    void AddListener(CmdListener* pListener);
    void AddRetransChannel(RetransChannel retransChannel);
private:
    static int UdpServerCallBack(char *sRemoteIP, int nRemotePort, DatagramSocket &localSocket, char *sData, int nData, void *pContext);
    int ProcessCmd(char *sRemoteIP, int nRemotePort, DatagramSocket &localSocket, char *sData, int nData);
    void DoRetrans(char *sData, int nData);

	//for test
	std::string TestProtocol(std::string strIn);
	//获取光驱列表
	std::string GetCDRomList(std::string strIn);
	//开始刻录
	std::string StartBurn(std::string strIn);
	//暂停刻录
	std::string PauseBurn(std::string strIn);
	//继续刻录
	std::string ResumBurn(std::string strIn);
	//继续刻录
	std::string StopBurn(std::string strIn);
	//获取光驱信息
	std::string GetCDRomInfo(std::string strIn);
	//增加刻录文件
	std::string AddBurnFile(std::string strIn);


private:
	UdpServerCallBackFunc   m_CallBackFunc;
    std::vector<CmdListener*>   m_vectListener;
    Poco::Mutex    m_mutexListenerVect;
	void                    *m_pContext;
	Poco::Net::DatagramSocket m_socket;
	Poco::Thread m_thread;
	Poco::Event  m_ready;
	bool         m_stop;

    std::string m_strName;
    std::vector<RetransChannel> m_vectRetransChannels;
    Poco::Mutex    m_mutexRetransChannelsVect;
};


#endif // UDPSERVERCHANNEL_INCLUDED
