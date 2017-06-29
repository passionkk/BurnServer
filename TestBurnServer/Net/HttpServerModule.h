#ifndef HTTPSERVERMODULE_INCLUDED
#define HTTPSERVERMODULE_INCLUDED
#include "HttpServerChannel.h"
#include "JSON.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <vector>

typedef void(*Fun)(LPVOID lpVoid, std::string, int nProtocol);

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
    std::string ProcessProtocol(std::string sMethod, std::string jsonRecv/*, std::string& strOut*/);

	int ProcessGETRequest(struct MHD_Connection *connection, const char *  url);

	void SetCallback(Fun, LPVOID pVoid);

	static std::string GetContentTypeByUrl(std::string sUrl);
private:

	std::string TestProtocol(std::string strIn);
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

	//����ǰ����
	std::string FeedbackBeforeCloseDisc(std::string strIn/*, std::string& strOut*/);
	//��¼״̬����Э��
	std::string BurnStateFeedback(std::string strIn/*, std::string& strOut*/);

private:
    static HttpServerModule   *m_pInstance;
    std::vector<HttpServerChannel*>  m_vectChannels;
	Fun m_pCallbackFun;
	LPVOID m_pVoid;
};

#endif