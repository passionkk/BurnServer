#include "Business.h"
#include "HttpServerModule.h"
#include "UDPServerModule.h"
#include "json/json.h"
//#include "json.h"
#include "UDPClient.h"
#include "Poco/Net/NetException.h"
#include "LibDVDSDK.h"
#include "curl/curl.h"
#include "FileUtil.h"
#include "DirectoryUtil.h"
#include "MainConfig.h"
#include "DownloadFile.h"

#define DEFAULTPACKED 32 * 1024

CBusiness* CBusiness::m_pInstance = NULL;

CBusiness::CBusiness() :Runnable(),
	m_thread("CBusiness"),
	m_ready(),
	m_bStop(true)
{
}

CBusiness::~CBusiness()
{
}

CBusiness* CBusiness::Initialize()
{
	return CBusiness::GetInstance();
}

void CBusiness::Uninitialize()
{
	if (m_pInstance != NULL)
	{
		UDPServerModule::Uninitialize();
		HttpServerModule::Uninitialize();
		curl_global_cleanup();
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

CBusiness* CBusiness::GetInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CBusiness;
		m_pInstance->Init();
	}

	return m_pInstance;
}

void CBusiness::run()
{
	m_ready.set();
	while (!m_bStop)
	{
		try
		{
			//1.检测光驱状态，
			//2.如果第一个光驱处于空闲状态，取一个 使用第一个光驱刻录的任务
			//3.如果第二个光驱处于空闲状态，取一个 使用第二个光驱刻录的任务
			//依次类推
			//遇到双盘刻录任务，
			//4.双盘续刻 则等待直到其中某个处于空闲状态等待直到
			//5.双盘同刻，则等待直到两个光驱都空闲再执行刻录
			printf("GetUndoTask..\n");
			//BurnTask task;
			if (0== GetUndoTask(m_burnTask/*task*/))
				DoTask(m_burnTask/*task*/);
#if defined (_LINUX_)
			sleep(1);	//sleep 1 sec
#endif 
		}
		catch (Poco::Net::NetException & exc)
		{
			//NetException
		}
		catch (...)
		{
			//g_NetLog.Debug("[UDPServerChannel::run] catch!\n");
		}
	}
}

void CBusiness::Init()
{
	std::string strCurPath = CBusiness::GetCurDir();
#if defined(_LINUX_)
	chdir(strCurPath.c_str());
#endif
	GetCDRomListFromFile("./CDRom_List");
	for (int i = 0; i < m_vecCDRomInfo.size(); i++)
	{
		printf("cdrom :%d, cdromID:%s, cdromName:%s.\n", i, m_vecCDRomInfo.at(i).m_strCDRomID.c_str(), m_vecCDRomInfo.at(i).m_strCDRomName.c_str());
	}
	//初始化CURL
	curl_global_init(CURL_GLOBAL_ALL);
	HttpServerModule::Initialize();
	UDPServerModule::Initialize();
	m_bStop = false;
	m_thread.start(*this);
	m_ready.wait();
}

std::string CBusiness::GetCurDir()
{
	char szCwd[1024] = { 0 };

#ifdef WIN32
	GetModuleFileNameA(NULL, szCwd, sizeof(szCwd) / sizeof(char));

	int nLen = strlen(szCwd);
	while (nLen > 0 &&
		   szCwd[nLen - 1] != '\\')
	{
		szCwd[nLen - 1] = '\0';
		nLen = strlen(szCwd);
	}
#else
	int nLen = readlink("/proc/self/exe", szCwd, sizeof(szCwd));
	if (nLen > 0)
	{
		szCwd[nLen] = '\0';

		while (nLen > 0 &&
			   szCwd[nLen - 1] != '/')
		{
			szCwd[nLen - 1] = '\0';

			nLen = strlen(szCwd);
		}
	}
#endif

	return std::string(szCwd);
}

