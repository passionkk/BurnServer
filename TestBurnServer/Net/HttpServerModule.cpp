#include "../stdafx.h"
#include "HttpServerModule.h"
#include "../CommonDefine.h"
#include "jsoncpp/json/json.h"

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
	pChannel->Start(1001, &Access_callback, (void*)this, &response_completed_callback);
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
