#include "UDPServerChannel.h"
#include "CommonDefine.h"
#include "Poco/Timespan.h"
#include "Business.h"
#include "CCBUtility.h"
#include "jsoncpp/json/json.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/NetException.h"

using namespace Poco::JSON;
using namespace Poco::Dynamic;
using Poco::DynamicStruct;

using Poco::Net::StreamSocket;
using std::string;


#define UDP_BUFFER_SIZE (64 * 1024)

UDPServerChannel::UDPServerChannel(std::string strName) :Runnable(),
	m_thread("UDPServer"),
	m_CallBackFunc(NULL),
	m_pContext(NULL),
	m_stop(true),
    m_ready(),
    m_strName(strName)
{

}

UDPServerChannel::~UDPServerChannel()
{
	if (false == m_stop)
	{
		m_stop = true;
		m_thread.join();
	}
}

/// start the UDPServer
int UDPServerChannel::start(int iPort)
{
    try
    {
        if (true == m_stop)
        {
            m_CallBackFunc = UdpServerCallBack;
            m_pContext = this;
            Poco::Net::SocketAddress sa(iPort);
            m_socket.bind(sa, true);
            m_socket.setBroadcast(true);
            m_socket.setReceiveBufferSize(UDP_BUFFER_SIZE);
            m_stop = false;
            m_thread.start(*this);
            m_ready.wait();
        }
        return 0;
    }
    catch (...)
    {
        //g_NetLog.Debug("[UDPServerChannel::start] catch!\n");
        return -1;
    }
}

/// stop the UDPServer.
int UDPServerChannel::stop()
{
    try
    {
        if (false == m_stop)
        {
            m_stop = true;
            m_thread.join();
        }
        return 0;
    }
    catch (...)
    {
        //g_NetLog.Debug("[UDPServerChannel::stop] catch!\n");
        return -1;
    }
}

Poco::UInt16 UDPServerChannel::port() const
{
	return m_socket.address().port();
}

SocketAddress UDPServerChannel::address() const
{
    return m_socket.address();
}

void UDPServerChannel::run()
{
    m_ready.set();
	Poco::Timespan span(250*1000);
	while (!m_stop)
	{
		if (m_socket.poll(span, Socket::SELECT_READ))
		{
			try
			{
				char buffer[UDP_BUFFER_SIZE];
				memset(buffer, 0, UDP_BUFFER_SIZE);
				SocketAddress sender;
				int n = m_socket.receiveFrom(buffer, sizeof(buffer), sender);
				if (m_CallBackFunc != NULL && n > 0)
				{
                    m_CallBackFunc((char *)sender.host().toString().c_str(), 
                        sender.port(),
                        m_socket, 
                        buffer, n, m_pContext);
                    //std::string strResponse = "hello";
                    //m_socket.sendTo(strResponse.c_str(), strResponse.length(), sender);
				}
			}
            catch (Poco::Net::NetException & exc)
            {
                //NetException
            }
            catch (...)
            {
                //g_NetLog.Debug("[UDPServerChannel::run] catch!\n");
            }
		}
	}
}


int UDPServerChannel::UdpServerCallBack(char *sRemoteIP, int nRemotePort, DatagramSocket &localSocket, char *sData, int nData, void *pContext)
{
    UDPServerChannel *pUDPServerChannel = (UDPServerChannel *)pContext;
    pUDPServerChannel->ProcessCmd(sRemoteIP,nRemotePort,localSocket,sData,nData);
    //pUDPServerChannel->DoRetrans(sData,nData);
    return 0;
}

