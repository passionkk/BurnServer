#include "stdafx.h"
#include "DirectoryUtil.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>
#include <direct.h>


const int DirectoryUtil::msc_nMaxFilePath = 1024;

DirectoryUtil::DirectoryUtil()
{

}

DirectoryUtil::~DirectoryUtil()
{
    //
}

bool DirectoryUtil::CreateDir(const char *szDirPath)
{
    if (
        (szDirPath != NULL)
        && ((int)strlen(szDirPath) < msc_nMaxFilePath)
        )
    {
        char sPath[msc_nMaxFilePath];
        char *pTemp;

        memset(sPath, 0, msc_nMaxFilePath);
        //copy szDirPath to sPath and change '\' to '/'
        for (size_t i = 0; i < strlen(szDirPath); i ++)
        {
            if (*(szDirPath+i) == '\\')
            {
                sPath[i] = '/';
            }
            else
            {
                sPath[i] = *(szDirPath+i);
            }
        }

        //
        pTemp = sPath;
        while (*pTemp == '/')
        {// skip '/' in header
            pTemp ++;
        }
        while (*pTemp != '\0')
        {
            if (*pTemp == '/')
            {
                *pTemp = '\0';
                //create dir
                if (MakeDir(sPath) != 0)
                {
                    *pTemp = '/';
                    break;
                }
                *pTemp = '/';
            }
            pTemp ++;
        }

        if (MakeDir(sPath) == 0)
        {
            return true;
        }
    }

    return false;
}

bool DirectoryUtil::DeleteDir(const char *szDirPath)
{
#ifdef WIN32
    if (szDirPath != NULL)
    {
        std::string strDirPath = EnsureNoSlashEnd(szDirPath);
        char szTempDirPath[msc_nMaxFilePath+4];
        memset(szTempDirPath, 0, sizeof(szTempDirPath));
        strncpy(szTempDirPath, strDirPath.c_str(), msc_nMaxFilePath);

        SHFILEOPSTRUCTA fileOp = {0};
        fileOp.fFlags = FOF_NOCONFIRMATION;
        fileOp.pFrom = szTempDirPath;
        fileOp.pTo = NULL;
        fileOp.wFunc = FO_DELETE;
        if (SHFileOperationA(&fileOp) == 0)
        {
            return true;
        }
    }

    return false;
#else
    if (szDirPath != NULL)
    {
        char szPath[msc_nMaxFilePath];

        sprintf(szPath, "rm -rf %s", szDirPath);
        if (system(szPath) == 0)
        {
            return true;
        }
    }

    return false;
#endif//_WIN32_
}

bool DirectoryUtil::IsDirExist(const char *szDirPath)
{
#ifdef WIN32
    struct _stat64 thestat;

    if (szDirPath != NULL)
    {
        std::string strDirPath = EnsureNoSlashEnd(szDirPath);
        if(::_stat64(strDirPath.c_str(), &thestat) >= 0)
        {
            if((thestat.st_mode&_S_IFDIR))
            {
                return true;
            }
        }
    }

    return false;
#else
    struct stat64 thestat;

    if (szDirPath != NULL)
    {
        std::string strDirPath = EnsureNoSlashEnd(szDirPath);
        if(::stat64(strDirPath.c_str(), &thestat) >= 0)
        {
            if(S_ISDIR(thestat.st_mode))
            {
                return true;
            }
        }
    }

    return false;
#endif
}

bool DirectoryUtil::CopyDirToDir(const char *szDstDir, const char *szSrcDir)
{
#ifdef WIN32
    if (szSrcDir != NULL && szDstDir != NULL)
    {
        if (CreateDir(szDstDir))
        {
            char szSrcTemp[msc_nMaxFilePath+4];
            memset(szSrcTemp, 0, sizeof(szSrcTemp));
            strncpy(szSrcTemp, szSrcDir, msc_nMaxFilePath);

            char szDstTemp[msc_nMaxFilePath+4];
            memset(szDstTemp, 0, sizeof(szDstTemp));
            strncpy(szDstTemp, szDstDir, msc_nMaxFilePath);

            SHFILEOPSTRUCTA fileOp = {0};
            fileOp.fFlags = FOF_NOCONFIRMATION;
            fileOp.pFrom = szSrcTemp;
            fileOp.pTo = szDstTemp;
            fileOp.wFunc = FO_COPY;
            if (SHFileOperationA(&fileOp) == 0)
            {
                return true;
            }
        }
    }

    return false;
#else
    if (szSrcDir != NULL && szDstDir != NULL)
    {
        if (CreateDir(szDstDir))
        {
            char    szCmd[msc_nMaxFilePath*2];

            memset(szCmd, 0, msc_nMaxFilePath*2);
            sprintf(szCmd, "cp -rf %s %s", szSrcDir, szDstDir);
            if (system(szCmd) == 0)
            {
                return true;
            }
        }
    }

    return false;
#endif//_WIN32_
}

