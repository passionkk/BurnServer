#pragma once
#include "CommonDefine.h"

struct stLogRecvInfo
{
	stLogRecvInfo(){
		strLogRecvIP = "";
		nLogRecvPort = 0;
	};
	stLogRecvInfo(std::string strIP, int nPort){
		strLogRecvIP = strIP;
		nLogRecvPort = nPort;
	};
	std::string strLogRecvIP;
	int nLogRecvPort;
};

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
	std::string GetDownloadDir();

	//获取报警大小值
	int GetDiscAlarmSize();

	//获取LogServer信息
	void GetLogRecvInfo(std::vector<stLogRecvInfo>& pLogRecvInfo);

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

	//disc alarm size
	Poco::DynamicStruct			m_dsAlarmSize;
	int							m_nDiscAlarmSize;

	//Log Recv config
	Poco::DynamicStruct			m_LogRecvInfo;
	std::vector<stLogRecvInfo>	m_vecLogRecvInfo;
};

