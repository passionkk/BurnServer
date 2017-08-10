#include "HttpServerModule.h"
#include "Business.h"
#include "CommonDefine.h"
#include "json/json.h"
#include "MainConfig.h"
#include "FileLog.h"
#include "NetLog.h"
#include "FileUtil.h"
#include "DirectoryUtil.h"

HttpServerModule* HttpServerModule::m_pInstance = NULL;

#define BUFFER_SIZE  (128*1024)

static int
Access_callback(void *cls,
struct MHD_Connection *connection,
    const char *url,
    const char *method,
    const char *version,
    const char *upload_data,
    size_t *upload_data_size, void **ptr)
{
    try
    {
        int ret = MHD_NO;

        if ((0 != strcmp(method, MHD_HTTP_METHOD_GET)) &&
            (0 != strcmp(method, MHD_HTTP_METHOD_POST)) )
            return MHD_NO;


        if (0 == strcmp(method, MHD_HTTP_METHOD_POST))
        {
            if(NULL != strstr(url, "activeProtocol.action"))
            {
                if (*ptr == NULL)
                {
                    char * psz = new char[BUFFER_SIZE];
                    memset(psz, 0, BUFFER_SIZE);
                    *ptr = psz;
                    return MHD_YES;
                }
                char *  pszContent = (char *)*ptr;

                if (*upload_data_size == 0)
                {
                    ret = ((HttpServerModule*)cls)->ProcessPassiveProtocol(connection,pszContent);
                    if (pszContent != NULL)
                    {
                        delete[] pszContent;
                        pszContent = NULL;
                        *ptr = NULL;
                    }
                }
                else
                {
                    int iSize = strlen(pszContent);
                    if ((iSize + *upload_data_size) > (BUFFER_SIZE - 4))
                    {
                        printf("protocol too big\n");
                    }
                    else
                    {
                        memcpy(pszContent + iSize, upload_data, *upload_data_size);
                    }
                    *upload_data_size = 0;
                    return MHD_YES;
                }
            }
            else
            {
                ret = MHD_NO;
            }
        }
        return ret;
    }
    catch (...)
    {
        printf("Access_callback  catch!\n");
        return MHD_NO;
    }
}

static void
response_completed_callback (void *cls,
			     struct MHD_Connection *connection,
			     void **con_cls,
			     enum MHD_RequestTerminationCode toe)
{

}


HttpServerModule::HttpServerModule(void)
: m_mutexHttpModule()
{

}

HttpServerModule::~HttpServerModule(void)
{

}

HttpServerModule *HttpServerModule::Initialize()
{
    return HttpServerModule::GetInstance();
}

void HttpServerModule::Uninitialize()
{
    if (m_pInstance != NULL)
    {
        m_pInstance->UnInit();
        delete m_pInstance;
        m_pInstance = NULL;
    }
}

HttpServerModule *HttpServerModule::GetInstance()
{
    if (m_pInstance == NULL)
    {
        m_pInstance = new HttpServerModule;
        m_pInstance->Init();
    }

    return m_pInstance;
}

void HttpServerModule::Init()
{
    HttpServerChannel* pChannel = new HttpServerChannel();
	RetransChannel httpInfo = MainConfig::GetInstance()->GetServerConfigInfo(0);
	int iPort = httpInfo.m_iPort;
	if (0 == pChannel->Start(iPort, &Access_callback, (void*)this, &response_completed_callback))
		printf("Create Http Server Success, Port :%d.\n", iPort);
	else
		printf("Create Http Server Fail.\n");
	m_vectChannels.push_back(pChannel);
}

void HttpServerModule::UnInit()
{
    for(int i = 0;i< (int)m_vectChannels.size();i++)
    {
        if(m_vectChannels[i] != NULL)
        {
            m_vectChannels[i]->Stop();
            delete m_vectChannels[i];
            m_vectChannels[i] = NULL;
        }
    }
}


