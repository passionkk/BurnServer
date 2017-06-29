#include "../stdafx.h"
#include "HttpServerModule.h"
#include "../CommonDefine.h"
#include "jsoncpp/json/json.h"
#include "CCBUtility.h"
#include "../FileUtil.h"

HttpServerModule* HttpServerModule::m_pInstance = NULL;

#define BUFFER_SIZE  (128*1024)

#define PAGE "<html><head><title>File not found</title></head><body>File not found</body></html>"
#define REDIRECT "<html><head><title>redirect</title></head><body><a href=\"http://127.0.0.1:80/web/login.html\">main</a></body></html>"
#define BUFFER_SIZE  (128*1024)
#define MAX_HEADER_LENGTH (256*1024)
#define MAX_BODY_LENGTH (30*1024*1024)


static ssize_t
file_reader(void *cls,
uint64_t pos,
char *buf,
size_t max)
{
	FILE *file = (FILE*)cls;
	fseek(file, pos, SEEK_SET);
	int nRead = fread(buf, 1, max, file);
	TRACE("file_reader read %d bytes.\n", nRead);
	return nRead;
}


static void
free_callback(void *cls)
{
	FILE *file = (FILE*)cls;
	fclose(file);
	TRACE("free_callback \n");
}

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
		else if (0 == strcmp(method, MHD_HTTP_METHOD_GET))
		{
			ret = ((HttpServerModule*)cls)->ProcessGETRequest(connection, url);
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
	m_pCallbackFun = NULL;
	m_pVoid = NULL;
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

std::string HttpServerModule::ProcessProtocol(std::string sMethod, std::string jsonRecv/*, std::string& strOut*/)
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
		else if (sMethod == "closeDiscFeedback")
		{
			jsonSend = FeedbackBeforeCloseDisc(jsonRecv/*, strOut*/);
		}
		else if (sMethod == "burnStateFeedback")
		{
			jsonSend = BurnStateFeedback(jsonRecv/*, strOut*/);
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

static int
print_out_key(void *cls, enum MHD_ValueKind kind, const char *key,
const char *value)
{
	if (key != NULL && value != NULL)
	{
		printf("%s : %s\n", key, value);//g_NetLog.Debug("%s: %s\n", key, value);
	}
	return MHD_YES;
}

int HttpServerModule::ProcessGETRequest(struct MHD_Connection *connection, const char *  url)
{
	try
	{
		struct MHD_Response *response;
		int ret = MHD_NO;
		if (strstr(url, "download") != NULL)
		{

			MHD_get_connection_values(connection, MHD_GET_ARGUMENT_KIND, print_out_key, NULL);
			const char* szFilePath = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "filePath");

			if (FileUtil::FileExist(szFilePath))
			{
				TRACE(L"download config file\n");
				//发送文件
				FILE *file = NULL;
				INT64 i64Size = FileUtil::FileSize(szFilePath);
				file = fopen(szFilePath, "rb");

				if (NULL == file)
				{
					return MHD_NO;
				}
				TRACE(L"File Size : %lld.\n", i64Size);
				response = MHD_create_response_from_callback(i64Size/*nFileSize*/, 32 * 1024,     /* 32k page size */
															 &file_reader,
															 file,
															 &free_callback);
				if (NULL == response)
				{
					fclose(file);
					ret = MHD_NO;
				}
				else
				{
					MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_DISPOSITION, "attachment;filename=uart.txt");
					ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
					TRACE(L"ret : %d.\n", ret);
					MHD_destroy_response(response);
				}
			}
			else
			{
				ret = MHD_NO;
			}
		}
		else
		{
			FILE *file = NULL;
			struct stat buf;
			char szUrl[1024];
			int iPosUrl = 0;
			memset(szUrl, 0, 1024);
			strncpy(szUrl, &url[1], 1024);
			TRACE(L"szUrl [%s]\n", szUrl);

#ifdef WIN32
			if (GetModuleFileNameA(NULL, szUrl, 1024) > 0)
			{
				iPosUrl = strlen(szUrl) - 1;
				while (iPosUrl > 0 && szUrl[iPosUrl] != '\\')
				{
					szUrl[iPosUrl] = '\0';
					iPosUrl--;
				}
				strncat(szUrl, &url[1], 1024);
			}
#endif

			if (0 == stat(szUrl, &buf))
				file = fopen(szUrl, "rb");
			else
				file = NULL;
			if (NULL == file)
			{
				if (strlen(&url[1]) == 0)
				{
					response = MHD_create_response_from_buffer(strlen(REDIRECT),
															   (void *)REDIRECT,
															   MHD_RESPMEM_PERSISTENT);
					MHD_add_response_header(response, MHD_HTTP_HEADER_LOCATION, "/web/login.html");
					ret = MHD_queue_response(connection, MHD_HTTP_MOVED_PERMANENTLY, response);
					MHD_destroy_response(response);
				}
				else
				{
					response = MHD_create_response_from_buffer(strlen(PAGE),
															   (void *)PAGE,
															   MHD_RESPMEM_PERSISTENT);
					ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
					MHD_destroy_response(response);
				}
			}
			else
			{
				response = MHD_create_response_from_callback(buf.st_size, 32 * 1024,     /* 32k page size */
															 &file_reader,
															 file,
															 &free_callback);
				if (NULL == response)
				{
					fclose(file);
					ret = MHD_NO;
				}
				else
				{
					std::string sUrl = szUrl;
					std::string sContentType = HttpServerModule::GetContentTypeByUrl(sUrl);
					if (sContentType != "")
					{
						MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_TYPE, sContentType.c_str());
					}
					ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
					MHD_destroy_response(response);
				}
			}
		}
		return ret;
	}
	catch (...)
	{
		TRACE(L"%s catched\n", __PRETTY_FUNCTION__);
		return MHD_NO;
	}
}