//协议处理
int CBusiness::GetCDRomListFromFile(const char* pFilePath)
{
	if (m_vecCDRomInfo.size() > 0)
		return 0;
	char buf[2000];
	int size;
	FILE *fd;
	system("bash ./Get_CDRom_Dev_Info.sh");
	fd = fopen(pFilePath, "r");
	if (fd == NULL)
	{
		printf("open file error!\n");
		return -1;
	}
	size = fread(buf, 1, 1024, fd);
	if (size <= 0)
	{
		printf("read file error!\n");
		fclose(fd);
		return -1;
	}
	buf[size] = '\0';
	fclose(fd);

	//std::vector<CDRomInfo> vecCDRomInfoTmp;
	m_vecCDRomInfo.clear();
	char dev[200];
	memset(dev, 0, 200);
	if (ExtractString("<dev1>", "</dev1>", buf, dev) == 0)
	{
		CDRomInfo info;
		info.m_strCDRomName = "光驱1";
		info.m_strCDRomID = dev;
		m_vecCDRomInfo.push_back(info);
		printf("dev1 = %s\n", dev);
	}
	memset(dev, 0, 200);
	if (ExtractString("<dev2>", "</dev2>", buf, dev) == 0)
	{
		CDRomInfo info;
		info.m_strCDRomName = "光驱2";
		info.m_strCDRomID = dev;
		m_vecCDRomInfo.push_back(info);
		printf("dev2 = %s\n", dev);
	}
	memset(dev, 0, 200);
	if (ExtractString("<dev3>", "</dev3>", buf, dev) == 0)
	{
		CDRomInfo info;
		info.m_strCDRomName = "光驱3";
		info.m_strCDRomID = dev;
		m_vecCDRomInfo.push_back(info);
		printf("dev3 = %s\n", dev);
	}
	memset(dev, 0, 200);
	if (ExtractString("<dev4>", "</dev4>", buf, dev) == 0)
	{
		CDRomInfo info;
		info.m_strCDRomName = "光驱4";
		info.m_strCDRomID = dev;
		m_vecCDRomInfo.push_back(info);
		printf("dev3 = %s\n", dev);
	}

#if 0
	if (m_vecCDRomInfo.size() == 0)
	{
		for (int i = 0; i < vecCDRomInfoTmp.size(); i++)
		{
			m_vecCDRomInfo.push_back(vecCDRomInfoTmp.at(i));
		}
	}
	else
	{	//剔除无效的光驱
		std::vector<int> vecIndexAdd;
		bool bExist = false;
		for (int i = 0; i < m_vecCDRomInfo.size(); i++)
		{
			CDRomInfo cdRomInfo = m_vecCDRomInfo.at(i);
			bExist = false
			for(int j = 0; j < vecCDRomInfoTmp.size(); j++)
			{
				if (cdRomInfo.m_strCDRomID.compare(vecCDRomInfoTmp.at(j).m_strCDRomID) == 0)
				{
					bExist = true;
					vecIndexAdd.push_back(j);
					break;
				}
				if (j == vecCDRomInfoTmp.size() - 1)

			}
		}
	}
#endif
	return 0;
}

int	CBusiness::ExtractString(const char *head, const char *end,
				   char *src, char *buffer)
{
	int i = 0;
	int hn = 0, en = 0, sn = 0;
	char *hp, *ep;

	hn = strlen(head);
	en = strlen(end);
	sn = strlen(src);
	hp = strstr(src, head);
	ep = strstr(src, end);

	if ((hp != NULL) && (ep != NULL) && (hp < ep))
	{
		hp = hp + hn;
		while (hp < ep)
		{
			*(buffer + i) = *(hp++);
			i++;
		}
	}
	else
	{
		printf("error :strings head or end \n");
		return -1;
	}

	*(buffer + i) = '\0';

	return 0;
}

void CBusiness::GetCDRomList(std::vector<CDRomInfo>& vecCDRomInfo)
{
	// 调用shell脚本获取光驱列表
	printf("GetCDRomList.\n");
	//system("./Get_CDRom_Dev_Info.sh");
	GetCDRomListFromFile("CDRomList");
	vecCDRomInfo.clear();
	for (int i = 0; i < m_vecCDRomInfo.size(); i++)
		vecCDRomInfo.push_back(m_vecCDRomInfo.at(i));
}

bool CBusiness::StartBurn(BurnTask& task)
{
	bool bRet = true;
	Mutex::ScopedLock lock(m_mutexBurnTaskVec);
	m_vecBurnTask.push_back(task);
	return bRet;
}

bool CBusiness::PauseBurn(std::string strSessionID)
{
	bool bRet = true;
	if (m_burnTask.m_strSessionID.compare(strSessionID) == 0)
	{
		m_burnTask.m_taskState = TASK_PAUSE;
	}
	else
	{
		bRet = false;
		printf("PauseBurn fail, can't find seesionID : %s.\n", strSessionID.c_str());
	}
	return bRet;
}

