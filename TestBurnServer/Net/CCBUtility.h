#ifndef _CCB_UTILITY_H_
#define _CCB_UTILITY_H_

#include "../commondefine.h"

class CCBUtility
{
public:
    static std::string GetIP(std::string sEthName);
    //strFileName = 123.mp4
    //strNameNoExt = 123
    //strExt = mp4
    static int SplitFileExt(std::string strFileName, std::string &strNameNoExt, std::string &strExt);
    static int GetFileMD5Value(const std::string sFilePath,std::string &sMD5Value);
    static int ExecuteCMD(const char *cmd, char *result);
    static int GetIPEachByte(char *sIP, int &nByte1, int &nByte2, int &nByte3, int &nByte4);
    static std::string GetFileName(std::string strFilePath);
    //eg. /mnt/HD0/ccb/1.mp4 return /mnt/HD0/ccb/
    static std::string GetFileDir(std::string strFilePath);
    static void CopyFileOrDirectory(const char* sSrcPath,const char* sDestPath);
    static int ChangeCharInString(char *sSrc, char cOld, char cNew);
    static int TCPShortConnect(const char *sIP, int nPort, std::string strSend, std::string &strRecv, int nConTime = 1000, int nRecvTime = 1000, int nSendCount = -1);
    static int UDPSend(const char *sIP, int nPort, std::string strSend, int nSendCount = -1);
    static int UDPSend(Poco::Net::DatagramSocket &localSocket, std::string strRemoteIP, int iRemotePort,char* sSendData,int nSendCount);
    static int UDPBroadcast(Poco::Net::DatagramSocket &localSocket,int iRemotePort, char* sSendData,int nSendCount);
    static std::string DomainURIToIPURI(std::string strSrcURI);
    static std::string GetIPFromDomainURI(std::string strSrcURI);
    static Poco::Int64 FileSize(const char*szFilePath);
    static void string_replace( std::string &strBig, const std::string &strsrc, const std::string &strdst );
public:
    CCBUtility();
    ~CCBUtility();
};

#endif//_CCB_UTILITY_H_

