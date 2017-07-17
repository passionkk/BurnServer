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
		g_NetLog.Debug("[HttpServerModule::ProcessPassiveProtocol] jsonRecv : %s.\n", jsonRecv.c_str());
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
		if (sMethod == "testProtocol")
		{
			jsonSend = TestProtocol(jsonRecv);
		}
		else if (sMethod.compare("getCDRomList") == 0)
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
		else if (sMethod == "agentHeartBeat")
        {
            jsonSend = AgentHeartBeat(jsonRecv);
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

std::string HttpServerModule::TestProtocol(std::string strIn)
{
	try
	{
		Json::Reader    jsonReader;
		Json::Value     jsonValueIn;

		if (jsonReader.parse(strIn, jsonValueIn))
		{
			std::string sMethod = jsonValueIn["method"].asString();
			Json::Value   jsonValueParams = jsonValueIn["params"];
			
			int nCloseDisk = jsonValueParams["CloseDisk"].asInt();
			if (nCloseDisk == 1)
				CBusiness::GetInstance()->CloseDiscFeedback();
			else
				CBusiness::GetInstance()->BurnStateFeedback();

			int iLogicNo = jsonValueParams["logicNo"].asInt();

			Json::Value     jsonValueRoot;
			Json::Value     jsonValue1;
			Json::Value     jsonValue2;
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



std::string HttpServerModule::AgentHeartBeat(std::string strIn)
{
    try
    {
        /*Json::Reader    jsonReader;
        Json::Value     jsonValueIn;

        if (jsonReader.parse(strIn, jsonValueIn))
        {
            std::string sMethod = jsonValueIn["method"].asString();
            Json::Value   jsonValueParams = jsonValueIn["params"];
            int iLogicNo = jsonValueParams["logicNo"].asInt();

            CBusiness::GetInstance()->SiPWrap_HeartBeat(iLogicNo);

            Json::Value     jsonValueRoot;
            Json::Value     jsonValue1;
            Json::Value     jsonValue2;
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
        }*/
		return "";
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
			if (strCheckCDRomID.empty())
				strCheckCDRomID = "CDRom_1"; 
			//验证是否存在 cdRomID的光驱
			bool bRet = CBusiness::GetInstance()->CheckExistCDRom(strCheckCDRomID);

			if (CBusiness::GetInstance()->GetBurnTaskSize() > 0 || !bRet)
			{	
				BurnTask task;
				CBusiness::GetInstance()->GetCurTask(task);
				// 仅保留一个刻录任务
				Json::Value     jsonValueRoot;
				Json::Value     jsonValue1;
				Json::Value     jsonValue2;
				jsonValue2["retCode"] = Json::Value(-1);
				jsonValue2["retMessage"] = Json::Value("Exist Burn Task.");
				jsonValue2["sessionID"] = Json::Value(task.m_strSessionID);
				jsonValue1["method"] = Json::Value(sMethod.c_str());
				jsonValue1["params"] = jsonValue2;
				jsonValueRoot["result"] = jsonValue1;
				strOut = jsonValueRoot.toStyledString();
			}
			else
			{
				BurnTask task;
				//discName
				task.m_strDiscName = jsonValueParams["discName"].asString();
				//strCDRomID
				task.m_strCDRomID = strCheckCDRomID;//jsonValueParams["cdRomID"].asString();
				//burnMode
				task.m_strBurnMode = jsonValueParams["burnMode"].asString();
				//burnType
				task.m_strBurnType = jsonValueParams["burnType"].asString();
				//AlarmSize  设置到配置文件中可配
				task.m_nAlarmSize = 4300;//jsonValueParams["alarmSize"].asInt();
				//StreamInfo
				Json::Value	jsonStreamInfo = jsonValueParams["streamInfo"];
				task.m_burnStreamInfo.m_strBurnFileName = jsonStreamInfo["burnFileName"].asString();
				task.m_burnStreamInfo.m_strPlayListContent = jsonStreamInfo["playlistInfo"].asString();

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
					task.m_vecBurnFileInfo.push_back(fileInfo);
					g_NetLog.Debug("get protocol fileInfo.m_strType is %s.\n", fileInfo.m_strType.c_str());
					if (fileInfo.m_strType.compare("dir") == 0)
					{
						g_NetLog.Debug("get protocol file dest path is %s.\n", fileInfo.m_strDestFilePath.c_str());
					}
				}

				//burnSpeed
				task.m_nBurnSpeed = jsonValueParams["burnSpeed"].asInt();
				Json::Value jsonFeedback = jsonValueParams["feedback"];
				task.m_burnStateFeedback.m_strNeedFeedback = jsonFeedback["needFeedback"].asString();
				task.m_burnStateFeedback.m_strFeedbackIP = jsonFeedback["feedbackIP"].asString();
				task.m_burnStateFeedback.m_nFeedbackPort = jsonFeedback["feedbackPort"].asInt();
				task.m_burnStateFeedback.m_transType = jsonFeedback["transType"].asString();
				task.m_burnStateFeedback.m_nFeedbackInterval = jsonFeedback["feedIterval"].asInt();
				task.m_taskState = task.m_taskRealState = TASK_INIT;

				Poco::UUIDGenerator& gen = Poco::UUIDGenerator::defaultGenerator();
				Poco::UUID uSessionID = gen.createRandom();
				task.m_strSessionID = uSessionID.toString();
				g_NetLog.Debug("[%s] Add a Task.\n", __PRETTY_FUNCTION__);
				CBusiness::GetInstance()->StartBurn(task);

				Json::Value     jsonValueRoot;
				Json::Value     jsonValue1;
				Json::Value     jsonValue2;
				jsonValue2["retCode"] = Json::Value(0);
				jsonValue2["retMessage"] = Json::Value("ok");
				jsonValue2["sessionID"] = Json::Value(task.m_strSessionID);
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
			CBusiness::GetInstance()->PauseBurn(strSessionID);

			Json::Value     jsonValueRoot;
			Json::Value     jsonValue1;
			Json::Value     jsonValue2;
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
			CBusiness::GetInstance()->ResumeBurn(strSessionID);

			Json::Value     jsonValueRoot;
			Json::Value     jsonValue1;
			Json::Value     jsonValue2;
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
			int nTaskSize = CBusiness::GetInstance()->GetBurnTaskSize();
			BurnTask task;
			CBusiness::GetInstance()->GetCurTask(task);

			Json::Value     jsonValueRoot;
			Json::Value     jsonValue1;
			Json::Value     jsonValue2;
			if (bRet || nTaskSize == 0 || task.m_taskRealState != TASK_BURN)
			{
				jsonValue2["retCode"] = Json::Value(0);
				jsonValue2["retMessage"] = Json::Value("ok");
			}
			else
			{
				jsonValue2["retCode"] = Json::Value(-1);
				char szSize[256] = { 0 };
				sprintf(szSize, "sessionID %s not belong to burn task, burn task sessionID is %s.", task.m_strSessionID.c_str());
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
		g_NetLog.Debug("Enter %s.\n", __PRETTY_FUNCTION__);
		if (jsonReader.parse(strIn, jsonValueIn))
		{
			std::string sMethod = jsonValueIn["method"].asString();
			Json::Value   jsonValueParams = jsonValueIn["params"];

			//CDRomID 
			std::string strCDRomID = jsonValueParams["cdRomID"].asString();
			
			//std::string strOut = GetCDRonInfo(strCDRomID);
			CDRomInfo cdRomInfo;
			//DiskInfo diskInfo;

#if 0
			Json::Value     jsonValueRoot;
			Json::Value     jsonValue1;
			Json::Value     jsonValue2;
			int nRet = CBusiness::GetInstance()->GetCDRomInfo(strCDRomID, cdRomInfo, diskInfo);
			if (nRet == 0)
			{
				jsonValue2["retCode"] = Json::Value(0);
				jsonValue2["retMessage"] = Json::Value("ok");

				//从当前刻录任务中获取刻录相关信息，内核调用获取的剩余大小不准确
				BurnTask task;
				CBusiness::GetInstance()->GetCurTask(task);
				g_NetLog.Debug("%s GetCDRomInfo.\n", __PRETTY_FUNCTION__);
				int nIndex = -1;
				for (int i = 0; i < task.m_vecCDRomInfo.size(); i++)
				{
					g_NetLog.Debug("%s i = %d.\n", __PRETTY_FUNCTION__, i);
					if (task.m_vecCDRomInfo.at(i).m_strCDRomID.compare(strCDRomID) == 0)
					{
						nIndex = i;
						nRet = 0;
						cdRomInfo = task.m_vecCDRomInfo.at(i);
						break;
					}
				}
				g_NetLog.Debug("%s nIndex = %d, nRet = %d.\n", __PRETTY_FUNCTION__, nIndex, nRet);

				//返回实际光驱信息
				jsonValue2["cdRomID"] = Json::Value(strCDRomID);
				jsonValue2["cdRomName"] = Json::Value(cdRomInfo.m_strCDRomName);
				std::string strDes = "";
				if (nIndex != -1)
					CBusiness::GetInstance()->GetBurnStateString(task.m_taskRealState, strDes);
				else
					strDes = "空闲";
				jsonValue2["burnState"] = Json::Value(cdRomInfo.m_euWorkState);
				jsonValue2["burnStateDescription"] = Json::Value(strDes);
				jsonValue2["hasDVD"] = Json::Value(cdRomInfo.m_nHasDisc);
				char szSize[256] = { 0 };
				if (nIndex != -1)
					sprintf(szSize, "%dMB", diskInfo.discsize - task.m_nBurnedSize);
				else
					sprintf(szSize, "%dMB", diskInfo.freesize);
				jsonValue2["DVDLeftCapcity"] = Json::Value(szSize);//Json::Value(diskInfo.freesize);
				memset(szSize, 0, 256);
				sprintf(szSize, "%dMB", diskInfo.discsize);
				jsonValue2["DVDTotalCapcity"] = Json::Value(szSize);//Json::Value(diskInfo.discsize);
				g_NetLog.Debug("%s nRet = %d. assign json value.\n", __PRETTY_FUNCTION__, nRet);
			}
			else
			{
				jsonValue2["retCode"] = Json::Value(1);
				jsonValue2["retMessage"] = Json::Value("Get CDRomInfo fail.");

				//返回实际光驱信息
				jsonValue2["cdRomID"] = Json::Value(strCDRomID);
				jsonValue2["cdRomName"] = Json::Value("");
				jsonValue2["burnState"] = Json::Value(0);
				jsonValue2["burnStateDescription"] = Json::Value("");
				jsonValue2["hasDVD"] = Json::Value(0);
				jsonValue2["DVDLeftCapcity"] = Json::Value("0MB");
				jsonValue2["DVDTotalCapcity"] = Json::Value("0MB");
				g_NetLog.Debug("%s nRet = %d. assign json value.\n", __PRETTY_FUNCTION__, nRet);
			}
#endif
#if 1
			//int nRet = -1;// CBusiness::GetInstance()->GetCDRomInfo(strCDRomID, cdRomInfo, diskInfo);
			BurnTask task;
			int nRet = CBusiness::GetInstance()->GetCurTask(task);
			g_NetLog.Debug("%s GetCDRomInfo.\n", __PRETTY_FUNCTION__);
			
			int nIndex = -1;
			if (nRet == 0)
			{
				nRet = -1;
				for (int i = 0; i < task.m_vecCDRomInfo.size(); i++)
				{
					g_NetLog.Debug("%s i = %d.\n", __PRETTY_FUNCTION__, i);
					if (task.m_vecCDRomInfo.at(i).m_strCDRomID.compare(strCDRomID) == 0/* && task.m_nUseCDRomIndex == i*/)
					{
						nIndex = i;
						nRet = 0;
						cdRomInfo = task.m_vecCDRomInfo.at(i);
						//diskInfo = task.m_vecCDRomInfo.at(i).m_discInfo;
						break;
					}
				}
			}
			g_NetLog.Debug("%s nIndex = %d, nRet = %d.\n", __PRETTY_FUNCTION__, nIndex, nRet);
			Json::Value     jsonValueRoot;
			Json::Value     jsonValue1;
			Json::Value     jsonValue2;
			if (nRet == 0 && nIndex > -1)
			{
				if (nRet == 0)
				{	//当前刻录光驱
					jsonValue2["retCode"] = Json::Value(0);
					jsonValue2["retMessage"] = Json::Value("ok");

					//返回实际光驱信息
					jsonValue2["cdRomID"] = Json::Value(strCDRomID);
					jsonValue2["cdRomName"] = Json::Value(cdRomInfo.m_strCDRomName);
					std::string strDes = "";
					int nFeedbackState = 0;
					strDes = CBusiness::GetInstance()->GetCDRomState(task.m_vecCDRomInfo.at(nIndex).m_euWorkState, nFeedbackState);
					g_NetLog.Debug("[HttpServerModule::GetCDRomInfo]m_euWorkState %d, nFeedbackState : %d.\n",
								   (int)task.m_vecCDRomInfo.at(nIndex).m_euWorkState, nFeedbackState);
					jsonValue2["burnState"] = Json::Value(nFeedbackState);//1);
					jsonValue2["burnStateDescription"] = Json::Value(/*"刻录中"*/strDes);
					jsonValue2["hasDVD"] = Json::Value(cdRomInfo.m_discInfo.discsize > 0 ? 1: 0);
					char szSize[256] = { 0 };
					sprintf(szSize, "%dMB", max(0, int(cdRomInfo.m_discInfo.discsize - task.m_vecCDRomInfo.at(nIndex).m_nBurnedSize)));
					jsonValue2["DVDLeftCapcity"] = Json::Value(szSize);
					memset(szSize, 0, 256);
					sprintf(szSize, "%dMB", cdRomInfo.m_discInfo.discsize);
					jsonValue2["DVDTotalCapcity"] = Json::Value(szSize);
				}
			}
			else
			{	//任务不存在，或者任务存在但是任务工作光驱不是所要查询的。
				jsonValue2["retCode"] = Json::Value(0);
				jsonValue2["retMessage"] = Json::Value("ok");
				CBusiness::GetInstance()->GetCDRomInfoFromSavedVector(strCDRomID, cdRomInfo);
				//nRet = CBusiness::GetInstance()->GetCDRomInfo(strCDRomID, cdRomInfo, diskInfo);
				//返回实际光驱信息
				jsonValue2["cdRomID"] = Json::Value(strCDRomID);
				jsonValue2["cdRomName"] = Json::Value(cdRomInfo.m_strCDRomName);
				jsonValue2["burnState"] = Json::Value(0);//无任务
				std::string strDes = "空闲";
				jsonValue2["burnStateDescription"] = Json::Value(strDes);
				jsonValue2["hasDVD"] = Json::Value(0);
				jsonValue2["DVDLeftCapcity"] = Json::Value("0MB");
				jsonValue2["DVDTotalCapcity"] = Json::Value("0MB");
			}
#endif
			jsonValue1["method"] = Json::Value(sMethod.c_str());
			jsonValue1["params"] = jsonValue2;
			jsonValueRoot["result"] = jsonValue1;
			string strOut = jsonValueRoot.toStyledString();
			g_NetLog.Debug("%s nRet = %d. return string is %s.\n", __PRETTY_FUNCTION__, nRet, strOut.c_str());
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

			BurnTask task;
			CBusiness::GetInstance()->GetCurTask(task);
			
			if ((task.m_taskState == TASK_STOP) || (task.m_strSessionID.compare(strSessionID) != 0))
			{
				jsonValue2["retCode"] = Json::Value(-1);
				jsonValue2["retMessage"] = Json::Value("task has be set stop.");
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