bool CBusiness::ResumeBurn(std::string strSessionID)
{
	bool bRet = true;
	if (m_burnTask.m_strSessionID.compare(strSessionID) == 0)
	{
		m_burnTask.m_taskState = TASK_BURN;
	}
	else
	{
		bRet = false;
		printf("PauseBurn fail, can't find seesionID : %s.\n", strSessionID.c_str());
	}
	return bRet;
}

bool CBusiness::StopBurn(std::string strSessionID)
{
	bool bRet = true;
	if (m_burnTask.m_strSessionID.compare(strSessionID) == 0)
	{
		m_burnTask.m_taskState = TASK_STOP;
	}
	else
	{
		bRet = false;
		printf("PauseBurn fail, can't find seesionID : %s.\n", strSessionID.c_str());
	}
	return bRet;
}

void CBusiness::GetCDRomInfo(std::string strCDRomID)
{
	Mutex::ScopedLock lock(m_mutexCDRomInfoVec);
	//调底层接口获取光驱信息
}

void CBusiness::AddBurnFile(std::string strSessionID, std::vector<FileInfo>& vecFileInfo)
{
	if (m_burnTask.m_strSessionID.compare(strSessionID) == 0)
	{
		m_mutexVecBurnFileInfo.lock();
		m_burnTask.m_vecBurnFileInfo.resize(m_burnTask.m_vecBurnFileInfo.size() + vecFileInfo.size());
		m_burnTask.m_vecBurnFileInfo.insert(m_burnTask.m_vecBurnFileInfo.end(), vecFileInfo.begin(), vecFileInfo.end());
		m_mutexVecBurnFileInfo.unlock();
		return;
	}
	for (size_t i = 0; i< m_vecBurnTask.size(); i++)
	{
		if (!m_vecBurnTask.at(i).m_strSessionID.empty() && m_vecBurnTask.at(i).m_strSessionID.compare(strSessionID) == 0)
		{
			m_vecBurnTask.at(i).m_vecBurnFileInfo.resize(m_vecBurnTask.at(i).m_vecBurnFileInfo.size() + vecFileInfo.size());
			m_vecBurnTask.at(i).m_vecBurnFileInfo.insert(m_vecBurnTask.at(i).m_vecBurnFileInfo.end(), vecFileInfo.begin(), vecFileInfo.end());
			break;
		}
	}
}

void CBusiness::BurnStateFeedback()
{
	try
	{
		if (m_burnTask.m_burnStateFeedback.m_strNeedFeedback.compare("yes") == 0)
		{
			std::string		sMethod = "burnStateFeedback";
			Json::Value     jsonValueRoot;
			Json::Value     jsonValue1;
			Json::Value     jsonValue2;
			jsonValue2["cdRomID"] = Json::Value(m_burnTask.m_strCDRomID);
			jsonValue2["cdRomName"] = Json::Value(m_burnTask.m_strCDRomName);
			jsonValue2["sessionID"] = Json::Value(m_burnTask.m_strSessionID);
#if 0 // 调用底层接口
			jsonValue2["burnState"] = Json::Value(m_burnTask.m_strSessionID);
			jsonValue2["burnStateDescription"] = Json::Value(m_burnTask.m_strSessionID);
			jsonValue2["hasDVD"] = Json::Value(m_burnTask.m_strSessionID);
			jsonValue2["DVDLeftCapcity"] = Json::Value(m_burnTask.m_strSessionID);
			jsonValue2["DVDTotalCapcity"] = Json::Value(m_burnTask.m_strSessionID);
#endif
			jsonValue1["method"] = Json::Value(sMethod.c_str());
			jsonValue1["params"] = jsonValue2;
			jsonValueRoot["result"] = jsonValue1;
			string strOut = jsonValueRoot.toStyledString();
			UDPClient client;
			client.Init(m_burnTask.m_burnStateFeedback.m_strFeedbackIP, m_burnTask.m_burnStateFeedback.m_nFeedbackPort);
			client.ConnectServer();
			std::string strRecv;
			client.SendProtocol(strOut, strRecv);
			client.Close();
		}
	}
	catch (...)
	{
		printf("%s catched\n", __PRETTY_FUNCTION__);
	}

}

