#pragma once
//#include <iostream>
#include <string>
#include <vector>
#include <pthread.h>
#include "Poco/Mutex.h"

enum CDROMSTATE
{
	CDROM_INIT = 0,
	CDROM_WAIT_INSERT_DISC, //等待插入光盘
	CDROM_READY,		//准备刻录
	CDROM_FORMAT,		//光盘格式化
	CDROM_BURNING,		//刻录中
	CDROM_CLOSEDISC,	//封盘
	CDROM_UNINIT
};

enum TASKSTATE
{
	TASK_INIT = 0,
	TASK_BURN,
	TASK_PAUSE,			//暂停  由刻录状态转换到PAUSE
	TASK_STOP
};

class DiskInfo
{
public:
	DiskInfo()
	{
		ntype = 0;
		maxpeed = 16;
		discsize = 0;
		usedsize = 0;
		freesize = 0;
	};

	DiskInfo(const DiskInfo& diskInfo)
	{
		ntype = diskInfo.ntype;
		maxpeed = diskInfo.maxpeed;
		discsize = diskInfo.discsize;
		usedsize = diskInfo.usedsize;
		freesize = diskInfo.freesize;
	};

	DiskInfo& operator= (const DiskInfo& diskInfo)
	{
		if (this != &diskInfo)
		{
			ntype = diskInfo.ntype;
			maxpeed = diskInfo.maxpeed;
			discsize = diskInfo.discsize;
			usedsize = diskInfo.usedsize;
			freesize = diskInfo.freesize;
		}
		return *this;
	};

	virtual ~DiskInfo(){};

public:
	int 	 	 ntype;			// 光盘类型	DISC_TYPE
	int			 maxpeed;		// 最大速度(写速度)
	unsigned int discsize;		// 光盘容量(MB)
	unsigned int usedsize;		// 已使用的大小(MB)	
	unsigned int freesize;		// 可用大小(MB)
};

class CDRomInfo
{
public:
	CDRomInfo()
		: m_discInfo()
	{
		m_strCDRomName = "";
		m_strCDRomID = "";
		m_strCDRomDevID = "";
		m_nHasDisc = 0;
		m_euWorkState = CDROM_UNINIT;
		m_pDVDHandle = NULL;
		m_nBurnedSize = 0;
	};

	CDRomInfo(const CDRomInfo& cdRomInfo)
	{
		if (this != &cdRomInfo)
		{
			m_strCDRomName = cdRomInfo.m_strCDRomName;
			m_strCDRomID = cdRomInfo.m_strCDRomID;
			m_strCDRomDevID = cdRomInfo.m_strCDRomDevID;
			m_nHasDisc = cdRomInfo.m_nHasDisc;
			m_euWorkState = cdRomInfo.m_euWorkState;
			m_pDVDHandle = cdRomInfo.m_pDVDHandle;
			m_nBurnedSize = cdRomInfo.m_nBurnedSize;
			m_discInfo = cdRomInfo.m_discInfo;
		}
	};

	CDRomInfo& operator=(const CDRomInfo& cdRomInfo)
	{
		if (this != &cdRomInfo)
		{
			m_strCDRomName = cdRomInfo.m_strCDRomName;
			m_strCDRomID = cdRomInfo.m_strCDRomID;
			m_strCDRomDevID = cdRomInfo.m_strCDRomDevID;
			m_nHasDisc = cdRomInfo.m_nHasDisc;
			m_euWorkState = cdRomInfo.m_euWorkState;
			m_pDVDHandle = cdRomInfo.m_pDVDHandle;
			m_nBurnedSize = cdRomInfo.m_nBurnedSize;
			m_discInfo = cdRomInfo.m_discInfo;
		}
		return *this;
	};

	virtual ~CDRomInfo(){};

public:
	std::string		m_strCDRomName;		//光驱名: 光驱1
	std::string		m_strCDRomID;		//光驱ID: CDRom_1 CDRom_2...CDRom_n  从1开始
	std::string		m_strCDRomDevID;	//光驱设备ID: /dev/sr0
	CDROMSTATE		m_euWorkState;		//光驱工作状态
	int				m_nHasDisc;			//是否有光盘
	void*			m_pDVDHandle;		//光驱Handle
	int				m_nBurnedSize;		//已刻录大小
	DiskInfo		m_discInfo;			//光盘信息
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
		m_strRemoteFileLocalPath = "";
		m_strDescription = "";
	};

public:
	int				m_nFlag;			//flag为0时代表stream类型，为1时代表文件类型
	std::string		m_strFileLocation;	//remote or local
	std::string		m_strType;			//文件类型，目录：dir || 文件：file
	std::string		m_strSrcUrl;		//刻录流源地址或者文件源路径
	std::string		m_strDestFilePath;	//
	std::string		m_strRemoteFileLocalPath;	//下载远端文件成功后的本地存储路径
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
		m_nThreadID = 0;
		m_nHasDisc = 0;
		m_nDVDLefCap = 0;
		m_nDVDTotalCap = 0;
	};

public:
	std::string		m_strNeedFeedback;		//是否需要反馈刻录信息，若不需要则只需在封盘前反馈
	std::string		m_strFeedbackIP;		//反馈主机IP
	int				m_nFeedbackPort;		//反馈主机端口
	std::string		m_transType;			//通信协议方式 http 或者 udp，目前反馈都用udp
	int				m_nFeedbackInterval;	//反馈时间间隔 单位毫秒
	pthread_t		m_nThreadID;			//反馈线程ID
	int				m_nHasDisc;				//是否有光盘
	int				m_nDVDLefCap;			//光盘剩余空间
	int				m_nDVDTotalCap;			//光驱总大小
};

class BurnTask
{
public:
	BurnTask()
	{
		m_strCDRomID = "";
		m_strCDRomName = "";
		m_vecCDRomInfo.clear();
		m_nUseCDRomIndex = 0;
		m_strBurnMode = "";
		m_strBurnType = "";
		m_strDiscName = "";
		m_nBurnedSize = 0;
		m_nAlarmSize = 0;
		m_vecBurnFileInfo.clear();
		m_nCurBurnFileIndex = 0;
		m_nBurnSpeed = 0;
		m_strSessionID = "";
		m_taskState = TASK_INIT;
	};

public:
	std::string					m_strCDRomID;
	std::string					m_strCDRomName;
	std::vector<CDRomInfo>		m_vecCDRomInfo;
	int							m_nUseCDRomIndex;
	DiskInfo					m_discInfo;			//光盘信息，包含光盘类型，大小等。
	int							m_nBurnedSize;		//已刻录大小
	int							m_nAlarmSize;		//报警大小阈值
	std::string					m_strBurnMode;		//"singleBurn" "doubleParallelBurn" "doubleRelayBurn" 
	std::string					m_strDiscName;
	std::string					m_strBurnType;		//"realTimeBurn" ”pseudoRealTimeBurn” ”fileBurn”
	BurnStreamInfo				m_burnStreamInfo;
	std::vector<FileInfo>		m_vecBurnFileInfo;
	std::vector<FileInfo>		m_vecFeedbackFileInfo;
	int							m_nCurBurnFileIndex;	//当前刻录文件索引
	int							m_nBurnSpeed;		//光驱刻录速度
	BurnStateFeedbcak			m_burnStateFeedback;
	std::string					m_strSessionID;
	TASKSTATE					m_taskState;		//接收上层控制刻录状态
	TASKSTATE					m_taskRealState;	//实际刻录状态
};