bool DirectoryUtil::CopyFileToDir(const char *szDstDir, std::vector<std::string> vectSrcFiles)
{
#ifdef WIN32
    bool bReturn = false;

    if (szDstDir != NULL)
    {
        if (CreateDir(szDstDir))
        {
            for (size_t i = 0; i < vectSrcFiles.size(); i ++)
            {
                char szSrcTemp[msc_nMaxFilePath+4];
                memset(szSrcTemp, 0, sizeof(szSrcTemp));
                strncpy(szSrcTemp, vectSrcFiles.at(i).c_str(), msc_nMaxFilePath);

                char szDstTemp[msc_nMaxFilePath+4];
                memset(szDstTemp, 0, sizeof(szDstTemp));
                strncpy(szDstTemp, szDstDir, msc_nMaxFilePath);

                SHFILEOPSTRUCTA fileOp = {0};
                fileOp.fFlags = FOF_NOCONFIRMATION;
                fileOp.pFrom = szSrcTemp;
                fileOp.pTo = szDstTemp;
                fileOp.wFunc = FO_COPY;
                if (SHFileOperationA(&fileOp) == 0)
                {
                    bReturn = true;
                }
                else
                {
                    bReturn = false;
                    break;
                }
            }
        }//CreateDir(szDstDir)
    }

    return bReturn;
#else
    bool bReturn = false;

    if (szDstDir != NULL)
    {
        if (CreateDir(szDstDir))
        {
            for (size_t i = 0; i < vectSrcFiles.size(); i ++)
            {
                char    szCmd[msc_nMaxFilePath*2];

                memset(szCmd, 0, msc_nMaxFilePath*2);
                sprintf(szCmd, "cp -rf %s %s", vectSrcFiles.at(i).c_str(), szDstDir);
                if (system(szCmd) == 0)
                {
                    bReturn = true;
                }
                else
                {
                    bReturn = false;
                    break;
                }
            }
        }//CreateDir(szDstDir)
    }

    return bReturn;
#endif//WIN32
}

bool DirectoryUtil::CopyFileToDir(const char *szDstDir, std::vector<std::string> vectSrcFiles, std::vector<std::string> vectDstFiles)
{
#ifdef WIN32
    bool bReturn = false;

    if (szDstDir != NULL)
    {
        if (CreateDir(szDstDir))
        {
            if (vectSrcFiles.size() == vectDstFiles.size())
            {
                for (size_t i = 0; i < vectSrcFiles.size(); i ++)
                {
                    char szSrcTemp[msc_nMaxFilePath+4];
                    memset(szSrcTemp, 0, sizeof(szSrcTemp));
                    strncpy(szSrcTemp, vectSrcFiles.at(i).c_str(), msc_nMaxFilePath);

                    char szDstTemp[msc_nMaxFilePath+4];
                    memset(szDstTemp, 0, sizeof(szDstTemp));
                    strncpy(szDstTemp, szDstDir, msc_nMaxFilePath);
                    strcat(szDstTemp, vectDstFiles.at(i).c_str());

                    SHFILEOPSTRUCTA fileOp = {0};
                    fileOp.fFlags = FOF_NOCONFIRMATION;
                    fileOp.pFrom = szSrcTemp;
                    fileOp.pTo = szDstTemp;
                    fileOp.wFunc = FO_COPY;
                    if (SHFileOperationA(&fileOp) == 0)
                    {
                        bReturn = true;
                    }
                    else
                    {
                        bReturn = false;
                        break;
                    }
                }
            }
        }//CreateDir(szDstDir)
    }

    return bReturn;
#else
    bool bReturn = false;

    if (szDstDir != NULL)
    {
        if (CreateDir(szDstDir))
        {
            if (vectSrcFiles.size() == vectDstFiles.size())
            {
                for (size_t i = 0; i < vectSrcFiles.size(); i ++)
                {
                    char    szCmd[msc_nMaxFilePath*2];

                    memset(szCmd, 0, msc_nMaxFilePath*2);
                    sprintf(szCmd, "cp -rf %s %s%s", vectSrcFiles.at(i).c_str(), szDstDir, vectDstFiles.at(i).c_str());
                    if (system(szCmd) == 0)
                    {
                        bReturn = true;
                    }
                    else
                    {
                        bReturn = false;
                        break;
                    }
                }
            }
        }//CreateDir(szDstDir)
    }

    return bReturn;
#endif//WIN32
}

