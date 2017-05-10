#include "stdafx.h"
#include "HttpServerChannel.h"

HttpServerChannel::HttpServerChannel(void)
{
    m_d = NULL;
}

HttpServerChannel::~HttpServerChannel(void)
{
    if (m_d != NULL)
    {
        MHD_stop_daemon(m_d);
        m_d = NULL;
    }
}

int HttpServerChannel::Start(int iPort,MHD_AccessHandlerCallback cbfAccess,void* dh_cls,MHD_RequestCompletedCallback cbfComplete)
{
    m_d = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION,
        iPort,
        NULL, NULL, cbfAccess, dh_cls, 
        MHD_OPTION_CONNECTION_TIMEOUT, 120 /* seconds */,
        MHD_OPTION_NOTIFY_COMPLETED, cbfComplete, NULL,
        MHD_OPTION_END);
    if (m_d == NULL)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

int HttpServerChannel::Stop()
{
    if (m_d != NULL)
    {
        MHD_stop_daemon(m_d);
        m_d = NULL;
    }
    return 0;
}
