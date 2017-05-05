#include "UDPServerModule.h"
#include "CommonDefine.h"
#include "MainConfig.h"

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
	RetransChannel UDPChannel = MainConfig::GetInstance()->GetServerConfigInfo(1);

	std::string strIP = UDPChannel.m_strIP;
	int iPort = UDPChannel.m_iPort;
	UDPServerChannel* pChannel = new UDPServerChannel(strIP);
	pChannel->AddRetransChannel(UDPChannel);
	if (0 == pChannel->start(iPort))
    {
		m_vectChannels.push_back(pChannel);
	}
	else
    {
		delete pChannel;
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