int HttpServerModule::ProcessPassiveProtocol(struct MHD_Connection *connection,char *  pszContent)
{
    try
    {      
        int ret = MHD_NO;
        struct MHD_Response *response;
        //解析协议，并返回
        std::string jsonRecv = pszContent;
        std::string jsonSend = "";

		JSON::Parser	jsonParser;
		//g_NetLog.Debug("[HttpServerModule::ProcessPassiveProtocol] jsonRecv : %s.\n", jsonRecv.c_str());
		Dynamic::Var result = jsonParser.parse(jsonRecv);
		JSON::Object::Ptr pObj = result.extract<JSON::Object::Ptr>();
		Dynamic::Var varMethod = pObj->get("method");
		jsonSend = ProcessProtocol(varMethod.toString(), jsonRecv);

        /*Json::Reader    jsonReader;
        Json::Value     jsonValue;

        if (jsonReader.parse(jsonRecv, jsonValue))
        {
            std::string sMethod = jsonValue["method"].asString();
            jsonSend = ProcessProtocol(sMethod,jsonRecv);
        }*/

        response = MHD_create_response_from_buffer(jsonSend.length(), (void *)jsonSend.c_str(), MHD_RESPMEM_MUST_COPY);


        MHD_add_response_header(response, "Content-Type", "application/json");
        MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");

        ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        MHD_destroy_response(response);
        return ret;
    }
    catch(...)
    {
        printf("http server  process passive protocol break.\n");
        return MHD_NO;
    }
}

std::string HttpServerModule::ProcessProtocol(std::string sMethod, std::string jsonRecv)
{
    try
    {
		Poco::Mutex::ScopedLock     lock(m_mutexHttpModule);
        std::string jsonSend = "";
		if (sMethod.compare("getCDRomList") == 0)
		{
			jsonSend = GetCDRomList(jsonRecv);
		}
		else if (sMethod.compare("startBurn") == 0)
		{
			g_NetLog.Debug("%s Handle StartBrun begin.\n", __PRETTY_FUNCTION__);
			jsonSend = StartBurn(jsonRecv);
			g_NetLog.Debug("%s Handle StartBrun end.\n", __PRETTY_FUNCTION__);
		}
		else if (sMethod.compare("pauseBurn") == 0)
		{
			jsonSend = PauseBurn(jsonRecv);
		}
		else if (sMethod.compare("resumeBurn") == 0)
		{
			jsonSend = ResumBurn(jsonRecv);
		}
		else if (sMethod.compare("stopBurn") == 0)
		{
			jsonSend = StopBurn(jsonRecv);
		}
		else if (sMethod.compare("getCDRomInfo") == 0)
		{
			jsonSend = GetCDRomInfo(jsonRecv);
		}
		else if (sMethod.compare("addBurnFile") == 0)
		{
			jsonSend = AddBurnFile(jsonRecv);
		}
		else if (sMethod == "setLogServer")
		{
			jsonSend = SetLogServer(jsonRecv);
		}
        else
        {												
            printf("%s : do not support this method\n", __PRETTY_FUNCTION__);
        }
        return jsonSend;
    }
    catch(...)
    {
        printf("%s catched\n", __PRETTY_FUNCTION__);
        return "";
    }
}

std::string HttpServerModule::GetCDRomList(std::string strIn)
{
	try
	{
		Json::Reader    jsonReader;
		Json::Value     jsonValueIn;

		if (jsonReader.parse(strIn, jsonValueIn))
		{
			std::string sMethod = jsonValueIn["method"].asString();
			Json::Value   jsonValueParams = jsonValueIn["params"];

			std::vector<CDRomInfo> vecCDRomInfo;
			CBusiness::GetInstance()->GetCDRomList(vecCDRomInfo);

			Json::Value     jsonValueRoot;
			Json::Value     jsonValue1;
			Json::Value     jsonValue2;
			Json::Value		jsonCDRomList;
			Json::Value		jsonCDRomInfo;
			for (int i = 0; i < vecCDRomInfo.size(); i++)
			{
				jsonCDRomInfo["cdRomID"] = vecCDRomInfo.at(i).m_strCDRomID;
				jsonCDRomInfo["cdRomName"] = vecCDRomInfo.at(i).m_strCDRomName;
				printf("CDRomName:%s.\n", vecCDRomInfo.at(i).m_strCDRomName.c_str());
				jsonCDRomList.append(jsonCDRomInfo);
			}

			jsonValue2["listInfo"] = jsonCDRomList;
			jsonValue2["retCode"] = Json::Value(0);
			jsonValue2["retMessage"] = Json::Value("ok");
			jsonValue1["method"] = Json::Value(sMethod.c_str());
			jsonValue1["params"] = jsonValue2;
			jsonValueRoot["result"] = jsonValue1;
			string strOut = jsonValueRoot.toStyledString();
			return strOut;
		}
		else
		{
		return "";
		}
		return "";
	}
	catch (...)
	{
		printf("%s catched\n", __PRETTY_FUNCTION__);
		return "";
	}
}

