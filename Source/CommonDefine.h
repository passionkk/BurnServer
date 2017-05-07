#pragma  once

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else  /* __cplusplus */
#define NULL    ((void *)0)
#endif  /* __cplusplus */
#endif  /* NULL */

#ifdef WIN32
#define __PRETTY_FUNCTION__  __FUNCTION__
#endif

#include "Poco/Foundation.h"
#include "Poco/File.h"
#include "poco/StringTokenizer.h"
#include "poco/JSON/Parser.h"
#include "poco/Dynamic/Var.h"
#include "Poco/net/Net.h"
#include "poco/net/DatagramSocket.h"
#include "poco/net/streamsocket.h"
#include "poco/FileStream.h"

using namespace Poco;
using namespace Poco::Dynamic;
using namespace Poco::JSON;
using namespace Poco::Net;

class RetransChannel
{
public:
	RetransChannel()
	{
		m_strType = "";
		m_strIP = "";
		m_iPort = 0;
		m_strSerialName = "";
	}
public:
	std::string m_strType;
	std::string m_strIP;
	int m_iPort;
	std::string m_strSerialName;
};