UINT64 DirectoryUtil::GetDirSize(const char *szDirPath)
{
    std::vector<std::string>        vectFileList;
    UINT64                          nTotalSize;

    nTotalSize = 0;
    GetFileList(szDirPath, vectFileList);
    for (size_t i = 0; i < vectFileList.size(); i ++)
    {
#ifdef WIN32
        struct _stat64 thestat;
        if(::_stat64(vectFileList.at(i).c_str(), &thestat) >= 0)
        {
            nTotalSize += thestat.st_size;
        }
#else
        struct stat64 thestat;
        if(::stat64(vectFileList.at(i).c_str(), &thestat) >= 0)
        {
            nTotalSize += thestat.st_size;
        }
#endif//WIN32
    }

    return nTotalSize;
}

bool DirectoryUtil::GetFileList(std::string strDirPath, std::vector<std::string> &vectFileList)
{
#ifdef WIN32
    WIN32_FIND_DATAA ffd;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    std::string     strTempDir;
    std::string     strCurFileFullPath;

    strDirPath = EnsureSlashEnd(strDirPath);
    strTempDir = strDirPath;
    strTempDir += "*";

    hFind = FindFirstFileA(strTempDir.c_str(), &ffd);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do 
        {
            if (ffd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
            {
                if (ffd.cFileName[0] != '.')
                {
                    strTempDir = strDirPath + ffd.cFileName;
                    strTempDir += "\\";
                    GetFileList(strTempDir, vectFileList);
                }
            }
            else if (ffd.dwFileAttributes&FILE_ATTRIBUTE_ARCHIVE)
            {
                strCurFileFullPath = strDirPath + ffd.cFileName;
                vectFileList.push_back(strCurFileFullPath);
            }
        } while (FindNextFileA(hFind, &ffd));
    }

    FindClose(hFind);
#else

    struct dirent *pEntry = NULL;
    DIR	*pDir = NULL;

    strDirPath = EnsureSlashEnd(strDirPath) ;
    pDir = opendir(strDirPath.c_str());
    if (pDir != NULL)
    {
        pEntry = readdir(pDir);
        while (pEntry != NULL)
        {
            if (pEntry->d_type == DT_REG)
            {
                vectFileList.push_back(strDirPath+pEntry->d_name);
            }
            else if (pEntry->d_type == DT_DIR)
            {
                if (pEntry->d_name[0] != '.')
                {
                    GetFileList(strDirPath+pEntry->d_name+"/", vectFileList);
                }
            }
            pEntry = readdir(pDir);
        }
        closedir(pDir);
    }

#endif//WIN32

    return true;
}

bool DirectoryUtil::GetFileListNoSub(std::string strDirPath, std::vector<std::string> &vectFileList)
{
#ifdef WIN32
    WIN32_FIND_DATAA ffd;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    std::string     strTempDir;
    std::string     strCurFileFullPath;

    strDirPath = EnsureSlashEnd(strDirPath);
    strTempDir = strDirPath;
    strTempDir += "*";

    hFind = FindFirstFileA(strTempDir.c_str(), &ffd);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do 
        {
            if (ffd.dwFileAttributes&FILE_ATTRIBUTE_ARCHIVE)
            {
                strCurFileFullPath = strDirPath + ffd.cFileName;
                vectFileList.push_back(strCurFileFullPath);
            }
        } while (FindNextFileA(hFind, &ffd));
    }

    FindClose(hFind);
#else

    struct dirent *pEntry = NULL;
    DIR	*pDir = NULL;

    strDirPath = EnsureSlashEnd(strDirPath) ;
    pDir = opendir(strDirPath.c_str());
    if (pDir != NULL)
    {
        pEntry = readdir(pDir);
        while (pEntry != NULL)
        {
            if (pEntry->d_type == DT_REG)
            {
                vectFileList.push_back(strDirPath+pEntry->d_name);
            }
            pEntry = readdir(pDir);
        }
        closedir(pDir);
    }

#endif//WIN32

    return true;
}