std::string HttpServerModule::StartBurn(std::string strIn)
{
	try
	{
		Json::Reader    jsonReader;
		Json::Value     jsonValueIn;
		g_NetLog.Debug("[%s] Enter StartBurn.\n", __PRETTY_FUNCTION__);
		if (jsonReader.parse(strIn, jsonValueIn))
		{
			std::string sMethod = jsonValueIn["method"].asString();
			string strOut = "";
			Json::Value   jsonValueParams = jsonValueIn["params"];

			std::string strCheckCDRomID = jsonValueParams["cdRomID"].asString();
			std::string strBurnMode = jsonValueParams["burnMode"].asString();
			if (strCheckCDRomID.empty())
				strCheckCDRomID = "CDRom_1";

			int nError = 0;
			std::vector<CDRomInfo> vecCDromInfo;
			CBusiness::GetInstance()->GetCDRomList(vecCDromInfo);
			int nDriverCount = vecCDromInfo.size();
			//验证是否存在 cdRomID的光驱
			bool bRet = CBusiness::GetInstance()->CheckExistCDRom(strCheckCDRomID);
			if (strBurnMode.compare("doubleParallelBurn") == 0 || strBurnMode.compare("doubleRelayBurn") == 0)
			{
				bRet = nDriverCount < 2 ? false : true;
				nError = 1;
			}
			if (CBusiness::GetInstance()->GetBurnTaskSize() > 0 || !bRet)
			{	
				BurnTask* task = NULL;
				CBusiness::GetInstance()->GetCurTask(task);
				// 仅保留一个刻录任务
				Json::Value     jsonValueRoot;
				Json::Value     jsonValue1;
				Json::Value     jsonValue2;
				jsonValue2["retCode"] = Json::Value(-1);
				char szErrInfo[200] = { 0 };
				if (!bRet && nError == 0)
					sprintf(szErrInfo, "%s Not Exist.", strCheckCDRomID.c_str());
				else if (!bRet && nError == 1)
					sprintf(szErrInfo, "count of cdrom is %d, need 2 cdrom.", nDriverCount);
				else
					sprintf(szErrInfo, "Exist Burn Task.");
				jsonValue2["retMessage"] = Json::Value(szErrInfo);
				std::string strSessionID = "";
				if (task != NULL)
					strSessionID = task->m_strSessionID;
				jsonValue2["sessionID"] = Json::Value(strSessionID);
				jsonValue1["method"] = Json::Value(sMethod.c_str());
				jsonValue1["params"] = jsonValue2;
				jsonValueRoot["result"] = jsonValue1;
				strOut = jsonValueRoot.toStyledString();
			}
			else
			{
				BurnTask* task = new BurnTask;
				//discName
				task->m_strDiscName = jsonValueParams["discName"].asString();
				//strCDRomID
				task->m_strCDRomID = strCheckCDRomID;//jsonValueParams["cdRomID"].asString();
				//burnMode
				task->m_strBurnMode = jsonValueParams["burnMode"].asString();
				//burnType
				task->m_strBurnType = jsonValueParams["burnType"].asString();
				//AlarmSize  设置到配置文件中可配
				int nAlarmSize =  MainConfig::GetInstance()->GetDiscAlarmSize();
				if (jsonValueParams["alarmSize"].asInt() <= 0)
					task->m_nAlarmSize =  nAlarmSize;//3300;//jsonValueParams["alarmSize"].asInt();//
				else
					task->m_nAlarmSize = jsonValueParams["alarmSize"].asInt();//
				g_NetLog.Debug("config file AlarmSize is :%d, task alarmsize is %d.\n", nAlarmSize, task->m_nAlarmSize);
				//StreamInfo
				Json::Value	jsonStreamInfo = jsonValueParams["streamInfo"];
				task->m_burnStreamInfo.m_strBurnFileName = jsonStreamInfo["burnFileName"].asString();
				task->m_burnStreamInfo.m_strPlayListContent = jsonStreamInfo["playlistInfo"].asString();

				Json::Value	jsonBurnUrlList = jsonStreamInfo["burnUrlList"];
				for (int i = 0; i < jsonBurnUrlList.size(); i++)
				{
					FileInfo fileInfo;
					fileInfo.m_nFlag = 0;
					fileInfo.m_strSrcUrl = jsonBurnUrlList[i]["burnUrl"].asString();
					fileInfo.m_strDescription = jsonBurnUrlList[i]["urlDescription"].asString();
				}

				//fileInfo
				Json::Value	jsonFileInfo = jsonValueParams["fileInfo"];
				Json::Value jsonValueFileList = jsonFileInfo["burnFileList"];
				for (int i = 0; i < jsonValueFileList.size(); i++)
				{
					FileInfo fileInfo;
					fileInfo.m_strFileLocation = jsonValueFileList[i]["fileLocation"].asString();
					fileInfo.m_strType = jsonValueFileList[i]["fileType"].asString();
					fileInfo.m_strSrcUrl = jsonValueFileList[i]["burnSrcFilePath"].asString();
					fileInfo.m_strDestFilePath = jsonValueFileList[i]["burnDstFilePath"].asString();
					if (fileInfo.m_strDestFilePath.empty())
					{	//目标路径为空 采用源路径的文件名或者文件夹名
						if (fileInfo.m_strType.compare("file") == 0)
						{
							fileInfo.m_strDestFilePath = FileUtil::GetFileName(fileInfo.m_strSrcUrl);
						}
						else
						{	//当文件夹处理 这里可能最后带有/
							std::string strNoSlashDir = DirectoryUtil::EnsureNoSlashEnd(fileInfo.m_strSrcUrl);
							fileInfo.m_strDestFilePath = FileUtil::GetFileName(strNoSlashDir);
						}
						g_NetLog.Debug("[HttpServerModule::StartBurn]DestFilePaht is empty, default is %s", fileInfo.m_strDestFilePath.c_str());
					}
					fileInfo.m_strDescription = jsonValueFileList[i]["fileDescription"].asString();
					task->m_vecBurnFileInfo.push_back(fileInfo);
					g_NetLog.Debug("get protocol fileInfo.m_strType is %s.\n", fileInfo.m_strType.c_str());
					if (fileInfo.m_strType.compare("dir") == 0)
					{
						g_NetLog.Debug("get protocol file dest path is %s.\n", fileInfo.m_strDestFilePath.c_str());
					}
				}

				//burnSpeed
				task->m_nBurnSpeed = jsonValueParams["burnSpeed"].asInt();
				Json::Value jsonFeedback = jsonValueParams["feedback"];
				task->m_burnStateFeedback.m_strNeedFeedback = jsonFeedback["needFeedback"].asString();
				task->m_burnStateFeedback.m_strFeedbackIP = jsonFeedback["feedbackIP"].asString();
				task->m_burnStateFeedback.m_nFeedbackPort = jsonFeedback["feedbackPort"].asInt();
				task->m_burnStateFeedback.m_transType = jsonFeedback["transType"].asString();
				task->m_burnStateFeedback.m_nFeedbackInterval = jsonFeedback["feedIterval"].asInt();
				task->m_taskState = task->m_taskRealState = TASK_INIT;

				Poco::UUIDGenerator& gen = Poco::UUIDGenerator::defaultGenerator();
				Poco::UUID uSessionID = gen.createRandom();
				task->m_strSessionID = uSessionID.toString();
				g_NetLog.Debug("[%s] Add a Task.\n", __PRETTY_FUNCTION__);

				g_NetLog.Debug("[%s] Add a Task.task addr is %0x.\n", task);
				CBusiness::GetInstance()->StartBurn(task);

				Json::Value     jsonValueRoot;
				Json::Value     jsonValue1;
				Json::Value     jsonValue2;
				jsonValue2["retCode"] = Json::Value(0);
				jsonValue2["retMessage"] = Json::Value("ok");
				jsonValue2["sessionID"] = Json::Value(task->m_strSessionID);
				jsonValue1["method"] = Json::Value(sMethod.c_str());
				jsonValue1["params"] = jsonValue2;
				jsonValueRoot["result"] = jsonValue1;
				strOut = jsonValueRoot.toStyledString();
			}
			return strOut;
		}
		else
		{
			return "";
		}
	}
	catch (...)
	{
		printf("%s catched\n", __PRETTY_FUNCTION__);
		return "";
	}
}

