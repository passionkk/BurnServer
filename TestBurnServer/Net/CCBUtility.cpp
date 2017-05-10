#include "../stdafx.h"

#if defined(_LINUX_)
#include <ifaddrs.h>
#include <arpa/inet.h>
#else
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#endif
#include "CCBUtility.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../CommonDefine.h"
#include "Poco/Net/DNS.h"
#include "Poco/URI.h"

using Poco::Net::DatagramSocket;
using Poco::Net::IPAddress;
using Poco::Net::StreamSocket;
using Poco::Net::SocketAddress;
using Poco::Net::Socket;

std::string CCBUtility::GetIP(std::string sEthName)
{
#if defined(_LINUX_)
    struct ifaddrs * ifAddrStruct = NULL;
    void * tmpAddrPtr = NULL;
    std::string strIP = "";

    if (getifaddrs(&ifAddrStruct) == -1)
    {
        return strIP;
    }

    while (ifAddrStruct != NULL)
    {
        if (ifAddrStruct->ifa_addr->sa_family == AF_INET)
        {   // check it is IP4  
            // is a valid IP4 Address  
            tmpAddrPtr = &((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            memset(addressBuffer, 0, INET_ADDRSTRLEN);
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            g_NetLog.Debug("%s IPV4 Address %s\n", ifAddrStruct->ifa_name, addressBuffer);
            std::string sName(ifAddrStruct->ifa_name);
            std::string sAddress(addressBuffer);
            if (sEthName == sName)
            {
                strIP = sAddress;
                break;
            }
        }
        ifAddrStruct = ifAddrStruct->ifa_next;
    }
    freeifaddrs(ifAddrStruct);
    return strIP;
#else
    return "";
#endif
}

int CCBUtility::SplitFileExt(std::string strFileName, std::string &strNameNoExt, std::string &strExt)
{
    size_t nPointIndex = strFileName.rfind('.');
    if (nPointIndex == std::string::npos)
    {
        strNameNoExt = strFileName;
        strExt = "";
    }
    else
    {
        strNameNoExt = strFileName.substr(0, nPointIndex);
        if (nPointIndex+1 < strFileName.length())
        {
            strExt = strFileName.substr(nPointIndex+1);
        }
        else
        {
            strExt = "";
        }
    }

    return 0;
}

int CCBUtility::GetFileMD5Value(const std::string sFilePath,std::string &sMD5Value)
{
    char szCmd[1024];
    char szCmdResult[1024];
    char szMD5[1024];
    memset(szCmd,0,1024);
    memset(szCmdResult,0,1024);
    memset(szMD5,0,1024);
    sprintf(szCmd,"md5sum %s",sFilePath.c_str());
    if(ExecuteCMD(szCmd ,szCmdResult) != 0)
    {
        return -1;
    }
    int iSpacePos = 0;
    for(iSpacePos = 0; iSpacePos < 1024; iSpacePos++)
    {
        if(szCmdResult[iSpacePos] == ' ')
        {
            break;
        }
    }
    memcpy(szMD5,szCmdResult,iSpacePos);
    sMD5Value = szMD5;
    return 0;
}

int CCBUtility::ExecuteCMD(const char *cmd, char *result)    
{
    int iRet = 0;
    char buf_ps[1024] = {0};    
    char ps[1024] = {0};    
    FILE *ptr;    
    strcpy(ps, cmd);
#if defined(_LINUX_)
    if((ptr = popen(ps, "r")) != NULL)    
    {    
        while(fgets(buf_ps, 1024, ptr) != NULL)    
        {    
            strcat(result, buf_ps);    
            if(strlen(result) > 1024)
            {
                iRet = -1;
                break;
            }   
        }    
        pclose(ptr);    
        ptr = NULL;
    }    
    else   
    {    
        g_NetLog.Debug("popen %s error\n", ps);  
        iRet = -1;
    }
#endif
    return iRet;
}

int CCBUtility::GetIPEachByte(char *sIP, int &nByte1, int &nByte2, int &nByte3, int &nByte4)
{
    unsigned long nIP = inet_addr(sIP);
    unsigned char sIPByte[4];
    unsigned char *sIPTmp = (unsigned char *)&nIP;
    sIPByte[0] = sIPTmp[0];
    sIPByte[1] = sIPTmp[1];
    sIPByte[2] = sIPTmp[2];
    sIPByte[3] = sIPTmp[3];

    nByte1 = sIPByte[0];
    nByte2 = sIPByte[1];
    nByte3 = sIPByte[2];
    nByte4 = sIPByte[3];
	return 0;
}

std::string CCBUtility::GetFileName(std::string strFilePath)
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
    else if (strFilePath.length() >= 1)
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

std::string CCBUtility::GetFileDir(std::string strFilePath)
{
    std::string strFileDir;

    if (strFilePath.length() >= 2 && strFilePath.at(1) == ':')
    {//windows path
        size_t nPos = strFilePath.rfind('\\');
        if (nPos != std::string::npos)
        {
            strFileDir = strFilePath.substr(0, nPos+1);
        }
    }
    else if (strFilePath.length() >= 1)
    {
        size_t nPos = strFilePath.rfind('/');
        if (nPos != std::string::npos)
        {
            strFileDir = strFilePath.substr(0, nPos+1);
        }
    }

    return strFileDir;
}

void CCBUtility::CopyFileOrDirectory(const char* sSrcPath,const char* sDestPath)
{
    try
    {
        //g_NetLog.Debug("copy file or directory in: %s to %s\n", sSrcPath,sDestPath);
        Poco::File file(sSrcPath);
        if(file.exists())
        {
            file.copyTo(sDestPath);
            //g_NetLog.Debug("copy file or directory success:  %s to %s\n", sSrcPath,sDestPath);
        }
    }
    catch (...)
    {
        //g_NetLog.Debug("%s catched\n", __PRETTY_FUNCTION__);
    }
}

int CCBUtility::ChangeCharInString(char *sSrc, char cOld, char cNew)
{
    int nLength = strlen(sSrc);

    for (size_t i = 0; i < nLength; i ++)
    {
        if (sSrc[i] == cOld)
        {
            sSrc[i] = cNew;
        }
    }

    return 0;
}

int CCBUtility::TCPShortConnect(const char *sIP, int nPort, std::string strSend, std::string &strRecv, int nConTime, int nRecvTime, int nSendCount)
{
    try
    {
        //connect
        StreamSocket ss;
        Poco::Timespan timeoutCon(nConTime*1000);
        try
        {
            ss.connect(SocketAddress(sIP, nPort), timeoutCon);
        }
        catch (...)
        {
           // g_NetLog.Debug("connect [%s:%d] failed\n", sIP, nPort);
            ss.close();
            return -1;
        }

        //send
        bool bSendSuccess = true;
        char *pSendStart = (char*)strSend.c_str();
        int nSendLeft = strSend.length()+1;
        if (nSendCount != -1)
        {
            nSendLeft = nSendCount;
        }
        do 
        {
            int nSend = ss.sendBytes(pSendStart, nSendLeft);
            if (nSend > 0)
            {
                pSendStart += nSend;
                nSendLeft -= nSend;
            }
            else
            {
                bSendSuccess = false;
                break;
            }
        } while (nSendLeft > 0);

        if (!bSendSuccess)
        {
            ss.close();
            //g_NetLog.Debug("send [%s:%d] failed\n", sIP, nPort);
            return -1;
        }

        //read
        do 
        {
            Poco::Timespan timeoutRecv(nRecvTime*1000);
            if (ss.poll(timeoutRecv, Socket::SELECT_READ))
            {
                char buffer[256];
                int n = ss.receiveBytes(buffer, sizeof(buffer));
                if (n > 0)
                {
                    strRecv.append(buffer, n);
                }
                else
                {
                    break;
                }
            }
            else
            {
                break;
            }
        } while (true);

        ss.close();
    }
    catch (...)
    {
       // g_NetLog.Debug("%s catched\n", __PRETTY_FUNCTION__);
    }

    return 0;
}

int CCBUtility::UDPSend(const char *sIP, int nPort, std::string strSend, int nSendCount)
{
    try
    {
        Poco::Net::DatagramSocket ss;
        ss.connect(SocketAddress(sIP, nPort));
        int n;
        int nNeedSend = strSend.length() + 1;
        if (nSendCount != -1)
        {
            nNeedSend = nSendCount;
        }
        n = ss.sendBytes(strSend.c_str(), nNeedSend);
        ss.close();
        if (n == nNeedSend)
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    catch (...)
    {
        //g_NetLog.Debug("%s catched\n", __PRETTY_FUNCTION__);
        return -1;
    }
}

int CCBUtility::UDPSend(DatagramSocket &localSocket, std::string strRemoteIP, int iRemotePort,char* sSendData,int nSendCount)
{
    try
    {
        int n = localSocket.sendTo(sSendData, nSendCount, SocketAddress(strRemoteIP, iRemotePort));
        return 0;
    }
    catch (...)
    {
        //g_NetLog.Debug("%s catched\n", __PRETTY_FUNCTION__);
        return -1;
    }
}

int CCBUtility::UDPBroadcast(DatagramSocket &localSocket,int iRemotePort, char* sSendData,int nSendCount)
{
    try
    {
        SocketAddress sa("255.255.255.255", iRemotePort);
        int n = localSocket.sendTo(sSendData, nSendCount, sa);
        return 0;
    }
    catch (...)
    {
        //g_NetLog.Debug("%s catched\n", __PRETTY_FUNCTION__);
        return -1;
    }
}

std::string CCBUtility::DomainURIToIPURI(std::string strSrcURI)
{
    std::string strDstURI = strSrcURI;
    try
    {
        Poco::URI srcUri(strSrcURI);
        //g_NetLog.Debug("src host is %s\n", srcUri.getHost().c_str());
        Poco::Net::HostEntry he1 = Poco::Net::DNS::hostByName(srcUri.getHost());
        if (he1.addresses().size() > 0)
        {
            std::string strIP = he1.addresses()[0].toString();
            //g_NetLog.Debug("dst host is %s\n", strIP.c_str());
            srcUri.setHost(strIP);
            strDstURI = srcUri.toString();
        }
    }
    catch (...)
    {
        //g_NetLog.Debug("DomainURIToIPURI failed %s", strSrcURI.c_str());
    }

    return strDstURI;
}

std::string CCBUtility::GetIPFromDomainURI(std::string strSrcURI)
{
    std::string strDstIP = "";
    try
    {
        Poco::URI srcUri(strSrcURI);
        Poco::Net::HostEntry he1 = Poco::Net::DNS::hostByName(srcUri.getHost());
        if (he1.addresses().size() > 0)
        {
            std::string strIP = he1.addresses()[0].toString();
            strDstIP = strIP;
        }
    }
    catch (...)
    {
        //g_NetLog.Debug("GetIPFromDomainURI failed %s", strSrcURI.c_str());
    }

    return strDstIP;
}

Poco::Int64 CCBUtility::FileSize(const char*szFilePath)
{
    Poco::Int64 nFileSize = -1;
#if defined(_LINUX_)
    struct stat64 thestat;
    if(::stat64(szFilePath, &thestat) >= 0)
    {
        nFileSize = thestat.st_size;
    }
#endif
    return nFileSize;
}

void CCBUtility::string_replace( std::string &strBig, const std::string &strsrc, const std::string &strdst )
{
    std::string::size_type pos = 0;
    std::string::size_type srclen = strsrc.size();
    std::string::size_type dstlen = strdst.size();

    while( (pos=strBig.find(strsrc, pos)) != std::string::npos )
    {
        strBig.replace( pos, srclen, strdst );
        pos += dstlen;
    }
}