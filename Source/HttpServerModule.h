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

    std::string AgentHeartBeat(std::string strIn);

private:
    static HttpServerModule   *m_pInstance;
    std::vector<HttpServerChannel*>  m_vectChannels;
};

#endif