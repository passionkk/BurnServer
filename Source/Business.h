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
	//被动协议处理
	std::string GetCDRomList();
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

private:
	void Init();

public:
	static CBusiness* m_pInstance;

private:
	std::vector<CDRomInfo>	m_vecCDRomInfo;
	BurnTask				m_burnTask;				//正在执行的刻录任务
	std::vector<BurnTask>	m_vecBurnTask;			//保存未完成的刻录任务
	std::vector<BurnTask>	m_vecFinishedTask;		//保存已完成的task
};

