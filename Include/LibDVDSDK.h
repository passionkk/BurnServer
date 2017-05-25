/*******************************************************************************
* 名称    : DVDSDK.h 
* 版权所有: passion
* 作    者: passion
* 设计日期: 2017.3.31
* 版本号:	v1.0.0
* $Id: 
* SDK功能说明:

  * 光驱刻录驱动接口
  * 支持普通DVD光驱和蓝光光驱
    1 最大支持4个光驱
    2 支持实时刻录
    3 支持光盘到光盘拷贝(光盘复制)
    4 支持文件到光盘拷贝
    5 支持光盘恢复(原盘恢复和新盘恢复)
    6 支持数据写入暂停、恢复
    7 支持实时状态查询: 剩余空间
    8 支持DVD+R,DVD-R,DVD-RW, BD-R, BD-RW碟片
	9 支持保留数据(存放在文件系统轨道空余扇区,大小:64K), 在文件系统中不可见，用于存放光盘序列号(UUID)等信息
   10 支持光盘剩余空间填充(文件系统中不可见，只是填充剩余轨道扇区)
	
  * 设备编号说明:
    使用驱动自动编号, 如1号光驱为 /dev/sr0, 2号光驱为 /dev/sr1
  
  * 接口使用流程
   1: DVDSDK_Load("/dev/sr0"); 加载光驱
   2: DVDSDK_GetDevInfo()      获取光驱信息
    .......(用户的实时刻录流程、文件复制流程)

   3: DVDSDK_UnLoad();	卸载光驱
   
  * 用户-实时刻录流程
   0:  DVDSDK_GetTrayState();           获取托盘状态，如果托盘是打开的需要关闭，否则跳到3执行
   1:  DVDSDK_Tray(FALSE)				关闭托盘
   2:  DVDSDK_HaveDisc()				判断是否有光盘
   3:  DVDSDK_LoadDisc();               有光盘则加载光盘
   4:  DVDSDK_GetDiscInfo				查询光盘信息
   5:  DVDSDK_SetWriteSpeed				设置光驱写倍速
   6:  DVDSDK_LockDoor					锁门
   7:  DVDSDK_FormatDisc				格式化光盘
   8:  DVDSDK_CreateDir					创建目录
   9:  DVDSDK_CreateFile				创建文件
   10:  DVDSDK_fillEmptyDataOnFirst		开始启动刻录
   11:  DVDSDK_SetFileLoca				设定刻录文件位置
   12:  DVDSDK_WriteData				刻录数据
   13:    ......(刻录过程必须连续写入数据，流量可以变化)
   14:  DVDSDK_CloseFile				关闭文件
   15: DVDSDK_GetUUID					获得UUID
   16: DVDSDK_SetRecvTrackData			写入UUID等其他隐藏数据
   17: DVDSDK_fillAllDiscEmptyData		填充剩余光盘空间，封盘前调用
   18: DVDSDK_CloseDisc					封盘，封闭后不能再次写入
   19: DVDSDK_Tray(TRUE)				弹出托盘
      
  * 用户-文件复制到光盘流程
   1:  DVDSDK_Tray(FALSE)				关闭托盘
   2:  DVDSDK_HaveDisc()				判断是否有光盘
   3:  DVDSDK_LoadDisc();               有光盘则加载光盘
   4:  DVDSDK_Tray(FALSE)				关闭托盘
   5:  DVDSDK_GetDiscInfo				查询光盘信息
   6:  DVDSDK_GetTotalWriteSize			获得可写大小，判断是否小于需要写入文件的大小
   7:  DVDSDK_SetWriteSpeed				设置光驱写倍速
   8:  DVDSDK_LockDoor					锁门
   9:  DVDSDK_FormatDisc				格式化光盘
       while(n) {
	    DVDSDK_CopyFile					拷贝文件
	   }
   10:  DVDSDK_fillAllDiscEmptyData		填充剩余光盘空间，封盘前调用
   11:  DVDSDK_CloseDisc					封盘，封闭后不能再次写入
   12:  DVDSDK_Tray(TRUE)				弹出托盘
   13:  DVDSDK_LockDoor					解锁
   
* 修改:
*******************************************************************************/

#ifndef __DVDSDK_H__
#define __DVDSDK_H__