bool DirectoryUtil::GetFileNameList(std::string strDirPath, std::vector<std::string> &vectFileNameList)
{
#ifdef WIN32
    WIN32_FIND_DATAA ffd;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    std::string     strTempDir;

    strDirPath = EnsureSlashEnd(strDirPath);
    strTempDir = strDirPath;
    strTempDir += "*";

    hFind = FindFirstFileA(strTempDir.c_str(), &ffd);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do 
        {
            if (ffd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
            {
                if (ffd.cFileName[0] != '.')
                {
                    strTempDir = strDirPath + ffd.cFileName;
                    strTempDir += "\\";
                    GetFileNameList(strTempDir, vectFileNameList);
                }
            }
            else if (ffd.dwFileAttributes&FILE_ATTRIBUTE_ARCHIVE)
            {
                vectFileNameList.push_back(ffd.cFileName);
            }
        } while (FindNextFileA(hFind, &ffd));
    }

    FindClose(hFind);
#else

    struct dirent *pEntry = NULL;
    DIR	*pDir = NULL;

    strDirPath = EnsureSlashEnd(strDirPath) ;
    pDir = opendir(strDirPath.c_str());
    if (pDir != NULL)
    {
        pEntry = readdir(pDir);
        while (pEntry != NULL)
        {
            if (pEntry->d_type == DT_REG)
            {
                vectFileNameList.push_back(pEntry->d_name);
            }
            else if (pEntry->d_type == DT_DIR)
            {
                if (pEntry->d_name[0] != '.')
                {
                    GetFileNameList(strDirPath+pEntry->d_name+"/", vectFileNameList);
                }
            }
            pEntry = readdir(pDir);
        }
        closedir(pDir);
    }

#endif//WIN32

    return true;
}

bool DirectoryUtil::GetFileNameListNoSub(std::string strDirPath, std::vector<std::string> &vectFileNameList)
{
#ifdef WIN32
    WIN32_FIND_DATAA ffd;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    std::string     strTempDir;

    strDirPath = EnsureSlashEnd(strDirPath);
    strTempDir = strDirPath;
    strTempDir += "*";

    hFind = FindFirstFileA(strTempDir.c_str(), &ffd);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do 
        {
            if (ffd.dwFileAttributes&FILE_ATTRIBUTE_ARCHIVE)
            {
                vectFileNameList.push_back(ffd.cFileName);
            }
        } while (FindNextFileA(hFind, &ffd));
    }

    FindClose(hFind);
#else

    struct dirent *pEntry = NULL;
    DIR	*pDir = NULL;

    strDirPath = EnsureSlashEnd(strDirPath) ;
    pDir = opendir(strDirPath.c_str());
    if (pDir != NULL)
    {
        pEntry = readdir(pDir);
        while (pEntry != NULL)
        {
            if (pEntry->d_type == DT_REG)
            {
                vectFileNameList.push_back(pEntry->d_name);
            }
            pEntry = readdir(pDir);
        }
        closedir(pDir);
    }

#endif//WIN32

    return true;
}

bool DirectoryUtil::GetPartitionList(std::vector<std::string> &vectPartitionList)
{
#if WIN32
    std::string strDriverName;
    DWORD dwDrivers = GetLogicalDrives();

    for (int i = 0; i < 'Z'-'A'; i ++)
    {
        if ((dwDrivers&(1<<i)) != 0)
        {
            strDriverName = char('A'+i);
            strDriverName += ":\\";
            if (GetPartitionTotalSize(strDriverName.c_str()) > 0)
            {
                vectPartitionList.push_back(strDriverName);
            }
        }
    }

    return true;
#else
    return false;
#endif
}

INT64 DirectoryUtil::GetPartitionTotalSize(const char *szPartitionPath)
{
#ifdef WIN32
    INT64 nSize = -1;

    ULARGE_INTEGER totalSpace;
    ULARGE_INTEGER freeSpace;
    if(GetDiskFreeSpaceExA(szPartitionPath, &freeSpace, &totalSpace, 0))
    {
        nSize = totalSpace.QuadPart;
    }

    return nSize;
#else
    return -1;
#endif//WIN32
}

INT64 DirectoryUtil::GetPartitionFreeSize(const char *szPartitionPath)
{
#ifdef WIN32
    INT64 nSize = -1;

    ULARGE_INTEGER totalSpace;
    ULARGE_INTEGER freeSpace;
    if(GetDiskFreeSpaceExA(szPartitionPath, &freeSpace, &totalSpace, 0))
    {
        nSize = freeSpace.QuadPart;
    }

    return nSize;
#else
    return -1;
#endif//WIN32
}

std::string DirectoryUtil::GetPartitionPath(std::string strDirPath)
{
#ifdef WIN32
    if (strDirPath.length() >= 3)
    {
        return strDirPath.substr(0, 3);
    }

    return "";
#else
    return "";
#endif//WIN32
}

std::string DirectoryUtil::EnsureSlashEnd(std::string strDirPath)
{
    if (strDirPath.length() >= 2 && strDirPath.at(1) == ':')
    {//windows path
        if (strDirPath.rfind("\\") != (strDirPath.length()-1))
        {
            return strDirPath + "\\";
        }
    }
    else if (strDirPath.length() >= 1 && strDirPath.at(0) == '/')
    {
        if (strDirPath.rfind("/") != (strDirPath.length()-1))
        {
            return strDirPath + "/";
        }
    }

    return strDirPath;
}