std::string HttpServerModule::PauseBurn(std::string strIn)
{
	try
	{
		Json::Reader    jsonReader;
		Json::Value     jsonValueIn;

		if (jsonReader.parse(strIn, jsonValueIn))
		{
			std::string sMethod = jsonValueIn["method"].asString();
			Json::Value   jsonValueParams = jsonValueIn["params"];

			std::string strSessionID = jsonValueParams["sessionID"].asString();
			bool bRet = CBusiness::GetInstance()->PauseBurn(strSessionID);

			Json::Value     jsonValueRoot;
			Json::Value     jsonValue1;
			Json::Value     jsonValue2;
			if (bRet)
			{
				jsonValue2["retCode"] = Json::Value(0);
				jsonValue2["retMessage"] = Json::Value("ok");
			}
			else
			{
				jsonValue2["retCode"] = Json::Value(-1);
				jsonValue2["retMessage"] = Json::Value("pause task fail.");
			}
			jsonValue1["method"] = Json::Value(sMethod.c_str());
			jsonValue1["params"] = jsonValue2;
			jsonValueRoot["result"] = jsonValue1;
			string strOut = jsonValueRoot.toStyledString();
			return strOut;
		}
		else
		{
			return "";
		}
	}
	catch (...)
	{
		printf("%s catched\n", __PRETTY_FUNCTION__);
		return "";
	}
}

