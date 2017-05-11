# Makefile for BurnServer

COMPILER_FLAGS	+= -D_LINUX_ -D_LINUX

COMPILER_FLAGS	+= -D_REENTRANT -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE -D__USE_POSIX

COMPILER_FLAGS	+= -Wall -Wno-switch -Wpointer-arith -Wredundant-decls -march=native -mtune=generic -pipe -ffast-math -fomit-frame-pointer

C++		=	g++
CC		=	gcc
CCFLAGS	+= $(COMPILER_FLAGS)  -g 
#CCFLAGS	+= $(COMPILER_FLAGS)
C++LINK = g++

# EACH DIRECTORY WITH HEADERS MUST BE APPENDED IN THIS MANNER TO THE CCFLAGS

#CCFLAGS += -I../Include
#CCFLAGS += -I../Include/Charset
#CCFLAGS += -I../Include/jsoncpp/json
#CCFLAGS += -I../Include/libcurl
#CCFLAGS += -I../Include/microhttpd
CCFLAGS += -I../Include/poco
#CCFLAGS += -I../Include/poco/Net
CCFLAGS += -I../Include/poco/JSON
CCFLAGS += -I../Include/poco/Dynamic

C++FLAGS	= $(CCFLAGS)

CPPFILES	= ../Source/BurnServer_main.cpp \
#../Source/Business.cpp \
#../Source/CCBUtility.cpp \
#../Source/InterfaceProtocol.cpp \
#../Source/HttpClient.cpp \
#../Source/BurnTask.cpp \
#../Source/HttpServerChannel.cpp \
#../Source/HttpServerModule.cpp \
#../Source/MainConfig.cpp \
#../Source/UDPClient.cpp \
#../Source/UDPServerChannel.cpp \
#../Source/UDPServerModule.cpp 

# DEPEND LIB PATH
LIBFILES = 	\
#../Depends/jsoncpp/libjson_linux-gcc-5.4.0_libmt.so \
#../Depends/libcurl/libcurl.so \
#../Depends/microhttpd/libmicrohttpd.so \
#../Depends/poco/libPocoCrypto.so \
#../Depends/poco/libPocoData.so \
../Depends/poco/libPocoFoundation.so \
../Depends/poco/libPocoJSON.so \
#../Depends/poco/libPocoNet.so \
#../Depends/poco/libPocoNetSSL.so \
#../Depends/poco/libPocoUtil.so \
#../Depends/poco/libPocoXML.so \
#../Depends/poco/libPocoZip.so \


CORE_LINK_LIBS = -lpthread -lrt -ldl -lstdc++ -lm -lcrypt

#LIBS = -ljson_linux-gcc-5.4.0_libmt -lcurl -lmicrohttpd -lPocoCrypto -lPocoData -lPocoFoundation -lPocoJSON -lPocoNet \
#	   -lPocoNetSSL -lPocoUtil -lPocoXML -lPocoZip $(CORE_LINK_LIBS)

LIBS =   $(CORE_LINK_LIBS)

LINKPATH = \
#-L../Depends/jsoncpp/ \
#-L../Depends/libcurl/ \
#-L../Depends/microhttpd/ \
-L../Depends/poco/
	
BurnServer : $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) 
	$(C++LINK) -o BurnServer.out $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) $(C++FLAGS) $(LINKPATH) $(LIBS)

clean:
	rm -f *.out *.log $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)

.cpp.o:
	$(C++) -c -o $*.o $(C++FLAGS) $*.cpp

.c.o:
	$(CC) -c -o $*.o $(CCFLAGS) $*.c
