#pragma once
#include <iostream>
#include <string>
#include "DataStructDefine.h"
#include <vector>
//#include "CommonDefine.h"
#include "poco/Runnable.h"
#include "poco/Thread.h"
#include "Poco/Event.h"
#include "Poco/Mutex.h"

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
	//Э�鴦��
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
	void		CloseDiscFeedback();//����ӿ�Ӧ�ô���task��Ϊ����

	//ҵ���߼�
	int			CheckUnusedCDRom();
	int			GetUndoTask(BurnTask& task);
	void		DoTask(BurnTask& task);

	void		SetCDRomState(std::string strCDRomID, CDROMSTATE state);
	void*		GetIdleCDRom(BurnTask& task, std::string& strCDRomID, int& nCDRomIndex);
	int			ChooseCDRomToBurn(BurnTask& task);
	int			InitCDRom(BurnTask& task);
	void		BurnStreamInfoToDisk(const BurnTask& task);
	void		BurnFileToDisk(BurnTask& task);
	int			WriteFileToDisk(void* pHandle, void* pFileHandle, std::string strLocalPath);

	//����Զ���ļ���Ŀ¼ strType: "file" or "dir"; strSrcUrl:Դ�ļ����ļ���·�� �� http://192.168.1.1:8080/download/a.mp4
	//											   strDestUrl:Ŀ���ļ����ļ���·��,Ĭ�ϴ������ļ��ж�ȡĿ���ļ��У�����ļ����� /media/BurnServer/Download/a.mp4											
	static std::string		Download(std::string strType, std::string strSrcUrl, std::string strDestUrl = "");
	//����ԭĿ���ַ���ɱ���Ŀ���ַ����������ʱĿ���ļ�������
	static void		GenerateLocalPath(std::string strSrcUrl, std::string& localPath);
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
	BurnTask				m_burnTask;				//����ִ�еĿ�¼����
	std::vector<BurnTask>	m_vecBurnTask;			//����δִ�еĿ�¼����
	std::vector<BurnTask>	m_vecBurningTask;		//��������ִ�еĿ�¼����
	std::vector<BurnTask>	m_vecFinishedTask;		//��������ɵ�task
	Poco::Mutex				m_mutexBurnTaskVec;
	Poco::Mutex				m_mutexVecBurnFileInfo;
	Poco::Thread			m_thread;
	Poco::Event				m_ready;
	bool					m_bStop;
};

