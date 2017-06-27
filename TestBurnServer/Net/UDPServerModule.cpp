#include "stdafx.h"
#include "UDPServerModule.h"
#include "../CommonDefine.h"

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
	RetransChannel UDPChannel;
	UDPChannel.m_iPort = 1002;
	UDPChannel.m_strIP = "127.0.0.1";
	UDPChannel.m_strSerialName = "UDP Server";
	UDPChannel.m_strType = "UDP";

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

void UDPServerModule::SetCallback(Fun pFun, LPVOID pVoid)
{
	for (int i = 0; i < m_vectChannels.size(); i++)
	{
		m_vectChannels.at(i)->SetCallback(pFun, pVoid);
	}
}