std::string DirectoryUtil::EnsureNoSlashEnd(std::string strDirPath)
{
    if (strDirPath.length() >= 2 && strDirPath.at(1) == ':')
    {//windows path
        if (strDirPath.rfind("\\") == (strDirPath.length()-1))
        {
            return strDirPath.substr(0, strDirPath.length()-1);
        }
    }
    else if (strDirPath.length() >= 1 && strDirPath.at(0) == '/')
    {
        if (strDirPath.rfind("/") == (strDirPath.length()-1))
        {
            return strDirPath.substr(0, strDirPath.length()-1);
        }
    }

    return strDirPath;
}

std::string DirectoryUtil::GetParentDir(std::string strPath)
{
    std::string strKey = "\\";
    if (strPath.length() >= 2 && strPath.at(1) == ':')
    {//windows path
        strKey = "\\";
    }
    else if (strPath.length() >= 1 && strPath.at(0) == '/')
    {
        strKey = "/";
    }

    size_t nPos = strPath.rfind(strKey);
    if (nPos == strPath.length()-1)
    {
         nPos = strPath.rfind(strKey, nPos-1);
    }
    if (nPos != std::string::npos)
    {
        return strPath.substr(0, nPos+1);
    }
    else
    {
        return "";
    }
}

std::string DirectoryUtil::NormalWindowsPath(std::string strPath)
{
    if (strPath.length() >= 2 && strPath.at(1) == ':')
    {//windows path, replace / to \;
        std::string strDest;
        for (size_t i = 0; i < strPath.length(); i ++)
        {
            if (strPath.at(i) == '/')
            {
                strDest.push_back('\\');
            }
            else
            {
                strDest.push_back(strPath.at(i));
            }
        }
        return strDest;
    }

    return strPath;
}

#ifdef WIN32
bool DirectoryUtil::CreateDirW(const wchar_t *szDirPath)
{
    bool            bReturn = TRUE;
    wchar_t         szTempDir[msc_nMaxFilePath];
    const wchar_t   *p = szDirPath;
    wchar_t         *q = szTempDir;

    memset(szTempDir, 0, sizeof(szTempDir));
    *q++ = L'\\';
    *q++ = L'\\';
    *q++ = L'?';
    *q++ = L'\\';

    while (*p != L'\0')
    {
        if (
            (*p == L'\\') 
            || (*p == L'/')
            )
        {
            if (*(p-1) != L':')
            {
                if (
                    (!CreateDirectoryW(szTempDir, NULL))
                    && (GetLastError() != ERROR_ALREADY_EXISTS)
                    )
                {
                    bReturn = false;
                    break;
                }
            }
        }
        *q++ = *p++;
        *q = L'\0';
    }

    if (bReturn)
    {
        if (
            (*(q-1) != L'\\')
            && (*(q-1) != L'/')
            )
        {
            if (
                (!CreateDirectoryW(szTempDir, NULL))
                && (GetLastError() != ERROR_ALREADY_EXISTS)
                )
            {
                bReturn = false;
            }
        }
    }

    return bReturn;
}

bool DirectoryUtil::DeleteDirW(const wchar_t *szDirPath)
{
    if (szDirPath != NULL)
    {
        std::wstring strDirPath = EnsureNoSlashEndW(szDirPath);
        wchar_t szTempDirPath[msc_nMaxFilePath+4];
        memset(szTempDirPath, 0, sizeof(szTempDirPath));
        swprintf(szTempDirPath, strDirPath.c_str());

        SHFILEOPSTRUCTW fileOp={0};
        fileOp.fFlags = FOF_NOCONFIRMATION;
        fileOp.pFrom = szTempDirPath;
        fileOp.pTo = NULL;
        fileOp.wFunc = FO_DELETE;
        if (SHFileOperationW(&fileOp) == 0)
        {
            return true;
        }
    }

    return false;
}

bool DirectoryUtil::IsDirExistW(const wchar_t *szDirPath)
{
    struct _stat64 thestat;

    if (szDirPath != NULL)
    {
        std::wstring strDirPath = EnsureNoSlashEndW(szDirPath);
        if(::_wstat64(strDirPath.c_str(), &thestat) >= 0)
        {
            if((thestat.st_mode&_S_IFDIR))
            {
                return true;
            }
        }
    }

    return false;
}

