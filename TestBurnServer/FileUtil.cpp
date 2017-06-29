#include "stdafx.h"
#include "FileUtil.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

#include "../Charset/CharsetConvertMFC.h"


FileUtil::FileUtil()
{

}

FileUtil::~FileUtil()
{

}

bool FileUtil::DelFile(const char *szFilePath)
{
    if (remove(szFilePath) == 0)
    {
        return true;
    }

    return false;
}

bool FileUtil::FileExist(const char *szFilePath)
{
#ifdef WIN32
    struct _stat64 thestat;

    if (szFilePath != NULL)
    {
        if(::_stat64(szFilePath, &thestat) >= 0)
        {
            if((thestat.st_mode&_S_IFREG))
            {
                return true;
            }
        }
    }

    return false;
#else
    struct stat64 thestat;

    if (szFilePath != NULL)
    {
        if(::stat64(szFilePath, &thestat) >= 0)
        {
            if(S_ISREG(thestat.st_mode))
            {
                return true;
            }
        }
    }

    return false;
#endif
}

INT64 FileUtil::FileSize(const char *szFilePath)
{
    INT64  nFileSize = -1;
#ifdef WIN32
    //struct _stat64 thestat;
    //if(::_stat64(szFilePath, &thestat) >= 0)
    //{
    //    nFileSize = thestat.st_size;
    //}
    
    CStringW str=CharsetConvertMFC::UTF8ToUTF16(szFilePath);

    struct _stat64 thestat;
    if(::_wstat64(str.GetBuffer(), &thestat) >= 0)
    {
        nFileSize = thestat.st_size;
    }
#else
    struct stat64 thestat;
    if(::stat64(szFilePath, &thestat) >= 0)
    {
        nFileSize = thestat.st_size;
    }
#endif//WIN32
    return nFileSize;
}

std::string FileUtil::GetFileName(std::string strFilePath)
{
    std::string strFileName;

    if (strFilePath.length() >= 2 && strFilePath.at(1) == ':')
    {//windows path
        size_t nPos = strFilePath.rfind('\\');
        if (nPos != std::string::npos && (nPos < strFilePath.length()-1))
        {
            nPos = nPos + 1;
            strFileName = strFilePath.substr(nPos, strFilePath.length()-nPos);
        }
    }
    else if (strFilePath.length() >= 1 && strFilePath.at(0) == '/')
    {
        size_t nPos = strFilePath.rfind('/');
        if (nPos != std::string::npos && (nPos < strFilePath.length()-1))
        {
            nPos = nPos + 1;
            strFileName = strFilePath.substr(nPos, strFilePath.length()-nPos);
        }
    }

    return strFileName;
}

std::string FileUtil::GetFileNameNoExtent(std::string strFilePath)
{
    std::string strFileNameNoExtent;
    std::string strFileName = FileUtil::GetFileName(strFilePath);
    if (strFileName == "")
    {
        strFileName = strFilePath;
    }

    size_t nPos;
    nPos = strFileName.rfind('.');
    if (nPos == std::string::npos)
    {
        strFileNameNoExtent = strFileName;
    }
    else
    {
        strFileNameNoExtent = strFileName.substr(0, nPos);
    }

    return strFileNameNoExtent;
}

#ifdef WIN32
bool FileUtil::DelFileW(const wchar_t *szFilePath)
{
    if (_wremove(szFilePath) == 0)
    {
        return true;
    }

    return false;
}

bool FileUtil::FileExistW(const wchar_t *szFilePath)
{
    struct _stat64 thestat;

    if (szFilePath != NULL)
    {
        if(::_wstat64(szFilePath, &thestat) >= 0)
        {
            if((thestat.st_mode&_S_IFREG))
            {
                return true;
            }
        }
    }

    return false;
}

INT64 FileUtil::FileSizeW(const wchar_t *szFilePath)
{
    INT64  nFileSize = 0;

    struct _stat64 thestat;
    if(::_wstat64(szFilePath, &thestat) >= 0)
    {
        nFileSize = thestat.st_size;
    }

    return nFileSize;
}

std::wstring FileUtil::GetFileNameW(std::wstring strFilePath)
{
    std::wstring strFileName;

    if (strFilePath.length() >= 2 && strFilePath.at(1) == L':')
    {//windows path
        size_t nPos = strFilePath.rfind(L'\\');
        if (nPos != std::string::npos && (nPos < strFilePath.length()-1))
        {
            nPos = nPos + 1;
            strFileName = strFilePath.substr(nPos, strFilePath.length()-nPos);
        }
    }

    return strFileName;
}

std::wstring FileUtil::GetFileNameNoExtentW(std::wstring strFilePath)
{
    std::wstring strFileNameNoExtent;
    std::wstring strFileName = FileUtil::GetFileNameW(strFilePath);
    if (strFileName == L"")
    {
        strFileName = strFilePath;
    }

    size_t nPos;
    nPos = strFileName.rfind(L'.');
    if (nPos == std::string::npos)
    {
        strFileNameNoExtent = strFileName;
    }
    else
    {
        strFileNameNoExtent = strFileName.substr(0, nPos);
    }

    return strFileNameNoExtent;
}

#endif//WIN32

