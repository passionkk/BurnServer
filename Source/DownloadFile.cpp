#include "DownloadFile.h"
#include "libcurl/curl.h"

DownloadFile::DownloadFile()
{
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
		FILE* pFile = fopen(strDestUrl.c_str(), "wb");
		if (pFile != NULL)
		{
			curl_easy_setopt(pCurl, CURLOPT_URL, strSrcUrl.c_str());
			curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, this->WriteFileData);
			curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, pFile);
			curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 60);	//time out
			CURLcode res = curl_easy_perform(pCurl);
			if (res != CURLE_OK)
				printf("curl download file error, srcFile is %s, errorNo:%d.\n", strSrcUrl, (int)res);
			curl_easy_cleanup(pCurl);
			fclose(pFile);
		}
	}
	return nRet;
}

int DownloadFile::CurlDownloadDir(std::string strSrcUrl, std::string strDestUrl)
{
	int nRet = 0;

	return nRet;
}

size_t	DownloadFile::WriteFileData(void *ptr, size_t size, size_t nmemb, FILE *stream)
{

}