#pragma once
#include "CommonDefine.h"

class MainConfig
{
public:
	static MainConfig* Initialize();
	static void Uninitialize();
	static MainConfig* GetInstance();
	
	bool IsConfigFileRight(std::string strFile);
	int ReadFromFile(std::string strFile);
	int SaveFile();

	//获取服务器配置信息 nServerType：0，Http；1，UDP
	RetransChannel GetServerConfigInfo(int nServerType);
	//int GetHttpServerConfigInfo();
	//int GetUDPServerConfigInfo();
	std::string GetDownloadDir();
public:
	MainConfig();
	virtual ~MainConfig();

protected:
	void Init();

protected:
	static MainConfig*			m_pInstance;
	std::string					m_strFile;
	Poco::Dynamic::Var          m_varFullConfig;
	
	//http server config
	Poco::DynamicStruct			m_dsHttpInfo;
	RetransChannel				m_chnnelHttp;
	
	//udp server config
	Poco::DynamicStruct			m_dsUDPInfo;
	RetransChannel				m_chnnelUdp;
	std::string					m_strConfigPath;
};

