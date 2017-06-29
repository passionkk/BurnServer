#ifndef _FILE_UTIL_H_
#define _FILE_UTIL_H_

#include <string>

#ifndef UINT64
#ifdef WIN32
typedef unsigned __int64    UINT64;
#else
typedef unsigned long long	UINT64;
#endif
#endif//UINT64

#ifndef INT64
#ifdef WIN32
typedef __int64				INT64;
#else
typedef signed long long	INT64;
#endif
#endif//INT64

class FileUtil
{
public:
    static bool DelFile(const char *szFilePath);
    static bool FileExist(const char *szFilePath);
    static INT64 FileSize(const char *szFilePath);
    static std::string GetFileName(std::string strFilePath);
    /**
     *
     * @strFilePath
     * full path or only file name
     * 
     * 
     */
    static std::string GetFileNameNoExtent(std::string strFilePath);
public:
#ifdef WIN32
    static bool DelFileW(const wchar_t *szFilePath);
    static bool FileExistW(const wchar_t *szFilePath);
    static INT64 FileSizeW(const wchar_t *szFilePath);
    static std::wstring GetFileNameW(std::wstring strFilePath);
    static std::wstring GetFileNameNoExtentW(std::wstring strFilePath);
#endif//WIN32
private:
    FileUtil();
    ~FileUtil();
};

#endif//_FILE_UTIL_H_
