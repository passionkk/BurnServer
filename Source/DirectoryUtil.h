#ifndef _DIRECTORY_UTIL_H_
#define _DIRECTORY_UTIL_H_

#include <string>
#include <vector>

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

/**
 * szDirPath should format like:
 * in windows
 *      C:\1\ not C:\1
 * in linux
 *      /home/1/ not /home/1
 * 
 * 
 * why a function with two form.
 * 1 linux directory operation parameter is char *
 * 2 program cross platform use noly a single function than #ifdef.
 *   eg.
 *   a program can be used in windows and linux, and 
 * 
 */

class DirectoryUtil
{
public:
    /**
     * create directory. 
     * if parent dir is not exist, create it.
     * 
     * return true when create directory success, or directory is already exist.
     */
    static bool CreateDir(const char *szDirPath);

    /**
     * delete directory. include subdirectory.
     * 
     */
    static bool DeleteDir(const char *szDirPath);

    static bool IsDirExist(const char *szDirPath);

    /**
     * 
     * copy szSrcDir under szDstDir.
     * if szDstDir is not exist, create it automatic.
     * if szDstDir have the same folder or file in szSrcDir, szSrcDir's files override szDstDir's. 
     * 
     */
    static bool CopyDirToDir(const char *szDstDir, const char *szSrcDir);

    /**
     * 
     * copy each files in vectSrcFiles to folder szDstDir.
     * if szDstDir is not exist, create it automatic.
     * if szDstDir have the same file in vectSrcFiles, vectSrcFiles's files override szDstDir's. 
     * 
     */
    static bool CopyFileToDir(const char *szDstDir, std::vector<std::string> vectSrcFiles);
    
    /**
     *
     * copy vectSrcFiles.at(i) to szDstDir+vectDstFiles.at(i);
     * eg.
     * copy C:\1.txt 
     * to   D:\ + 2.txt
     * 
     */
    static bool CopyFileToDir(const char *szDstDir, std::vector<std::string> vectSrcFiles, std::vector<std::string> vectDstFiles);

    /**
     *
     *
     */
    static UINT64 GetDirSize(const char *szDirPath);

    /**
     * get files under strDirPath's directory, include files under subdirectory.
     * vectFileList will store file's full path.
     */
    static bool GetFileList(std::string strDirPath, std::vector<std::string> &vectFileList);
    
    /**
     * get files under strDirPath's directory, not include files under subdirectory.
     * vectFileList will store file's full path.
     */
    static bool GetFileListNoSub(std::string strDirPath, std::vector<std::string> &vectFileList);

    /**
     * get files under strDirPath's directory, include files under subdirectory.
     * vectFileNameList will store file's name, not full path.
     */
    static bool GetFileNameList(std::string strDirPath, std::vector<std::string> &vectFileNameList);

    /**
     * get files under strDirPath's directory, not include files under subdirectory.
     * vectFileNameList will store file's name, not full path.
     */
    static bool GetFileNameListNoSub(std::string strDirPath, std::vector<std::string> &vectFileNameList);


    /**
     *
     * linux is not support
     *
     * element in vectPartitionList
     * eg, C:\
     *     D:\
     * 
     */
    static bool GetPartitionList(std::vector<std::string> &vectPartitionList);

    /**
     *
     * linux is not support
     *
     * @szPartitionPath
     * eg, C:\
     *     D:\
     * 
     */
    static INT64 GetPartitionTotalSize(const char *szPartitionPath);

    /**
     *
     * linux is not support
     *
     * @szPartitionPath
     * eg, C:\
     *     D:\
     * 
     */
    static INT64 GetPartitionFreeSize(const char *szPartitionPath);


    /**
     *
     * linux is not support
     *
     * @strDirPath
     * eg, C:\abc\cde\   return C:\
     * eg, D:\           return D:\
     * eg, D:\abc\acd\   return D:\
     * 
     * return "" if strDirPath length less 3 character
     * return partitionPath.
     * 
     */
    static std::string GetPartitionPath(std::string strDirPath);

    /**
     * eg.
     * strDirPath = C:\1\2;     return C:\1\2\
     * strDirPath = C:\1\2\;    return C:\1\2\
     * strDirPath = /home/1/2;  return /home/1/2/
     * strDirPath = /home/1/2/; return /home/1/2/
     */
    static std::string EnsureSlashEnd(std::string strDirPath);

    /**
     * eg.
     * strDirPath = C:\1\2\;    return C:\1\2
     * strDirPath = C:\1\2;     return C:\1\2
     * strDirPath = /home/1/2/; return /home/1/2
     * strDirPath = /home/1/2;  return /home/1/2
     */
    static std::string EnsureNoSlashEnd(std::string strDirPath);

    /**
     * 
     * @strPath
     * strPath can be a file path, or a directory path.
     */
    static std::string GetParentDir(std::string strPath);

    /**
     * 
     * this function will be valid only when second char is ':'.
     * @strPath
     * 
     * eg
     * if strPath = C:/1/2/2.txt
     * return C:\1\2\2.txt
     * 
     */
    static std::string NormalWindowsPath(std::string strPath);

#ifdef WIN32/*for utf-16 use, only used in win32 system*/
    static bool CreateDirW(const wchar_t *szDirPath);
    
    static bool DeleteDirW(const wchar_t *szDirPath);

    static bool IsDirExistW(const wchar_t *szDirPath);
    
    static bool CopyDirToDirW(const wchar_t* szDstDir, const wchar_t* szSrcDir);

    static bool CopyFileToDirW(const wchar_t *szDstDir, std::vector<std::wstring> vectSrcFiles);

    static bool CopyFileToDirW(const wchar_t *szDstDir, std::vector<std::wstring> vectSrcFiles, std::vector<std::wstring> vectDstFiles);

    static UINT64 GetDirSizeW(const wchar_t *szDirPath);

    static bool GetFileListW(std::wstring strDirPath, std::vector<std::wstring> &vectFileList);

    static bool GetFileListNoSubW(std::wstring strDirPath, std::vector<std::wstring> &vectFileList);

    static bool GetFileNameListW(std::wstring strDirPath, std::vector<std::wstring> &vectFileNameList);

    static bool GetFileNameListNoSubW(std::wstring strDirPath, std::vector<std::wstring> &vectFileNameList);

    static bool GetPartitionListW(std::vector<std::wstring> &vectPartitionList);

    static INT64 GetPartitionTotalSizeW(const wchar_t *szPartitionPath);

    static INT64 GetPartitionFreeSizeW(const wchar_t *szPartitionPath);

    static std::wstring GetPartitionPathW(std::wstring strDirPath);

    static std::wstring EnsureSlashEndW(std::wstring strDirPath);

    static std::wstring EnsureNoSlashEndW(std::wstring strDirPath);

    static std::wstring GetParentDirW(std::wstring strPath);

    static std::wstring NormalWindowsPath(std::wstring strPath);
#endif//WIN32

private:
    DirectoryUtil();
    ~DirectoryUtil();
protected:
    static int MakeDir(const char *szDirPath);
private:
    static const int    msc_nMaxFilePath;
};

#endif//_DIRECTORY_UTIL_H_