bool DirectoryUtil::CopyDirToDirW(const wchar_t* szDstDir, const wchar_t* szSrcDir)
{
    if (szSrcDir != NULL && szDstDir != NULL)
    {
        if (CreateDirW(szDstDir))
        {
            wchar_t szSrcTemp[msc_nMaxFilePath+4];
            memset(szSrcTemp, 0, sizeof(szSrcTemp));
            wcsncpy(szSrcTemp, szSrcDir, msc_nMaxFilePath);

            wchar_t szDstTemp[msc_nMaxFilePath+4];
            memset(szDstTemp, 0, sizeof(szDstTemp));
            wcsncpy(szDstTemp, szDstDir, msc_nMaxFilePath);

            SHFILEOPSTRUCT fileOp = {0};
            fileOp.fFlags = FOF_NOCONFIRMATION;
            fileOp.pFrom = szSrcTemp;
            fileOp.pTo = szDstTemp;
            fileOp.wFunc = FO_COPY;
            if (SHFileOperation(&fileOp) == 0)
            {
                return true;
            }
        }
    }

    return false;
}

bool DirectoryUtil::CopyFileToDirW(const wchar_t *szDstDir, std::vector<std::wstring> vectSrcFiles)
{
    bool bReturn = false;

    if (szDstDir != NULL)
    {
        if (CreateDirW(szDstDir))
        {
            for (size_t i = 0; i < vectSrcFiles.size(); i ++)
            {
                wchar_t szSrcTemp[msc_nMaxFilePath+4];
                memset(szSrcTemp, 0, sizeof(szSrcTemp));
                wcsncpy(szSrcTemp, vectSrcFiles.at(i).c_str(), msc_nMaxFilePath);

                wchar_t szDstTemp[msc_nMaxFilePath+4];
                memset(szDstTemp, 0, sizeof(szDstTemp));
                wcsncpy(szDstTemp, szDstDir, msc_nMaxFilePath);

                SHFILEOPSTRUCT fileOp = {0};
                fileOp.fFlags = FOF_NOCONFIRMATION;
                fileOp.pFrom = szSrcTemp;
                fileOp.pTo = szDstTemp;
                fileOp.wFunc = FO_COPY;
                if (SHFileOperation(&fileOp) == 0)
                {
                    bReturn = true;
                }
                else
                {
                    bReturn = false;
                    break;
                }
            }
        }//CreateDir(szDstDir)
    }

    return bReturn;
}

bool DirectoryUtil::CopyFileToDirW(const wchar_t *szDstDir, std::vector<std::wstring> vectSrcFiles, std::vector<std::wstring> vectDstFiles)
{
    bool bReturn = false;

    if (szDstDir != NULL)
    {
        if (CreateDirW(szDstDir))
        {
            if (vectSrcFiles.size() == vectDstFiles.size())
            {
                for (size_t i = 0; i < vectSrcFiles.size(); i ++)
                {
                    wchar_t szSrcTemp[msc_nMaxFilePath+4];
                    memset(szSrcTemp, 0, sizeof(szSrcTemp));
                    wcsncpy(szSrcTemp, vectSrcFiles.at(i).c_str(), msc_nMaxFilePath);

                    wchar_t szDstTemp[msc_nMaxFilePath+4];
                    memset(szDstTemp, 0, sizeof(szDstTemp));
                    wcsncpy(szDstTemp, szDstDir, msc_nMaxFilePath);
                    wcscat(szDstTemp, vectDstFiles.at(i).c_str());

                    SHFILEOPSTRUCT fileOp = {0};
                    fileOp.fFlags = FOF_NOCONFIRMATION;
                    fileOp.pFrom = szSrcTemp;
                    fileOp.pTo = szDstTemp;
                    fileOp.wFunc = FO_COPY;
                    if (SHFileOperation(&fileOp) == 0)
                    {
                        bReturn = true;
                    }
                    else
                    {
                        bReturn = false;
                        break;
                    }
                }
            }
        }//CreateDir(szDstDir)
    }

    return bReturn;
}

UINT64 DirectoryUtil::GetDirSizeW(const wchar_t *szDirPath)
{
    std::vector<std::wstring>       vectFileList;
    UINT64                          nTotalSize;

    nTotalSize = 0;
    GetFileListW(szDirPath, vectFileList);
    for (size_t i = 0; i < vectFileList.size(); i ++)
    {
        struct __stat64 thestat;
        if(::_wstat64(vectFileList.at(i).c_str(), &thestat) >= 0)
        {
            nTotalSize += thestat.st_size;
        }
    }

    return nTotalSize;  
}