//封盘前刻录状态反馈协议
void CBusiness::CloseDiscFeedback(int leftCap, int totalCap)
{
	try
	{
		std::string		sMethod = "closeDiscFeedback";
		Json::Value     jsonValueRoot;
		Json::Value     jsonValue1;
		Json::Value     jsonValue2;
		jsonValue2["cdRomID"] = Json::Value(m_burnTask.m_strCDRomID);
		jsonValue2["cdRomName"] = Json::Value(m_burnTask.m_strCDRomName);
		jsonValue2["sessionID"] = Json::Value(m_burnTask.m_strSessionID);
#if 1 // 调用底层接口
		jsonValue2["DVDLeftCapcity"] = Json::Value(leftCap);
		jsonValue2["DVDTotalCapcity"] = Json::Value(totalCap);
#endif
		jsonValue1["method"] = Json::Value(sMethod.c_str());
		jsonValue1["params"] = jsonValue2;
		jsonValueRoot["result"] = jsonValue1;
		string strOut = jsonValueRoot.toStyledString();
		UDPClient client;
		client.Init(m_burnTask.m_burnStateFeedback.m_strFeedbackIP, m_burnTask.m_burnStateFeedback.m_nFeedbackPort);
		client.ConnectServer();
		std::string strRecv;
		client.SendProtocol(strOut, strRecv);

		Json::Reader    jsonReader;
		Json::Value     jsonValueIn;

		if (jsonReader.parse(strRecv, jsonValueIn))
		{
			std::string sMethod = jsonValueIn["method"].asString();
			Json::Value   jsonValueParams = jsonValueIn["params"];

			//DocFile 
			Json::Value jsonDocFile = jsonValueParams["DocFile"].asString();
			FileInfo fileInfo;
			fileInfo.m_strType = "file";
			fileInfo.m_strFileLocation = jsonDocFile["fileLocation"].asString();
			fileInfo.m_strSrcUrl = jsonDocFile["burnSrcFilePath"].asString();
			fileInfo.m_strDestFilePath = jsonDocFile["burnDstFilePath"].asString();
			fileInfo.m_strDescription = jsonDocFile["fileDescription"].asString();

			//fileInfo vector
			std::vector<FileInfo> vecFileInfo;
			Json::Value jsonValueFileList = jsonValueParams["burnFileList"];
			for (int i = 0; i < jsonValueFileList.size(); i++)
			{
				FileInfo fileInfo;
				fileInfo.m_strFileLocation = jsonValueFileList[i]["fileLocation"].asString();
				fileInfo.m_strType = jsonValueFileList[i]["fileType"].asString();
				fileInfo.m_strSrcUrl = jsonValueFileList[i]["burnSrcFilePath"].asString();
				fileInfo.m_strDestFilePath = jsonValueFileList[i]["burnDstFilePath"].asString();
				fileInfo.m_strDescription = jsonValueFileList[i]["fileDescription"].asString();
				vecFileInfo.push_back(fileInfo);
			}
			CBusiness::GetInstance()->AddBurnFile(m_burnTask.m_strSessionID, vecFileInfo);
		}
	}
	catch (...)
	{
		printf("%s catched\n", __PRETTY_FUNCTION__);
	}
}


//业务处理
int CBusiness::GetUndoTask(BurnTask& task)
{
	int nRet = -1;
	Mutex::ScopedLock   lock(m_mutexBurnTaskVec);
	if (m_vecBurnTask.size() > 0)
	{
		printf("task size is %d.\n", m_vecBurnTask.size());
		task = m_vecBurnTask.at(0);
		m_vecBurnTask.erase(m_vecBurnTask.begin());
		nRet = 0;
	}
	return nRet;
}

void CBusiness::DoTask(BurnTask& task)
{
	m_burnTask = task;
	std::string strBurnCDRomID = task.m_strCDRomID;
	if (0 != ChooseCDRomToBurn(task))
	{
		printf("Choose CDRom fail.\n");
	}
	if (0 != InitCDRom(task))
	{
		printf("Init CDRom fail.\n");
	}
	if (task.m_taskState == TASK_INIT)
		task.m_taskState = TASK_BURN;
	
	BurnStreamInfoToDisk(task);

	BurnFileToDisk(task);
}

void CBusiness::SetCDRomState(std::string strCDRomID, CDROMSTATE state)
{
	for (int i = 0; i < m_vecCDRomInfo.size(); i++)
	{
		if (m_vecCDRomInfo.at(i).m_strCDRomID.compare(strCDRomID) == 0)
		{
			m_vecCDRomInfo.at(i).m_euWorkState = state;
			break;
		}
	}
}

