#pragma once
#include <iostream>
#include <string>
#include "DataStructDefine.h"
#include <vector>

class CBusiness
{
public:
	CBusiness();
	~CBusiness();

	static CBusiness *Initialize();
	static void Uninitialize();
	static CBusiness *GetInstance();

public:
	//����Э�鴦��
	std::string GetCDRomList();
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

private:
	void Init();

public:
	static CBusiness* m_pInstance;

private:
	std::vector<CDRomInfo>	m_vecCDRomInfo;
	BurnTask				m_burnTask;				//����ִ�еĿ�¼����
	std::vector<BurnTask>	m_vecBurnTask;			//����δ��ɵĿ�¼����
	std::vector<BurnTask>	m_vecFinishedTask;		//��������ɵ�task
};

