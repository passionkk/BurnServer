#ifndef HTTPSERVERMODULE_INCLUDED
#define HTTPSERVERMODULE_INCLUDED
#include "HttpServerChannel.h"
#include "JSON.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <vector>

class HttpServerModule
{
private:
    HttpServerModule(void);
    ~HttpServerModule(void);
public:
    static HttpServerModule *Initialize();
    static void Uninitialize();
    static HttpServerModule *GetInstance();
private:
    void Init();
    void UnInit();
public:
    int ProcessPassiveProtocol(struct MHD_Connection *connection,char *  pszContent);
    std::string ProcessProtocol(std::string sMethod, std::string jsonRecv);

private:

	std::string TestProtocol(std::string strIn);
	std::string TestCallBurnStateFeedbackProtocol(std::string strIn);
	std::string TestCallCloseDiskProtocol(std::string strIn);
	std::string AgentHeartBeat(std::string strIn);
	
	//��ȡ�����б�
	std::string GetCDRomList(std::string strIn);
	//��ʼ��¼
	std::string StartBurn(std::string strIn);
	//��ͣ��¼
	std::string PauseBurn(std::string strIn);
	//������¼
	std::string ResumBurn(std::string strIn);
	//������¼
	std::string StopBurn(std::string strIn);
	//��ȡ������Ϣ
	std::string GetCDRomInfo(std::string strIn);
	//���ӿ�¼�ļ�
	std::string AddBurnFile(std::string strIn);

private:
    static HttpServerModule   *m_pInstance;
    std::vector<HttpServerChannel*>  m_vectChannels;
};

#endif