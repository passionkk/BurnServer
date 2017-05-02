#include "UDPServerModule.h"
#include "CommonDefine.h"

UDPServerModule* UDPServerModule::m_pInstance = NULL;

UDPServerModule::UDPServerModule(void)
{

}

UDPServerModule::~UDPServerModule(void)
{

}

UDPServerModule *UDPServerModule::Initialize()
{
    return UDPServerModule::GetInstance();
}

void UDPServerModule::Uninitialize()
{
    if (m_pInstance != NULL)
    {
        m_pInstance->UnInit();
        delete m_pInstance;
        m_pInstance = NULL;
    }
}

UDPServerModule *UDPServerModule::GetInstance()
{
    if (m_pInstance == NULL)
    {
        m_pInstance = new UDPServerModule;
        m_pInstance->Init();
    }

    return m_pInstance;
}

void UDPServerModule::Init()
{
    //read config
	Poco::DynamicStruct  udpServerModule;// = MainConfig::GetInstance()->UdpServerModule_GetCopy();

    for(int i = 0;i < udpServerModule["channels"].size();i++)
    {
        std::string sName = udpServerModule["channels"][i]["name"].toString();
        int iPort = int(udpServerModule["channels"][i]["port"]);

        UDPServerChannel* pChannel = new UDPServerChannel(sName);

        for(int j = 0; j < udpServerModule["channels"][i]["retrans"]["entities"].size(); j++)
        {
			/*RetransChannel retransChannel;
			retransChannel.m_strType =  udpServerModule["channels"][i]["retrans"]["entities"][j]["type"].toString();
			retransChannel.m_strIP =  udpServerModule["channels"][i]["retrans"]["entities"][j]["ip"].toString();
			retransChannel.m_iPort =  int(udpServerModule["channels"][i]["retrans"]["entities"][j]["port"]);
			retransChannel.m_strSerialName =  udpServerModule["channels"][i]["retrans"]["entities"][j]["serialPortName"].toString();

			pChannel->AddRetransChannel(retransChannel);*/
        }

        //pChannel->AddListener(CmdProcessor::GetInstance());

        if (0 == pChannel->start(iPort))
        {
            m_vectChannels.push_back(pChannel);
        }
        else
        {
            delete pChannel;
        }
    }
}

void UDPServerModule::UnInit()
{
    for(int i = 0;i< (int)m_vectChannels.size();i++)
    {
        if(m_vectChannels[i] != NULL)
        {
            m_vectChannels[i]->stop();
            delete m_vectChannels[i];
            m_vectChannels[i] = NULL;
        }
    }
}