bool DirectoryUtil::GetFileListW(std::wstring strDirPath, std::vector<std::wstring> &vectFileList)
{
    WIN32_FIND_DATAW ffd;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    std::wstring     strTempDir;
    std::wstring     strCurFileFullPath;

    strDirPath = EnsureSlashEndW(strDirPath);
    strTempDir = strDirPath;
    strTempDir += L"*";

    hFind = FindFirstFileW(strTempDir.c_str(), &ffd);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do 
        {
            if (ffd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
            {
                if (ffd.cFileName[0] != L'.')
                {
                    strTempDir = strDirPath + ffd.cFileName;
                    strTempDir += L"\\";
                    GetFileListW(strTempDir, vectFileList);
                }
            }
            else if (ffd.dwFileAttributes&FILE_ATTRIBUTE_ARCHIVE)
            {
                strCurFileFullPath = strDirPath + ffd.cFileName;
                vectFileList.push_back(strCurFileFullPath);
            }
        } while (FindNextFileW(hFind, &ffd));
    }

    FindClose(hFind);

    return true;
}

bool DirectoryUtil::GetFileListNoSubW(std::wstring strDirPath, std::vector<std::wstring> &vectFileList)
{
    WIN32_FIND_DATAW ffd;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    std::wstring     strTempDir;
    std::wstring     strCurFileFullPath;

    strDirPath = EnsureSlashEndW(strDirPath);
    strTempDir = strDirPath;
    strTempDir += L"*";

    hFind = FindFirstFileW(strTempDir.c_str(), &ffd);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do 
        {
            if (ffd.dwFileAttributes&FILE_ATTRIBUTE_ARCHIVE)
            {
                strCurFileFullPath = strDirPath + ffd.cFileName;
                vectFileList.push_back(strCurFileFullPath);
            }
        } while (FindNextFileW(hFind, &ffd));
    }

    FindClose(hFind);

    return true;
}

bool DirectoryUtil::GetFileNameListW(std::wstring strDirPath, std::vector<std::wstring> &vectFileNameList)
{
    WIN32_FIND_DATAW ffd;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    std::wstring     strTempDir;

    strDirPath = EnsureSlashEndW(strDirPath);
    strTempDir = strDirPath;
    strTempDir += L"*";

    hFind = FindFirstFileW(strTempDir.c_str(), &ffd);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do 
        {
            if (ffd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
            {
                if (ffd.cFileName[0] != L'.')
                {
                    strTempDir = strDirPath + ffd.cFileName;
                    strTempDir += L"\\";
                    GetFileNameListW(strTempDir, vectFileNameList);
                }
            }
            else if (ffd.dwFileAttributes&FILE_ATTRIBUTE_ARCHIVE)
            {
                vectFileNameList.push_back(ffd.cFileName);
            }
        } while (FindNextFileW(hFind, &ffd));
    }

    FindClose(hFind);

    return true;
}

bool DirectoryUtil::GetFileNameListNoSubW(std::wstring strDirPath, std::vector<std::wstring> &vectFileNameList)
{
    WIN32_FIND_DATAW ffd;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    std::wstring     strTempDir;

    strDirPath = EnsureSlashEndW(strDirPath);
    strTempDir = strDirPath;
    strTempDir += L"*";

    hFind = FindFirstFileW(strTempDir.c_str(), &ffd);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do 
        {
            if (ffd.dwFileAttributes&FILE_ATTRIBUTE_ARCHIVE)
            {
                vectFileNameList.push_back(ffd.cFileName);
            }
        } while (FindNextFileW(hFind, &ffd));
    }

    FindClose(hFind);

    return true;
}

bool DirectoryUtil::GetPartitionListW(std::vector<std::wstring> &vectPartitionList)
{
    std::wstring strDriverName;
    DWORD dwDrivers = GetLogicalDrives();

    for (int i = 0; i < 'Z'-'A'; i ++)
    {
        if ((dwDrivers&(1<<i)) != 0)
        {
            strDriverName = wchar_t(L'A'+i);
            strDriverName += L":\\";
            if (GetPartitionTotalSizeW(strDriverName.c_str()) > 0)
            {
                vectPartitionList.push_back(strDriverName);
            }
        }
    }

    return true;
}

INT64 DirectoryUtil::GetPartitionTotalSizeW(const wchar_t *szPartitionPath)
{
    INT64 nSize = -1;

    ULARGE_INTEGER totalSpace;
    ULARGE_INTEGER freeSpace;
    if(GetDiskFreeSpaceExW(szPartitionPath, &freeSpace, &totalSpace, 0))
    {
        nSize = totalSpace.QuadPart;
    }

    return nSize;
}

