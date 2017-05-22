#pragma once
#include <string>

class DownloadFile
{
public:
	DownloadFile();
	virtual ~DownloadFile();

public:
	int CurlDownloadFile(std::string strSrcUrl, std::string strDestUrl);
	int CurlDownloadDir(std::string strSrcUrl, std::string strDestUrl);

	size_t	WriteFileData(void *ptr, size_t size, size_t nmemb, FILE *stream);
};

