#include "DownloadFile.h"
#include "libcurl/curl.h"

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
			curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 60);	//time out
			CURLcode res = curl_easy_perform(pCurl);
			if (res != CURLE_OK)
				printf("curl download file error, srcFile is %s, errorNo:%d.\n", strSrcUrl, (int)res);
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
		return fwrite((const char*)ptr, size, nmemb, m_pFile);
	return 0;
}