std::string HttpServerModule::ResumBurn(std::string strIn)
{
	try
	{
		Json::Reader    jsonReader;
		Json::Value     jsonValueIn;

		if (jsonReader.parse(strIn, jsonValueIn))
		{
			std::string sMethod = jsonValueIn["method"].asString();
			Json::Value   jsonValueParams = jsonValueIn["params"];

			std::string strSessionID = jsonValueParams["sessionID"].asString();
			bool bRet = CBusiness::GetInstance()->ResumeBurn(strSessionID);

			Json::Value     jsonValueRoot;
			Json::Value     jsonValue1;
			Json::Value     jsonValue2;
			if (bRet)
			{
				jsonValue2["retCode"] = Json::Value(0);
				jsonValue2["retMessage"] = Json::Value("ok");
			}
			else
			{
				jsonValue2["retCode"] = Json::Value(0);
				jsonValue2["retMessage"] = Json::Value("resume task fail.");
			}
			jsonValue1["method"] = Json::Value(sMethod.c_str());
			jsonValue1["params"] = jsonValue2;
			jsonValueRoot["result"] = jsonValue1;
			string strOut = jsonValueRoot.toStyledString();
			return strOut;
		}
		else
		{
			return "";
		}
	}
	catch (...)
	{
		printf("%s catched\n", __PRETTY_FUNCTION__);
		return "";
	}
}

