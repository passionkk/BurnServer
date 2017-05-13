#include "HttpClient.h"
#ifdef WIN32
#include <windows.h>
#endif
#include "libcurl/curl.h"
#include <string>

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
	* ������̶߳�ʹ�ó�ʱ�����ʱ��ͬʱ���߳�����sleep����wait�Ȳ�����
	* ������������ѡ�libcurl���ᷢ�źŴ�����wait�Ӷ����³����˳���
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
		//ȱʡ�������PEM�������������ã�����֧��DER
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
		curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str()); //url��ַ  
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, client_write_data); //�Է��ص����ݽ��в����ĺ�����ַ  
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &sRecv); //����write_data�ĵ��ĸ�����ֵ  
		curl_easy_setopt(curl, CURLOPT_POST, 1); //����Ϊ��0��ʾ���β���Ϊpost  
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1); //����Ϊ��0,��Ӧͷ��Ϣlocation
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

		// ����ҪPOST��JSON����  
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, sSend.c_str());


		res = curl_easy_perform(curl);

		if (res != CURLE_OK)
		{
			switch (res)
			{
			case CURLE_UNSUPPORTED_PROTOCOL:
				fprintf(stderr, "��֧�ֵ�Э��,��URL��ͷ��ָ��\n");
			case CURLE_COULDNT_CONNECT:
				fprintf(stderr, "�������ӵ�remote�������ߴ���\n");
			case CURLE_HTTP_RETURNED_ERROR:
				fprintf(stderr, "http���ش���\n");
			case CURLE_READ_ERROR:
				fprintf(stderr, "�������ļ�����\n");
			default:
				fprintf(stderr, "����ֵ:%d\n", res);
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


int CHttpClient::BurnServerConnect(std::string& strRecv)
{
	try
	{
		Object::Ptr pObj = new Object(true);
		Object::Ptr pObj1 = new Object(true);
		pObj1->set("videoEncodeType", 8);
		pObj1->set("logicNo", 88);
		pObj->set("params", pObj1);
		pObj->set("method", "testProtocol");

		std::stringstream ss;
		pObj->stringify(ss);
		std::string sSend = ss.str();
		
		if (SendHttpProtocol(sSend, strRecv) == 0)
		{		
			printf("send BurnServer Connect success.\n");
		}

	}
	catch (...)
	{
		//g_NetLog.Debug("%s catched\n", __PRETTY_FUNCTION__);
	}
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////

void CHttpClient::SetDebug(bool bDebug)
{
	m_bDebug = bDebug;
}