void* CBusiness::GetIdleCDRom(BurnTask& task, std::string& strCDRomID, int& nCDRomIndex)
{
	void* pHandle = NULL;
	for (int i = 0; i < task.m_vecCDRomInfo.size(); i++)
	{
		if (task.m_vecCDRomInfo.at(i).m_euWorkState == CDROM_READY)
		{
			pHandle = task.m_vecCDRomInfo.at(i).m_pDVDHandle;
			strCDRomID = task.m_vecCDRomInfo.at(i).m_strCDRomID;
			nCDRomIndex = i;
			break;
		}
	}
	return pHandle;
}

int CBusiness::ChooseCDRomToBurn(BurnTask& task)
{
	int nRet = -1;

	int nNeedCDRomCount = 0;
	if (task.m_strBurnMode.compare("singleBurn") == 0)
		nNeedCDRomCount = 1;
	else if (task.m_strBurnMode.compare("doubleParallelBurn") == 0 || task.m_strBurnMode.compare("doubleRelayBurn") == 0)
		nNeedCDRomCount = 2;
	else
		nNeedCDRomCount = 1;
	if (m_vecCDRomInfo.size() < nNeedCDRomCount)	//光驱数量不足
	{
		printf("count of cdRom is %d, need cdRom is %d, ChooseCDRomToBurn faile.\n", m_vecCDRomInfo.size(), nNeedCDRomCount);
		return nRet;
	}
	for (int i = 0; i < nNeedCDRomCount; i++)
	{
		if (m_vecCDRomInfo.at(i).m_euWorkState == CDROM_UNINIT)
			task.m_vecCDRomInfo.push_back(m_vecCDRomInfo.at(i));
	}
	if (task.m_vecCDRomInfo.size() < nNeedCDRomCount)
	{
		printf("task can use CDRom Num is %d, need cdRom is %d, ChooseCDRomToBurn faile.\n", task.m_vecCDRomInfo.size(), nNeedCDRomCount);
		return nRet;
	}

	printf("Load CDRom. task.m_vecCDRomInfo.size() is %d\n", task.m_vecCDRomInfo.size());
	for (int i = 0; i < m_vecCDRomInfo.size(); i++)
	{
		printf("index: %d, workstate: %d, id: %s, name: %s, dvdHandle:%0x.\n",
			   i, m_vecCDRomInfo.at(i).m_euWorkState, m_vecCDRomInfo.at(i).m_strCDRomID.c_str(),
			   m_vecCDRomInfo.at(i).m_strCDRomName.c_str(), m_vecCDRomInfo.at(i).m_pDVDHandle);
	}
	int nErroNo = 0;
	printf("define a dvdsdkinterface object.\n");
	//DVDSDKInterface dvdInterface;
	for (int i = 0; i < task.m_vecCDRomInfo.size(); i++)
	{
		std::string strCDRomID = task.m_vecCDRomInfo.at(i).m_strCDRomID;
		//DVDDRV_HANDLE hDvD = dvdInterface.DVDSDK_Load(strCDRomID.c_str());
		printf("dvdInterface.DVDSDK_Load.\n");
		/*if (hDvD != NULL)
		{
			task.m_vecCDRomInfo.at(i).m_pDVDHandle = hDvD;
			if (dvdInterface.DVDSDK_GetTrayState(hDvD) == 1)
			{
				dvdInterface.DVDSDK_Tray(hDvD, 0);
				if (dvdInterface.DVDSDK_GetTrayState(hDvD) == 1)
				{
					printf("close disk faile, please close the disk manual.\n");
				}
			}
			if (dvdInterface.DVDSDK_HaveDisc(hDvD) == 0)
				printf("cdrom %s has no disk.\n", strCDRomID);
			if ((nErroNo = dvdInterface.DVDSDK_LoadDisc(hDvD)) != 0)
				printf("cdrom load disk fail, error number is %d", nErroNo);
			dvdInterface.DVDSDK_LockDoor(hDvD, 1);
		}*/
		if (task.m_strBurnMode.compare("doubleRelayBurn") == 0 || task.m_strBurnMode.compare("singleBurn") == 0)
		{
			printf("task.m_strBurnMode is %s.\n", task.m_strBurnMode.c_str());
			break;
		}
	}
	nRet = 0;
	return nRet;
}

