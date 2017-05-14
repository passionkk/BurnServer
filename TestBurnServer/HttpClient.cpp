#include "stdafx.h"
#include "HttpClient.h"
#include "libcurl/curl.h"
#include <string>

#ifdef WIN32
#include <windows.h>
#endif

#ifdef DEBUG
//#pragma comment(lib, "libcurld.lib")     
#endif // DEBUG

//#pragma comment(lib, "wldap32.lib")     
//#pragma comment(lib, "ws2_32.lib")     
//#pragma comment(lib, "winmm.lib")  
#include "Poco/Foundation.h"
//#include "Poco/File.h"
//#include "poco/StringTokenizer.h"
#include "poco/JSON/Parser.h"
#include "poco/Dynamic/Var.h"
#include "Poco/net/Net.h"
#include "poco/net/DatagramSocket.h"
#include "poco/net/streamsocket.h"

using namespace Poco::JSON;
using namespace Poco::Dynamic;
using Poco::DynamicStruct;

#define UDP_BUFFER_SIZE (64 * 1024)

size_t client_write_data(void* buffer, size_t size, size_t nmemb, void *stream)
{
	((std::string*)stream)->append((char*)buffer, size * nmemb);
	return size * nmemb;
}

CHttpClient::CHttpClient(void) :
m_bDebug(false)
{

}

CHttpClient::~CHttpClient(void)
{

}

static int OnDebug(CURL *, curl_infotype itype, char * pData, size_t size, void *)
{
	if (itype == CURLINFO_TEXT)
	{
		//printf("[TEXT]%s\n", pData);
	}
	else if (itype == CURLINFO_HEADER_IN)
	{
		printf("[HEADER_IN]%s\n", pData);
	}
	else if (itype == CURLINFO_HEADER_OUT)
	{
		printf("[HEADER_OUT]%s\n", pData);
	}
	else if (itype == CURLINFO_DATA_IN)
	{
		printf("[DATA_IN]%s\n", pData);
	}
	else if (itype == CURLINFO_DATA_OUT)
	{
		printf("[DATA_OUT]%s\n", pData);
	}
	return 0;
}

static size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid)
{
	std::string* str = dynamic_cast<std::string*>((std::string *)lpVoid);
	if (NULL == str || NULL == buffer)
	{
		return -1;
	}

	char* pData = (char*)buffer;
	str->append(pData, size * nmemb);
	return nmemb;
}

int CHttpClient::Post(const std::string & strUrl, const std::string & strPost, std::string & strResponse)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if (m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

int CHttpClient::Get(const std::string & strUrl, std::string & strResponse)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if (m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	/**
	* 当多个线程都使用超时处理的时候，同时主线程中有sleep或是wait等操作。
	* 如果不设置这个选项，libcurl将会发信号打断这个wait从而导致程序退出。
	*/
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

int CHttpClient::Posts(const std::string & strUrl, const std::string & strPost, std::string & strResponse, const char * pCaPath)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if (m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	if (NULL == pCaPath)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		//缺省情况就是PEM，所以无需设置，另外支持DER
		//curl_easy_setopt(curl,CURLOPT_SSLCERTTYPE,"PEM");
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
	}
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

int CHttpClient::Gets(const std::string & strUrl, std::string & strResponse, const char * pCaPath)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if (m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	if (NULL == pCaPath)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
	}
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

#include "../include/Charset/CharsetConvertSTD.h"

int CHttpClient::SendHttpProtocol(std::string sSend, std::string &sRecv, bool bLog)
{
	if (bLog)
	{
		//g_NetLog.Debug("[H323Channel::SendHttpProtocol] send content: %s\n", sSend.c_str());
	}
	int iRet = -1;
	std::string strUrl = "http://127.0.0.1:90/activeProtocol.action";

	CURL *curl = NULL;
	CURLcode res;

	curl = curl_easy_init();
	if (curl != NULL)
	{
		curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str()); //url地址  
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, client_write_data); //对返回的数据进行操作的函数地址  
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &sRecv); //这是write_data的第四个参数值  
		curl_easy_setopt(curl, CURLOPT_POST, 1); //设置为非0表示本次操作为post  
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1); //设置为非0,响应头信息location
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

		// 设置要POST的JSON数据  
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, sSend.c_str());
		
		res = curl_easy_perform(curl);

		if (res != CURLE_OK)
		{
			switch (res)
			{
			case CURLE_UNSUPPORTED_PROTOCOL:
				fprintf(stderr, "不支持的协议,由URL的头部指定\n");
			case CURLE_COULDNT_CONNECT:
				fprintf(stderr, "不能连接到remote主机或者代理\n");
			case CURLE_HTTP_RETURNED_ERROR:
				fprintf(stderr, "http返回错误\n");
			case CURLE_READ_ERROR:
				fprintf(stderr, "读本地文件错误\n");
			default:
				fprintf(stderr, "返回值:%d\n", res);
			}
		}
		else
		{
			if (bLog)
			{
				//g_NetLog.Debug("[H323Channel::SendHttpProtocol] recv content: %s\n", sRecv.c_str());
			}
			iRet = 0;
		}

		curl_easy_cleanup(curl);
	}
	return 0;
}


