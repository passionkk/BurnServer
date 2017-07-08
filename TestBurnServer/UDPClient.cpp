#include "stdafx.h"
#include "UDPClient.h"
#include "Poco/JSON/Parser.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/JSON/JSON.h"

using namespace Poco;
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

int UDPClient::SendProtocol(std::string strSend, std::string& strRecv)
{
	try
	{
		char buffer[UDP_BUFFER_SIZE];
		memset(buffer, 0, UDP_BUFFER_SIZE);
		Poco::Timespan timeSpan(3 * 1000 * 1000);
		m_udpSocket.setReceiveTimeout(timeSpan);
		m_udpSocket.sendBytes(strSend.c_str(), strSend.length());
		int nRecvByte = m_udpSocket.receiveBytes(buffer, UDP_BUFFER_SIZE);
		strRecv = buffer;
		return 0;
	}
	catch (...)
	{
		printf("Send Protocol fail.\n");
		return -1;
	}
}

bool UDPClient::Close()
{
	m_udpSocket.close();
	return true;
}

int UDPClient::SendGetCDRomListProtocol(std::string & strSend, std::string& strRecv)
{
	try
	{
		Object::Ptr pObj = new Object(true);
		pObj->set("method", "getCDRomList");
		pObj->set("params", "");

		std::stringstream ss;
		pObj->stringify(ss);
		std::string sSend = ss.str();
		std::string sRecv = "";

		if (SendProtocol(sSend, sRecv) == 0)
		{
			strSend = sSend;
			strRecv = sRecv;
		}
		return 0;
	}
	catch (...)
	{
		//g_NetLog.Debug("%s catched\n", __PRETTY_FUNCTION__);
		return -1;
	}
}