int CBusiness::InitCDRom(BurnTask& task)
{
	printf("CBusiness::InitCDRom.\n");
	printf("define a dvdsdkinterface object.\n");
	int nRet = -1;
	DVDSDKInterface dvdInterface;
	for (int i = 0; i < task.m_vecCDRomInfo.size(); i++)
	{
	//	DVDDRV_HANDLE hDvD = task.m_vecCDRomInfo.at(i).m_pDVDHandle;
	//	if (hDvD != NULL)
		{
	//		DVD_DISC_INFO_T diskInfo;
	//		dvdInterface.DVDSDK_GetDiscInfo(hDvD, &diskInfo);
	//		task.m_diskInfo.ntype = diskInfo.ntype;
	//		task.m_diskInfo.maxpeed = diskInfo.maxpeed;
	//		task.m_diskInfo.discsize = diskInfo.discsize;
	//		task.m_diskInfo.usedsize = diskInfo.usedsize;
	//		task.m_diskInfo.freesize = diskInfo.freesize;

	//		if (dvdInterface.DVDSDK_DiscCanWrite(hDvD) != ERROR_DVD_OK)
	//		{
	//			printf("Disc Can not be Writed! \n");
	//			break;
	//		}
	//		if (task.m_nBurnSpeed != 8) //默认写入速度是8X
	//			dvdInterface.DVDSDK_SetWriteSpeed(hDvD, task.m_nBurnSpeed, diskInfo.ntype);
	//		dvdInterface.DVDSDK_FormatDisc(hDvD, (char*)task.m_strSessionID.c_str());//光盘名称 协议里是否需要涉及
			//置此光驱为Ready状态
			printf("set cdrom work state.\n");
			task.m_vecCDRomInfo.at(i).m_euWorkState = CDROM_READY;
			SetCDRomState(task.m_vecCDRomInfo.at(i).m_strCDRomID, CDROM_READY);
			if (task.m_strBurnMode.compare("doubleRelayBurn") == 0 || task.m_strBurnMode.compare("singleBurn") == 0)
				break;
		}
		nRet = 0;
	}
	return nRet;
}

void CBusiness::BurnStreamInfoToDisk(const BurnTask& task)
{
	printf("CBusiness::BurnStreamInfoToDisk.\n");
}