INT64 DirectoryUtil::GetPartitionFreeSizeW(const wchar_t *szPartitionPath)
{
    INT64 nSize = -1;

    ULARGE_INTEGER totalSpace;
    ULARGE_INTEGER freeSpace;
    if(GetDiskFreeSpaceExW(szPartitionPath, &freeSpace, &totalSpace, 0))
    {
        nSize = freeSpace.QuadPart;
    }

    return nSize;
}

std::wstring DirectoryUtil::GetPartitionPathW(std::wstring strDirPath)
{
    if (strDirPath.length() >= 3)
    {
        return strDirPath.substr(0, 3);
    }

    return L"";
}

std::wstring DirectoryUtil::EnsureSlashEndW(std::wstring strDirPath)
{
    if (strDirPath.length() >= 2 && strDirPath.at(1) == L':')
    {//windows path
        if (strDirPath.rfind(L"\\") != (strDirPath.length()-1))
        {
            return strDirPath + L"\\";
        }
    }
    else if (strDirPath.length() >= 1 && strDirPath.at(0) == L'/')
    {
        if (strDirPath.rfind(L"/") != (strDirPath.length()-1))
        {
            return strDirPath + L"/";
        }
    }

    return strDirPath;
}

std::wstring DirectoryUtil::EnsureNoSlashEndW(std::wstring strDirPath)
{
    if (strDirPath.length() >= 2 && strDirPath.at(1) == L':')
    {//windows path
        if (strDirPath.rfind(L"\\") == (strDirPath.length()-1))
        {
            return strDirPath.substr(0, strDirPath.length()-1);
        }
    }
    else if (strDirPath.length() >= 1 && strDirPath.at(0) == L'/')
    {
        if (strDirPath.rfind(L"/") == (strDirPath.length()-1))
        {
            return strDirPath.substr(0, strDirPath.length()-1);
        }
    }

    return strDirPath;
}

std::wstring DirectoryUtil::GetParentDirW(std::wstring strPath)
{
    std::wstring strKey = L"\\";
    if (strPath.length() >= 2 && strPath.at(1) == L':')
    {//windows path
        strKey = L"\\";
    }
    else if (strPath.length() >= 1 && strPath.at(0) == L'/')
    {
        strKey = L"/";
    }

    size_t nPos = strPath.rfind(strKey);
    if (nPos == strPath.length()-1)
    {
        nPos = strPath.rfind(strKey, nPos-1);
    }
    if (nPos != std::wstring::npos)
    {
        return strPath.substr(0, nPos+1);
    }
    else
    {
        return L"";
    }
}

std::wstring DirectoryUtil::NormalWindowsPath(std::wstring strPath)
{
    if (strPath.length() >= 2 && strPath.at(1) == L':')
    {//windows path, replace / to \;
        std::wstring strDest;
        for (size_t i = 0; i < strPath.length(); i ++)
        {
            if (strPath.at(i) == L'/')
            {
                strDest.push_back(L'\\');
            }
            else
            {
                strDest.push_back(strPath.at(i));
            }
        }
        return strDest;
    }

    return strPath;
}

#endif//WIN32

int DirectoryUtil::MakeDir(const char *szDirPath)
{
#ifdef WIN32
    if (szDirPath != NULL)
    {
        struct stat	thestat;
        if (::stat(szDirPath, &thestat) == -1)
        {//not exist,create it
            if (::_mkdir(szDirPath) == -1)
            {
                if(::_chdir(szDirPath) != 0)
                {
                    return ::GetLastError();
                }
            }
        }
        else if(!(thestat.st_mode&_S_IFDIR))
        {//exist, but is file
            // there is a file at this point in the path
            return EEXIST;
        }
        else
        {//exist, ensure
            if(::_chdir(szDirPath) != 0)
            {
                return ::GetLastError();
            }
        }

        return 0;
    }

    return -1;
#else
    if (szDirPath != NULL)
    {
        struct stat	thestat;
        if (::stat(szDirPath, &thestat) == -1)
        {//not exist,create it
            if (::mkdir(szDirPath, (S_IRWXU|S_IRWXG|S_IRWXO)) == -1)
            {
                if(::chdir(szDirPath) != 0)
                {
                    return errno;
                }
            }
        }
        else if(!S_ISDIR(thestat.st_mode))
        {//exist, but is file
            // there is a file at this point in the path
            return EEXIST;
        }
        else
        {//exist, ensure
            if(::chdir(szDirPath) != 0)
            {
                return errno;
            }
        }

        chmod(szDirPath, (S_IRWXU|S_IRWXG|S_IRWXO));

        return 0;
    }

    return -1;
#endif
}
//////////////////////////////////////////////////////////////////////////