void HttpServerModule::SetCallback(Fun pFun, LPVOID pVoid)
{
	m_pCallbackFun = pFun;
	m_pVoid = pVoid;
}

std::string HttpServerModule::GetContentTypeByUrl(std::string sUrl)
{
	std::string sNameNoExt = "";
	std::string sExt = "";
	std::string sContentType = "";
	CCBUtility::SplitFileExt(sUrl, sNameNoExt, sExt);
	if (sExt == "js")
	{
		sContentType = "application/javascript";
	}
	else if (sExt == "asp")
	{
		sContentType = "text/asp";
	}
	else if (sExt == "bmp")
	{
		sContentType = "application/x-bmp";
	}
	else if (sExt == "doc")
	{
		sContentType = "application/msword";
	}
	else if (sExt == "gif")
	{
		sContentType = "image/gif";
	}
	else if (sExt == "html")
	{
		sContentType = "text/html";
	}
	else if (sExt == "htx")
	{
		sContentType = "text/html";
	}
	else if (sExt == "ico")
	{
		sContentType = "image/x-icon";
	}
	else if (sExt == "img")
	{
		sContentType = "application/x-img";
	}
	else if (sExt == "java")
	{
		sContentType = "java/*";
	}
	else if (sExt == "jpe")
	{
		sContentType = "image/jpeg";
	}
	else if (sExt == "jpeg")
	{
		sContentType = "image/jpeg";
	}
	else if (sExt == "jpg")
	{
		sContentType = "application/x-jpg";
	}
	else if (sExt == "jsp")
	{
		sContentType = "text/html";
	}
	else if (sExt == "mp4")
	{
		sContentType = "video/mpeg4";
	}
	else if (sExt == "png")
	{
		sContentType = "application/x-png";
	}
	else if (sExt == "wma")
	{
		sContentType = "audio/x-ms-wma";
	}
	else if (sExt == "xml")
	{
		sContentType = "text/xml";
	}
	else if (sExt == "tif")
	{
		sContentType = "image/tiff";
	}
	else if (sExt == "avi")
	{
		sContentType = "video/avi";
	}
	else if (sExt == "class")
	{
		sContentType = "java/*";
	}
	else if (sExt == "css")
	{
		sContentType = "text/css";
	}
	else if (sExt == "htm")
	{
		sContentType = "text/html";
	}
	else if (sExt == "m3u")
	{
		sContentType = "audio/mpegurl";
	}
	else if (sExt == "mp3")
	{
		sContentType = "audio/mp3";
	}

	return sContentType;
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


//封盘前反馈
std::string HttpServerModule::FeedbackBeforeCloseDisc(std::string strIn/*, std::string& strOut*/)
{
	try
	{
#if 0
		Json::Reader    jsonReader;
		Json::Value     jsonValueIn;

		if (jsonReader.parse(strIn, jsonValueIn))
		{
			std::string sMethod = jsonValueIn["method"].asString();
			Json::Value   jsonValueParams = jsonValueIn["params"];

			Json::Value     jsonValueRoot;
			Json::Value     jsonValue1;
			Json::Value     jsonValue2;

			Json::Value		jsonDocFile;
			jsonDocFile["fileLocation"] = Json::Value("local");
			jsonDocFile["burnSrcFilePath"] = Json::Value("/mnt/HD0/burnTestFile/DocFile.docx");
			jsonDocFile["burnDstFilePath"] = Json::Value("DocFile.docx");
			jsonDocFile["fileDescription"] = Json::Value("Doc File");
			jsonValue2["DocFile"] = jsonDocFile;

			Json::Value		jsonFileList;
			Json::Value		jsonPlayer;
			jsonPlayer["fileLocation"] = Json::Value("local");
			jsonPlayer["burnSrcFilePath"] = Json::Value("/mnt/HD0/burnTestFile/player.exe");
			jsonPlayer["burnDstFilePath"] = Json::Value("player.exe");
			jsonPlayer["fileDescription"] = Json::Value("player");
			jsonFileList.append(jsonPlayer);
			
			Json::Value		jsonTestFile;
			jsonTestFile["fileLocation"] = Json::Value("local");
			jsonTestFile["burnSrcFilePath"] = Json::Value("/mnt/HD0/burnTestFile/1.mp4");
			jsonTestFile["burnDstFilePath"] = Json::Value("burnFeedbackTestFile.mp4");
			jsonTestFile["fileDescription"] = Json::Value("burnFeedbackTestFile");
			jsonFileList.append(jsonTestFile);

			jsonValue2["burnFileList"] = jsonFileList;

			jsonValue2["retCode"] = Json::Value(0);
			jsonValue2["retMessage"] = Json::Value("ok");
			jsonValue1["method"] = Json::Value(sMethod.c_str());
			jsonValue1["params"] = jsonValue2;
			jsonValueRoot["result"] = jsonValue1;
			string strOut = jsonValueRoot.toStyledString();

			if (m_pCallbackFun)
			{
				m_pCallbackFun(m_pVoid, strOut, 0);
			}
			return strOut;
		}
#endif

		Object::Ptr pObj = new Object(true);
		pObj->set("method", "closeDiscFeedback");
		
		Object::Ptr pParams = new Object(true);
		pParams->set("retCode", 0);
		pParams->set("retMessage", "ok");


		//fileInfo
		Object::Ptr	burnDocFile = new Object(true);
		burnDocFile->set("fileLocation", "local");
		burnDocFile->set("burnSrcFilePath", "/mnt/HD0/burnTestFile/DocFile.docx");
		burnDocFile->set("burnDstFilePath", "DocFile.docx");
		burnDocFile->set("fileDescription", "Doc File");
		burnDocFile->set("fileLocation", "local");
		burnDocFile->set("filetype", "file");
		pParams->set("DocFile", burnDocFile);

		JSON::Array burnFileArray;

		Object::Ptr	burnPlayer = new Object(true);
		burnPlayer->set("fileLocation", "local");
		burnPlayer->set("burnSrcFilePath", "/mnt/HD0/burnTestFile/player.exe");
		burnPlayer->set("burnDstFilePath", "player.exe");
		burnPlayer->set("fileDescription", "player");
		burnPlayer->set("fileLocation", "local");
		burnPlayer->set("filetype", "file");
		burnFileArray.add(burnPlayer);

		Object::Ptr	burnCommonFile = new Object(true);
		burnCommonFile->set("fileLocation", "local");
		burnCommonFile->set("burnSrcFilePath", "/mnt/HD0/burnTestFile/1.mp4");
		burnCommonFile->set("burnDstFilePath", "burnFeedbackTestFile.mp4");
		burnCommonFile->set("fileDescription", "burnFeedbackTestFile");
		burnCommonFile->set("fileLocation", "local");
		burnCommonFile->set("filetype", "file");
		burnFileArray.add(burnCommonFile);
		
		pParams->set("burnFileList", burnFileArray);

		pObj->set("params", pParams);
		std::stringstream ss;
		pObj->stringify(ss);
		std::string strRet = ss.str();
		TRACE(L"%s.\n", strRet.c_str());
		return strRet;
	}
	catch (...)
	{
		printf("%s catched\n", __PRETTY_FUNCTION__);
		return "";
	}
}

//刻录状态反馈协议
std::string HttpServerModule::BurnStateFeedback(std::string strIn/*, std::string& strOut*/)
{
	try
	{
		Json::Reader    jsonReader;
		Json::Value     jsonValueIn;

		if (jsonReader.parse(strIn, jsonValueIn))
		{
			std::string sMethod = jsonValueIn["method"].asString();
			Json::Value   jsonValueParams = jsonValueIn["params"];

			std::string strOut = jsonValueParams.toStyledString();
			if (m_pCallbackFun)
			{
				m_pCallbackFun(m_pVoid, strOut, 1);
			}

			Json::Value     jsonValueRoot;
			Json::Value     jsonValue1;
			Json::Value     jsonValue2;
			jsonValue2["retCode"] = Json::Value(0);
			jsonValue2["retMessage"] = Json::Value("ok");
			jsonValue1["method"] = Json::Value(sMethod.c_str());
			jsonValue1["params"] = jsonValue2;
			jsonValueRoot["result"] = jsonValue1;
			string strRet = jsonValueRoot.toStyledString();
			TRACE(L"%s.\n", strRet.c_str());
			return strRet;
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