int CHttpClient::BurnServerConnect(CString& strRecv, int nCallCloseDisk)
{
	try
	{
		Object::Ptr pObj = new Object(true);
		Object::Ptr pObj1 = new Object(true);
		pObj1->set("CloseDisk", nCallCloseDisk);
		pObj1->set("videoEncodeType", 8);
		pObj1->set("logicNo", 88);
		pObj->set("params", pObj1);
		pObj->set("method", "testProtocol");

		std::stringstream ss;
		pObj->stringify(ss);
		std::string sSend = ss.str();
		std::string sRecv = "";

		if (SendHttpProtocol(sSend, sRecv) == 0)
		{		
			CString str(sRecv.c_str());
			strRecv = str;//strRecv.Format(L"%s", sRecv.c_str());
		}
		return 0;
	}
	catch (...)
	{
		//g_NetLog.Debug("%s catched\n", __PRETTY_FUNCTION__);
		return -1;
	}
}

void CHttpClient::SetDebug(bool bDebug)
{
	m_bDebug = bDebug;
}

//////////////////////////////////////////////////////////////////////////
int CHttpClient::SendGetCDRomListProtocol(CString& strSend, CString& strRecv)
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

		if (SendHttpProtocol(sSend, sRecv) == 0)
		{
			CString strS(sSend.c_str());
			strSend = strS;
			CString strR(sRecv.c_str());
			strRecv = strR;
		}
		return 0;
	}
	catch (...)
	{
		//g_NetLog.Debug("%s catched\n", __PRETTY_FUNCTION__);
		return -1;
	}
}

int CHttpClient::SendStartBurnProtocol(CString& strSend, CString& strRecv)
{
	try
	{
		Object::Ptr pObj = new Object(true);
		pObj->set("method", "startBurn");
		
		Object::Ptr pParams = new Object(true);
		pParams->set("burnMode", "singleBurn");
	
		pParams->set("alarmSize", 300);
		pParams->set("burnType", "fileBurn");

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
		//fileInfo
		JSON::Array burnFileArray;
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

		Object::Ptr	burnFileList = new Object(true);
		burnFileList->set("burnFileList", burnFileArray);
		pParams->set("fileInfo", burnFileList);
		
		pParams->set("burnSpeed", 8);

		Object::Ptr pFeedbackParam = new Object(true);
		pFeedbackParam->set("needFeedback", "yes");
		pFeedbackParam->set("feedbackIP", "192.168.253.6");
		pFeedbackParam->set("feedbackPort", 12345);
		pFeedbackParam->set("transType", "http");
		pFeedbackParam->set("feedIterval", 2000);
		
		pParams->set("feedback", pFeedbackParam);

		pObj->set("params", pParams);

		std::stringstream ss;
		pObj->stringify(ss);
		std::string sSend = ss.str();
		std::string sRecv = "";
		if (SendHttpProtocol(sSend, sRecv) == 0)
		{
			CString strS(sSend.c_str());
			strSend = strS; 
			CString str(sRecv.c_str());
			strRecv = str;
		}
		return 0;
	}
	catch (...)
	{
		//g_NetLog.Debug("%s catched\n", __PRETTY_FUNCTION__);
		return -1;
	}
}

int CHttpClient::SendPauseBurnProtocol(CString& strSend, CString& strRecv)
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

		if (SendHttpProtocol(sSend, sRecv) == 0)
		{
			CString strS(sSend.c_str());
			strSend = strS;
			CString strR(sRecv.c_str());
			strRecv = strR;
		}
		return 0;
	}
	catch (...)
	{
		//g_NetLog.Debug("%s catched\n", __PRETTY_FUNCTION__);
		return -1;
	}
}

int CHttpClient::SendRemuseBurnProtocol(CString& strSend, CString& strRecv)
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

		if (SendHttpProtocol(sSend, sRecv) == 0)
		{
			CString strS(sSend.c_str());
			strSend = strS;
			CString strR(sRecv.c_str());
			strRecv = strR;
		}
		return 0;
	}
	catch (...)
	{
		//g_NetLog.Debug("%s catched\n", __PRETTY_FUNCTION__);
		return -1;
	}
}

int CHttpClient::SendStopBurnProtocol(CString& strSend, CString& strRecv)
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

		if (SendHttpProtocol(sSend, sRecv) == 0)
		{
			CString strS(sSend.c_str());
			strSend = strS;
			CString strR(sRecv.c_str());
			strRecv = strR;
		}
		return 0;
	}
	catch (...)
	{
		//g_NetLog.Debug("%s catched\n", __PRETTY_FUNCTION__);
		return -1;
	}
}

int CHttpClient::SendGetCDRomInfoProtocol(CString& strSend, CString& strRecv)
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

		if (SendHttpProtocol(sSend, sRecv) == 0)
		{
			CString strS(sSend.c_str());
			strSend = strS;
			CString strR(sRecv.c_str());
			strRecv = strR;
		}
		return 0;
	}
	catch (...)
	{
		//g_NetLog.Debug("%s catched\n", __PRETTY_FUNCTION__);
		return -1;
	}
}

int CHttpClient::SendAddBurnFileProtocol(CString& strSend, CString& strRecv)
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

		if (SendHttpProtocol(sSend, sRecv) == 0)
		{
			CString strS(sSend.c_str());
			strSend = strS;
			CString strR(sRecv.c_str());
			strRecv = strR;
		}
		return 0;
	}
	catch (...)
	{
		//g_NetLog.Debug("%s catched\n", __PRETTY_FUNCTION__);
		return -1;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

