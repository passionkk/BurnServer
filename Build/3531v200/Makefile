#Makefile for BurnServer
TARGET = BurnServer

COMPILER_FLAGS	+= -DLINUX -D_LINUX_ -D_LINUX -DPOCO_BUILD_HOST=ubuntu -DPOCO_TARGET_OSNAME=Linux -DPOCO_TARGET_OSARCH=ARM \
		           -D_XOPEN_SOURCE=500 -D_BSD_SOURCE -D_REENTRANT -D_THREAD_SAFE -DPOCO_NO_FPENVIRONMENT -DUSE_B0 -DPF3531 \
                   -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE

CC=arm-hisiv200-linux-gcc
AR=arm-hisiv200-linux-ar
LD=arm-hisiv200-linux-ld
C++ = arm-hisiv200-linux-g++
C++LINK = arm-hisiv200-linux-g++
STRIP = arm-hisiv200-linux-strip

CCFLAGS	+= $(COMPILER_FLAGS)  -O2

#HEADERS PATH

CCFLAGS += -I/usr/local/v200/include/stlport
CCFLAGS += -I/usr/local/v200/include
CCFLAGS += -I../../Depends/v200
CCFLAGS += -I../../Depends/v200/DVDSDK
CCFLAGS += -I../Source
CCFLAGS += -I/usr/local/v200/include/curl

C++FLAGS	= $(CCFLAGS)

#SRC FILES
CFILES		=	

CPPFILES	= ../../Source/BurnServer_main.cpp \
../../Source/Business.cpp \
../../Source/CCBUtility.cpp \
../../Source/HttpClient.cpp \
../../Source/HttpServerChannel.cpp \
../../Source/HttpServerModule.cpp \
../../Source/MainConfig.cpp \
../../Source/UDPClient.cpp \
../../Source/UDPServerChannel.cpp \
../../Source/UDPServerModule.cpp \
../../Source/DownloadFile.cpp 		\
../../Source/FileUtil.cpp \
../../Source/DirectoryUtil.cpp \
../../Source/FileLog.cpp \
../../Source/NetLog.cpp \
../../Source/CharsetConvert.cpp \
../../Source/CharsetConvertMFC.cpp \
../../Source/CharsetConvertSTD.cpp \


# DEPEND LIB PATH
LIBS = -lDVDSDK -lmicrohttpd -lcurl -lPocoUtil -lPocoJSON -lPocoNet -lPocoXML -lPocoFoundation -ljsoncppStlport -lstlport -lpthread -ldl -lrt \

LINKPATH = \
-L../../Depends/v200/DVDSDK \
-L/usr/local/v200/lib \
-L/usr/local/v200/arm-hisiv200-linux-lib \

BurnServer :   $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) 
	$(C++LINK) -o $(TARGET) $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) $(C++FLAGS) $(LINKPATH) $(LIBS)
	$(STRIP) $(TARGET)
	cp $(TARGET) /var/local/BurnServer/3531

clean:
	rm -f *.out *.log $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)

.cpp.o:
	$(C++) -c -o $*.o $(C++FLAGS) $*.cpp

.c.o:
	$(CC) -c -o $*.o $(CCFLAGS) $*.c
