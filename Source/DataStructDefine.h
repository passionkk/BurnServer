#pragma once
//#include <iostream>
#include <string>
#include <vector>
#include "Poco/Mutex.h"

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
	std::string		m_strCDRomName;		//������: ����1
	std::string		m_strCDRomID;		//����ID: /dev/sr0
	int				m_nWorkState;		//��������״̬  0��δ���� 1��������
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

	virtual ~DiskInfo(){};

public:
	int 	 	 ntype;			// ��������	DISC_TYPE
	int			 maxpeed;		// ����ٶ�(д�ٶ�)
	unsigned int discsize;		// ��������(MB)
	unsigned int usedsize;		// ��ʹ�õĴ�С(MB)	
	unsigned int freesize;		// ���ô�С(MB)
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
	int				m_nFlag;			//flagΪ0ʱ����stream���ͣ�Ϊ1ʱ�����ļ�����
	std::string		m_strFileLocation;	//remote or local
	std::string		m_strType;			//�ļ����ͣ�Ŀ¼��dir || �ļ���file
	std::string		m_strSrcUrl;		//��¼��Դ��ַ�����ļ�Դ·��
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

//��¼״̬����
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
	std::string		m_strNeedFeedback;		//�Ƿ���Ҫ������¼��Ϣ��������Ҫ��ֻ���ڷ���ǰ����
	std::string		m_strFeedbackIP;		//��������IP
	int				m_nFeedbackPort;		//���������˿�
	std::string		m_transType;			//ͨ��Э�鷽ʽ http ���� udp��Ŀǰ��������udp
	int				m_nFeedbackInterval;	//����ʱ���� ��λ����
};

class BurnTask
{
public:
	BurnTask()
	{
		m_strCDRomID = "";
		m_strCDRomName = "";
		m_vecCDRomInfo.clear();
		m_vecDVDHandle.clear();
		m_strBurnMode = "";
		m_strBurnType = "";
		m_ullBurnedSize = 0;
		m_nAlarmSize = 0;
		m_vecBurnFileInfo.clear();
		m_nCurBurnFileIndex = 0;
		m_nBurnSpeed = 0;
		m_strSessionID = "";
	};

public:
	std::string					m_strCDRomID;
	std::string					m_strCDRomName;
	std::vector<CDRomInfo>		m_vecCDRomInfo;
	std::vector<void*>			m_vecDVDHandle;
	DiskInfo					m_diskInfo;			//������Ϣ�������������ͣ���С�ȡ�
	unsigned long long			m_ullBurnedSize;	//�ѿ�¼��С
	int							m_nAlarmSize;		//������С��ֵ
	std::string					m_strBurnMode;		//"singleBurn" "doubleParallelBurn" "doubleRelayBurn" 
	std::string					m_strBurnType;		//"realTimeBurn" ��pseudoRealTimeBurn�� ��fileBurn��
	BurnStreamInfo				m_burnStreamInfo;
	std::vector<FileInfo>		m_vecBurnFileInfo;
	int							m_nCurBurnFileIndex;	//��ǰ��¼�ļ�����
	int							m_nBurnSpeed;		//������¼�ٶ�
	BurnStateFeedbcak			m_burnStateFeedback;
	std::string					m_strSessionID;
};