int UDPClient::SendStartBurnProtocol(std::string strCDRomID, std::string strBurnType, std::string strBurnMode, int nAlarmSize,
									 const std::vector<FileInfo>& vecFileInfo, const BurnStateFeedbcak feedback, std::string & strSend, std::string& strRecv)
{
	try
	{
		Object::Ptr pObj = new Object(true);
		pObj->set("method", "startBurn");

		Object::Ptr pParams = new Object(true);
		pParams->set("burnMode", strBurnMode);//"singleBurn");
		pParams->set("discName", "discName");
		pParams->set("cdRomID", strCDRomID);
		pParams->set("alarmSize", nAlarmSize);//300);
		pParams->set("burnType", "fileBurn");

#if 0
		Object::Ptr pStreamInfo = new Object(true);
		pStreamInfo->set("burnFileName", "1.mp4");
		pStreamInfo->set("playlistInfo", "<?xml><config></config>");

		JSON::Array burnUrlArray;
		Object::Ptr pBurnUrl1 = new Object(true);
		pBurnUrl1->set("burnUrl", "rtsp://10.1.2.16/1");
		//const char* pSrc = "luzhiyuan 1";
		//std::string strUtf8;
		//Poco::UnicodeConverter::toUTF8(pSrc, strUtf8);
		pBurnUrl1->set("urlDescription", "luzhiyuan 1");
		burnUrlArray.add(pBurnUrl1);

		Object::Ptr pBurnUrl2 = new Object(true);
		pBurnUrl2->set("burnUrl", "rtsp://10.1.2.16/2");
		pBurnUrl2->set("urlDescription", "luzhiyuan2");
		burnUrlArray.add(pBurnUrl2);

		Object::Ptr pBurnUrl3 = new Object(true);
		pBurnUrl3->set("burnUrl", "rtsp://10.1.2.16/3");
		pBurnUrl3->set("urlDescription", "luzhiyuan 3");
		burnUrlArray.add(pBurnUrl3);

		Object::Ptr pBurnUrl4 = new Object(true);
		pBurnUrl4->set("burnUrl", "rtsp://10.1.2.16/4");
		pBurnUrl4->set("urlDescription", "luzhiyuan 4");
		burnUrlArray.add(pBurnUrl4);

		pStreamInfo->set("burnUrlList", burnUrlArray); // add burnUrlList
		pParams->set("streamInfo", pStreamInfo);
#endif
		//fileInfo
		JSON::Array burnFileArray;
		for (int i = 0; i < vecFileInfo.size(); i++)
		{
			Object::Ptr	burnFile = new Object(true);
			burnFile->set("fileLocation", vecFileInfo.at(i).m_strFileLocation);
			burnFile->set("fileType", vecFileInfo.at(i).m_strType);
			burnFile->set("burnSrcFilePath", vecFileInfo.at(i).m_strSrcUrl);
			burnFile->set("burnDstFilePath", vecFileInfo.at(i).m_strDestFilePath);
			burnFile->set("fileDescription", vecFileInfo.at(i).m_strDescription);
			burnFileArray.add(burnFile);
		}
#if 0
		Object::Ptr	burnFile = new Object(true);
		burnFile->set("fileLocation", "local");
		burnFile->set("fileType", "file");
		burnFile->set("burnSrcFilePath", "/mnt/A1.mp4");
		burnFile->set("burnDstFilePath", "/Media/A1.mp4");
		burnFile->set("fileDescription", "From Judger camera");
		burnFileArray.add(burnFile);

		Object::Ptr	burnFile2 = new Object(true);
		burnFile2->set("fileLocation", "local");
		burnFile2->set("fileType", "file");
		burnFile2->set("burnSrcFilePath", "/mnt/A2.mp4");
		burnFile2->set("burnDstFilePath", "/Media/A2.mp4");
		burnFile2->set("fileDescription", "From Judger camera");
		burnFileArray.add(burnFile2);

		Object::Ptr	burnFile3 = new Object(true);
		burnFile3->set("fileLocation", "local");
		burnFile3->set("fileType", "file");
		burnFile3->set("burnSrcFilePath", "/mnt/A3.mp4");
		burnFile3->set("burnDstFilePath", "/Media/A3.mp4");
		burnFile3->set("fileDescription", "From Judger camera");
		burnFileArray.add(burnFile3);
#endif
		Object::Ptr	burnFileList = new Object(true);
		burnFileList->set("burnFileList", burnFileArray);
		pParams->set("fileInfo", burnFileList);

		pParams->set("burnSpeed", 8);

		Object::Ptr pFeedbackParam = new Object(true);
		if (feedback.m_strNeedFeedback.compare("yes") == 0)
		{
			pFeedbackParam->set("needFeedback", "yes");
			pFeedbackParam->set("feedbackIP", feedback.m_strFeedbackIP);
			pFeedbackParam->set("feedbackPort", feedback.m_nFeedbackPort);
			pFeedbackParam->set("transType", feedback.m_transType);
			pFeedbackParam->set("feedIterval", feedback.m_nFeedbackInterval);
		}
		else
		{
			pFeedbackParam->set("needFeedback", "no");
			pFeedbackParam->set("feedbackIP", "127.0.0.1");
			pFeedbackParam->set("feedbackPort", 12345);
			pFeedbackParam->set("transType", "http");
			pFeedbackParam->set("feedIterval", 2000);
		}

		pParams->set("feedback", pFeedbackParam);

		pObj->set("params", pParams);

		std::stringstream ss;
		pObj->stringify(ss);
		std::string sSend = ss.str();
		std::string sRecv = "";
		if (SendProtocol(sSend, sRecv) == 0)
		{
			strSend = sSend;
			strRecv = sRecv;
		}
		return 0;
	}
	catch (...)
	{
		//g_NetLog.Debug("%s catched\n", __PRETTY_FUNCTION__);
		return -1;
	}
}

int UDPClient::SendPauseBurnProtocol(std::string & strSend, std::string& strRecv)
{
	try
	{
		Object::Ptr pObj = new Object(true);
		pObj->set("method", "pauseBurn");

		Object::Ptr pObjParams = new Object(true);
		pObjParams->set("sessionID", "201704260001");
		pObj->set("params", pObjParams);


		std::stringstream ss;
		pObj->stringify(ss);
		std::string sSend = ss.str();
		std::string sRecv = "";

		if (SendProtocol(sSend, sRecv) == 0)
		{
			strSend = sSend;
			strRecv = sRecv;
		}
		return 0;
	}
	catch (...)
	{
		//g_NetLog.Debug("%s catched\n", __PRETTY_FUNCTION__);
		return -1;
	}
}