int UDPServerChannel::ProcessCmd(char *sRemoteIP, int nRemotePort, DatagramSocket &localSocket, char *sData, int nData)
{
    //处理udp命令
    try
    {
        if(localSocket.address().port() == 91)
        {
            char* pData = new char[nData+1];
            memset(pData,0,nData+1);
            memcpy(pData,sData,nData);
            std::string strData = pData;
            delete pData;
            pData = NULL;

            Parser parser;
            Var result;

            result = parser.parse(strData);
            if (result.type() == typeid(Object::Ptr))
            {
                Object::Ptr object = result.extract<Object::Ptr>();
                if ((!object.isNull()) &&(object->size() > 0))
                {
                    DynamicStruct ds = Object::makeStruct(object);
                    std::string sMethod = ds["method"];
                    if (sMethod == "modifyNetNotice")
                    {
                        //g_NetLog.Debug("[UDPServerChannel::ProcessCmd] modifyNetNotice\n");
                        std::string strIP = ds["params"]["ip"];
                        std::string strMask = ds["params"]["mask"];
                        std::string strGateway = ds["params"]["gateway"];
                        std::string strMac = ds["params"]["mac"];
                        std::string strUUID = ds["params"]["uuid"];
                        bool bIsHost = ds["params"]["isHost"];

						Poco::DynamicStruct dsNetModule;// = MainConfig::GetInstance()->NetModule_GetCopy();
                        if (dsNetModule["interfaces"].size() > 0)
                        {
                            std::string strOrgIP = dsNetModule["interfaces"][0]["ip"];
                            std::string strOrgNetMask = dsNetModule["interfaces"][0]["mask"];
                            std::string strOrgGateWay = dsNetModule["interfaces"][0]["gateway"];
                            std::string strOrgMac = dsNetModule["interfaces"][0]["macAddr"];
                            if (
                                (strOrgIP != strIP)
                                || (strOrgNetMask != strMask)
                                || (strOrgGateWay != strGateway)
                                || (strOrgMac != strMac)
                                )
                            {
                                /*MainConfig::GetInstance()->NetModule_SetNetInterface(strUUID,bIsHost,strIP,strMask,strGateway,strMac);
                                if (
                                    (MainConfig::GetInstance()->SysModule_GetMachineType() == MACHINE_TYPE_HXGZ)
                                    && (MainConfig::GetInstance()->SysModule_GetRoleType() == ROLE_TYPE_MASTER)
                                    )
                                {
                                    HXGZCmd::GetInstance()->ResetVerAndNet();
                                }
                                ARPSend::GetInstance()->NotifyIPChange();
                                if(MainConfig::GetInstance()->SysModule_GetRoleType() == ROLE_TYPE_MASTER)
                                {
                                    //ip改变，重新注册sip
                                    SIPModule::GetInstance()->RegisterAll();
                                }*/
                            }
                        }
                    }
                    else if(sMethod == "getDeviceInfo")
                    {
                        //g_NetLog.Debug("[UDPServerChannel::ProcessCmd] getDeviceInfo\n");
                        /*std::string strIP = ds["params"]["ip"];
                        std::string strMask = ds["params"]["mask"];
                        std::string strGateway = ds["params"]["gateway"];
                        std::string strMac = ds["params"]["mac"];
                        std::string strUUID = ds["params"]["uuid"];
                        bool bIsHost = ds["params"]["isHost"];

                        Poco::DynamicStruct dsNetModule = MainConfig::GetInstance()->NetModule_GetCopy();
                        if (dsNetModule["interfaces"].size() > 0)
                        {
                            std::string strOrgIP = dsNetModule["interfaces"][0]["ip"];
                            std::string strOrgNetMask = dsNetModule["interfaces"][0]["mask"];
                            std::string strOrgGateWay = dsNetModule["interfaces"][0]["gateway"];
                            std::string strOrgMac = dsNetModule["interfaces"][0]["macAddr"];
                            if (
                                (strOrgIP != strIP)
                                || (strOrgNetMask != strMask)
                                || (strOrgGateWay != strGateway)
                                || (strOrgMac != strMac)
                                )
                            {
                                MainConfig::GetInstance()->NetModule_SetNetInterface(strUUID, bIsHost, strIP, strMask, strGateway, strMac);
                            }
                        }

                        bool bEnable = MainConfig::GetInstance()->HighGradeModule_GetDHCPEnable();
                        if(bEnable == true)
                        {
                            Bussiness::GetInstance()->EnableDHCP(true);
                        }*/
                    }
                    else if(sMethod == "getUUID")
                    {
                        std::string sUUID = ds["params"]["uuid"];
                        //g_NetLog.Debug("[UDPServerChannel::ProcessCmd] getUUID :%s\n",sUUID.c_str());

                       
                    }
                    else if(sMethod == "setUUID")
                    {
                       
                    }  
					else if (sMethod == "testProtocol")
					{
						printf("recv UDP test protocol.\n");
						std::string strSendToClient = "server recv u'r test protocol";
						printf("remoteIP:%s, remotePort:%d.\n", sRemoteIP, nRemotePort);
						//CCBUtility::UDPSend(sRemoteIP, nRemotePort, strSendToClient, strSendToClient.length());
						std::string strRemoteIP = sRemoteIP;
						//char* pStrSend = 
						Poco::Thread::sleep(5000);
						CCBUtility::UDPSend(localSocket, strRemoteIP, nRemotePort, (char*)strSendToClient.c_str(), strSendToClient.length());
					}
				}
            }
        }
        else if(localSocket.address().port() == 6180)
        {
            char szData[10];
            memset(szData,0,sizeof(szData));
            if(nData == 4)
            {
                memcpy(szData,sData,4);
                if(szData[0] == 0x00 && szData[1] == 0x0a && szData[2] == 0x00 && szData[3] == 0x00)
                {
                    //收到搜索设备协议
                    /*if(MainConfig::GetInstance()->HighGradeModule_GetCompatibleOldDeviceSearchEnable())
                    {
                        //回复广播+单播，格式如下
                        //80 0a xx xx 00 00 +“net.ipaddr=DVC的IP地址\n
                        //net.netmask=DVC子网掩码\n
                        //net.gateway=DVC网关地址\n
                        //net.ethaddr=DVC的MAC地址\n
                        //sys._swver=DVC的软件版本\n
                        //sys.dtyp=DVC的设备名称\n
                        //sys.subVersion=3.0\n”
                        int nMaxReturnData = 16*1024;
                        char *pReturnData = new char[nMaxReturnData];
                        int nReturnData = 0;
                        pReturnData[0] = 0x80;
                        pReturnData[1] = 0x0a;
                        pReturnData[4] = 0x00;
                        pReturnData[5] = 0x00;
                        nReturnData = 6;
                        char szReturnContent[8*1024];
                        memset(szReturnContent, 0, sizeof(szReturnContent));

                        std::string sIP = "";
                        std::string sMask = "";
                        std::string sGateway = "";
                        std::string sMacAddr = "";
                        Poco::DynamicStruct  netModule = MainConfig::GetInstance()->NetModule_GetCopy();
                        if(netModule["interfaces"].size() > 0)
                        {
                            sIP = netModule["interfaces"][0]["ip"].toString();
                            sMask = netModule["interfaces"][0]["mask"].toString();
                            sGateway = netModule["interfaces"][0]["gateway"].toString();
                            sMacAddr = netModule["interfaces"][0]["macAddr"].toString();
                        }
                        std::string sFirmwareVersion = MainConfig::GetInstance()->SysModule_GetFirmwareVersion();
                        Poco::DynamicStruct  sysModule = MainConfig::GetInstance()->SysModule_GetCopy();
                        std::string sModel = sysModule["model"].toString();
                        sprintf(szReturnContent, "net.ipaddr=%s\nnet.netmask=%s\nnet.gateway=%s\nnet.ethaddr=%s\nsys._swver=%s\nsys.dtyp=%s\nsys.subVersion=3.0\n",
                            sIP.c_str(),sMask.c_str(),sGateway.c_str(),sMacAddr.c_str(),sFirmwareVersion.c_str(),sModel.c_str());

                        if (strlen(szReturnContent) + nReturnData < nMaxReturnData)
                        {
                            memcpy(pReturnData+nReturnData, szReturnContent, strlen(szReturnContent));
                            nReturnData += strlen(szReturnContent);
                        }
                        pReturnData[2] = (nReturnData-4)>>8;
                        pReturnData[3] = (nReturnData-4)&0xff;

                        CCBUtility::UDPSend(localSocket,sRemoteIP,6180,pReturnData,nReturnData);
                        CCBUtility::UDPBroadcast(localSocket,6180,pReturnData,nReturnData);

                        if (pReturnData != NULL)
                        {
                            delete []pReturnData;
                            pReturnData = NULL;
                        }
                    }
					*/
				}
            }    
            else if(nData > 6)
            {
                memcpy(szData,sData,6);
                if(szData[0] == 0x90 && szData[1] == 0x0a && szData[4] == 0x00 && szData[5] == 0x00)
                {
                    //收到设置网络参数协议
                    //90 0a xx xx 00 00 +“net.ipaddr=DVC的IP地址\n
                    //net.netmask=DVC子网掩码\n
                    //net.gateway=DVC网关地址\n
                    //net.ethaddr=DVC的MAC地址\n
                    //sys._swver=DVC的软件版本\n
                    //sys.dtyp=DVC的设备名称\n”
                    std::string sRecvCmd = std::string(sData+6);
                    //g_NetLog.Debug("recv cmd: %s \n",sRecvCmd.c_str());

                    std::string sName = "";
                    std::string sIP = "";
                    std::string sMask = "";
                    std::string sGateway = "";
                    std::string sMacAddr = "";
                    string sRecvMacAddr = "";
					Poco::DynamicStruct  netModule;// = MainConfig::GetInstance()->NetModule_GetCopy();
                    if(netModule["interfaces"].size() > 0)
                    {
                        sName = netModule["interfaces"][0]["name"].toString();
                        sIP = netModule["interfaces"][0]["ip"].toString();
                        sMask = netModule["interfaces"][0]["mask"].toString();
                        sGateway = netModule["interfaces"][0]["gateway"].toString();
                        sMacAddr = netModule["interfaces"][0]["macAddr"].toString();
                    }

                    Poco::StringTokenizer   stRecvCmd(sRecvCmd, "\n", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
                    for(int i = 0; i < (int)stRecvCmd.count(); i++)
                    {
                        std::string sOneCmd = stRecvCmd[i];
                        char szValue[1024];
                        memset(szValue,0,sizeof(szValue));
                        if(sOneCmd.find("net.ipaddr=") != std::string::npos)
                        {
                            if(sscanf(sOneCmd.c_str(),"net.ipaddr=%s",szValue) == 1)
                            {
                                sIP = szValue;
                            }
                        }
                        else if(sOneCmd.find("net.netmask=") != std::string::npos)
                        {
                            if(sscanf(sOneCmd.c_str(),"net.netmask=%s",szValue) == 1)
                            {
                                sMask = szValue;
                            }
                        }
                        else if(sOneCmd.find("net.gateway=") != std::string::npos)
                        {
                            if(sscanf(sOneCmd.c_str(),"net.gateway=%s",szValue) == 1)
                            {
                                sGateway = szValue;
                            }
                        }
                        else if(sOneCmd.find("net.ethaddr=") != std::string::npos)
                        {
                            if(sscanf(sOneCmd.c_str(),"net.ethaddr=%s",szValue) == 1)
                            {
                                sRecvMacAddr = szValue;
                            }
                        }
                    }
                    
                    if(sMacAddr == sRecvMacAddr && sIP != "" && sMask != "" && sGateway != "" && sMacAddr != "")
                    {
                        //Bussiness::GetInstance()->ModifyNetInfo(sName,sIP,sMask,sGateway,sMacAddr);
                    }
                }
                else if(szData[0] == 0x91 && szData[1] == 0x0a && szData[4] == 0x00 && szData[5] == 0x00)
                {
                    //收到设置DHCP协议
                    //91 0a xx xx 00 00 +“net.ethaddr=DVC的MAC地址\n
                    //sys.dhcp=1\n”
                    std::string sRecvCmd = std::string(sData+6);
                    //g_NetLog.Debug("recv cmd: %s \n",sRecvCmd.c_str());

                    std::string sMacAddr = "";
                    string sRecvMacAddr = "";
                    int iDHCP = -1;
					Poco::DynamicStruct  netModule;// = MainConfig::GetInstance()->NetModule_GetCopy();
                    if(netModule["interfaces"].size() > 0)
                    {
                        sMacAddr = netModule["interfaces"][0]["macAddr"].toString();
                    }

                    Poco::StringTokenizer   stRecvCmd(sRecvCmd, "\n", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
                    for(int i = 0; i < (int)stRecvCmd.count(); i++)
                    {
                        std::string sOneCmd = stRecvCmd[i];
                        char szValue[1024];
                        memset(szValue,0,sizeof(szValue));
                        if(sOneCmd.find("net.ethaddr=") != std::string::npos)
                        {
                            if(sscanf(sOneCmd.c_str(),"net.ethaddr=%s",szValue) == 1)
                            {
                                sRecvMacAddr = szValue;
                            }
                        }
                        else if(sOneCmd.find("sys.dhcp=") != std::string::npos)
                        {
                            if(sscanf(sOneCmd.c_str(),"sys.dhcp=%d",&iDHCP) != 1)
                            {
                                iDHCP = -1;
                            }
                        }
                    }

                    if(sMacAddr == sRecvMacAddr && sMacAddr != "")
                    {
                        if(iDHCP == 0)
                        {
                            //MainConfig::GetInstance()->HighGradeModule_SetDHCPEnable(false);
                            //Bussiness::GetInstance()->EnableDHCP(false);
                        }
                        else if(iDHCP == 1)
                        {
                            //MainConfig::GetInstance()->HighGradeModule_SetDHCPEnable(true);
                            //Bussiness::GetInstance()->EnableDHCP(true);
                        }
                    }
                }
            }
        }
        else
        {
            if(localSocket.address().port() == 606)
            {
                if(nData < (16*1024))
                {
                    //转换为网管设置命令
                    //前加00 03 00 00，并确保末尾为\n
                    int nSendData = 0;
                    char szSendData[16*1024+5];
                    memset(szSendData,0,sizeof(szSendData));

                    szSendData[0] = 0x00;
                    szSendData[1] = 0x03;
                    szSendData[2] = 0x00;
                    szSendData[3] = 0x00;
                    nSendData = 4;
                    memcpy(szSendData+4,sData,nData);
                    nSendData += nData;
                    if(szSendData[nSendData-1] != '\n')
                    {
                        szSendData[nSendData] = '\n';
                        nSendData += 1;
                    }
                    //Bussiness::GetInstance()->SendNMCmdToSelf(szSendData,nSendData);
                }
            }

            Mutex::ScopedLock   lock(m_mutexListenerVect);
            for(int i = 0; i < (int)m_vectListener.size(); i++)
            {
                if(m_vectListener[i] != NULL)
                {
                     //CmdInfo cmdInfo;
                     //cmdInfo.m_pCmd = new char[nData+1];
                     //memset(cmdInfo.m_pCmd,0,nData+1);
                     //memcpy(cmdInfo.m_pCmd,sData,nData);
                     //cmdInfo.m_iLength = nData;
                     //m_vectListener[i]->AddCmd(cmdInfo);
                }
            }
        }
        return 0;
    }
    catch(...)
    {
        return -1;
    }
}

void UDPServerChannel::DoRetrans(char *sData, int nData)
{
    try
    {
        Mutex::ScopedLock   lock(m_mutexRetransChannelsVect);
        for(int i = 0; i < (int)m_vectRetransChannels.size(); i++)
        {
            if(m_vectRetransChannels[i].m_strType == "tcp")
            {
                std::string sIP = m_vectRetransChannels[i].m_strIP;
                if(sIP != "")
                {
                    int iPort = m_vectRetransChannels[i].m_iPort;

                    StreamSocket ss;
                    ss.connect(SocketAddress(sIP, iPort),300 * 1000);
                    int n = ss.sendBytes(sData, nData);
                    ss.close();
                }
            }
            else if(m_vectRetransChannels[i].m_strType == "udp")
            {
                std::string sIP = m_vectRetransChannels[i].m_strIP;
                if(sIP != "")
                {
                    int iPort = m_vectRetransChannels[i].m_iPort;

                    Poco::Net::DatagramSocket ss;
                    ss.connect(SocketAddress(sIP, iPort));
                    int n = ss.sendBytes(sData, nData);
                    ss.close();
                }
            }
            else if(m_vectRetransChannels[i].m_strType == "serial")
            {
                /*std::vector<SerialChannel*> vectpSerialChannel = SerialModule::GetInstance()->GetVectChannels();
                for(int j = 0; j < (int)vectpSerialChannel.size(); j++)
                {
                    if(vectpSerialChannel[j]->Name() == m_vectRetransChannels[i].m_strSerialName)
                    {
                        vectpSerialChannel[j]->Write(sData,nData);
                        break;
                    }
                }*/
            }
        }
    }
    catch(...)
    {
        //g_NetLog.Debug("[UDPServerChannel::DoRetrans] catch!\n");
    }
}

void UDPServerChannel::AddListener(CmdListener* pListener)
{
    Mutex::ScopedLock   lock(m_mutexListenerVect);
    m_vectListener.push_back(pListener);
}

void UDPServerChannel::AddRetransChannel(RetransChannel retransChannel)
{
    Mutex::ScopedLock   lock(m_mutexRetransChannelsVect);
    m_vectRetransChannels.push_back(retransChannel);
}

std::string UDPServerChannel::TestProtocol(std::string strSend)
{
	try
	{
		Json::Reader    jsonReader;
		Json::Value     jsonValueIn;

		if (jsonReader.parse(strSend, jsonValueIn))
		{
			std::string sMethod = jsonValueIn["method"].asString();
			Json::Value   jsonValueParams = jsonValueIn["params"];
			int iLogicNo = jsonValueParams["logicNo"].asInt();

			Json::Value     jsonValueRoot;
			Json::Value     jsonValue1;
			Json::Value     jsonValue2;
			jsonValue2["retCode"] = Json::Value(0);
			jsonValue2["retMessage"] = Json::Value("udp test protocol ok");
			jsonValue1["method"] = Json::Value(sMethod.c_str());
			jsonValue1["params"] = jsonValue2;
			jsonValueRoot["result"] = jsonValue1;
			string strOut = jsonValueRoot.toStyledString();
			return strOut;
		}
		else
		{
			return "";
		}
		return "";
	}
	catch (...)
	{
		printf("%s catched\n", __PRETTY_FUNCTION__);
		return "";
	}
}