#include "MainConfig.h"
#include "CommonDefine.h"

const char* g_pConfigPath = "E:\\Work\\GitRepos\\BurnServer\\Bin\\x86\\Debug\\config.json";
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
	m_pInstance->Init();
	return m_pInstance;
}

void MainConfig::Init()
{
	if (m_pInstance)
	{
		if (m_pInstance->IsConfigFileRight(g_pConfigPath))
			m_pInstance->ReadFromFile(g_pConfigPath);
	}
}

bool MainConfig::IsConfigFileRight(std::string strFile)
{
	try
	{
		File file(strFile);
		if (!file.exists())
		{
			//g_NetLog.Debug("[MainConfig::ConfigFileRight] file %s do not exist\n", strFile.c_str());
			return false;
		}

		Parser  parser;
		Var     result;

		FileInputStream fis(strFile);
		result = parser.parse(fis);
		fis.close();

		Object::Ptr obj = result.extract<Object::Ptr>();

		if (obj->getObject("HttpServer").isNull())
		{
			//g_NetLog.Debug("[MainConfig::ConfigFileRight]config file %s is wrong\n", strFile.c_str());
			return false;
		}
		else
		{
			//g_NetLog.Debug("[MainConfig::ConfigFileRight]config file %s is right\n", strFile.c_str());
			return true;
		}
	}
	catch (...)
	{
		//g_NetLog.Debug("[MainConfig::ConfigFileRight] catched!\n");
		return false;
	}
}

int MainConfig::ReadFromFile(std::string strFile)
{
	try
	{
		File file(strFile);
		if (!file.exists())
		{
			//g_NetLog.Debug("config file do not exist\n");
			return -1;
		}

		Parser  parser;
		Var     result;

		FileInputStream fis(strFile);
		result = parser.parse(fis);
		fis.close();

		m_strFile = strFile;
		m_varFullConfig = result;
		
		return 0;
	}
	catch (...)
	{
		//g_NetLog.Debug("read config file catched! \n");
		return -1;
	}
	return 0;
}

int MainConfig::SaveFile()
{
	return 0;
}

RetransChannel MainConfig::GetServerConfigInfo(int nServerType)
{
	try
	{
		Object::Ptr obj = m_varFullConfig.extract<Object::Ptr>();
		if (nServerType == 0)
		{
			m_dsHttpInfo = Object::makeStruct(obj->getObject("HttpServer"));
			m_chnnelHttp.m_strType = m_dsHttpInfo["strType"].toString();
			m_chnnelHttp.m_strIP = m_dsHttpInfo["IP"].toString();
			m_chnnelHttp.m_iPort = (int)m_dsHttpInfo["port"];
			m_chnnelHttp.m_strSerialName = m_dsHttpInfo["serialName"].toString();
			return m_chnnelHttp;
		}
		else
		{
			m_dsUDPInfo = Object::makeStruct(obj->getObject("UDPServer"));
			m_chnnelUdp.m_strType = m_dsUDPInfo["strType"].toString();
			m_chnnelUdp.m_strIP = m_dsUDPInfo["IP"].toString();
			m_chnnelUdp.m_iPort = (int)m_dsUDPInfo["port"];
			m_chnnelUdp.m_strSerialName = m_dsUDPInfo["serialName"].toString();
			return m_chnnelUdp;

		}
	}
	catch (...)
	{
		//g_NetLog.Debug("%s catched\n", __PRETTY_FUNCTION__);
	}
	RetransChannel channel;
	return channel;
}