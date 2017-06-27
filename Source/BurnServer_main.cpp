// BurnServer.cpp : 定义控制台应用程序的入口点。
//
#if defined(_LINUX_)
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h> 
#include <sys/resource.h>
#include <ifaddrs.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <sys/ioctl.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <net/if.h> 
#include <string.h>

#elif defined WIN32
#include <Windows.h>
#pragma comment(linker,"/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#endif	//defined(_LINUX_)

#include "Business.h"

#include <iostream>

#if 0 /*for gdb*/
#if defined(_LINUX_)
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#endif	//defined(_LINUX_)
#endif/*for gdb*/

using namespace std;

#define version ("1.0.0.1")
//#include "Platform.h"
//#include "microhttpd.h"

int RunBurnServer()
{
	printf("start BurnServer!\n");
#if 0 /*gdb*/
	struct	rlimit		rl;

	rl.rlim_cur = 10240;
	rl.rlim_max = 10240;
	setrlimit(RLIMIT_NOFILE, &rl);

	rl.rlim_cur = RLIM_INFINITY;
	rl.rlim_max = RLIM_INFINITY;
	setrlimit(RLIMIT_CORE, &rl);
#endif

	CBusiness::Initialize();
	while (true)
	{
#if defined(_LINUX_)
		sleep(100);
#else
		Sleep(100);
#endif
	}
	CBusiness::Uninitialize();

	return 0;
}

#if defined(_LINUX_)
int RunDaemon()
{
	printf("Enter RunDaemon.\n");
	pid_t	processID = 0;
	int		status = 0;
	int		pid = 0;

	//if (daemon(0, 0) != 0)
	if (daemon(1, 0) != 0)
	{
		exit(-1);
	}

	while (true)
	{
		processID = fork();
		if (processID > 0)
		{
			status = 0;
			while (status == 0)
			{
				pid = ::wait(&status);
				int	exitStatus = (unsigned char)WEXITSTATUS(status);
				if (WIFEXITED(status) && pid > 0 && status != 0)
				{
					if (exitStatus == -1)
					{
						exit(EXIT_FAILURE);
					}
					break;
				}
				if (WIFSIGNALED(status))
				{
					break;
				}
				if (WIFSTOPPED(status))
				{
					break;
				}
				if (pid == -1 && status == 0)
				{
					continue;
				}
				if (pid > 0 && status == 0)
				{
					exit(EXIT_SUCCESS);
				}
				exit(EXIT_FAILURE);
			}
		}
		else if (processID == 0)
		{
			break;
		}
		else
		{
			exit(EXIT_FAILURE);
		}
		sleep(1);
	}

	if (processID != 0)
	{
		exit(EXIT_SUCCESS);
	}
	else
	{
		RunBurnServer();
	}

	return 0;
}

#endif	//

#include "Poco/Poco.h"
#include "Poco/JSON/JSON.h"
#include "Poco/JSON/Parser.h"
#include "Poco/DateTime.h"
using std::string;

int main(int argc, char *argv[])
{
	printf("Enter main.\n");
#if 0 // for test
	Poco::DateTime now;
	cout << "data time is :" << now.day() << endl;
	
	string jsonString = "{ \"name\" : \"罗伯特\" }";
	cout << "jsonString" << jsonString << endl;
	Poco::JSON::Parser	jsonParser;
	Poco::Dynamic::Var result = jsonParser.parse(jsonString);
	Poco::JSON::Object::Ptr pObj = result.extract<Poco::JSON::Object::Ptr>();
	Poco::Dynamic::Var varName = pObj->get("name");
	cout << "name is " << varName.toString() << endl;
#endif 

#ifdef WIN32
#else

	struct	sigaction	act;
	struct	rlimit		rl;

	act.sa_flags = 0;
	act.sa_handler = SIG_IGN;

	(void)::sigaction(SIGPIPE, &act, NULL);
	(void)::sigaction(SIGHUP, &act, NULL);
	//(void)::sigaction(SIGINT, &act, NULL);
	(void)::sigaction(SIGTERM, &act, NULL);
	(void)::sigaction(SIGQUIT, &act, NULL);
	(void)::sigaction(SIGALRM, &act, NULL);

	//rl.rlim_cur = 10240;
	//rl.rlim_max = 10240;
	//setrlimit(RLIMIT_NOFILE, &rl);

	//rl.rlim_cur = RLIM_INFINITY;
	//rl.rlim_max = RLIM_INFINITY;
	//setrlimit(RLIMIT_CORE, &rl);
#endif


#ifdef WIN32

#ifdef _DEBUG
	::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	::AllocConsole();
	freopen("CON", "w", stdout);
#endif
#endif

#ifdef WIN32
	return RunBurnServer();
#else
	int opt;
	bool bDebug = false;
	while ((opt = getopt(argc, argv, "vd")) != -1)
	{
		// 参数列表中，后面有参数的加“:”，后面没有参数的不加“:”
		// 如getopt(argc, argv, "c:i:hv")表示-c和-i后面是跟参数的，-h和-v后面是不跟参数的
		switch (opt)
		{
		case 'v':
			printf("version:%s\n", version);
			exit(0);
		case 'd':
			bDebug = true;
			break;
		default:
			printf("do not support this param\n");
			exit(1);
		}
	}
	if (bDebug)
	{
		return RunBurnServer();
	}
	else
	{
		return RunDaemon();
	}
#endif	
	return 0;
}
