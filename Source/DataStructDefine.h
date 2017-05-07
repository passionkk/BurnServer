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
	std::string					m_strBurnType;		//"realTimeBurn" ��pseudoRealTimeBurn�� ��fileBurn��
	int							m_nAlarmSize;
	BurnStreamInfo				m_burnStreamInfo;
	std::vector<FileInfo>		m_vecBurnFileInfo;
	int							m_nBurnSpeed;		//������¼�ٶ�
	BurnStateFeedbcak			m_burnStateFeedback;
	std::string					m_strSessionID;
};