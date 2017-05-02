#ifndef HTTPSERVERCHANNEL_INCLUDED
#define HTTPSERVERCHANNEL_INCLUDED
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/stat.h>
#include "microhttpd.h"

using namespace std;


class HttpServerChannel
{
public:
    HttpServerChannel(void);
    ~HttpServerChannel(void);
public:
    int Start(int iPort,MHD_AccessHandlerCallback cbfAccess,void* dh_cls,
             MHD_RequestCompletedCallback cbfComplete);
    int Stop();
private:
	struct MHD_Daemon *m_d;
};

#endif