#ifdef __cplusplus
extern "C" {
#endif  //__cplusplus

#ifndef BOOL
    #define BOOL	int
#endif//BOOL

#ifndef TRUE
    #define TRUE	1
#endif//TRUE

#ifndef FALSE
    #define FALSE	0
#endif//FALSE

// 光驱类型
typedef enum{
	DVDDRIVER_UNKNOWN = 0,	// 未知光驱类型
	DVDDRIVER_PRVIDVD,		// 专用刻录DVD光盘
	DVDDRIVER_CDR,			// CD只读光驱
	DVDDRIVER_CDRW,			// CD读写光驱
	DVDDRIVER_DVDR,			// DVD只读光驱
	DVDDRIVER_DVDRW,		// DVD读写光驱
	DVDDRIVER_BLUER,		// 蓝光只读光驱
	DVDDRIVER_BLUERW,		// 蓝光读写光驱
}DVDDRIVER_TYPE;

// 光盘类型
typedef enum{
	DISC_UNKNOWN = 0,	// 未知类型
	DISC_CD_ROM,
	DISC_CD_R,
	DISC_CD_RW,
	DISC_DVD_ROM,
	DISC_DVD_R,          //5
	DISC_DVD_RAM,
	DISC_DVD_RW,
	DISC_DVD_RW_SEQ,
	DISC_DVD_RW_DL,
	DISC_DVD_R_DL_SEQ,    //10
	DISC_DVD_R_DL_LJ,       
	DISC_DVD_R_DL,       
	DISC_DVD_PLUS_R,
	DISC_DVD_PLUS_RW,
	DISC_DVD_PLUS_RW_DL, //15
	DISC_DVD_PLUS_R_DL,
	DISC_BD_ROM,
	DISC_BD_R_SEQ,
	DISC_BD_R,
	DISC_BD_RE,			//20
	DISC_HDVD_ROM,       
	DISC_HDVD_R,
	DISC_HDVD_RAM,
}DVDDISC_TYPE;

//错误码
#define _ERRC(X)   (0x1000 + X)

//DVD_SDK返回错误码
#define ERROR_DVD_OK				0				//成功
#define ERROR_DVD_NODEV				_ERRC(300)		// 设备不存在
#define ERROR_DVD_ERRDEVNO			_ERRC(301)		// 错误的设备号
#define ERROR_DVD_BUSY				_ERRC(302)		// 光驱正忙
#define ERROR_DVD_OPTERFAILED		_ERRC(303)		// 光驱操作失败
#define ERROR_DVD_NODISC			_ERRC(304)		// 光驱无CD
#define ERROR_DVD_UNKNOWNDISCTYPE	_ERRC(305)		// 未知的CD类型(不支持该光盘类型)
#define ERROR_DVD_UNBLANKDISC		_ERRC(306)		// 不是空白盘
#define ERROR_DVD_RESERVETRACKERR	_ERRC(307)		// 保留轨道错误
#define ERROR_DVD_CDNOFILES			_ERRC(308)		// 没有找到文件
#define ERROR_DVD_FORMATFAILED		_ERRC(309)		// 格式化光盘失败
#define ERROR_DVD_OPENFILEERROR		_ERRC(310)		// 打开文件失败
#define ERROR_DVD_WRITEERROR		_ERRC(311)		// 写盘发生错误
#define ERROR_DVD_DISCNOFREESIZE	_ERRC(312)		// 无可用空间
#define ERROR_DVD_LOADDISCFAILED	_ERRC(313)		// 加载失败
#define ERROR_DVD_NAMEEMPTY			_ERRC(314)		// 名称为空
#define ERROR_DVD_NAMEEXIST			_ERRC(315)		// 名称已经存在
#define ERROR_DVD_DISCDIFFTYPE		_ERRC(316)		// 光盘类型不同
#define ERROR_DVD_CANTRESUMEBLANK	_ERRC(317)		// 光盘为空盘
#define ERROR_DVD_CANTRESUMEDISC    _ERRC(318)		// 不能恢复的光盘
#define ERROR_DVD_CANTCOPYDISC      _ERRC(319)		// 不能光盘容量不同不能复制
#define ERROR_DVD_SRCBLANKDISC		_ERRC(320)		// 源盘为空盘


typedef void *DVDSDK_DIR;		// 目录节点指针
typedef void *DVDSDK_FILE;		// 文件指针
typedef void *DVDDRV_HANDLE;			// DVD驱动句柄

// 光盘信息
typedef struct{
	int 	 	 ntype;			// 光盘类型	DISC_TYPE
	int			 maxpeed;		// 最大速度(写速度)
	unsigned int discsize;		// 光盘容量(MB)
	unsigned int usedsize;		// 已使用的大小(MB)	
	unsigned int freesize;		// 可用大小(MB)
}DVD_DISC_INFO_T;

// 光驱设备信息
typedef struct{
	char szVender[128];					// 厂家名称
	int  drvtype;						// 光驱类型			DVDDRIVER_TYPE
	int  discsupts;						// 光盘支持总数
	unsigned char disclist[255];		// 光盘里类型列表，见DVDDISC_TYPE
}DVD_DEV_INFO_T;

#include <stdint.h>

class CTest
{
public:
	CTest();
	~CTest();
public:
	void Call();
};

class DVDSDKInterface
{
public:
	DVDSDKInterface();
	~DVDSDKInterface();
public:
	void PrintABC();
	/*******************************************************************************
	* 名称  : DVDSDK_Load
	* 描述  : 加载光驱,
	* 参数  :
	szDevName : 驱动名称，如: /dev/sr0, /dev/sr1
	* 返回值: 设备句柄，NULL为失败
	* 作者  : passion
	* 日期  : 2017.3.31
	*******************************************************************************/
	DVDDRV_HANDLE DVDSDK_Load(const char *szDevName);

	/*******************************************************************************
	* 名称  : DVDSDK_UnLoad
	* 描述  : 卸载光驱
	* 参数  :
	hDVD : DVDSDK_Load的返回值
	* 返回值: 0: 成功，其他为错误值
	* 作者  : passion
	* 日期  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_UnLoad(DVDDRV_HANDLE hDVD);

	/*******************************************************************************
	* 名称  : DVDSDK_Tray
	* 描述  : 打开/关闭托盘
	* 参数  :
	hDVD : DVDSDK_Load的返回值
	bOpen  : TRUE:打开托盘, FALSE:关闭托盘
	* 返回值: 0: 成功，其他为错误值
	* 作者  : passion
	* 日期  : 2017.3.31
	*******************************************************************************/
	int	DVDSDK_Tray(DVDDRV_HANDLE hDVD, int bOpen);

	/*******************************************************************************
	* 名称  : DVDSDK_GetTrayState
	* 描述  : 获得托盘状态(打开/关闭)
	* 参数  :
	hDVD : DVDSDK_Load的返回值
	* 返回值: 1：打开，0：关闭，其他为错误代码
	* 作者  : passion
	* 日期  : 2017.3.31
	*******************************************************************************/
	int	DVDSDK_GetTrayState(DVDDRV_HANDLE hDVD);

	/*******************************************************************************
	* 名称  : DVDSDK_LockDoor
	* 描述  : 锁定/解锁光驱门, 防止在刻录过程中意外打开
	* 参数  :
	hDVD : DVDSDK_Load的返回值
	bLocked: TRUE:锁定, FALSE:解锁
	* 返回值: 0:成功，其他为错误值
	* 作者  : passion
	* 日期  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_LockDoor(DVDDRV_HANDLE hDVD, int bLocked);

	/*******************************************************************************
	* 名称  : DVDSDK_GetDevInfo
	* 描述  : 获得光驱信息
	* 参数  :
	hDVD : DVDSDK_Load的返回值
	pDevInfo   : 光驱信息指针
	* 返回值: 0: 成功，其他为错误值
	* 作者  : passion
	* 日期  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_GetDevInfo(DVDDRV_HANDLE hDVD, DVD_DEV_INFO_T *pDevInfo);

	/*******************************************************************************
	* 名称  : DVDSDK_GetDiscInfo
	* 描述  : 获得碟片信息
	* 参数  :
	nDevNo     : 设备号，0-n, -1为全部设备
	pDiscInfo  : 碟片信息结构指针
	* 返回值: 0: 成功，其他为错误值
	* 作者  : passion
	* 日期  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_GetDiscInfo(DVDDRV_HANDLE hDVD, DVD_DISC_INFO_T *pDiscInfo);

	/*******************************************************************************
	* 名称  : DVDSDK_HaveDisc
	* 描述  : 判断是否有光盘
	* 参数  :
	nDevNo     : 设备号，0-n, -1为全部设备
	* 返回值: TRUE: 有，FALSE：无
	* 作者  : passion
	* 日期  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_HaveDisc(DVDDRV_HANDLE hDVD);

	/*******************************************************************************
	* 名称  : DVDSDK_GetMediaExactType
	* 描述  : 获取光盘类型(精确类型)
	* 参数  :
	nDevNo : 设备号，0-n, -1为全部设备
	* 返回值: 光盘类型
	* 作者  : passion
	* 日期  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_GetMediaExactType(DVDDRV_HANDLE hDVD);

	/*******************************************************************************
	* 名称  : DVDSDK_GetMediaBasicType
	* 描述  : 获取光盘类型(基本类型)(用于设定光驱速度和复制光盘时)
	* 参数  :
	nDevNo : 设备号，0-n, -1为全部设备
	* 返回值: 光盘类型
	* 作者  : passion
	* 日期  : 2017.12.29
	*******************************************************************************/
	int DVDSDK_GetMediaBasicType(DVDDRV_HANDLE hDVD);

	/*******************************************************************************
	* 名称  : DVDSDK_SetWriteSpeed
	* 描述  : 设定刻录速度
	* 参数  :
	nDevNo : 设备号，0-n, -1为全部设备
	speed  : 速度，必须是 1 2 4 6 8 12
	disctpye  : 光盘类型   	DVD_DISC = 0 ,DVD_DL_DISC = 1 ,CD_DISC = 2
	* 返回值: 0: 成功，其他为错误值
	* 作者  : passion
	* 日期  : 2017.3.31
	* 修改  : 2017.4.30 Modify by passion add disctype
	*******************************************************************************/
	int DVDSDK_SetWriteSpeed(DVDDRV_HANDLE hDVD, int speed, int disctype);

	/*******************************************************************************
	* 名称  : DVDSDK_SetCopySpeed
	* 描述  : 设定光盘复制的速度
	* 参数  :
	nSrcDevno : 源设备号，0-n
	nDstDevno : 目的设备号，0-n
	srctype  : 光盘类型:B_DVD_DISC = 0,DVD_DL_DISC = 1,DVD_DISC = 2,CD_DISC = 3
	dsttype  : 光盘类型:B_DVD_DISC = 0,DVD_DL_DISC = 1,DVD_DISC = 2,CD_DISC = 3
	* 返回值: 0: 成功，其他为错误值
	* 作者  : passion
	* 日期  : 2017.4.14
	*******************************************************************************/
	int DVDSDK_SetCopySpeed(DVDDRV_HANDLE HDVDSrc, DVDDRV_HANDLE HDVDDst, int srctype, int dsttype);

	/*******************************************************************************
	* 名称  : DVDSDK_LoadDisc
	* 描述  : 加载光盘
	* 参数  :
	nDevNo : 设备号，0-n
	* 返回值: 0: 成功，其他为错误值
	* 作者  : passion
	* 日期  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_LoadDisc(DVDDRV_HANDLE hDVD);

	/*******************************************************************************
	* 名称  : DVDSDK_DiscCanWrite
	* 描述  : 判断光盘是否可写
	* 参数  :
	nDevNo : 设备号，0-n
	* 返回值: 0: 可写，其他为错误值
	* 作者  : passion
	* 日期  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_DiscCanWrite(DVDDRV_HANDLE hDVD);

	/*******************************************************************************
	* 名称  : DVDSDK_FormatDisc
	* 描述  : 格式化光盘, 分轨道, 创建UDF文件系统, 准备开始写数据; 目前只支持一个分区，2个轨道
	* 参数  :
	nDevNo : 设备号，0-n
	szDiscName: 光盘名称
	* 返回值: 0: 成功，其他为错误值
	* 作者  : passion
	* 日期  : 2017.3.31
	*******************************************************************************/
	int	DVDSDK_FormatDisc(DVDDRV_HANDLE hDVD, char *szDiscName);

	/*******************************************************************************
	* 名称  : DVDSDK_SetFileLoca
	* 描述  : 设定文件位置 (写文件前都需要调用这个函数)
	* 参数  :
	nDevNo : 设备号，0-n
	FileNode : 文件节点
	* 返回值: 0: 成功，其他为错误值
	* 作者  : passion
	* 日期  : 2017.4.17
	*******************************************************************************/
	int DVDSDK_SetFileLoca(DVDDRV_HANDLE hDVD, DVDSDK_FILE FileNode);

	/*******************************************************************************
	* 名称  : DVDSDK_fillEmptyDataOnFirst
	* 描述  : 填充空数据，在格式化之后，开始刻录之前调用，避免开始刻录时写轨道数据的停滞，
	造成视频丢帧  (第一个文件前调用，之后的文件调用DVDSDK_SetFileLoca)
	* 参数  :
	nDevNo : 设备号，0-n
	fillsize: 填充大小，0为自动计算填充大小
	* 返回值: 0: 成功，其他为错误值
	* 作者  : passion
	* 日期  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_FillEmptyDataOnFirst(DVDDRV_HANDLE hDVD, unsigned int fillsize);

	/*******************************************************************************
	* 名称  : DVDSDK_CreateDir
	* 描述  : 创建目录
	* 参数  :
	nDevNo : 设备号，0-n
	szDirName : 目录名称，不能为空
	* 返回值: 目录节点指针, DVDSDK_CreateFile函数会用到, NULL:创建目录失败
	* 作者  : passion
	* 日期  : 2017.3.31
	* 修改  : 2017.4.21 passion
	参数:	nDevNo : 设备号，0-n
	szDirName : 要创建的目录,方式为"/root/test1dir/test2dir"
	*******************************************************************************/
	DVDSDK_DIR DVDSDK_CreateDir(DVDDRV_HANDLE hDVD, char *szDirName);

	/*******************************************************************************
	* 名称  : DVDSDK_CreateFile
	* 描述  : 创建文件，开始写数据
	* 参数  :
	nDevNo : 设备号，0-n
	pDir   : 目录节点指针, NULL为根目录, DVDSDK_CreateDir的返回值
	szFileName: 文件名称
	filesize: 默认0
	* 返回值: 文件节点指针，NULL：创建失败
	* 作者  : passion
	* 日期  : 2017.3.31
	*******************************************************************************/
	DVDSDK_FILE DVDSDK_CreateFile(DVDDRV_HANDLE hDVD, DVDSDK_DIR pDir, char *szFileName, uint64_t filesize);

	/*******************************************************************************
	* 名称  : DVDSDK_WriteData
	* 描述  : 向文件中写数据, size = 32 * 1024
	* 参数  :
	nDevNo : 设备号，0-n
	pFile  : 文件节点指针，DVDSDK_CreateFile的返回值
	pBuffer: 数据buffer
	size   : 数据大小，必须是 32*1024 的整数倍
	* 返回值: 0: 成功，其他为错误值
	* 作者  : passion
	* 日期  : 2017.3.31
	*******************************************************************************/
	int	DVDSDK_WriteData(DVDDRV_HANDLE hDVD, DVDSDK_FILE pFile, unsigned char *pBuffer, int size);

	/*******************************************************************************
	* 名称  : DVDSDK_CloseFile
	* 描述  : 关闭文件
	* 参数  :
	nDevNo : 设备号，0-n
	pFile  : 文件节点指针，DVDSDK_CreateFile的返回值
	* 返回值: 0: 成功，其他为错误值
	* 作者  : passion
	* 日期  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_CloseFile(DVDDRV_HANDLE hDVD, DVDSDK_FILE pFile);

	/*******************************************************************************
	* 名称  : DVDSDK_fillAllDiscEmptyData
	* 描述  : 填充光盘所有剩余空间(在刻录停止之后，封盘之前调用)
	* 参数  :
	nDevNo : 设备号，0-n
	* 返回值: 0: 成功，其他为错误值
	* 作者  : passion
	* 日期  : 2017.3.31
	* 修改  : 2017.4.17 modify by passion for FUNC
	*******************************************************************************/
	int DVDSDK_FillAllDiscEmptyData(DVDDRV_HANDLE hDVD);

	/*******************************************************************************
	* 名称  : DVDSDK_CloseDisc
	* 描述  : 封闭刻录轨道; 调用该接口后，光盘将不可写;
	* 参数  :
	nDevNo : 设备号，0-n
	* 返回值: 0: 成功，其他为错误值
	* 作者  : passion
	* 日期  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_CloseDisc(DVDDRV_HANDLE hDVD);

	/*******************************************************************************
	* 名称  : DVDSDK_CopyDisc
	* 描述  : 光盘复制,直接轨道拷贝，但是保留轨道不复制
	* 参数  :
	nSrcDevno : 源设备号，0-n
	nDstDevno : 目的设备号，0-n
	* 返回值: 0: 成功，其他为错误值
	* 作者  : passion
	* 日期  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_CopyDisc(DVDDRV_HANDLE HDVDSrc, DVDDRV_HANDLE HDVDDst);

	/*******************************************************************************
	* 名称  : DVDSDK_ResumeDisc
	* 描述  : 光盘恢复,如果在刻录过程中断电，调用该函数，恢复文件系统
	* 参数  :
	nDevNo : 设备号，0-n
	DiscName : 光盘名称
	DirName : 目录名称
	FileName : 文件名称
	FillSize : 填充的数据数
	* 返回值: 0: 成功，其他为错误值
	* 作者  : passion
	* 日期  : 2017.3.31
	* 修改  : 2017.4.30 modify by passion for Complete Func
	*******************************************************************************/
	int DVDSDK_ResumeDisc(DVDDRV_HANDLE hDVD, char *DiscName, char *DirName, char *FileName, int FillSize);

	/*******************************************************************************
	* 名称  : DVDSDK_GetReserveData
	* 描述  : 获得保留轨道数据, 对于已经封盘的光盘才有效, 读取时调用
	* 参数  :
	nDevNo : 设备号，0-n
	pBuffer: 返回保留数据指针
	pSize  : 返回Buffer长度
	* 返回值: 0: 成功，其他为错误值
	* 作者  : passion
	* 日期  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_GetReserveData(DVDDRV_HANDLE hDVD, unsigned char **pBuffer, int *pSize);

	/*******************************************************************************
	* 名称  : DVDSDK_GetReserveBuffer
	* 描述  : 获得保留轨道数据指针, 这个函数在保留轨道前调用，直接修改Buffer，在
	封盘时写入该数据,大小 32K， 刻录时调用
	* 参数  :
	nDevNo : 设备号，0-n
	pBuffer: 返回保留数据指针
	pSize  : 返回Buffer长度
	* 返回值: 0: 成功，其他为错误值
	* 作者  : passion
	* 日期  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_GetReserveBuffer(DVDDRV_HANDLE hDVD, unsigned char **pBuffer, int *pSize);

	/*******************************************************************************
	* 名称  : DVDSDK_GetTotalWriteSize
	* 描述  : 获得光盘整个可写空间
	* 参数  :
	nDevNo    : 设备号，0-n
	pTotalSize: 返回整个可写空间
	* 返回值: 0: 成功，其他为错误值
	* 作者  : passion
	* 日期  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_GetTotalWriteSize(DVDDRV_HANDLE hDVD, unsigned long long *pTotalSize);

	/*******************************************************************************
	* 名称  : DVDSDK_GetFreeWriteSize
	* 描述  : 获得剩余可写空间
	* 参数  :
	nDevNo   : 设备号，0-n
	pFreeSize: 返回剩余可写空间
	* 返回值: 0: 成功，其他为错误值
	* 作者  : passion
	* 日期  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_GetFreeWriteSize(DVDDRV_HANDLE hDVD, unsigned long long *pFreeSize);

	/*******************************************************************************
	* 名称  : DVDSDK_PrintProfile
	* 描述  : 打印profile
	* 参数  :
	nDevNo   : 设备号，0-n
	* 返回值: 0: 成功
	* 作者  : passion
	* 日期  : 2017.4.14
	*******************************************************************************/
	int DVDSDK_PrintProfile(DVDDRV_HANDLE hDVD);

	/*******************************************************************************
	* 名称  : DVDSDK_SetRecordStatus
	* 描述  : 设置光驱刻录状态（用于光盘格式化后刻录失败未封盘，再次格式化前设置刻录状态）

	* 参数  :
	hDVD   : DVD驱动句柄
	* 返回值:
	* 作者  : passion
	* 日期  : 2017.4.5
	*******************************************************************************/
	void DVDSDK_SetRecordStatus(DVDDRV_HANDLE hDVD, BOOL bRecordStatus);

};

#ifdef __cplusplus
}
#endif  //__cplusplus

#endif//__DVDSDK_H__
