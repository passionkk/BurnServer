#pragma once
//#include <iostream>
#include <string>
#include <vector>

class CDRomInfo
{
public:
	CDRomInfo()
	{
		m_strCDRomName = "";
		m_strCDRomID = "";
	};

	virtual ~CDRomInfo(){};

public:
	std::string		m_strCDRomName;
	std::string		m_strCDRomID;
};

class FileInfo
{
public:
	FileInfo()
	{
		m_nFlag = 0;
		m_strFileLocation = "";
		m_strType = "";
		m_strSrcUrl = "";
		m_strDestFilePath = "";
		m_strDescription = "";
	};

public:
	int				m_nFlag;			//flag为0时代表stream类型，为1时代表文件类型
	std::string		m_strFileLocation;	//remote or local
	std::string		m_strType;			//文件类型，目录：dir || 文件：file
	std::string		m_strSrcUrl;		//刻录流源地址或者文件源路径
	std::string		m_strDestFilePath;
	std::string		m_strDescription;
};

class BurnStreamInfo
{
public:
	BurnStreamInfo()
	{
		m_strBurnFileName = "";
		m_strPlayListContent = "";
		m_vecUrlList.clear();
	};

public:
	std::string		m_strBurnFileName;
	std::string		m_strPlayListContent;
	std::vector<FileInfo>	m_vecUrlList;
};

//刻录状态反馈
class BurnStateFeedbcak
{
public:
	BurnStateFeedbcak()
	{
		m_strNeedFeedback = "";
		m_strFeedbackIP = "";
		m_nFeedbackPort = 93;
		m_transType = "udp";
		m_nFeedbackInterval = 5000;
	};

public:
	std::string		m_strNeedFeedback;		//是否需要反馈刻录信息，若不需要则只需在封盘前反馈
	std::string		m_strFeedbackIP;		//反馈主机IP
	int				m_nFeedbackPort;		//反馈主机端口
	std::string		m_transType;			//通信协议方式 http 或者 udp，目前反馈都用udp
	int				m_nFeedbackInterval;	//反馈时间间隔 单位毫秒
};

class BurnTask
{
public:
	BurnTask()
	{
		m_strBurnMode = "";
		m_strBurnType = "";
		m_nAlarmSize = 0;
		m_vecBurnFileInfo.clear();
		m_nBurnSpeed = 0;
		m_strSessionID = "";
	};

public:
	std::string					m_strCDRomID;
	std::string					m_strCDRomName;
	std::string					m_strBurnMode;		//"singleBurn" "doubleParallelBurn" "DoubleRelayBurn" 
	std::string					m_strBurnType;		//"realTimeBurn" ”pseudoRealTimeBurn” ”fileBurn”
	int							m_nAlarmSize;
	BurnStreamInfo				m_burnStreamInfo;
	std::vector<FileInfo>		m_vecBurnFileInfo;
	int							m_nBurnSpeed;		//光驱刻录速度
	BurnStateFeedbcak			m_burnStateFeedback;
	std::string					m_strSessionID;
};