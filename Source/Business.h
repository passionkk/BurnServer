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
	//协议处理
	//被动协议处理
	void		GetCDRomList(std::vector<CDRomInfo>& vecCDRomInfo);
	bool		StartBurn(BurnTask& task);
	bool		PauseBurn(std::string strSessionID);
	bool		ResumeBurn(std::string strSessionID);
	bool		StopBurn(std::string strSessionID);
	void		GetCDRomInfo(std::string strCDRomID);// 这里参数不够，肯定要返回值，没想好是类对象还是string对象
	void		AddBurnFile(std::string strSessionID, std::vector<FileInfo>& vecFileInfo);
	//主动协议处理
	//刻录状态反馈协议
	void		BurnStateFeedback();
	//封盘前刻录状态反馈协议
	void		CloseDiscFeedback();

	//业务逻辑
	int			CheckUnusedCDRom();
	int			GetUndoTask(BurnTask& task);
	void		DoTask(const BurnTask& task);

	int			CheckCDDriveState(const char* pCDRomID);
	void		BurnStreamInfoToFile(const BurnTask& task);
	void		BurnFileToFile(const BurnTask& task);

private:
	void Init();

	static std::string GetCurDir();
	int  GetCDRomListFromFile(const char* pFilePath);
	int	 ExtractString(const char *head, char *end,
					   char *src, char *buffer);
public:
	static CBusiness* m_pInstance;

private:
	Poco::Mutex				m_mutexCDRomInfoVec;
	std::vector<CDRomInfo>	m_vecCDRomInfo;
	BurnTask				m_burnTask;				//正在执行的刻录任务
	std::vector<BurnTask>	m_vecBurnTask;			//保存未执行的刻录任务
	std::vector<BurnTask>	m_vecBurningTask;		//保存正在执行的刻录任务
	std::vector<BurnTask>	m_vecFinishedTask;		//保存已完成的task
	Poco::Mutex				m_mutexBurnTaskVec;
	Poco::Thread			m_thread;
	Poco::Event				m_ready;
	bool					m_bStop;
};

