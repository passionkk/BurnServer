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
#include "NetLog.h"
#include "FileLog.h"
#include "CCBUtility.h"
#include "HttpClient.h"

#define DEFAULTPACKED 32 * 1024

CBusiness* CBusiness::m_pInstance = NULL;

CBusiness::CBusiness() :Runnable(),
	m_thread("CBusiness"),
	m_ready(),
	m_bStop(true),
	m_bThreadState(false),
	m_pCloseHandle(NULL)
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
			//g_NetLog.Debug("GetUndoTask..\n");
			if (0 == GetUndoTask(m_burnTask))
				DoTask(m_burnTask);
#if defined (_LINUX_)
			sleep(1);	//sleep 1 sec
#endif 
		}
		catch (...)
		{
			g_NetLog.Debug("[CBusiness::run] catch!\n");
		}
	}
}

void CBusiness::Init()
{
	try
	{
		InitLog();
		std::string strCurPath = CBusiness::GetCurDir();
#if defined(_LINUX_)
		cout << "chdir to " << strCurPath << endl;
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
	catch (...)
	{
		g_NetLog.Debug("[CBusiness::Init] catch!\n");
	}
}

void CBusiness::InitLog()
{
	try
	{
		g_NetLog.SetLevel(NET_LOG_LEVEL_DEBUG);
		g_FileLog.SetLevel(FILE_LOG_LEVEL_ERROR);
		std::vector<stLogRecvInfo> vecLogRecvInfo;
		cout << "GetLogRecvInfo" << endl;
		MainConfig::GetInstance()->GetLogRecvInfo(vecLogRecvInfo);
		cout << "GetLogRecvInfo size is:" << vecLogRecvInfo.size() << endl;
		if (vecLogRecvInfo.size() > 0)
		{
			g_NetLog.SetDest1(vecLogRecvInfo.at(0).strLogRecvIP, vecLogRecvInfo.at(0).nLogRecvPort);
			cout << "strLogRecvIP: " << vecLogRecvInfo.at(0).strLogRecvIP << ", port: " << vecLogRecvInfo.at(0).nLogRecvPort << endl;
		}
		if (vecLogRecvInfo.size() > 1)
		{
			g_NetLog.SetDest2(vecLogRecvInfo.at(1).strLogRecvIP, vecLogRecvInfo.at(1).nLogRecvPort);
			cout << "strLogRecvIP: " << vecLogRecvInfo.at(1).strLogRecvIP << ", port: " << vecLogRecvInfo.at(1).nLogRecvPort << endl;
		}
		if (vecLogRecvInfo.size() > 2)
		{
			g_NetLog.SetDest3(vecLogRecvInfo.at(2).strLogRecvIP, vecLogRecvInfo.at(2).nLogRecvPort);
			cout << "strLogRecvIP: " << vecLogRecvInfo.at(2).strLogRecvIP << ", port: " << vecLogRecvInfo.at(2).nLogRecvPort << endl;
		}
	}
	catch (...)
	{
		g_NetLog.Debug("[CBusiness::InitLog] catch!\n");
	}
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
int CBusiness::GetCDRomListFromFile(const char* pFilePath, bool bReCheck)
{
	if (m_vecCDRomInfo.size() > 0 && !bReCheck)
		return 0;
	char buf[2000];
	int size;
	FILE *fd;
	system("./Get_CDRom_Dev_Info.sh");
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
	m_mutexCDRomInfoVec.lock();
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
	m_mutexCDRomInfoVec.unlock();
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
	g_NetLog.Debug("GetCDRomList.\n");
	//system("./Get_CDRom_Dev_Info.sh");
	GetCDRomListFromFile("CDRomList", true);	//每次都重新获取
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
	try
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
	catch (...)
	{
		g_NetLog.Debug("CBusiness::PauseBurn error. \n");
	}
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
		printf("ResumeBurn fail, can't find seesionID : %s.\n", strSessionID.c_str());
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
		printf("StopBurn fail, can't find seesionID : %s.\n", strSessionID.c_str());
	}
	return bRet;
}

int CBusiness::GetCDRomInfo(std::string strCDRomID, CDRomInfo& cdRomInfo, DiskInfo& discInfo)
{
	int nRet = -1;
	int nIndex = -1;
	//调底层接口获取光驱信息
	for (int i = 0; i < m_vecCDRomInfo.size(); i++)
	{
		if (strCDRomID.compare(m_vecCDRomInfo.at(i).m_strCDRomID) == 0)
		{
			nIndex = i;
			nRet = 0;
			break;
		}
	}
	if (nIndex != -1)
	{
		DVDSDKInterface dvdInterface;
		DVD_DEV_INFO_T dvdDev;
		DVD_DISC_INFO_T dvdDisc;
		if (0 == dvdInterface.DVDSDK_GetDevInfo(m_vecCDRomInfo.at(nIndex).m_pDVDHandle, &dvdDev))
		{
			if (0 == dvdInterface.DVDSDK_GetDiscInfo(m_vecCDRomInfo.at(nIndex).m_pDVDHandle, &dvdDisc))
			{
				cdRomInfo.m_euWorkState = m_vecCDRomInfo.at(nIndex).m_euWorkState;
				discInfo.discsize = dvdDisc.discsize;
				discInfo.freesize = dvdDisc.freesize;
				discInfo.maxpeed = dvdDisc.maxpeed;
				discInfo.ntype = dvdDisc.ntype;
				discInfo.usedsize = dvdDisc.usedsize;
				nRet = 0;
			}
		}
	}
	return nRet;
}

void CBusiness::GetCurTask(BurnTask& task)
{
	task = m_burnTask;
}

void CBusiness::AddBurnFile(std::string strSessionID, std::vector<FileInfo>& vecFileInfo)
{
	if (m_burnTask.m_strSessionID.compare(strSessionID) == 0)
	{
		m_mutexVecBurnFileInfo.lock();
		int nSize = m_burnTask.m_vecBurnFileInfo.size();
		g_NetLog.Debug("current vector file size is %d.\n", nSize);
		//m_burnTask.m_vecBurnFileInfo.resize(m_burnTask.m_vecBurnFileInfo.size() + vecFileInfo.size());
		//m_burnTask.m_vecBurnFileInfo.insert(m_burnTask.m_vecBurnFileInfo.end(), vecFileInfo.begin(), vecFileInfo.end());
		for (int i = 0; i < vecFileInfo.size(); i++)
		{
			m_burnTask.m_vecBurnFileInfo.push_back(vecFileInfo.at(i));
		}
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

void CBusiness::AddFeedbackFile(std::string strSessionID, std::vector<FileInfo>& vecFileInfo)
{
	//if (m_burnTask.m_strSessionID.compare(strSessionID) == 0)
	{
		m_burnTask.m_vecFeedbackFileInfo.clear();
		for (int i = 0; i < vecFileInfo.size(); i++)
		{
			m_burnTask.m_vecFeedbackFileInfo.push_back(vecFileInfo.at(i));
		}
		//m_burnTask.m_vecFeedbackFileInfo.insert(m_burnTask.m_vecFeedbackFileInfo.end(), vecFileInfo.begin(), vecFileInfo.end());
		return;
	}
}

void* CBusiness::BurnStateFeedbackThread(void* pVoid)
{
	if (pVoid != NULL)
	{
		CBusiness* pThis = (CBusiness*)pVoid;
		pThis->BurnStateFeedback();
	}
	return NULL;
}

void CBusiness::BurnStateFeedback()
{
	try
	{
		int nInterval = m_burnTask.m_burnStateFeedback.m_nFeedbackInterval;
		while (true)
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

				jsonValue2["burnState"] = Json::Value(m_burnTask.m_taskRealState);
				std::string strDes = "";
				GetBurnStateString(m_burnTask.m_taskRealState, strDes);
				jsonValue2["burnStateDescription"] = Json::Value(strDes);
				jsonValue2["hasDVD"] = Json::Value(m_burnTask.m_burnStateFeedback.m_nHasDisc);
				jsonValue2["DVDLeftCapcity"] = Json::Value(m_burnTask.m_diskInfo.discsize - m_burnTask.m_nBurnedSize);
				jsonValue2["DVDTotalCapcity"] = Json::Value(m_burnTask.m_diskInfo.discsize);

				jsonValue1["method"] = Json::Value(sMethod.c_str());
				jsonValue1["params"] = jsonValue2;
				std::string strSend = jsonValue1.toStyledString();
				std::string strRecv;
				if (m_burnTask.m_burnStateFeedback.m_transType.compare("udp") == 0)
				{
					UDPClient client;
					client.Init(m_burnTask.m_burnStateFeedback.m_strFeedbackIP, m_burnTask.m_burnStateFeedback.m_nFeedbackPort);
					client.ConnectServer();
					client.SendProtocol(strSend, strRecv);
					client.Close();
				}
				else
				{
					CHttpClient client;
					client.SendHttpProtocol(m_burnTask.m_burnStateFeedback.m_strFeedbackIP, m_burnTask.m_burnStateFeedback.m_nFeedbackPort,
											strSend, strRecv);
				}
			}
			if (nInterval <= 0)
				nInterval = 2000;
			sleep(nInterval/1000);
			if (m_burnTask.m_taskRealState == TASK_STOP)
				break;
		}
	}
	catch (...)
	{
		g_NetLog.Debug("%s catched\n", __PRETTY_FUNCTION__);
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
		jsonValue2["sessionID"] = Json::Value(m_burnTask.m_strSessionID);

		jsonValue2["DVDLeftCapcity"] = Json::Value(leftCap);
		jsonValue2["DVDTotalCapcity"] = Json::Value(totalCap);

		jsonValue1["method"] = Json::Value(sMethod.c_str());
		jsonValue1["params"] = jsonValue2;
		//jsonValueRoot["result"] = jsonValue1;
		string strSend = jsonValue1.toStyledString(); //jsonValueRoot.toStyledString();
		//g_NetLog.Debug("[CBusiness::CloseDiscFeedback]send string is %s.\n", strSend.c_str());
		std::string strRecv;
		if (m_burnTask.m_burnStateFeedback.m_transType.compare("udp") == 0)
		{
			UDPClient client;
			client.Init(m_burnTask.m_burnStateFeedback.m_strFeedbackIP, m_burnTask.m_burnStateFeedback.m_nFeedbackPort);
			client.ConnectServer();
			client.SendProtocol(strSend, strRecv);
			client.Close();
			/*g_NetLog.Debug("ip : %s, port : %d, send string :  %s, recv string %s.\n",
				m_burnTask.m_burnStateFeedback.m_strFeedbackIP.c_str(), m_burnTask.m_burnStateFeedback.m_nFeedbackPort,
				strSend.c_str(), strRecv.c_str());*/
		}
		else
		{
			CHttpClient client;
			client.SendHttpProtocol(m_burnTask.m_burnStateFeedback.m_strFeedbackIP, m_burnTask.m_burnStateFeedback.m_nFeedbackPort,
				strSend, strRecv);

			g_NetLog.Debug("ip : %s, port : %d, send string : %s, recv string %s.\n",
				m_burnTask.m_burnStateFeedback.m_strFeedbackIP.c_str(), m_burnTask.m_burnStateFeedback.m_nFeedbackPort,
				strSend.c_str(), strRecv.c_str());
		}
		Json::Reader    jsonReader;
		Json::Value     jsonValueIn;

		if (jsonReader.parse(strRecv, jsonValueIn))
		{
			std::string sMethod = jsonValueIn["method"].asString();
			Json::Value   jsonValueParams = jsonValueIn["params"];
			g_NetLog.Debug("method : %s.\n", sMethod.c_str());
			//fileInfo vector
			std::vector<FileInfo> vecFileInfo;
			
			//DocFile 
			Json::Value jsonDocFile = jsonValueParams["DocFile"];
			FileInfo fileInfo;
			fileInfo.m_strType = jsonDocFile["file"].asString();
			if (fileInfo.m_strType.empty())
			{
				fileInfo.m_strType = "file";
			}
			fileInfo.m_strFileLocation = jsonDocFile["fileLocation"].asString();
			fileInfo.m_strSrcUrl = jsonDocFile["burnSrcFilePath"].asString();
			fileInfo.m_strDestFilePath = jsonDocFile["burnDstFilePath"].asString();
			fileInfo.m_strDescription = jsonDocFile["fileDescription"].asString();
			g_NetLog.Debug("Doc File SrcPath : %s .\n", fileInfo.m_strSrcUrl.c_str());
			vecFileInfo.push_back(fileInfo);

			Json::Value jsonValueFileList = jsonValueParams["burnFileList"];
			for (int i = 0; i < jsonValueFileList.size(); i++)
			{
				FileInfo fileInfo;
				fileInfo.m_strFileLocation = jsonValueFileList[i]["fileLocation"].asString();
				fileInfo.m_strType = jsonValueFileList[i]["fileType"].asString();
				if (fileInfo.m_strType.empty())
				{
					fileInfo.m_strType = "file";
				}
				fileInfo.m_strSrcUrl = jsonValueFileList[i]["burnSrcFilePath"].asString();
				fileInfo.m_strDestFilePath = jsonValueFileList[i]["burnDstFilePath"].asString();
				fileInfo.m_strDescription = jsonValueFileList[i]["fileDescription"].asString();
				g_NetLog.Debug("filetype is %s, burnSrcFilePath is %s, burnDstFilePath is %s.\n", 
					fileInfo.m_strType.c_str(), fileInfo.m_strSrcUrl.c_str(), fileInfo.m_strDestFilePath.c_str());
				vecFileInfo.push_back(fileInfo);
			}
			g_NetLog.Debug("call add feedbackfile. feedbackFile size is %d\n", vecFileInfo.size());
			CBusiness::GetInstance()->AddFeedbackFile(m_burnTask.m_strSessionID, vecFileInfo);
		}
	}
	catch (...)
	{
		g_NetLog.Debug("%s catched\n", __PRETTY_FUNCTION__);
	}
}

void* CBusiness::CloseDiscThread(void* pVoid)
{
	if (pVoid != NULL)
	{
		CBusiness* pThis = (CBusiness*)pVoid;
		pThis->CloseDisc();
	}
	return NULL;
}

void CBusiness::CloseDisc()
{
	if (m_pCloseHandle != NULL)
	{
		CloseDisk(m_pCloseHandle);
		m_pCloseHandle = NULL;
	}
}


//业务处理
int CBusiness::GetUndoTask(BurnTask& task)
{
	int nRet = -1;
	Mutex::ScopedLock   lock(m_mutexBurnTaskVec);
	g_NetLog.Debug("task size is %d.\n", m_vecBurnTask.size());
	if (m_vecBurnTask.size() > 0)
	{
		task = m_vecBurnTask.at(0);
		nRet = 0;
	}
	return nRet;
}

void CBusiness::DoTask(BurnTask& task)
{
	/*std::string str = "1.mp4";
	g_NetLog.Debug("%s", str.c_str());
	if (str.length() >= 1 && str.at(0) != '/')
	{
	std::string str1 = "/";
	g_NetLog.Debug("%s", str.c_str());
	str = str1 + str;
	}
	g_NetLog.Debug("fileInfo.m_strDestFilePath: %s.\n", str.c_str());*/
	//CBusiness::Download("file", "http://192.168.1.149:1001/download?filePath=D:\\download\\小鸡快跑.mkv", "/mnt/HD0/burnTestFile/Download/小鸡快跑.mkv");

	g_NetLog.Debug("Get One Task, DoTask.\n");
	m_burnTask = task;
	std::string strBurnCDRomID = task.m_strCDRomID;

	int nRet = ChooseCDRomToBurn(task);
	if (0 != nRet)
	{
		g_NetLog.Debug("Choose CDRom fail.\n");
		if (nRet == -1)
		{
			g_NetLog.Debug("Load CDRom fail. may change device, do check CDRom.\n");
			GetCDRomListFromFile("./CDRom_List", true);
			g_NetLog.Debug("Load CDRom fail. may change device, check CDRom finish.\n");
		}
		return;
	}
	if (0 != InitCDRom(task))
	{
		g_NetLog.Debug("Init CDRom fail.\n");
		return;
	}

	if (task.m_taskState == TASK_INIT)
		task.m_taskState = TASK_BURN;
	
	task.m_taskRealState = TASK_INIT;
	//开线程进行刻录状态的反馈
	if (task.m_burnStateFeedback.m_strNeedFeedback.compare("yes") == 0)
	{
		pthread_create(&task.m_burnStateFeedback.m_nThreadID, NULL, &(CBusiness::BurnStateFeedbackThread), (void*)this);
		m_bThreadState = true;
	}
	m_mapDirToHandle.clear();

	do
	{
		BurnStreamInfoToDisk(task);
	} while (task.m_taskState == TASK_PAUSE);

	do
	{
		BurnFileToDisk(task);
	} while (task.m_taskState == TASK_PAUSE);
	
	DeleteTask();
	m_bThreadState = false;
}

void CBusiness::DeleteTask()
{
	Mutex::ScopedLock   lock(m_mutexBurnTaskVec);
	if (m_vecBurnTask.size() > 0)
		m_vecBurnTask.erase(m_vecBurnTask.begin());
}

void CBusiness::SetCDRomState(std::string strCDRomID, CDROMSTATE state)
{
	m_mutexCDRomInfoVec.lock();
	for (int i = 0; i < m_vecCDRomInfo.size(); i++)
	{
		if (m_vecCDRomInfo.at(i).m_strCDRomID.compare(strCDRomID) == 0)
		{
			m_vecCDRomInfo.at(i).m_euWorkState = state;
			break;
		}
	}
	m_mutexCDRomInfoVec.unlock();
}

void* CBusiness::GetIdleCDRom(BurnTask& task, std::string& strCDRomID, int& nCDRomIndex)
{
	void* pHandle = NULL;
	for (int i = 0; i < task.m_vecCDRomInfo.size(); i++)
	{
		if (task.m_vecCDRomInfo.at(i).m_euWorkState == CDROM_READY || task.m_vecCDRomInfo.at(i).m_euWorkState == CDROM_UNINIT)
		{
			pHandle = task.m_vecCDRomInfo.at(i).m_pDVDHandle;
			strCDRomID = task.m_vecCDRomInfo.at(i).m_strCDRomID;
			nCDRomIndex = i;
			break;
		}
	}
	return pHandle;
}

void*	CBusiness::GetWorkingCDRomHandle(BurnTask& task, std::string& strCDRomID)
{
	void* pHandle = NULL;
	for (int i = 0; i < task.m_vecCDRomInfo.size(); i++)
	{
		if (task.m_vecCDRomInfo.at(i).m_euWorkState == CDROM_BURNING)
		{
			pHandle = task.m_vecCDRomInfo.at(i).m_pDVDHandle;
			strCDRomID = task.m_vecCDRomInfo.at(i).m_strCDRomID;
			break;
		}
	}
	return pHandle;
}

int CBusiness::ChooseCDRomToBurn(BurnTask& task)
{
	try
	{
		int nRet = -1;

		if (task.m_taskRealState == TASK_BURN)// 续刻的任务
		{
			if (task.m_strBurnMode.compare("doubleRelayBurn") == 0)//双盘续刻
			{
				task.m_nUseCDRomIndex = task.m_vecCDRomInfo.size() - 1 - task.m_nUseCDRomIndex;
			}
			return 0;
		}

		int nNeedCDRomCount = 0;
		if (task.m_strBurnMode.compare("singleBurn") == 0)
			nNeedCDRomCount = 1;
		else if (task.m_strBurnMode.compare("doubleParallelBurn") == 0 || task.m_strBurnMode.compare("doubleRelayBurn") == 0)
			nNeedCDRomCount = 2;
		else
			nNeedCDRomCount = 1;
		if (m_vecCDRomInfo.size() < nNeedCDRomCount)	//光驱数量不足
		{
			g_NetLog.Debug("count of cdRom is %d, need cdRom is %d, ChooseCDRomToBurn faile.\n", m_vecCDRomInfo.size(), nNeedCDRomCount);
			return nRet;
		}
		task.m_vecCDRomInfo.clear();
		m_mutexCDRomInfoVec.lock();
		for (int i = 0; i < nNeedCDRomCount; i++)
		{
			if (m_vecCDRomInfo.at(i).m_euWorkState == CDROM_UNINIT)
				task.m_vecCDRomInfo.push_back(m_vecCDRomInfo.at(i));
		}
		m_mutexCDRomInfoVec.unlock();
		if (task.m_vecCDRomInfo.size() < nNeedCDRomCount)
		{
			g_NetLog.Debug("task can use CDRom Num is %d, need cdRom is %d, ChooseCDRomToBurn faile.\n", task.m_vecCDRomInfo.size(), nNeedCDRomCount);
			return nRet;
		}

		g_NetLog.Debug("Load CDRom. task.m_vecCDRomInfo.size() is %d\n", task.m_vecCDRomInfo.size());
		for (int i = 0; i < m_vecCDRomInfo.size(); i++)
		{
			g_NetLog.Debug("index: %d, workstate: %d, id: %s, name: %s, dvdHandle:%0x.\n",
						   i, m_vecCDRomInfo.at(i).m_euWorkState, m_vecCDRomInfo.at(i).m_strCDRomID.c_str(),
						   m_vecCDRomInfo.at(i).m_strCDRomName.c_str(), m_vecCDRomInfo.at(i).m_pDVDHandle);
		}
		int nErroNo = 0;
		g_NetLog.Debug("define a dvdsdkinterface object.\n");
		bool bInitState = true;
		DVDSDKInterface dvdInterface;
		DVDDRV_HANDLE hDvD = NULL;
		for (int i = 0; i < task.m_vecCDRomInfo.size(); i++)
		{
			std::string strCDRomID = task.m_vecCDRomInfo.at(i).m_strCDRomID;
			hDvD = dvdInterface.DVDSDK_Load(strCDRomID.c_str());
			g_NetLog.Debug("dvdInterface.DVDSDK_Load.\n");
			if (hDvD != NULL)
			{
				task.m_vecCDRomInfo.at(i).m_pDVDHandle = hDvD;
				if (dvdInterface.DVDSDK_GetTrayState(hDvD) == 1)
				{
					dvdInterface.DVDSDK_Tray(hDvD, 0);
					if (dvdInterface.DVDSDK_GetTrayState(hDvD) == 1)
					{
						g_NetLog.Debug("close disk fail, please close the disk manual.\n");
					}
				}
				if (dvdInterface.DVDSDK_HaveDisc(hDvD) == 0)
				{
					g_NetLog.Debug("cdrom %s has no disk.\n", strCDRomID.c_str());
					dvdInterface.DVDSDK_Tray(hDvD, 1);
					g_NetLog.Debug("Open CDRom Tray, please insert Disc in 150 seconds.\n");
					task.m_burnStateFeedback.m_nHasDisc = 0;
					sleep(3);
				}
				//这里不会阻塞
				int nCheckTime = 0;
				while (nErroNo = dvdInterface.DVDSDK_LoadDisc(hDvD) != 0)
				{
					g_NetLog.Debug("CDRom load disk fail, error number is %d, check time : %d.\n", nErroNo, nCheckTime);
					bInitState = false;
					if (nCheckTime++ == 30)
					{
						dvdInterface.DVDSDK_UnLoad(hDvD);
						break;
					}
					sleep(5);//5秒检测一次，150s为一个任务周期
				}
				if (nCheckTime <= 30)
				{
					bInitState = true;
					task.m_burnStateFeedback.m_nHasDisc = 1;
					dvdInterface.DVDSDK_LockDoor(hDvD, 1);
				}
			}
			else
				nRet = -2;
			if (hDvD != NULL && task.m_strBurnMode.compare("doubleRelayBurn") == 0 || task.m_strBurnMode.compare("singleBurn") == 0)
			{
				g_NetLog.Debug("task.m_strBurnMode is %s.\n", task.m_strBurnMode.c_str());
				break;
			}
		}
		if (bInitState && hDvD != NULL)
			nRet = 0;
		return nRet;
	}
	catch (...)
	{
		g_NetLog.Debug("[CBusiness::ChooseCDRomToBurn] catch exception.\n");
	}
}

int CBusiness::InitCDRom(BurnTask& task)
{
	g_NetLog.Debug("CBusiness::InitCDRom.\n");
	int nRet = -1;
	DVDSDKInterface dvdInterface;
	if (task.m_taskRealState == TASK_BURN)
	{
		if (task.m_strBurnMode.compare("doubleRelayBurn") == 0)
		{	//双盘续刻，需要挑选另外一个未使用光驱
			int nCDRomIndex = task.m_nUseCDRomIndex;
			DVDDRV_HANDLE hDvD = task.m_vecCDRomInfo.at(nCDRomIndex).m_pDVDHandle;
			if (hDvD != NULL)
			{
				DVD_DISC_INFO_T diskInfo;
				dvdInterface.DVDSDK_GetDiscInfo(hDvD, &diskInfo);
				task.m_diskInfo.ntype = diskInfo.ntype;
				task.m_diskInfo.maxpeed = diskInfo.maxpeed;
				task.m_diskInfo.discsize = diskInfo.discsize;
				task.m_diskInfo.usedsize = diskInfo.usedsize;
				task.m_diskInfo.freesize = diskInfo.freesize;

				if (dvdInterface.DVDSDK_DiscCanWrite(hDvD) != ERROR_DVD_OK)
				{
					g_NetLog.Debug("Disc Can not be Writed! \n");
					dvdInterface.DVDSDK_UnLoad(hDvD);
					return -1;
				}
				//if (task.m_nBurnSpeed != 8) //默认写入速度是8X
				//	dvdInterface.DVDSDK_SetWriteSpeed(hDvD, task.m_nBurnSpeed, diskInfo.ntype);
				dvdInterface.DVDSDK_FormatDisc(hDvD, (char*)task.m_strSessionID.c_str());//光盘名称 协议里是否需要涉及
				//置此光驱为Ready状态
				g_NetLog.Debug("set cdrom work state.\n");
				task.m_vecCDRomInfo.at(nCDRomIndex).m_euWorkState = CDROM_READY;
				SetCDRomState(task.m_vecCDRomInfo.at(nCDRomIndex).m_strCDRomID, CDROM_READY);
			}
			return 0;
		}
		//单盘续刻继续走下面逻辑，只有一个盘，无所谓挑选
	}
	for (int i = 0; i < task.m_vecCDRomInfo.size(); i++)
	{
		DVDDRV_HANDLE hDvD = task.m_vecCDRomInfo.at(i).m_pDVDHandle;
		if (hDvD != NULL)
		{
			DVD_DISC_INFO_T diskInfo;
			dvdInterface.DVDSDK_GetDiscInfo(hDvD, &diskInfo);
			task.m_diskInfo.ntype = diskInfo.ntype;
			task.m_diskInfo.maxpeed = diskInfo.maxpeed;
			task.m_diskInfo.discsize = diskInfo.discsize;
			task.m_diskInfo.usedsize = diskInfo.usedsize;
			task.m_diskInfo.freesize = diskInfo.freesize;

			if (dvdInterface.DVDSDK_DiscCanWrite(hDvD) != ERROR_DVD_OK)
			{
				g_NetLog.Debug("Disc Can not be Writed! \n");
				dvdInterface.DVDSDK_UnLoad(hDvD);
				break;
			}
			//if (task.m_nBurnSpeed != 8) //默认写入速度是8X
			//	dvdInterface.DVDSDK_SetWriteSpeed(hDvD, task.m_nBurnSpeed, diskInfo.ntype);
			dvdInterface.DVDSDK_FormatDisc(hDvD, (char*)task.m_strSessionID.c_str());//光盘名称 协议里是否需要涉及
			//置此光驱为Ready状态
			g_NetLog.Debug("set cdrom work state.\n");
			task.m_vecCDRomInfo.at(i).m_euWorkState = CDROM_READY;
			SetCDRomState(task.m_vecCDRomInfo.at(i).m_strCDRomID, CDROM_READY);
			if (task.m_strBurnMode.compare("doubleRelayBurn") == 0 || task.m_strBurnMode.compare("singleBurn") == 0)
			{
				task.m_nUseCDRomIndex = i;//赋值使用的光驱索引
				nRet = 0;
				break;	//只格式化一个光驱的光盘
			}
		}
		nRet = 0;
	}
	return nRet;
}

void CBusiness::BurnStreamInfoToDisk(const BurnTask& task)
{
	g_NetLog.Debug("CBusiness::BurnStreamInfoToDisk.\n");
}

void CBusiness::BurnFileToDisk(BurnTask& task)
{
	g_NetLog.Debug("CBusiness::BurnFileToDisk.\n");
	int nFileCount = 0;
	while (nFileCount <= 0)
	{
		m_mutexVecBurnFileInfo.lock();
		nFileCount = task.m_vecBurnFileInfo.size();
		m_mutexVecBurnFileInfo.unlock();
		if (nFileCount <= 0)
		{
			//没文件，等待500ms
			g_NetLog.Debug("No file to Burn.wait 5s to get file\n");
#if defined(_LINUX_)
			sleep(5);
#endif
		}
		if (task.m_taskState == TASK_STOP)
			break;
	}

	std::string strCDRomID = "";
	int nCDRomIndex = -1;
	//只是单盘刻录和双盘续刻的情况，未包含双盘同刻情况
	g_NetLog.Debug("task.m_vecCDrom size() is %d.\n", task.m_vecCDRomInfo.size());
	DVDDRV_HANDLE pHandle = GetIdleCDRom(task, strCDRomID, nCDRomIndex);
	if (pHandle == NULL || nCDRomIndex == -1)
	{
		g_NetLog.Debug("GetIdleCDRom fail, pHandle : %0x, nCDRomIndex : %d.\n", pHandle, nCDRomIndex);
		return;
	}
	task.m_vecCDRomInfo.at(nCDRomIndex).m_euWorkState = CDROM_BURNING;
	SetCDRomState(strCDRomID, CDROM_BURNING);
	task.m_strCDRomID = strCDRomID;

	DVDSDKInterface dvdInterface;
	DVD_DISC_INFO_T discInfo;
	task.m_nBurnedSize = 0;//清空刻录总量 不采用光驱获取的值，测试不准确
	do
	{
		m_mutexVecBurnFileInfo.lock();
		int nFileCount = task.m_vecBurnFileInfo.size();
		m_mutexVecBurnFileInfo.unlock();
		if(nFileCount == 0 && task.m_taskState == TASK_STOP)
		{	//发送封盘反馈协议
			dvdInterface.DVDSDK_GetDiscInfo(pHandle, &discInfo);
			g_NetLog.Debug("no file to burn and task state is stop, send close disc feedback .\n" );
			CBusiness::CloseDiscFeedback(discInfo.discsize - m_burnTask.m_nBurnedSize, discInfo.discsize);
			BurnFeedbackFileToDisc(task, pHandle);
			task.m_taskRealState = TASK_STOP;
			break;
		}
		if (nFileCount == 0 || task.m_taskState == TASK_PAUSE)
		{
#if defined (_LINUX_)
			sleep(2);
#endif
			continue;
		}

		m_mutexVecBurnFileInfo.lock();
		FileInfo fileInfo = task.m_vecBurnFileInfo.at(0);
		m_mutexVecBurnFileInfo.unlock();
		std::string strLocalPath = "";
		if (fileInfo.m_strFileLocation.compare("remote") == 0)
		{	//远端	--- 下载跟刻录 分离，异步执行
			//下载前应判断本地是否存在相同文件，这里可以作续传的逻辑
			std::string strLocalPath;
			std::string strDestFilePathTmp; // only for add / before file name eg : a.mp4 --> /a.mp4
			g_NetLog.Debug("fileInfo.m_strDestFilePath: %s.\n", fileInfo.m_strDestFilePath.c_str());
			if(fileInfo.m_strDestFilePath.length() >= 1 && fileInfo.m_strDestFilePath.at(0) != '/')
			{
				std::string str = "/";
				g_NetLog.Debug("%s", str.c_str());
				strDestFilePathTmp = str + fileInfo.m_strDestFilePath;
			}
			else if (fileInfo.m_strDestFilePath.length() >= 1)
			{
				std::string str = "/";
				g_NetLog.Debug("%s", str.c_str());
				strDestFilePathTmp = str + FileUtil::GetFileName(fileInfo.m_strDestFilePath);
			}
			else
			{
				g_NetLog.Debug("fileInfo.m_strDestFilePath is %s.\n", fileInfo.m_strDestFilePath.c_str());
			}
			g_NetLog.Debug("fileInfo.m_strDestFilePath: %s.\n", strDestFilePathTmp.c_str());
			GenerateLocalPath(strDestFilePathTmp, strLocalPath);
			Download(fileInfo.m_strType, fileInfo.m_strSrcUrl, strLocalPath);
			m_mutexVecBurnFileInfo.lock();
			task.m_vecBurnFileInfo.at(0).m_strRemoteFileLocalPath = strLocalPath;
			fileInfo.m_strRemoteFileLocalPath = strLocalPath;
			m_mutexVecBurnFileInfo.unlock();
		}
		else
		{
			strLocalPath = fileInfo.m_strSrcUrl;
		}
		g_NetLog.Debug("GetDisc Info.\n");
		if (0 != dvdInterface.DVDSDK_GetDiscInfo(pHandle, &discInfo))
		{
			g_NetLog.Debug("call dvd sdk get disk info fail.\n");
		}
		//task.m_nBurnedSize = discInfo.discsize - discInfo.freesize;
		double dSizeMB = 0;
		//开始刻录
		if (fileInfo.m_strType.compare("file") == 0)
		{
			//文件
			INT64 nSize = FileUtil::FileSize(strLocalPath.c_str());	//字节数
			dSizeMB = nSize * 1.0 / 1024 / 1024;	//MB
			g_NetLog.Debug("task.m_BrunSize is %d, disc freesize is %d, disc usedsize is %d, disc discsize is %d, disc task alarm size is %d, file size is %g.\n", 
							task.m_nBurnedSize, discInfo.freesize, discInfo.usedsize, discInfo.discsize, task.m_nAlarmSize, dSizeMB);
			
			//if (discInfo.freesize * 1.0 - dSizeMB <= task.m_nAlarmSize*1.0)
			if (discInfo.discsize - dSizeMB - task.m_nBurnedSize <= task.m_nAlarmSize)
			{
				//刻录此文件将报警 发送封盘协议
				dvdInterface.DVDSDK_GetDiscInfo(pHandle, &discInfo);
				CBusiness::CloseDiscFeedback(discInfo.discsize - m_burnTask.m_nBurnedSize, discInfo.discsize);
				g_NetLog.Debug("disc capcity is not enough, send closeDiscFeedback. will close disc.\n");
				BurnFeedbackFileToDisc(task, pHandle);
				dSizeMB = 0;
				break;
			}
			if (-1 == BurnLocalFile(pHandle, fileInfo))
			{
				g_NetLog.Debug("BurnLocalFile %s fail.\n", fileInfo.m_strSrcUrl.c_str());
				//break;   ---->不应该break
			}
		}
		else if (fileInfo.m_strType.compare("dir") == 0)
		{
			//目录 
			dSizeMB = DirectoryUtil::GetDirSize(fileInfo.m_strSrcUrl.c_str()) / 1024 / 1024;
			if (discInfo.discsize - dSizeMB - task.m_nBurnedSize <= task.m_nAlarmSize)//if (dSizeMB + task.m_nAlarmSize * 1.0 >= discInfo.freesize * 1.0)
			{	//目录大小太大，拆分目录为多个文件
				ConvertDirToFileInfo(task, fileInfo.m_strSrcUrl, fileInfo.m_strDestFilePath);
				dSizeMB = 0;
			}
			else
			{
				g_NetLog.Debug("[CBusiness::BurnFileToDisk] fileInfo.destPath is : %s.\n", fileInfo.m_strDestFilePath.c_str());
				if (-1 == BurnLocalDir(pHandle, fileInfo))
				{
					g_NetLog.Debug("BurnLocalDir fail.\n");
					break;
				}
			}
		}
		else
		{
			g_NetLog.Debug("error fileType, neither file or dir. fileType is %s\n", fileInfo.m_strType.c_str());
		}

		//刻录结束，更新刻录大小信息
		task.m_nBurnedSize += dSizeMB;
		g_NetLog.Debug("has burned size is %d.\n", task.m_nBurnedSize);
		m_mutexVecBurnFileInfo.lock();
		if (task.m_vecBurnFileInfo.size() > 0)
			task.m_vecBurnFileInfo.erase(task.m_vecBurnFileInfo.begin());
		g_NetLog.Debug("left burn file count is : %d.\n", task.m_vecBurnFileInfo.size());
		m_mutexVecBurnFileInfo.unlock();
	} while (true);

	//封盘
	if (task.m_taskRealState != TASK_STOP && task.m_strBurnMode.compare("doubleRelayBurn") == 0)
	{
		m_pCloseHandle = task.m_vecCDRomInfo.at(nCDRomIndex).m_pDVDHandle;
		g_NetLog.Debug("create thread to close disc, disc handle : %0x.\n", m_pCloseHandle);
		pthread_create(NULL, NULL, &(CBusiness::CloseDiscThread), (void*)this);
	}
	else
	{
		CloseDisk(pHandle);
	}
	//反馈线程停止
	m_bThreadState = false;
	if (task.m_taskRealState != TASK_STOP)
	{
		task.m_vecCDRomInfo.at(nCDRomIndex).m_euWorkState = CDROM_UNINIT;
		SetCDRomState(strCDRomID, CDROM_UNINIT);
		return DoTask(task);//双盘续刻 还要进行逻辑处理
	}
	task.m_vecCDRomInfo.at(nCDRomIndex).m_euWorkState = CDROM_UNINIT;
	SetCDRomState(strCDRomID, CDROM_UNINIT);
}

void CBusiness::BurnFeedbackFileToDisc(BurnTask& task, const DVDDRV_HANDLE pHandle)
{
	DVDSDKInterface dvdInterface;
	DVD_DISC_INFO_T discInfo;
	g_NetLog.Debug("[CBusiness::BurnFeedbackFileToDisc] feedback fileInfo size is %d.\n", task.m_vecFeedbackFileInfo.size());
	while (task.m_vecFeedbackFileInfo.size() > 0)
	{
		FileInfo fileInfo = task.m_vecFeedbackFileInfo.at(0);
		
		std::string strLocalPath = "";
		if (fileInfo.m_strFileLocation.compare("remote") == 0)
		{	//远端	--- 下载跟刻录 分离，异步执行
			//下载前应判断本地是否存在相同文件
			g_NetLog.Debug("fileInfo.m_strDestFilePath: %s.\n", fileInfo.m_strDestFilePath.c_str());
			if (fileInfo.m_strDestFilePath.length() >= 1 && fileInfo.m_strDestFilePath.at(0) != '/')
			{
				std::string str = "/";
				g_NetLog.Debug("%s", str.c_str());
				fileInfo.m_strDestFilePath = str + fileInfo.m_strDestFilePath;
			}
			g_NetLog.Debug("fileInfo.m_strDestFilePath: %s.\n", fileInfo.m_strDestFilePath.c_str());
			GenerateLocalPath(fileInfo.m_strDestFilePath, strLocalPath);
			Download(fileInfo.m_strType, fileInfo.m_strSrcUrl, strLocalPath);
			m_mutexVecBurnFileInfo.lock();
			task.m_vecBurnFileInfo.at(0).m_strRemoteFileLocalPath = strLocalPath;
			fileInfo.m_strRemoteFileLocalPath = strLocalPath;
			m_mutexVecBurnFileInfo.unlock();
		}
		else
		{
			strLocalPath = fileInfo.m_strSrcUrl;
		}
		g_NetLog.Debug("[CBusiness::BurnFeedbackFileToDisc]GetDisc Info.\n");
		dvdInterface.DVDSDK_GetDiscInfo(pHandle, &discInfo);
		task.m_nBurnedSize = discInfo.discsize - discInfo.freesize;
		double dSizeMB = 0;
		//开始刻录
		if (fileInfo.m_strType.compare("file") == 0)
		{
			//文件
			INT64 nSize = FileUtil::FileSize(strLocalPath.c_str());	//字节数
			dSizeMB = nSize * 1.0 / 1024 / 1024;	//MB
			g_NetLog.Debug("[CBusiness::BurnFeedbackFileToDisc]disc freesize is %d, task alarm size is %d, file size is %g.\n", discInfo.freesize, task.m_nAlarmSize, dSizeMB);
			if (discInfo.freesize * 1.0 - dSizeMB <= task.m_nAlarmSize*1.0)
			{
				g_NetLog.Debug("[CBusiness::BurnFeedbackFileToDisc]disc capcity is not enough, will close disc.\n");
				dSizeMB = 0;
				break;
			}
			if (-1 == BurnLocalFile(pHandle, fileInfo))
			{
				g_NetLog.Debug("[CBusiness::BurnFeedbackFileToDisc]BurnLocalFile %s fail.\n", fileInfo.m_strSrcUrl.c_str());
			}
		}
		else if (fileInfo.m_strType.compare("dir") == 0)
		{
			//目录 
			dSizeMB = DirectoryUtil::GetDirSize(fileInfo.m_strSrcUrl.c_str()) / 1024 / 1024;
			if (dSizeMB + task.m_nAlarmSize * 1.0 >= discInfo.freesize * 1.0)
			{	//目录大小太大，拆分目录为多个文件
				ConvertDirToFileInfo(task, fileInfo.m_strSrcUrl, fileInfo.m_strDestFilePath, true);
				dSizeMB = 0;
			}
			else
			{
				if (-1 == BurnLocalDir(pHandle, fileInfo))
				{
					g_NetLog.Debug("BurnLocalDir fail.\n");
					break;
				}
			}
		}
		else
		{
			g_NetLog.Debug("error fileType, neither file or dir. fileType is %s\n", fileInfo.m_strType.c_str());
		}

		//刻录结束，更新刻录大小信息
		task.m_nBurnedSize += dSizeMB;
		if (task.m_vecFeedbackFileInfo.size() > 0)
			task.m_vecFeedbackFileInfo.erase(task.m_vecFeedbackFileInfo.begin());
		g_NetLog.Debug("left burn file count is : %d.\n", task.m_vecFeedbackFileInfo.size());
	}
}

std::string CBusiness::Download(std::string strType, std::string strSrcUrl, std::string strDestUrl)
{
	if (strType.compare("file") == 0)
	{	//文件
// 		if (strDestUrl.empty())
// 		{
// 			CBusiness::GenerateLocalPath(strSrcUrl, strDestUrl);
// 		}
		DownloadFile download;
		g_NetLog.Debug("download file, srcUrl : %s, destUrl:%s.\n", strSrcUrl.c_str(), strDestUrl.c_str());
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
	if (!DirectoryUtil::IsDirExist(strDownloadDir.c_str()))
		DirectoryUtil::CreateDir(strDownloadDir.c_str());

	std::string strFileName = FileUtil::GetFileName(strSrcUrl);
	localPath = DirectoryUtil::EnsureSlashEnd(strDownloadDir) + strFileName;
	g_NetLog.Debug("[CBusiness::GenerateLocalPath]strDownloadDir is %s, strFileName is %s, localPath is %s.\n", strDownloadDir.c_str(), strFileName.c_str(), localPath.c_str());
}

int CBusiness::WriteFileToDisk(void* pHandle, void* pFileHandle, std::string strLocalPath)
{
	g_NetLog.Debug("[CBusiness::WriteFileToDisk] write file %s to disc.\n", strLocalPath.c_str());
	unsigned char buffer[DEFAULTPACKED * 2];
	int size, ret;
	FILE *fd;
	unsigned long num = 0;
	const char* pFileName = strLocalPath.c_str();
	g_NetLog.Debug("Open Local File is [%s]\n", pFileName);
	fd = fopen(pFileName, "r");
	if (fd == NULL)
	{
		g_NetLog.Debug("Open Local File Failed\n");
		return -1;
	}
	else
		g_NetLog.Debug("Open [%s] Success\n", pFileName);

	memset(buffer, 0, DEFAULTPACKED);
	size = fread(buffer, 1, DEFAULTPACKED, fd);
	//g_NetLog.Debug("Read [%d]\n", size);
	
	DVDSDKInterface dvdInterface;
	while (size)
	{
		num += size;
		ret = dvdInterface.DVDSDK_WriteData(pHandle, pFileHandle, buffer, size);
		if (ret != 0)
		{
			g_NetLog.Debug("========= Write [%s] [%ld] Is Failed =========\n", pFileName, num);

			fclose(fd);
			g_NetLog.Debug(" WriteFileToDisk %s failed!\n", pFileName);
			g_FileLog.Debug(" WriteFileToDisk %s failed!\n", pFileName);
			return -1;
		}
#if defined (_LINUX_)
		//usleep(1000);
#endif
		size = fread(buffer, 1, DEFAULTPACKED, fd);
		//printf("Read [%d]\n", size);
	}
	
	fclose(fd);

	g_NetLog.Debug(" WriteFileToDisk %s [%ld] is ok!\n", pFileName, num);
	return 0;
}

int CBusiness::CloseDisk(void* pvHandle)
{
	try
	{
		if (pvHandle != NULL)
		{
			DVDSDKInterface dvdInterface;
			DVDDRV_HANDLE* pHandle = (DVDDRV_HANDLE*)pvHandle;
			g_NetLog.Debug("call DVDSDK_CloseDisc.\n");
			dvdInterface.DVDSDK_CloseDisc(pHandle);

			g_NetLog.Debug("call DVDSDK_LockDoor.\n");
			dvdInterface.DVDSDK_LockDoor(pHandle, 0);

			g_NetLog.Debug("call DVDSDK_Tray.\n");
			dvdInterface.DVDSDK_Tray(pHandle, 1);

			g_NetLog.Debug("call DVDSDK_UnLoad.\n");
			dvdInterface.DVDSDK_UnLoad(pHandle);
			return 0;
		}
		else
		{
			g_NetLog.Debug("CloseDisc...param pvHandle is NULL.\n");
			return -1;
		}
	}
	catch (...)
	{
		g_NetLog.Debug("%s catch.\n", __PRETTY_FUNCTION__);
	}
}

int CBusiness::BurnLocalFile(void* pHandle, FileInfo& fileInfo/*std::string strSrcPath, std::string strDestPath*/)
{
	DVDSDKInterface dvdInterface;
	DVDSDK_DIR pParent = NULL;

	std::string strLocalPath = "";
	std::string strDir = DirectoryUtil::GetParentDir(fileInfo.m_strDestFilePath);
	strDir = DirectoryUtil::EnsureNoSlashEnd(strDir);
	std::string strFileName = FileUtil::GetFileName(fileInfo.m_strDestFilePath);
	g_NetLog.Debug("[CBusiness::BurnLocalFile] File Dir is %s, File Name is %s .\n", strDir.c_str(), strFileName.c_str());

	if (!strDir.empty())
	{
		g_NetLog.Debug("[CBusiness::BurnLocalFile] File Dir is %s, file Path is %s .\n", strDir.c_str(), fileInfo.m_strDestFilePath.c_str());
		if (strDir.compare("/") != 0 && m_mapDirToHandle.find(strDir) == m_mapDirToHandle.end())
		{
			g_NetLog.Debug("[CBusiness::BurnLocalFile] DVDSDK CreateDir %s begin.\n", (char*)strDir.c_str());
			DVDSDK_DIR pDir = dvdInterface.DVDSDK_CreateDir(pHandle, (char*)strDir.c_str());
			g_NetLog.Debug("[CBusiness::BurnLocalFile] DVDSDK CreateDir %s end.Dir handle : %0x\n", (char*)strDir.c_str(), pDir);
			m_mapDirToHandle[strDir] = pDir;
			pParent = pDir;
		}
		else
		{
			if (m_mapDirToHandle.find(strDir) != m_mapDirToHandle.end())
			{
				pParent = m_mapDirToHandle[strDir];
				g_NetLog.Debug("[CBusiness::BurnLocalFile]Parent Dir is %0x.\n", pParent);
			}
		}
	}
	DVDSDK_FILE dvdFile;
	if (pParent == NULL)
	{
		g_NetLog.Debug("parent dir is null, create file :%s in disc.\n", fileInfo.m_strDestFilePath.c_str());
		dvdFile = dvdInterface.DVDSDK_CreateFile(pHandle, NULL, (char*)fileInfo.m_strDestFilePath.c_str(), 0);
	}
	else
	{
		g_NetLog.Debug("parent dir is not null, create file :%s in disc.\n", strFileName.c_str());
		dvdFile = dvdInterface.DVDSDK_CreateFile(pHandle, pParent, (char*)strFileName.c_str(), 0);
	}
	int nRet = dvdInterface.DVDSDK_SetFileLoca(pHandle, dvdFile);

	if (fileInfo.m_strFileLocation.compare("local") == 0)
		strLocalPath = fileInfo.m_strSrcUrl;
	else
		strLocalPath = fileInfo.m_strRemoteFileLocalPath;

	if (-1 == WriteFileToDisk(pHandle, dvdFile, strLocalPath))
	{
		printf("WriteFIleToDisk fail.\n");
		return -1;
	}
	g_NetLog.Debug("Write file %s to Disc Success.\n", fileInfo.m_strDestFilePath.c_str());
	dvdInterface.DVDSDK_CloseFile(pHandle, dvdFile);

	//删除下载的文件
	if (fileInfo.m_strFileLocation.compare("remote") == 0)
		FileUtil::DelFile(strLocalPath.c_str());

	return 0;
}

int	CBusiness::BurnLocalFile(void* pHandle, std::string strSrcPath, std::string strDestPath)
{
	g_NetLog.Debug("BurnLocalFile, src:%s, dst:%s.\n", strSrcPath.c_str(), strDestPath.c_str());
	if (!FileUtil::FileExist(strSrcPath.c_str()))
	{
		g_NetLog.Debug("file %s not exist.\n", strSrcPath.c_str());
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
		//源目录下文件列表 全路径
		std::vector<std::string> vecFilePathList;
		DirectoryUtil::GetFileList(strLocalDir, vecFilePathList);
		
		//源目录下文件列表 只有文件名
		std::vector<std::string> vecFileNameList;
		DirectoryUtil::GetFileNameList(strLocalDir, vecFileNameList);

		//拼刻录文件目标路径
		for (int i = 0; i < vecFileNameList.size(); i++)
		{
			std::string strDestDir = DirectoryUtil::EnsureSlashEnd(fileInfo.m_strDestFilePath);
			g_NetLog.Debug("old file name is %s, destDirPath %s.\n", vecFileNameList.at(i).c_str(), strDestDir.c_str());
			vecFileNameList.at(i) = DirectoryUtil::EnsureSlashEnd(fileInfo.m_strDestFilePath) + vecFileNameList.at(i);
			g_NetLog.Debug("Generate New DestFilePath %s, destDirPath %s.\n", strDestDir.c_str(), vecFileNameList.at(i).c_str());
		}

		for (int i = 0; i < vecFilePathList.size(); i++)
		{
			FileInfo fileInfo;
			fileInfo.m_strSrcUrl = vecFilePathList.at(i);
			fileInfo.m_strDestFilePath = vecFileNameList.at(i);
			fileInfo.m_strFileLocation = "local";
			fileInfo.m_strType = "file";
			BurnLocalFile(pHandle, fileInfo);
			//BurnLocalFile(pHandle, vecFilePathList.at(i), vecFileNameList.at(i));
		}
		
		nRet = 0;
	}
	else
	{
		nRet = -2;	//dir not exist
		g_NetLog.Debug("Local dir %s not exist.\n", strLocalDir.c_str());
	}
	return nRet;
}

void CBusiness::GetBurnStateString(int nBurnState, std::string& strDes)
{
	switch (nBurnState)
	{
	case 0:
		strDes = "未刻录";
		break;
	case 1:
		strDes = "刻录中";
		break;
	case 2:
		strDes = "暂停刻录";
		break;
	case 3:
		strDes = "停止刻录";
		break;
	default:
		break;
	}
}

void CBusiness::ConvertDirToFileInfo(BurnTask& task, std::string strSrcDir, std::string strDestDir, bool bFeedback)
{
	std::vector<string> vecFilePath;
	DirectoryUtil::GetFileListNoSub(strSrcDir, vecFilePath);

	std::vector<string> vecFileName;
	DirectoryUtil::GetFileNameListNoSub(strSrcDir, vecFileName);

	//vecFilePath 中 find strSrcDir replace 为strDestDir即可。
	if (!bFeedback)
	{
		std::vector<FileInfo>::iterator it = task.m_vecBurnFileInfo.begin();
		for (int i = 0; i < vecFileName.size(); i++)
		{
			vecFileName.at(i) = DirectoryUtil::EnsureSlashEnd(strDestDir) + vecFileName.at(i);
			FileInfo fileInfo;
			fileInfo.m_nFlag = 1;
			fileInfo.m_strType = "file";
			fileInfo.m_strFileLocation = "local";
			fileInfo.m_strSrcUrl = vecFilePath.at(i);
			fileInfo.m_strDestFilePath = vecFileName.at(i);
			if (it != task.m_vecBurnFileInfo.end())
			{
				task.m_vecBurnFileInfo.insert(it + i, fileInfo);
			}
			else
			{
				task.m_vecBurnFileInfo.push_back(fileInfo);
			}
		}
	}
	else
	{
		std::vector<FileInfo>::iterator it = task.m_vecFeedbackFileInfo.begin();
		for (int i = 0; i < vecFileName.size(); i++)
		{
			vecFileName.at(i) = DirectoryUtil::EnsureSlashEnd(strDestDir) + vecFileName.at(i);
			FileInfo fileInfo;
			fileInfo.m_nFlag = 1;
			fileInfo.m_strType = "file";
			fileInfo.m_strFileLocation = "local";
			fileInfo.m_strSrcUrl = vecFilePath.at(i);
			fileInfo.m_strDestFilePath = vecFileName.at(i);
			if (it != task.m_vecFeedbackFileInfo.end())
			{
				task.m_vecFeedbackFileInfo.insert(it + i, fileInfo);
			}
			else
			{
				task.m_vecFeedbackFileInfo.push_back(fileInfo);
			}
		}
	}
}