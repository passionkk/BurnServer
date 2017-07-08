#pragma once
#include <iostream>
#include <string>
#include "DataStructDefine.h"
#include <vector>
#include <map>
#include "Poco/Runnable.h"
#include "Poco/Thread.h"
#include "Poco/Event.h"
#include "Poco/Mutex.h"
#include "LibDVDSDK.h"

//////////////////////////////////////////////////////////////////////////
/*
单盘刻录	singleBurn
双盘续刻 doubleRelayBurn
双盘同刻 doubleParallelBurn
*/
//////////////////////////////////////////////////////////////////////////

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
	int			GetCDRomInfo(std::string strCDRomID, CDRomInfo& cdRomInfo, DiskInfo& discInfo);// 这里参数不够，肯定要返回值，没想好是类对象还是string对象
	std::string GetCDRonInfo(std::string strCDRomID, std::string strMethod = "");
	void		AddBurnFile(std::string strSessionID, std::vector<FileInfo>& vecFileInfo);
	void		AddFeedbackFile(std::string strSessionID, std::vector<FileInfo>& vecFileInfo);
	//主动协议处理 内部调用BurnStateFeedback
	static void* BurnStateFeedbackThread(void* pVoid);
	//刻录状态反馈协议
	void		BurnStateFeedback();

	//封盘前刻录状态反馈协议
	void		CloseDiscFeedback(int leftCap = 0, int totalCap = 0);//这个接口应该传给task作为参数

	//封盘线程
	static void* CloseDiscThread(void* pVoid);
	void		CloseDisc();

	int			GetCurTask(BurnTask& task);
	//业务逻辑
	int			CheckUnusedCDRom();
	int			GetUndoTask(BurnTask& task);
	void		DoTask(BurnTask& task);
	void		DeleteTask();

	void		SetCDRomState(std::string strCDRomID, CDROMSTATE state);
	void*		GetIdleCDRom(BurnTask& task, std::string& strCDRomID, int& nCDRomIndex);
	void*		GetSpecCDRom(BurnTask& task, std::string& strCDRomID, int& nCDRomIndex);
	void*		GetWorkingCDRomHandle(BurnTask& task, std::string& strCDRomID);
	int			ChooseCDRomToBurn(BurnTask& task);
	int			InitCDRom(BurnTask& task);
	void		BurnStreamInfoToDisk(const BurnTask& task);
	void		BurnFileToDisk(BurnTask& task);
	void		BurnFeedbackFileToDisc(BurnTask& task,const DVDDRV_HANDLE pHandle);
	int			WriteFileToDisk(void* pHandle, void* pFileHandle, std::string strLocalPath);
	int			CloseDisk(void* pvHandle);
	int			BurnLocalFile(void* pHandle, FileInfo& fileInfo /*std::string strSrcPath, std::string strDestPath*/);
	int			BurnLocalFile(void* pHandle, std::string strSrcPath, std::string strDestPath);
	int			BurnLocalDir(void* pHandle, FileInfo fileInfo /*std::string strSrcPath, std::string strDestPath*/);
	//下载远端文件或目录 strType: "file" or "dir"; strSrcUrl:源文件或文件夹路径 如 http://192.168.1.1:8080/download/a.mp4
	//											 strDestUrl:目标文件或文件夹路径,默认从配置文件中读取目标文件夹，后跟文件名如 /media/BurnServer/Download/a.mp4											
	static std::string		Download(std::string strType, std::string strSrcUrl, std::string strDestUrl = "");
	
	//根据原目标地址生成本地目标地址，用于下载时目标文件的生成
	static void		GenerateLocalPath(std::string strSrcUrl, std::string& localPath);
	void		GetBurnStateString(int nBurnState, std::string& strDes);

	void		ConvertDirToFileInfo(BurnTask& task, std::string strSrcDir, std::string strDestDir, bool bFeedback = false);
	void		GetFileListByDir(std::string strDir, std::vector<std::string>& vecFileList);
	int			GetBurnTaskSize();
	bool		CheckExistCDRom(std::string strCDRomID);
	void		GetCDRomInfoFromSavedVector(std::string strCDRomID, CDRomInfo& cdRomInfo);
private:
	void		Init();
	void		InitLog();

	static std::string GetCurDir();
	int			GetCDRomListFromFile(const char* pFilePath, bool bReCheck = false);
	int			ExtractString(const char *head, const char *end,
					   char *src, char *buffer);
public:
	static CBusiness* m_pInstance;

private:
	Poco::Mutex				m_mutexCDRomInfoVec;
	std::vector<CDRomInfo>	m_vecCDRomInfo;
	BurnTask				m_burnTask;				//正在执行的刻录任务
	std::vector<BurnTask>	m_vecBurnTask;			//保存未执行的刻录任务
	std::vector<BurnTask>	m_vecFinishedTask;		//保存已完成的task
	Poco::Mutex				m_mutexBurnTaskVec;
	Poco::Mutex				m_mutexVecBurnFileInfo;
	Poco::Thread			m_thread;
	Poco::Event				m_ready;
	bool					m_bStop;
	std::map<std::string, DVDSDK_DIR> m_mapDirToHandle;
	bool					m_bThreadState;
	void*					m_pCloseHandle;
};

