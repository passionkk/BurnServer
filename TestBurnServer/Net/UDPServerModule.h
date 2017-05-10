#ifndef UDPSERVERMODULE_INCLUDED
#define UDPSERVERMODULE_INCLUDED
#include "UDPServerChannel.h"

using Poco::Mutex;

class UDPServerModule
{
private:
    UDPServerModule(void);
    ~UDPServerModule(void);
public:
    static UDPServerModule *Initialize();
    static void Uninitialize();
    static UDPServerModule *GetInstance();
private:
    void Init();
    void UnInit();
private:
    static UDPServerModule   *m_pInstance;
    std::vector<UDPServerChannel*> m_vectChannels;
};

#endif