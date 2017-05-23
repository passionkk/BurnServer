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

	static size_t	WriteFileData(void *ptr, size_t size, size_t nmemb, DownloadFile *stream);
	size_t DoWriteFileData(void *ptr, size_t size, size_t nmemb);
private:
	FILE* m_pFile;
};

