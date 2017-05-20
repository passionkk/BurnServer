#include "Business.h"
#include "HttpServerModule.h"
#include "UDPServerModule.h"
#include "jsoncpp/json/json.h"
#include "UDPClient.h"
#include "poco/net/NetException.h"
#include "BurnCore/LibDVDSDK.h"

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
			BurnTask task;
			if (0== GetUndoTask(task))
				DoTask(task);
#if defined (_LINUX_)
			sleep(50);
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
		printf("cdrom :%d, cdromID:%s, cdromName:%s.\n", i, m_vecCDRomInfo.at(i).m_strCDRomID, m_vecCDRomInfo.at(i).m_strCDRomName);
	}
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

	m_vecCDRomInfo.clear();
	std::vector<CDRomInfo> vecCDRomInfoTmp;
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

int	CBusiness::ExtractString(const char *head, char *end,
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
	system("./Get_CDRom_Dev_Info.sh");
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
	return bRet;
}

bool CBusiness::ResumeBurn(std::string strSessionID)
{
	bool bRet = true;
	return bRet;
}

bool CBusiness::StopBurn(std::string strSessionID)
{
	bool bRet = true;
	return bRet;
}

void CBusiness::GetCDRomInfo(std::string strCDRomID)
{
	Mutex::ScopedLock lock(m_mutexCDRomInfoVec);
	//调底层接口获取光驱信息
}

void CBusiness::AddBurnFile(std::string strSessionID, std::vector<FileInfo>& vecFileInfo)
{
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
void CBusiness::CloseDiscFeedback()
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
#if 0 // 调用底层接口
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
		task = m_vecBurnTask.at(0);
		m_vecBurnTask.erase(m_vecBurnTask.begin());
		nRet = 0;
	}
	return nRet;
}

void CBusiness::DoTask(const BurnTask& task)
{
	std::string strBurnCDRomID = task.m_strCDRomID;
	if (0 == CheckCDDriveState(strBurnCDRomID.c_str()))
	{
		BurnStreamInfoToFile(task);
		BurnFileToFile(task);
	}
}

int CBusiness::CheckCDDriveState(const char* pCDRomID)
{
	int nRet = -1;
	DVDSDKInterface dvdInterface;
	DVDDRV_HANDLE hDvD = dvdInterface.DVDSDK_Load(pCDRomID);
	if (hDvD != NULL)
	{
		
		nRet = 0;
	}
	return nRet;
}

void CBusiness::BurnStreamInfoToFile(const BurnTask& task)
{

}

void CBusiness::BurnFileToFile(const BurnTask& task)
{

}