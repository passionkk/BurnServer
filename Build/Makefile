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

CCFLAGS += -I../Include
CCFLAGS += -I../Include/libcurl
CCFLAGS += -I../Include/microhttpd_linux
CCFLAGS += -I../Include/poco/Net
CCFLAGS += -I../Include/poco/JSON
CCFLAGS += -I../Include/poco/Dynamic
#CCFLAGS += -I../Include/Charset
#CCFLAGS += -I../Include/jsoncpp/json
#CCFLAGS += -I../Include/poco

C++FLAGS	= $(CCFLAGS)

CPPFILES	= ../Source/BurnServer_main.cpp \
../Source/Business.cpp \
../Source/CCBUtility.cpp \
../Source/HttpClient.cpp \
../Source/HttpServerChannel.cpp \
../Source/HttpServerModule.cpp \
../Source/MainConfig.cpp \
../Source/UDPClient.cpp \
../Source/UDPServerChannel.cpp \
../Source/UDPServerModule.cpp 

CORE_LINK_LIBS = -lpthread -lrt -ldl -lstdc++ -lm -lcrypt

#LIBS = -ljson_linux-gcc-5.4.0_libmt -lcurl -lmicrohttpd -lPocoCrypto -lPocoData -lPocoFoundation -lPocoJSON -lPocoNet \
#	   -lPocoNetSSL -lPocoUtil -lPocoXML -lPocoZip $(CORE_LINK_LIBS)

LIBS = -ljson_linux-gcc-5_libmt -lcurl -lmicrohttpd -lPocoJSON -lPocoNet -lPocoFoundation $(CORE_LINK_LIBS)
	   
LINKPATH = \
-L/usr/local/BurnServer/poco/lib/ \
-L/usr/local/BurnServer/curl/lib/ \
-L/usr/local/BurnServer/microhttpd/lib/ \
-L/media/sf_OpenSource/jsoncpp/buildscons/linux-gcc-5/src/lib_json/
#-L../Depends/poco/ \
#-L../Depends/libcurl/ \
#-L../Depends/microhttpd/ \
#-L../Depends/jsoncpp/
	
BurnServer : $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) 
	$(C++LINK) -o BurnServer.out $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) $(C++FLAGS) $(LINKPATH) $(LIBS)

clean:
	rm -f *.out *.log $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)

.cpp.o:
	$(C++) -c -o $*.o $(C++FLAGS) $*.cpp

.c.o:
	$(CC) -c -o $*.o $(CCFLAGS) $*.c