void CBusiness::BurnFileToDisk(BurnTask& task)
{
	printf("CBusiness::BurnFileToDisk.\n");
	m_mutexVecBurnFileInfo.lock();
	int nFileCount = task.m_vecBurnFileInfo.size();
	m_mutexVecBurnFileInfo.unlock();
	if (nFileCount <= 0)
	{
		//没文件，等待5s
		printf("No file to Burn.wait 5s to get file\n");
#if defined(_LINUX_)
		sleep(5);
#endif
		//printf("No file to Burn.Send Close DiscFeedback to get file\n");
		//封盘
		//CBusiness::CloseDiscFeedback();
		//return;
	}

	std::string strCDRomID = "";
	int nCDRomIndex = -1;
	//只是单盘刻录和双盘续刻的情况，未包含双盘同刻情况
	printf("task.m_vecCDrom size() is %d.\n", task.m_vecCDRomInfo.size());
	DVDDRV_HANDLE pHandle = GetIdleCDRom(task, strCDRomID, nCDRomIndex);
	if (pHandle == NULL || nCDRomIndex == -1)
	{
		printf("GetIdleCDRom fail, pHandle : %0x, nCDRomIndex : %d.\n", pHandle, nCDRomIndex);
		//return;	//for test 
	}
	task.m_vecCDRomInfo.at(nCDRomIndex).m_euWorkState = CDROM_BURNING;
	SetCDRomState(strCDRomID, CDROM_BURNING);
	
	DVDSDKInterface dvdInterface;
	DVD_DISC_INFO_T discInfo;
	do
	{
		m_mutexVecBurnFileInfo.lock();
		int nFileCount = task.m_vecCDRomInfo.size();
		m_mutexVecBurnFileInfo.unlock();
		if(nFileCount == 0)
		{	//没有刻录文件 发送封盘反馈协议
			dvdInterface.DVDSDK_GetDiscInfo(pHandle, &discInfo);
			CBusiness::CloseDiscFeedback(discInfo.freesize, discInfo.discsize);
			m_mutexVecBurnFileInfo.lock();
			nFileCount = task.m_vecCDRomInfo.size();
			m_mutexVecBurnFileInfo.unlock();
			if (nFileCount == 0)
			{
				CloseDisk(pHandle);
				task.m_taskState = TASK_STOP;
				break;
			}
		}
		if (task.m_taskState == TASK_PAUSE)
		{
#if defined (_LINUX_)
			sleep(1);
#endif
			continue;
		}
		if (task.m_taskState == TASK_STOP)
		{	
			//封盘
			//CBusiness::CloseDiscFeedback();
			m_mutexVecBurnFileInfo.lock();
			task.m_vecBurnFileInfo.clear();
			m_mutexVecBurnFileInfo.unlock();
			break;
		}

		m_mutexVecBurnFileInfo.lock();
		FileInfo fileInfo = task.m_vecBurnFileInfo.at(0);
		m_mutexVecBurnFileInfo.unlock();
		std::string strLocalPath = "";
		if (fileInfo.m_strFileLocation.compare("remote") == 0)
		{	//远端	--- 下载跟刻录 分离，异步执行
			std::string strLocalPath = Download(fileInfo.m_strType, fileInfo.m_strSrcUrl);
			m_mutexVecBurnFileInfo.lock();
			task.m_vecBurnFileInfo.at(0).m_strRemoteFileLocalPath = strLocalPath;
			fileInfo.m_strRemoteFileLocalPath = strLocalPath;
			m_mutexVecBurnFileInfo.unlock();
		}

		printf("GetDisc Info.\n");
		dvdInterface.DVDSDK_GetDiscInfo(pHandle, &discInfo);
		INT64 nSize = FileUtil::FileSize(strLocalPath.c_str());
		if (nSize + task.m_nAlarmSize >= discInfo.freesize)
		{	//刻录此文件将报警
			//发送封盘协议
			dvdInterface.DVDSDK_GetDiscInfo(pHandle, &discInfo);
			CBusiness::CloseDiscFeedback(discInfo.freesize, discInfo.discsize);
			CloseDisk(pHandle);
			break;
		}
		//开始刻录
		if (fileInfo.m_strType.compare("file") == 0)
		{	//文件
			if (-1 == BurnLocalFile(pHandle, fileInfo))
			{
				printf("BurnLocalFile fail.\n");
				break;
			}
		}
		else if (fileInfo.m_strType.compare("dir") == 0)
		{
			//目录 
			if (-1 == BurnLocalDir(pHandle, fileInfo))
			{
				printf("BurnLocalDir fail.\n");
				break;
			}
		}
		else
		{
			printf("error fileTyp,neither file or dir.\n");
		}
		m_mutexVecBurnFileInfo.lock();
		task.m_vecBurnFileInfo.erase(task.m_vecBurnFileInfo.begin());
		m_mutexVecBurnFileInfo.unlock();
	} while (true/*task.m_vecBurnFileInfo.size() > 0*/);

	//封盘
	if (task.m_taskState != TASK_STOP)
	{
		task.m_taskState = TASK_STOP;
		CloseDisk(pHandle);
		task.m_vecCDRomInfo.at(nCDRomIndex).m_euWorkState = CDROM_READY;
		SetCDRomState(strCDRomID, CDROM_READY);
		DoTask(task);
	}
	task.m_vecCDRomInfo.at(nCDRomIndex).m_euWorkState = CDROM_READY;
	SetCDRomState(strCDRomID, CDROM_READY);
}

std::string CBusiness::Download(std::string strType, std::string strSrcUrl, std::string strDestUrl)
{
	if (strType.compare("file") == 0)
	{	//文件
		if (strDestUrl.empty())
		{
			CBusiness::GenerateLocalPath(strSrcUrl, strDestUrl);
		}
		DownloadFile download;
		download.CurlDownloadFile(strSrcUrl, strDestUrl);
	}
	else
	{	//目录 
		
	}
	return strDestUrl;
}

void CBusiness::GenerateLocalPath(std::string strSrcUrl, std::string& localPath)
{
	std::string strDownloadDir = MainConfig::GetInstance()->GetDownloadDir();
	std::string strFileName = FileUtil::GetFileName(strSrcUrl);
	localPath = DirectoryUtil::EnsureSlashEnd(strDownloadDir) + strFileName;
	printf("[CBusiness::GenerateLocalPath]strDownloadDir is %s, strFileName is %s, localPath is %s.\n", strDownloadDir.c_str(), strFileName.c_str(), localPath.c_str());
}

