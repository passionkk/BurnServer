#include "HttpServerModule.h"
#include "Business.h"
#include "CommonDefine.h"
#include "jsoncpp/json/json.h"
#include "MainConfig.h"

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
	pChannel->Start(iPort, &Access_callback, (void*)this, &response_completed_callback);
	//pChannel->Start(90, &Access_callback, (void*)this, &response_completed_callback);
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
        printf("http server  process passive protocol break\n");
        return MHD_NO;
    }
}

std::string HttpServerModule::ProcessProtocol(std::string sMethod, std::string jsonRecv)
{
    try
    {
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
			jsonSend = StartBurn(jsonRecv);
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
		else if(sMethod == "agentHeartBeat")
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

			CBusiness::GetInstance()->GetCDRomList();

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

std::string HttpServerModule::StartBurn(std::string strIn)
{
	try
	{
		Json::Reader    jsonReader;
		Json::Value     jsonValueIn;

		if (jsonReader.parse(strIn, jsonValueIn))
		{
			std::string sMethod = jsonValueIn["method"].asString();
			Json::Value   jsonValueParams = jsonValueIn["params"];

			BurnTask task;
			//burnMode
			task.m_strBurnMode = jsonValueParams["burnMode"].asString();
			//burnType
			task.m_strBurnType = jsonValueParams["burnType"].asString();
			//AlarmSize
			task.m_nAlarmSize = jsonValueParams["alarmSize"].asInt();
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
				fileInfo.m_strDescription = jsonValueFileList[i]["fileDescription"].asString();
				task.m_vecBurnFileInfo.push_back(fileInfo);
			}
			
			//burnSpeed
			task.m_nBurnSpeed = jsonValueParams["burnSpeed"].asInt();
			Json::Value jsonFeedback = jsonValueParams["feedback"];
			task.m_burnStateFeedback.m_strNeedFeedback = jsonFeedback["needFeedback"].asString();
			task.m_burnStateFeedback.m_strFeedbackIP = jsonFeedback["feedbackIP"].asString();
			task.m_burnStateFeedback.m_nFeedbackPort = jsonFeedback["feedbackPort"].asInt();
			task.m_burnStateFeedback.m_transType = jsonFeedback["transType"].asString();
			task.m_burnStateFeedback.m_nFeedbackInterval = jsonFeedback["feedIterval"].asInt();
			
			Poco::UUIDGenerator& gen = Poco::UUIDGenerator::defaultGenerator();
			Poco::UUID uSessionID = gen.createRandom();
			task.m_strSessionID = uSessionID.toString();
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

			CBusiness::GetInstance()->StopBurn(strSessionID);

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

std::string HttpServerModule::GetCDRomInfo(std::string strIn)
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

			//sessionID 
			std::string strCDRomID = jsonValueParams["cdRomID"].asString();
					
			CBusiness::GetInstance()->GetCDRomInfo(strCDRomID);

			Json::Value     jsonValueRoot;
			Json::Value     jsonValue1;
			Json::Value     jsonValue2;
			jsonValue2["retCode"] = Json::Value(0);
			jsonValue2["retMessage"] = Json::Value("ok");
	
			//返回实际光驱信息
			jsonValue2["cdRomID"] = "CDRom_1";
			jsonValue2["cdRomName"] = "光驱1";
			jsonValue2["burnState"] = 0;
			jsonValue2["burnStateDescription"] = "未刻录";
			jsonValue2["hasDVD"] = 0;
			jsonValue2["DVDLeftCapcity"] = "0MB";
			jsonValue2["DVDTotalCapcity"] = "0MB",

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

			//sessionID 
			std::string strSessionID = jsonValueParams["sessionID"].asString();
			
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
				fileInfo.m_strDescription = jsonValueFileList[i]["fileDescription"].asString();
				vecFileInfo.push_back(fileInfo);
			}
			CBusiness::GetInstance()->AddBurnFile(strSessionID, vecFileInfo);

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
