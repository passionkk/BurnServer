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
#include "DataOpr.h"

//////////////////////////////////////////////////////////////////////////
/*
单盘刻录	singleBurn
双盘续刻 doubleRelayBurn
双盘同刻 doubleParallelBurn
*/
/*
重构实现的目标：
1.任务统一性：同一时刻操作的始终是同一个任务，不存在任务队列里的任务跟所刻录任务不一致情况
2.接口统一性：协议相同，获取信息相同的逻辑，统一接口获取，不存在不同的接口获取同一个或一类的信息。代码封装性要好
3.功能模块化：每个独立的流程模块化，不存在相互依赖或者干扰的情况
4.可拓展性好：为之后修改留余地，方便修改
5.修改时候想好双盘同刻情况
6.光驱跟任务的关系，是从属还是平行。从属的话对于任务执行过程中比较方便修改光驱信息，且外部改动对内部执行任务影响不大。
								平行的话，任务状态的变化会更改光驱信息，获取光驱列表和缓存不一致也会导致更改光驱信息。但是数据是一致性的。
								暂定为从属，平行关系不好处理
7.加上光驱智能判断，双盘刻录任务插入一个光驱时候，也要能刻录。插入2个之后能自动变更为2个
8.获取光驱信息做成一个接口，入出字符串，外部直接调用拿到拼好的json串发送
9.无文件刻录，此时结束刻录任务，是否需要弹盘
10.已知bug
	刻录同一文件到光盘，会break
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
	bool		StartBurn(BurnTask* task);
	bool		PauseBurn(std::string strSessionID);
	bool		ResumeBurn(std::string strSessionID);
	bool		StopBurn(std::string strSessionID);
	int			GetCDRomInfo(std::string strCDRomID, CDRomInfo& cdRomInfo, DiskInfo& discInfo);
	std::string GetCDRomInfo(std::string strCDRomID, std::string strMethod = "");
	void		AddBurnFile(std::string strSessionID, std::vector<FileInfo>& vecFileInfo);
	//nCDRomIndex 区分双盘同刻时的光驱索引
	void		AddFeedbackFile(std::string strSessionID, std::vector<FileInfo>& vecFileInfo, int nCDRomIndex = 0);

	//主动协议处理 内部调用BurnStateFeedback
	static void* BurnStateFeedbackThread(void* pVoid);
	//刻录状态反馈协议
	void		BurnStateFeedback();

	//封盘前刻录状态反馈协议
	void		CloseDiscFeedback(int leftCap = 0, int totalCap = 0, int nCDRomIndex = 0);//这个接口应该传给task作为参数

	//封盘线程
	static void* CloseDiscThread(void* pVoid);
	void		CloseDisc();

	int			GetCurTask(BurnTask* & task);
	int			GetFirstTaskInVec(BurnTask* & task);
	//业务逻辑
	int			CheckUnusedCDRom();
	int			GetUndoTask(BurnTask* & task);
	void		DoTask(BurnTask* task);
	void		DeleteTask();

	void        TrayCDRom(BurnTask* task, std::string strCDRomID);
	//异常情况弹盘处理  应用于 未开始刻录就停止刻录，或者刻录的第一个文件大小刻录结束后超过刻录报警大小
	void		TrayCDRomException(BurnTask* task, std::string strCDRomID, int nCDRomIndex);

	//设置光驱状态
	void		SetCDRomState(std::string strCDRomID, CDROMSTATE state);
	std::string GetCDRomState(CDROMSTATE state, int& nFeedbackState);
	void		ResetCDRomState();
	void		SetTaskCDRomState(std::string strCDRomID, CDROMSTATE state);

	//设置光驱大小
	void		SetCDRomBurnSize(std::string strCDRomID, int nBurnSize);

	//获取一个空闲状态的光驱
	void*		GetIdleCDRom(BurnTask* task, std::string& strCDRomID, int& nCDRomIndex);
	void*		GetSpecCDRom(BurnTask* task, std::string& strCDRomID, int& nCDRomIndex);
	void*		GetSpecCDRomHandle(BurnTask* task, std::string& strCDRomID, int& nCDRomIndex);
	void*		GetWorkingCDRomHandle(BurnTask* task, std::string& strCDRomID);
	int			ChooseCDRomToBurn(BurnTask*& task);
	int			InitCDRom(BurnTask*& task);
	void		BurnStreamInfoToDisk(const BurnTask* task);
	//单盘刻录 双盘续刻
	void		BurnFileToDisk(BurnTask* task);
	//双盘同刻
	void		StartParallelBurnFileToDisc(BurnTask* task);
	static void* ParallelBurnFileThread(void* pVoid);
	void		ParallelBurnFile();

	//nCDRomIndex  双盘同刻 区分光驱索引
	void		BurnFeedbackFileToDisc(BurnTask* task,const DVDDRV_HANDLE pHandle, int nCDRomIndex = 0);
	int			WriteFileToDisk(void* pHandle, void* pFileHandle, std::string strLocalPath);
	int			CloseDisc(void* pvHandle, bool bHasBurnFile = true);
	int			BurnLocalFile(void* pHandle, FileInfo& fileInfo /*std::string strSrcPath, std::string strDestPath*/);
	int			BurnLocalFile(void* pHandle, std::string strSrcPath, std::string strDestPath);
	int			BurnLocalDir(void* pHandle, FileInfo fileInfo /*std::string strSrcPath, std::string strDestPath*/);
	//下载远端文件或目录 strType: "file" or "dir"; strSrcUrl:源文件或文件夹路径 如 http://192.168.1.1:8080/download/a.mp4
	//											 strDestUrl:目标文件或文件夹路径,默认从配置文件中读取目标文件夹，后跟文件名如 /media/BurnServer/Download/a.mp4											
	static std::string		Download(std::string strType, std::string strSrcUrl, std::string strDestUrl = "");
	
	//根据原目标地址生成本地目标地址，用于下载时目标文件的生成  strSrcUrl /Dir1/abc.mp4  localPath--> /mnt/HD0/abc.mp4
	static void		GenerateLocalPath(std::string strSrcUrl, std::string& localPath);
	void		GetBurnStateString(int nBurnState, std::string& strDes);

	void		ConvertDirToFileInfo(BurnTask* task, std::string strSrcDir, std::string strDestDir, bool bFeedback = false);
	void		GetFileListByDir(std::string strDir, std::vector<std::string>& vecFileList);
	int			GetBurnTaskSize();
	bool		CheckExistCDRom(std::string strCDRomID);
	
	//获取光驱信息
	void		GetCDRomInfoFromSavedVector(std::string strCDRomID, CDRomInfo& cdRomInfo);

private:
	void		Init();
	void		InitLog();

	static std::string GetCurDir();
	int			GetCDRomListFromFile(const char* pFilePath, bool bReCheck = false);
	int			ExtractString(const char *head, const char *end, char *src, char *buffer);


public:
	static CBusiness* m_pInstance;

private:
	Poco::Mutex				m_mutexCDRomInfoVec;
	std::vector<CDRomInfo>	m_vecCDRomInfo;
	BurnTask*				m_burnTask;				//正在执行的刻录任务
	Poco::Mutex				m_mutexBurnTask;
	std::vector<BurnTask*>	m_vecBurnTask;			//保存未执行的刻录任务
	Poco::Mutex				m_mutexBurnTaskVec;
	Poco::Thread			m_thread;
	Poco::Event				m_ready;
	bool					m_bStop;
	std::map<std::string, DVDSDK_DIR> m_mapDirToHandle;
	bool					m_bThreadState;
	void*					m_pCloseHandle;
};