std::string HttpServerModule::StopBurn(std::string strIn)
{
	try
	{
		Json::Reader    jsonReader;
		Json::Value     jsonValueIn;

		if (jsonReader.parse(strIn, jsonValueIn))
		{
			std::string sMethod = jsonValueIn["method"].asString();
			Json::Value   jsonValueParams = jsonValueIn["params"];

			std::string strSessionID = jsonValueParams["sessionID"].asString();

			bool bRet = CBusiness::GetInstance()->StopBurn(strSessionID);
			/*int nTaskSize = CBusiness::GetInstance()->GetBurnTaskSize();
			BurnTask* task;
			CBusiness::GetInstance()->GetCurTask(task);*/

			Json::Value     jsonValueRoot;
			Json::Value     jsonValue1;
			Json::Value     jsonValue2;
			if (bRet /*|| nTaskSize == 0 || task != NULL || task->m_taskRealState != TASK_BURN*/)
			{
				jsonValue2["retCode"] = Json::Value(0);
				jsonValue2["retMessage"] = Json::Value("ok");
			}
			else
			{
				jsonValue2["retCode"] = Json::Value(-1);
				jsonValue2["retMessage"] = Json::Value("sessionID not find in task list.");
			}
			jsonValue1["method"] = Json::Value(sMethod.c_str());
			jsonValue1["params"] = jsonValue2;
			jsonValueRoot["result"] = jsonValue1;
			string strOut = jsonValueRoot.toStyledString();
			return strOut;
		}
		else
		{
			return "";
		}
	}
	catch (...)
	{
		printf("%s catched\n", __PRETTY_FUNCTION__);
		return "";
	}
}

std::string HttpServerModule::GetCDRomInfo(std::string strIn)
{
	std::string strRet;
	try
	{
		Json::Reader    jsonReader;
		Json::Value     jsonValueIn;
		//g_NetLog.Debug("Enter %s.\n", __PRETTY_FUNCTION__);
		if (jsonReader.parse(strIn, jsonValueIn))
		{
			std::string sMethod = jsonValueIn["method"].asString();
			Json::Value   jsonValueParams = jsonValueIn["params"];
			//CDRomID 
			std::string strCDRomID = jsonValueParams["cdRomID"].asString();
			std::string strOut = "";
			strOut = CBusiness::GetInstance()->GetCDRomInfo(strCDRomID, sMethod);
			return strOut;
		}
		else
		{
			return "";
		}
	}
	catch (...)
	{
		printf("%s catched\n", __PRETTY_FUNCTION__);
		return "";
	}
}

