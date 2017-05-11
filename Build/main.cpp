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
#include <cstring>
#endif
#include <stdio.h>

int main(int argc, char *argv[])
{
	for (int i = 0; i< 10; i++)
		printf("%d ", i);
	printf("\ninput a char:");
	getchar();
	printf("byebye!\n");
	return 0;
}
