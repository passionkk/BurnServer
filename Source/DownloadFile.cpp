#include "DownloadFile.h"
#include "curl/curl.h"

DownloadFile::DownloadFile()
{
	m_pFile = NULL;
}

DownloadFile::~DownloadFile()
{
}

int DownloadFile::CurlDownloadFile(std::string strSrcUrl, std::string strDestUrl)
{
	int nRet = -1;
	CURL* pCurl = curl_easy_init();
	if (pCurl)
	{
		m_pFile = fopen(strDestUrl.c_str(), "wb");
		if (m_pFile != NULL)
		{
			curl_easy_setopt(pCurl, CURLOPT_URL, strSrcUrl.c_str());
			curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, DownloadFile::WriteFileData);
			curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, this);
			//curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 60);	//time out
			curl_easy_setopt(pCurl, CURLOPT_CONNECTTIMEOUT, 5);
			curl_easy_setopt(pCurl, CURLOPT_NOSIGNAL, 1);
			CURLcode res = curl_easy_perform(pCurl);
			/*if (res != CURLE_OK)
				printf("curl download file error, srcFile is %s, errorNo:%d.\n", strSrcUrl.c_str(), (int)res);
				curl_easy_cleanup(pCurl);
				fclose(m_pFile);*/
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
				//if (bLog)
				{
					//g_NetLog.Debug("[H323Channel::SendHttpProtocol] recv content: %s\n", sRecv.c_str());
					fprintf(stderr, "recv content. \n");
				}
				nRet = 0;
			}
			curl_easy_cleanup(pCurl);
			fclose(m_pFile);
		}
	}
	return nRet;
}

int DownloadFile::CurlDownloadDir(std::string strSrcUrl, std::string strDestUrl)
{
	int nRet = 0;

	return nRet;
}

size_t	DownloadFile::WriteFileData(void *ptr, size_t size, size_t nmemb, DownloadFile *stream)
{
	DownloadFile* pThis = (DownloadFile*)stream;
	return pThis->DoWriteFileData(ptr, size, nmemb);
}

size_t DownloadFile::DoWriteFileData(void *ptr, size_t size, size_t nmemb)
{
	if (m_pFile)
		int nRet = fwrite((const char*)ptr, size, nmemb, m_pFile);
	return size*nmemb;
}