int UDPClient::SendRemuseBurnProtocol(std::string & strSend, std::string& strRecv)
{
	try
	{
		Object::Ptr pObj = new Object(true);
		pObj->set("method", "resumeBurn");

		Object::Ptr pObjParams = new Object(true);
		pObjParams->set("sessionID", "201704260001");
		pObj->set("params", pObjParams);


		std::stringstream ss;
		pObj->stringify(ss);
		std::string sSend = ss.str();
		std::string sRecv = "";

		if (SendProtocol(sSend, sRecv) == 0)
		{
			strSend = sSend;
			strRecv = sRecv;
		}
		return 0;
	}
	catch (...)
	{
		//g_NetLog.Debug("%s catched\n", __PRETTY_FUNCTION__);
		return -1;
	}
}

int UDPClient::SendStopBurnProtocol(std::string & strSend, std::string& strRecv)
{
	try
	{
		Object::Ptr pObj = new Object(true);
		pObj->set("method", "stopBurn");

		Object::Ptr pObjParams = new Object(true);
		pObjParams->set("sessionID", "201704260001");
		pObj->set("params", pObjParams);

		std::stringstream ss;
		pObj->stringify(ss);
		std::string sSend = ss.str();
		std::string sRecv = "";

		if (SendProtocol(sSend, sRecv) == 0)
		{
			strSend = sSend;
			strRecv = sRecv;
		}
		return 0;
	}
	catch (...)
	{
		//g_NetLog.Debug("%s catched\n", __PRETTY_FUNCTION__);
		return -1;
	}
}

int UDPClient::SendGetCDRomInfoProtocol(std::string & strSend, std::string& strRecv)
{
	try
	{
		Object::Ptr pObj = new Object(true);
		pObj->set("method", "getCDRomInfo");

		Object::Ptr pObjParams = new Object(true);
		pObjParams->set("sessionID", "201704260001");
		pObj->set("params", pObjParams);


		std::stringstream ss;
		pObj->stringify(ss);
		std::string sSend = ss.str();
		std::string sRecv = "";

		if (SendProtocol(sSend, sRecv) == 0)
		{
			strSend = sSend;
			strRecv = sRecv;
		}
		return 0;
	}
	catch (...)
	{
		//g_NetLog.Debug("%s catched\n", __PRETTY_FUNCTION__);
		return -1;
	}
}

int UDPClient::SendAddBurnFileProtocol(std::string & strSend, std::string& strRecv)
{
	try
	{
		Object::Ptr pObj = new Object(true);
		pObj->set("method", "addBurnFile");

		Object::Ptr pObjParams = new Object(true);
		pObjParams->set("sessionID", "201704260001");

		JSON::Array burnFileArray;
		Object objFile1;
		objFile1.set("fileLocation", "local");
		objFile1.set("fileType", "file");
		objFile1.set("burnSrcFilePath", "/mnt/A1.mp4");
		objFile1.set("fileLocation", "/Media/A1.mp4");
		objFile1.set("fileLocation", "From Judger camera");
		burnFileArray.add(objFile1);

		Object objFile2;
		objFile2.set("fileLocation", "local");
		objFile2.set("fileType", "file");
		objFile2.set("burnSrcFilePath", "/mnt/A1.mp4");
		objFile2.set("fileLocation", "/Media/A1.mp4");
		objFile2.set("fileLocation", "From Judger camera");
		burnFileArray.add(objFile2);

		Object objFile3;
		objFile3.set("fileLocation", "local");
		objFile3.set("fileType", "file");
		objFile3.set("burnSrcFilePath", "/mnt/A2.mp4");
		objFile3.set("fileLocation", "/Media/A2.mp4");
		objFile3.set("fileLocation", "From Judger camera");
		burnFileArray.add(objFile3);

		pObjParams->set("burnFileList", burnFileArray);
		pObj->set("params", pObjParams);

		std::stringstream ss;
		pObj->stringify(ss);
		std::string sSend = ss.str();
		std::string sRecv = "";

		if (SendProtocol(sSend, sRecv) == 0)
		{
			strSend = sSend;
			strRecv = sRecv;
		}
		return 0;
	}
	catch (...)
	{
		//g_NetLog.Debug("%s catched\n", __PRETTY_FUNCTION__);
		return -1;
	}
}
