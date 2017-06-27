#include "MainConfig.h"
#include "CommonDefine.h"
#include "NetLog.h"

#if defined(_LINUX_)
#include <unistd.h>
#else
#include <direct.h>
#endif

#include <stdio.h>
#ifndef MAX_PATH
#define  MAX_PATH 256
#endif
//const char* g_pConfigPath = "E:\\Work\\GitRepos\\BurnServer\\Bin\\x86\\Debug\\config.json";
const char* g_pConfigPath = "..\\Bin\\x86\\Debug\\config.json";
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
	{
		m_pInstance = new MainConfig;
		m_pInstance->Init();
	}
	return m_pInstance;
}

void MainConfig::Init()
{
	if (m_pInstance)
	{
		char* pPath = new char[MAX_PATH];
		memset(pPath, 0, MAX_PATH);
#if defined(_LINUX_)
		getcwd(pPath, MAX_PATH);
#else
		_getcwd(pPath, MAX_PATH);
#endif
		m_strConfigPath = pPath;
		delete[] pPath;

		m_strConfigPath += "/config.json";
		printf("config file path is %s.\n", m_strConfigPath.c_str());
		if (m_pInstance->IsConfigFileRight(m_strConfigPath))
			m_pInstance->ReadFromFile(m_strConfigPath);
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
			printf("config file : %s not exist.\n", strFile.c_str());
			return false;
		}
		printf("load config file : %s.\n", strFile.c_str());
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

std::string MainConfig::GetDownloadDir()
{
	try
	{
		Object::Ptr obj = m_varFullConfig.extract<Object::Ptr>();

		m_dsHttpInfo = Object::makeStruct(obj->getObject("DownloadDir"));
		std::string strDownloadDir = m_dsHttpInfo["DefaultDir"].toString();
		return strDownloadDir;
	}
	catch (...)
	{
		//g_NetLog.Debug("%s catched\n", __PRETTY_FUNCTION__);
	}
	return "";
}

void MainConfig::GetLogRecvInfo(std::vector<stLogRecvInfo>& vecLogRecvInfo)
{
	try
	{
		m_vecLogRecvInfo.clear();
		Object::Ptr obj = m_varFullConfig.extract<Object::Ptr>();
		m_LogRecvInfo = Object::makeStruct(obj->getObject("LogRecvInfo"));
		
		stLogRecvInfo logRecvInfo;
		logRecvInfo.strLogRecvIP = m_LogRecvInfo["ip1"].toString();
		logRecvInfo.nLogRecvPort = (int)m_LogRecvInfo["port1"];
		m_vecLogRecvInfo.push_back(logRecvInfo);

		logRecvInfo.strLogRecvIP = m_LogRecvInfo["ip2"].toString();
		logRecvInfo.nLogRecvPort = (int)m_LogRecvInfo["port2"];
		m_vecLogRecvInfo.push_back(logRecvInfo);

		logRecvInfo.strLogRecvIP = m_LogRecvInfo["ip3"].toString();
		logRecvInfo.nLogRecvPort = (int)m_LogRecvInfo["port3"];
		m_vecLogRecvInfo.push_back(logRecvInfo);

		for (int i = 0; i < m_vecLogRecvInfo.size(); i++)
		{
			vecLogRecvInfo.push_back(m_vecLogRecvInfo.at(i));
		}
	}
	catch (...)
	{
		g_NetLog.Debug("%s catched.\n", __PRETTY_FUNCTION__);
	}
}
