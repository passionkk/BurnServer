#pragma once

class RetransChannel
{
public:
	RetransChannel()
	{
		m_strType = "";
		m_strIP = "";
		m_iPort = 0;
		m_strSerialName = "";
	}
public:
	std::string m_strType;
	std::string m_strIP;
	int m_iPort;
	std::string m_strSerialName;
};

class MainConfig
{
public:
	static MainConfig* Initialize();
	static void Uninitialize();
	static MainConfig* GetInstance();
	
	int ReadFromFile(std::string strFile);
	int SaveFile();

	//获取服务器配置信息 nServerType：0，Http；1，UDP
	Poco::DynamicStruct GetServerConfigInfo(int nServerType);
	//int GetHttpServerConfigInfo();
	//int GetUDPServerConfigInfo();

public:
	MainConfig();
	virtual ~MainConfig();

protected:
	static MainConfig*	m_pInstance;
	std::string			m_strFile;
};