std::string HttpServerModule::AddBurnFile(std::string strIn)
{
	std::string strRet;
	try
	{
		Json::Reader    jsonReader;
		Json::Value     jsonValueIn;

		if (jsonReader.parse(strIn, jsonValueIn))
		{
			std::string sMethod = jsonValueIn["method"].asString();
			Json::Value   jsonValueParams = jsonValueIn["params"];

			Json::Value     jsonValueRoot;
			Json::Value     jsonValue1;
			Json::Value     jsonValue2;

			//sessionID 
			std::string strSessionID = jsonValueParams["sessionID"].asString();

			BurnTask* task;
			//CBusiness::GetInstance()->GetCurTask(task);
			int nRet = CBusiness::GetInstance()->GetFirstTaskInVec(task);
			//要对任务队列做判断
			if (nRet == -1 || task == NULL)
			{
				jsonValue2["retCode"] = Json::Value(-1);
				jsonValue2["retMessage"] = Json::Value("no task in tasklist.");
				jsonValue1["method"] = Json::Value(sMethod.c_str());
				jsonValue1["params"] = jsonValue2;
				jsonValueRoot["result"] = jsonValue1;
			}
			else if ((task->m_taskState == TASK_STOP) || (task->m_strSessionID.compare(strSessionID) != 0))
			{
				jsonValue2["retCode"] = Json::Value(-1);
				jsonValue2["retMessage"] = Json::Value("task has be set stop or sessionID different.");
				jsonValue1["method"] = Json::Value(sMethod.c_str());
				jsonValue1["params"] = jsonValue2;
				jsonValueRoot["result"] = jsonValue1;
			}
			else
			{
				//fileInfo vector
				std::vector<FileInfo> vecFileInfo;
				Json::Value jsonValueFileList = jsonValueParams["burnFileList"];
				for (int i = 0; i < jsonValueFileList.size(); i++)
				{
					FileInfo fileInfo;
					fileInfo.m_nFlag = 1;
					fileInfo.m_strFileLocation = jsonValueFileList[i]["fileLocation"].asString();
					fileInfo.m_strType = jsonValueFileList[i]["fileType"].asString();
					fileInfo.m_strSrcUrl = jsonValueFileList[i]["burnSrcFilePath"].asString();
					fileInfo.m_strDestFilePath = jsonValueFileList[i]["burnDstFilePath"].asString();
					if (fileInfo.m_strDestFilePath.empty())
					{	//目标路径为空 采用源路径的文件名或者文件夹名
						if (fileInfo.m_strType.compare("file") == 0)
						{
							fileInfo.m_strDestFilePath = FileUtil::GetFileName(fileInfo.m_strSrcUrl);
						}
						else
						{	//当文件夹处理 这里可能最后带有/
							std::string strNoSlashDir = DirectoryUtil::EnsureNoSlashEnd(fileInfo.m_strSrcUrl);
							fileInfo.m_strDestFilePath = FileUtil::GetFileName(strNoSlashDir);
						}
						g_NetLog.Debug("[HttpServerModule::AddBurnFile]DestFilePaht is empty, default is %s", fileInfo.m_strDestFilePath.c_str());
					}
					fileInfo.m_strDescription = jsonValueFileList[i]["fileDescription"].asString();
					vecFileInfo.push_back(fileInfo);
				}
				CBusiness::GetInstance()->AddBurnFile(strSessionID, vecFileInfo);

				jsonValue2["retCode"] = Json::Value(0);
				jsonValue2["retMessage"] = Json::Value("ok");
				jsonValue1["method"] = Json::Value(sMethod.c_str());
				jsonValue1["params"] = jsonValue2;
				jsonValueRoot["result"] = jsonValue1;
			}
			string strOut = jsonValueRoot.toStyledString();
			return strOut;
		}
		else
		{
			return "";
		}
	}
	catch (...)
	{
		printf("%s catched\n", __PRETTY_FUNCTION__);
		return "";
	}
}

std::string HttpServerModule::SetLogServer(std::string strIn)
{
	std::string strRet;
	try
	{
		Json::Reader    jsonReader;
		Json::Value     jsonValueIn;

		if (jsonReader.parse(strIn, jsonValueIn))
		{
			std::string sMethod = jsonValueIn["method"].asString();
			Json::Value jsonValueParams = jsonValueIn["params"];

			Json::Value jsonValueHost = jsonValueParams["host"];

			std::string strIP1 = jsonValueHost["ip1"].asString();
			int nPort1 = jsonValueHost["port1"].asInt();
			g_NetLog.SetDest1(strIP1, nPort1);

			std::string strIP2 = jsonValueHost["ip1"].asString();
			int nPort2 = jsonValueHost["port1"].asInt();
			g_NetLog.SetDest2(strIP2, nPort2);

			std::string strIP3 = jsonValueHost["ip3"].asString();
			int nPort3 = jsonValueHost["port3"].asInt();
			g_NetLog.SetDest3(strIP3, nPort3);

			Json::Value     jsonValueRoot;
			Json::Value     jsonValue1;
			Json::Value     jsonValue2;
			jsonValue2["retCode"] = Json::Value(0);
			jsonValue2["retMessage"] = Json::Value("ok");
			jsonValue1["method"] = Json::Value(sMethod.c_str());
			jsonValue1["params"] = jsonValue2;
			jsonValueRoot["result"] = jsonValue1;
			jsonValueRoot["result"] = jsonValue1;
			string strOut = jsonValueRoot.toStyledString();
			return strOut;
		}
		else
		{
			return "";
		}
	}
	catch (...)
	{
		g_FileLog.Info("%s catched\n", __PRETTY_FUNCTION__);
		return "";
	}
}