#pragma once
#include <iostream>
#include <string>
#include "DataStructDefine.h"
#include <vector>
//#include "CommonDefine.h"
#include "poco/Runnable.h"
#include "poco/Thread.h"
#include "Poco/Event.h"

class CBusiness : public Poco::Runnable
{
public:
	CBusiness();
	~CBusiness();

	static CBusiness *Initialize();
	static void Uninitialize();
	static CBusiness *GetInstance();

	virtual void run();
public:
	//����Э�鴦��
	void		GetCDRomList(std::vector<CDRomInfo>& vecCDRomInfo);
	bool		StartBurn(BurnTask& task);
	bool		PauseBurn(std::string strSessionID);
	bool		ResumeBurn(std::string strSessionID);
	bool		StopBurn(std::string strSessionID);
	void		GetCDRomInfo(std::string strCDRomID);// ��������������϶�Ҫ����ֵ��û������������string����
	void		AddBurnFile(std::string strSessionID, std::vector<FileInfo>& vecFileInfo);
	//����Э�鴦��
	//��¼״̬����Э��
	void		BurnStateFeedback();
	//����ǰ��¼״̬����Э��
	void		CloseDiscFeedback();

	//

private:
	void Init();

	int  GetCDRomListFromFile(const char* pFilePath);
	int	 ExtractString(const char *head, char *end,
					   char *src, char *buffer);
public:
	static CBusiness* m_pInstance;

private:
	std::vector<CDRomInfo>	m_vecCDRomInfo;
	BurnTask				m_burnTask;				//����ִ�еĿ�¼����
	std::vector<BurnTask>	m_vecBurnTask;			//����δִ�еĿ�¼����
	std::vector<BurnTask>	m_vecBurningTask;		//��������ִ�еĿ�¼����
	std::vector<BurnTask>	m_vecFinishedTask;		//��������ɵ�task
	Poco::Thread			m_thread;
	Poco::Event				m_ready;
	bool					m_bStop;
};

