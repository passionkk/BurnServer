#include "MainConfig.h"
#include "CommonDefine.h"


MainConfig *MainConfig::m_pInstance = NULL;

MainConfig::MainConfig()
:m_strFile("")
{
}

MainConfig::~MainConfig()
{
}

MainConfig* MainConfig::Initialize()
{
	return MainConfig::GetInstance();
}

void MainConfig::Uninitialize()
{
	if (m_pInstance)
		delete m_pInstance;
	m_pInstance = NULL;
}

MainConfig* MainConfig::GetInstance()
{
	if (m_pInstance == NULL)
		m_pInstance = new MainConfig;
	return m_pInstance;
}

int MainConfig::ReadFromFile(std::string strFile)
{
	return 0;
}

int MainConfig::SaveFile()
{
	return 0;
}

Poco::DynamicStruct GetServerConfigInfo(int nServerType)
{
	Poco::DynamicStruct dStruct;
	return dStruct;
}