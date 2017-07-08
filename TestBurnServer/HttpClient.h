#pragma once
#include <string>

#include "Poco/Mutex.h"
#include "Poco/Net/Net.h"
#include "Poco/Net/DatagramSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/Socket.h"
#include "poco/Foundation.h"
#include "poco/UTF8Encoding.h"
#include "DataStructDefine.h"

using namespace Poco;

class CHttpClient
{
public:
	CHttpClient(void);
	~CHttpClient(void);

public:
	/**
	* @brief HTTP POST请求
	* @param strUrl 输入参数,请求的Url地址,如:http://www.baidu.com
	* @param strPost 输入参数,使用如下格式para1=val1¶2=val2&…
	* @param strResponse 输出参数,返回的内容
	* @return 返回是否Post成功
	*/
	int Post(const std::string & strUrl, const std::string & strPost, std::string & strResponse);

	/**
	* @brief HTTP GET请求
	* @param strUrl 输入参数,请求的Url地址,如:http://www.baidu.com
	* @param strResponse 输出参数,返回的内容
	* @return 返回是否Post成功
	*/
	int Get(const std::string & strUrl, std::string & strResponse);

	/**
	* @brief HTTPS POST请求,无证书版本
	* @param strUrl 输入参数,请求的Url地址,如:https://www.alipay.com
	* @param strPost 输入参数,使用如下格式para1=val1¶2=val2&…
	* @param strResponse 输出参数,返回的内容
	* @param pCaPath 输入参数,为CA证书的路径.如果输入为NULL,则不验证服务器端证书的有效性.
	* @return 返回是否Post成功
	*/
	int Posts(const std::string & strUrl, const std::string & strPost, std::string & strResponse, const char * pCaPath = NULL);

	/**
	* @brief HTTPS GET请求,无证书版本
	* @param strUrl 输入参数,请求的Url地址,如:https://www.alipay.com
	* @param strResponse 输出参数,返回的内容
	* @param pCaPath 输入参数,为CA证书的路径.如果输入为NULL,则不验证服务器端证书的有效性.
	* @return 返回是否Post成功
	*/
	int Gets(const std::string & strUrl, std::string & strResponse, const char * pCaPath = NULL);

	int SendHttpProtocol(std::string strIP, int nPort, std::string sSend, std::string &sRecv, bool bLog = true);

	int BurnServerConnect(CString& strRecv, int nCallCloseDisk = 0);

	int SendGetCDRomListProtocol(CString& strSend, CString& strRecv);
	int SendStartBurnProtocol(std::string strCDRomID, std::string strBurnType, std::string strBurnMode, int nAlarmSize,
		const std::vector<FileInfo>& vecFileInfo, const BurnStateFeedbcak feedback, CString& strSend, CString& strRecv);
	int SendPauseBurnProtocol(std::string sessionID, CString& strSend, CString& strRecv);
	int SendRemuseBurnProtocol(std::string sessionID, CString& strSend, CString& strRecv);
	int SendStopBurnProtocol(std::string sessionID, CString& strSend, CString& strRecv);
	int SendGetCDRomInfoProtocol(std::string cdRomID, CString& strSend, CString& strRecv);
	int SendAddBurnFileProtocol(std::string sessionID,  const std::vector<FileInfo>& vecFileInfo, CString& strSend, CString& strRecv);
public:
	void SetDebug(bool bDebug);
	void SetServerInfo(std::string strIP, int nPort);

private:
	bool m_bDebug;
	std::string m_strServerIP;
	int			m_nServerPort;
};

//#endif