int CBusiness::WriteFileToDisk(void* pHandle, void* pFileHandle, std::string strLocalPath)
{
	unsigned char buffer[DEFAULTPACKED * 2];
	int size, ret;
	FILE *fd;
	unsigned long num = 0;
	const char* pFileName = strLocalPath.c_str();
	printf("Open Local File is [%s]\n", pFileName);
	fd = fopen(pFileName, "r");
	if (fd == NULL)
	{
		printf("Open Local File Failed\n");
		perror("open local file:");
		return -1;
	}
	else
		printf("Open [%s] Success\n", pFileName);

	memset(buffer, 0, DEFAULTPACKED);
	size = fread(buffer, 1, DEFAULTPACKED, fd);
	printf("Read [%d]\n", size);
	
	DVDSDKInterface dvdInterface;
	while (size)
	{
		num += size;
		ret = dvdInterface.DVDSDK_WriteData(pHandle, pFileHandle, buffer, size);
		if (ret != 0)
		{
			printf("========= Write [%s] [%ld] Is Failed =========\n", pFileName, num);

			fclose(fd);
			printf(" WriteFileToDisk %s failed!\n", pFileName);
			return -1;
		}
#if defined (_LINUX_)
		usleep(1000);
#endif
		size = fread(buffer, 1, DEFAULTPACKED, fd);
		printf("Read [%d]\n", size);
	}
	
	fclose(fd);

	printf(" WriteFileToDisk %s [%ld] is ok!\n", pFileName, num);
	return 0;
}

int CBusiness::CloseDisk(void* pvHandle)
{
	if (pvHandle != NULL)
	{
		DVDSDKInterface dvdInterface;
		DVDDRV_HANDLE* pHandle = (DVDDRV_HANDLE*)pvHandle;
		dvdInterface.DVDSDK_CloseDisc(pHandle);
		dvdInterface.DVDSDK_LockDoor(pHandle, 0);
		dvdInterface.DVDSDK_UnLoad(pHandle);
		return 0;
	}
	else
	{
		printf("CloseDisc...param pvHandle is NULL.\n");
		return -1;
	}
}

int CBusiness::BurnLocalFile(void* pHandle, FileInfo& fileInfo/*std::string strSrcPath, std::string strDestPath*/)
{
	DVDSDKInterface dvdInterface;
	std::string strLocalPath = "";
	std::string strDir = DirectoryUtil::GetParentDir(fileInfo.m_strDestFilePath);
	//目前只是根目录
	DVDSDK_FILE dvdFile = dvdInterface.DVDSDK_CreateFile(pHandle, NULL, (char*)fileInfo.m_strDestFilePath.c_str(), 0);
	int nRet = dvdInterface.DVDSDK_SetFileLoca(pHandle, dvdFile);
	if (fileInfo.m_strFileLocation.compare("local") == 0)
		strLocalPath = fileInfo.m_strSrcUrl;
	if (-1 == WriteFileToDisk(pHandle, dvdFile, strLocalPath))
	{
		printf("WriteFIleToDisk fail.\n");
		return -1;
	}
	dvdInterface.DVDSDK_CloseFile(pHandle, dvdFile);
	return 0;
}

int	CBusiness::BurnLocalFile(void* pHandle, std::string strSrcPath, std::string strDestPath)
{
	printf("BurnLocalFile, src:%s, dst:%s.\n", strSrcPath.c_str(), strDestPath.c_str());
	if (!FileUtil::FileExist(strSrcPath.c_str()))
	{
		printf("file %s not exist.\n", strSrcPath.c_str());
		return -1;
	}
	DVDSDKInterface dvdInterface;
	std::string strDestName = FileUtil::GetFileName(strDestPath);
	//目前只是根目录
	DVDSDK_FILE dvdFile = dvdInterface.DVDSDK_CreateFile(pHandle, NULL, (char*)strDestName.c_str(), 0);
	int nRet = dvdInterface.DVDSDK_SetFileLoca(pHandle, dvdFile);
	{
		if (-1 == WriteFileToDisk(pHandle, dvdFile, strSrcPath))
		{
			printf("WriteFIleToDisk fail.\n");
			return -1;
		}
	}
	dvdInterface.DVDSDK_CloseFile(pHandle, dvdFile);
	return 0;
}

int CBusiness::BurnLocalDir(void* pHandle, FileInfo fileInfo/*std::string strSrcPath, std::string strDestPath*/)
{
	int nRet = -1;
	std::string strLocalDir = fileInfo.m_strSrcUrl;
	if (DirectoryUtil::IsDirExist(strLocalDir.c_str()))
	{
		std::vector<std::string> vecFileList;
		DirectoryUtil::GetFileList(strLocalDir, vecFileList);
		for (int i = 0; i < vecFileList.size(); i++)
		{
			BurnLocalFile(pHandle, vecFileList.at(i), vecFileList.at(i));
		}
	}
	else
		nRet = -2;	//dir not exist
	return nRet;
}