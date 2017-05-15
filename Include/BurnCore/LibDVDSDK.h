/*******************************************************************************
* ����    : DVDSDK.h 
* ��Ȩ����: passion
* ��    ��: passion
* �������: 2017.3.31
* �汾��:	v1.0.0
* $Id: 
* SDK����˵��:

  * ������¼�����ӿ�
  * ֧����ͨDVD�������������
    1 ���֧��4������
    2 ֧��ʵʱ��¼
    3 ֧�ֹ��̵����̿���(���̸���)
    4 ֧���ļ������̿���
    5 ֧�ֹ��ָ̻�(ԭ�ָ̻������ָ̻�)
    6 ֧������д����ͣ���ָ�
    7 ֧��ʵʱ״̬��ѯ: ʣ��ռ�
    8 ֧��DVD+R,DVD-R,DVD-RW, BD-R, BD-RW��Ƭ
	9 ֧�ֱ�������(������ļ�ϵͳ�����������,��С:64K), ���ļ�ϵͳ�в��ɼ������ڴ�Ź������к�(UUID)����Ϣ
   10 ֧�ֹ���ʣ��ռ����(�ļ�ϵͳ�в��ɼ���ֻ�����ʣ��������)
	
  * �豸���˵��:
    ʹ�������Զ����, ��1�Ź���Ϊ /dev/sr0, 2�Ź���Ϊ /dev/sr1
  
  * �ӿ�ʹ������
   1: DVDSDK_Load("/dev/sr0"); ���ع���
   2: DVDSDK_GetDevInfo()      ��ȡ������Ϣ
    .......(�û���ʵʱ��¼���̡��ļ���������)

   3: DVDSDK_UnLoad();	ж�ع���
   
  * �û�-ʵʱ��¼����
   0:  DVDSDK_GetTrayState();           ��ȡ����״̬����������Ǵ򿪵���Ҫ�رգ���������3ִ��
   1:  DVDSDK_Tray(FALSE)				�ر�����
   2:  DVDSDK_HaveDisc()				�ж��Ƿ��й���
   3:  DVDSDK_LoadDisc();               �й�������ع���
   4:  DVDSDK_GetDiscInfo				��ѯ������Ϣ
   5:  DVDSDK_SetWriteSpeed				���ù���д����
   6:  DVDSDK_LockDoor					����
   7:  DVDSDK_FormatDisc				��ʽ������
   8:  DVDSDK_CreateDir					����Ŀ¼
   9:  DVDSDK_CreateFile				�����ļ�
   10:  DVDSDK_fillEmptyDataOnFirst		��ʼ������¼
   11:  DVDSDK_SetFileLoca				�趨��¼�ļ�λ��
   12:  DVDSDK_WriteData				��¼����
   13:    ......(��¼���̱�������д�����ݣ��������Ա仯)
   14:  DVDSDK_CloseFile				�ر��ļ�
   15: DVDSDK_GetUUID					���UUID
   16: DVDSDK_SetRecvTrackData			д��UUID��������������
   17: DVDSDK_fillAllDiscEmptyData		���ʣ����̿ռ䣬����ǰ����
   18: DVDSDK_CloseDisc					���̣���պ����ٴ�д��
   19: DVDSDK_Tray(TRUE)				��������
      
  * �û�-�ļ����Ƶ���������
   1:  DVDSDK_Tray(FALSE)				�ر�����
   2:  DVDSDK_HaveDisc()				�ж��Ƿ��й���
   3:  DVDSDK_LoadDisc();               �й�������ع���
   4:  DVDSDK_Tray(FALSE)				�ر�����
   5:  DVDSDK_GetDiscInfo				��ѯ������Ϣ
   6:  DVDSDK_GetTotalWriteSize			��ÿ�д��С���ж��Ƿ�С����Ҫд���ļ��Ĵ�С
   7:  DVDSDK_SetWriteSpeed				���ù���д����
   8:  DVDSDK_LockDoor					����
   9:  DVDSDK_FormatDisc				��ʽ������
       while(n) {
	    DVDSDK_CopyFile					�����ļ�
	   }
   10:  DVDSDK_fillAllDiscEmptyData		���ʣ����̿ռ䣬����ǰ����
   11:  DVDSDK_CloseDisc					���̣���պ����ٴ�д��
   12:  DVDSDK_Tray(TRUE)				��������
   13:  DVDSDK_LockDoor					����
   
* �޸�:
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

// ��������
typedef enum{
	DVDDRIVER_UNKNOWN = 0,	// δ֪��������
	DVDDRIVER_PRVIDVD,		// ר�ÿ�¼DVD����
	DVDDRIVER_CDR,			// CDֻ������
	DVDDRIVER_CDRW,			// CD��д����
	DVDDRIVER_DVDR,			// DVDֻ������
	DVDDRIVER_DVDRW,		// DVD��д����
	DVDDRIVER_BLUER,		// ����ֻ������
	DVDDRIVER_BLUERW,		// �����д����
}DVDDRIVER_TYPE;

// ��������
typedef enum{
	DISC_UNKNOWN = 0,	// δ֪����
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

//������
#define _ERRC(X)   (0x1000 + X)

//DVD_SDK���ش�����
#define ERROR_DVD_OK				0				//�ɹ�
#define ERROR_DVD_NODEV				_ERRC(300)		// �豸������
#define ERROR_DVD_ERRDEVNO			_ERRC(301)		// ������豸��
#define ERROR_DVD_BUSY				_ERRC(302)		// ������æ
#define ERROR_DVD_OPTERFAILED		_ERRC(303)		// ��������ʧ��
#define ERROR_DVD_NODISC			_ERRC(304)		// ������CD
#define ERROR_DVD_UNKNOWNDISCTYPE	_ERRC(305)		// δ֪��CD����(��֧�ָù�������)
#define ERROR_DVD_UNBLANKDISC		_ERRC(306)		// ���ǿհ���
#define ERROR_DVD_RESERVETRACKERR	_ERRC(307)		// �����������
#define ERROR_DVD_CDNOFILES			_ERRC(308)		// û���ҵ��ļ�
#define ERROR_DVD_FORMATFAILED		_ERRC(309)		// ��ʽ������ʧ��
#define ERROR_DVD_OPENFILEERROR		_ERRC(310)		// ���ļ�ʧ��
#define ERROR_DVD_WRITEERROR		_ERRC(311)		// д�̷�������
#define ERROR_DVD_DISCNOFREESIZE	_ERRC(312)		// �޿��ÿռ�
#define ERROR_DVD_LOADDISCFAILED	_ERRC(313)		// ����ʧ��
#define ERROR_DVD_NAMEEMPTY			_ERRC(314)		// ����Ϊ��
#define ERROR_DVD_NAMEEXIST			_ERRC(315)		// �����Ѿ�����
#define ERROR_DVD_DISCDIFFTYPE		_ERRC(316)		// �������Ͳ�ͬ
#define ERROR_DVD_CANTRESUMEBLANK	_ERRC(317)		// ����Ϊ����
#define ERROR_DVD_CANTRESUMEDISC    _ERRC(318)		// ���ָܻ��Ĺ���
#define ERROR_DVD_CANTCOPYDISC      _ERRC(319)		// ���ܹ���������ͬ���ܸ���
#define ERROR_DVD_SRCBLANKDISC		_ERRC(320)		// Դ��Ϊ����


typedef void *DVDSDK_DIR;		// Ŀ¼�ڵ�ָ��
typedef void *DVDSDK_FILE;		// �ļ�ָ��
typedef void *DVDDRV_HANDLE;			// DVD�������

// ������Ϣ
typedef struct{
	int 	 	 ntype;			// ��������	DISC_TYPE
	int			 maxpeed;		// ����ٶ�(д�ٶ�)
	unsigned int discsize;		// ��������(MB)
	unsigned int usedsize;		// ��ʹ�õĴ�С(MB)	
	unsigned int freesize;		// ���ô�С(MB)
}DVD_DISC_INFO_T;

// �����豸��Ϣ
typedef struct{
	char szVender[128];					// ��������
	int  drvtype;						// ��������			DVDDRIVER_TYPE
	int  discsupts;						// ����֧������
	unsigned char disclist[255];		// �����������б���DVDDISC_TYPE
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
	* ����  : DVDSDK_Load
	* ����  : ���ع���,
	* ����  :
	szDevName : �������ƣ���: /dev/sr0, /dev/sr1
	* ����ֵ: �豸�����NULLΪʧ��
	* ����  : passion
	* ����  : 2017.3.31
	*******************************************************************************/
	DVDDRV_HANDLE DVDSDK_Load(const char *szDevName);

	/*******************************************************************************
	* ����  : DVDSDK_UnLoad
	* ����  : ж�ع���
	* ����  :
	hDVD : DVDSDK_Load�ķ���ֵ
	* ����ֵ: 0: �ɹ�������Ϊ����ֵ
	* ����  : passion
	* ����  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_UnLoad(DVDDRV_HANDLE hDVD);

	/*******************************************************************************
	* ����  : DVDSDK_Tray
	* ����  : ��/�ر�����
	* ����  :
	hDVD : DVDSDK_Load�ķ���ֵ
	bOpen  : TRUE:������, FALSE:�ر�����
	* ����ֵ: 0: �ɹ�������Ϊ����ֵ
	* ����  : passion
	* ����  : 2017.3.31
	*******************************************************************************/
	int	DVDSDK_Tray(DVDDRV_HANDLE hDVD, int bOpen);

	/*******************************************************************************
	* ����  : DVDSDK_GetTrayState
	* ����  : �������״̬(��/�ر�)
	* ����  :
	hDVD : DVDSDK_Load�ķ���ֵ
	* ����ֵ: 1���򿪣�0���رգ�����Ϊ�������
	* ����  : passion
	* ����  : 2017.3.31
	*******************************************************************************/
	int	DVDSDK_GetTrayState(DVDDRV_HANDLE hDVD);

	/*******************************************************************************
	* ����  : DVDSDK_LockDoor
	* ����  : ����/����������, ��ֹ�ڿ�¼�����������
	* ����  :
	hDVD : DVDSDK_Load�ķ���ֵ
	bLocked: TRUE:����, FALSE:����
	* ����ֵ: 0:�ɹ�������Ϊ����ֵ
	* ����  : passion
	* ����  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_LockDoor(DVDDRV_HANDLE hDVD, int bLocked);

	/*******************************************************************************
	* ����  : DVDSDK_GetDevInfo
	* ����  : ��ù�����Ϣ
	* ����  :
	hDVD : DVDSDK_Load�ķ���ֵ
	pDevInfo   : ������Ϣָ��
	* ����ֵ: 0: �ɹ�������Ϊ����ֵ
	* ����  : passion
	* ����  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_GetDevInfo(DVDDRV_HANDLE hDVD, DVD_DEV_INFO_T *pDevInfo);

	/*******************************************************************************
	* ����  : DVDSDK_GetDiscInfo
	* ����  : ��õ�Ƭ��Ϣ
	* ����  :
	nDevNo     : �豸�ţ�0-n, -1Ϊȫ���豸
	pDiscInfo  : ��Ƭ��Ϣ�ṹָ��
	* ����ֵ: 0: �ɹ�������Ϊ����ֵ
	* ����  : passion
	* ����  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_GetDiscInfo(DVDDRV_HANDLE hDVD, DVD_DISC_INFO_T *pDiscInfo);

	/*******************************************************************************
	* ����  : DVDSDK_HaveDisc
	* ����  : �ж��Ƿ��й���
	* ����  :
	nDevNo     : �豸�ţ�0-n, -1Ϊȫ���豸
	* ����ֵ: TRUE: �У�FALSE����
	* ����  : passion
	* ����  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_HaveDisc(DVDDRV_HANDLE hDVD);

	/*******************************************************************************
	* ����  : DVDSDK_GetMediaExactType
	* ����  : ��ȡ��������(��ȷ����)
	* ����  :
	nDevNo : �豸�ţ�0-n, -1Ϊȫ���豸
	* ����ֵ: ��������
	* ����  : passion
	* ����  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_GetMediaExactType(DVDDRV_HANDLE hDVD);

	/*******************************************************************************
	* ����  : DVDSDK_GetMediaBasicType
	* ����  : ��ȡ��������(��������)(�����趨�����ٶȺ͸��ƹ���ʱ)
	* ����  :
	nDevNo : �豸�ţ�0-n, -1Ϊȫ���豸
	* ����ֵ: ��������
	* ����  : passion
	* ����  : 2017.12.29
	*******************************************************************************/
	int DVDSDK_GetMediaBasicType(DVDDRV_HANDLE hDVD);

	/*******************************************************************************
	* ����  : DVDSDK_SetWriteSpeed
	* ����  : �趨��¼�ٶ�
	* ����  :
	nDevNo : �豸�ţ�0-n, -1Ϊȫ���豸
	speed  : �ٶȣ������� 1 2 4 6 8 12
	disctpye  : ��������   	DVD_DISC = 0 ,DVD_DL_DISC = 1 ,CD_DISC = 2
	* ����ֵ: 0: �ɹ�������Ϊ����ֵ
	* ����  : passion
	* ����  : 2017.3.31
	* �޸�  : 2017.4.30 Modify by passion add disctype
	*******************************************************************************/
	int DVDSDK_SetWriteSpeed(DVDDRV_HANDLE hDVD, int speed, int disctype);

	/*******************************************************************************
	* ����  : DVDSDK_SetCopySpeed
	* ����  : �趨���̸��Ƶ��ٶ�
	* ����  :
	nSrcDevno : Դ�豸�ţ�0-n
	nDstDevno : Ŀ���豸�ţ�0-n
	srctype  : ��������:B_DVD_DISC = 0,DVD_DL_DISC = 1,DVD_DISC = 2,CD_DISC = 3
	dsttype  : ��������:B_DVD_DISC = 0,DVD_DL_DISC = 1,DVD_DISC = 2,CD_DISC = 3
	* ����ֵ: 0: �ɹ�������Ϊ����ֵ
	* ����  : passion
	* ����  : 2017.4.14
	*******************************************************************************/
	int DVDSDK_SetCopySpeed(DVDDRV_HANDLE HDVDSrc, DVDDRV_HANDLE HDVDDst, int srctype, int dsttype);

	/*******************************************************************************
	* ����  : DVDSDK_LoadDisc
	* ����  : ���ع���
	* ����  :
	nDevNo : �豸�ţ�0-n
	* ����ֵ: 0: �ɹ�������Ϊ����ֵ
	* ����  : passion
	* ����  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_LoadDisc(DVDDRV_HANDLE hDVD);

	/*******************************************************************************
	* ����  : DVDSDK_DiscCanWrite
	* ����  : �жϹ����Ƿ��д
	* ����  :
	nDevNo : �豸�ţ�0-n
	* ����ֵ: 0: ��д������Ϊ����ֵ
	* ����  : passion
	* ����  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_DiscCanWrite(DVDDRV_HANDLE hDVD);

	/*******************************************************************************
	* ����  : DVDSDK_FormatDisc
	* ����  : ��ʽ������, �ֹ��, ����UDF�ļ�ϵͳ, ׼����ʼд����; Ŀǰֻ֧��һ��������2�����
	* ����  :
	nDevNo : �豸�ţ�0-n
	szDiscName: ��������
	* ����ֵ: 0: �ɹ�������Ϊ����ֵ
	* ����  : passion
	* ����  : 2017.3.31
	*******************************************************************************/
	int	DVDSDK_FormatDisc(DVDDRV_HANDLE hDVD, char *szDiscName);

	/*******************************************************************************
	* ����  : DVDSDK_SetFileLoca
	* ����  : �趨�ļ�λ�� (д�ļ�ǰ����Ҫ�����������)
	* ����  :
	nDevNo : �豸�ţ�0-n
	FileNode : �ļ��ڵ�
	* ����ֵ: 0: �ɹ�������Ϊ����ֵ
	* ����  : passion
	* ����  : 2017.4.17
	*******************************************************************************/
	int DVDSDK_SetFileLoca(DVDDRV_HANDLE hDVD, DVDSDK_FILE FileNode);

	/*******************************************************************************
	* ����  : DVDSDK_fillEmptyDataOnFirst
	* ����  : �������ݣ��ڸ�ʽ��֮�󣬿�ʼ��¼֮ǰ���ã����⿪ʼ��¼ʱд������ݵ�ͣ�ͣ�
	�����Ƶ��֡  (��һ���ļ�ǰ���ã�֮����ļ�����DVDSDK_SetFileLoca)
	* ����  :
	nDevNo : �豸�ţ�0-n
	fillsize: ����С��0Ϊ�Զ���������С
	* ����ֵ: 0: �ɹ�������Ϊ����ֵ
	* ����  : passion
	* ����  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_FillEmptyDataOnFirst(DVDDRV_HANDLE hDVD, unsigned int fillsize);

	/*******************************************************************************
	* ����  : DVDSDK_CreateDir
	* ����  : ����Ŀ¼
	* ����  :
	nDevNo : �豸�ţ�0-n
	szDirName : Ŀ¼���ƣ�����Ϊ��
	* ����ֵ: Ŀ¼�ڵ�ָ��, DVDSDK_CreateFile�������õ�, NULL:����Ŀ¼ʧ��
	* ����  : passion
	* ����  : 2017.3.31
	* �޸�  : 2017.4.21 passion
	����:	nDevNo : �豸�ţ�0-n
	szDirName : Ҫ������Ŀ¼,��ʽΪ"/root/test1dir/test2dir"
	*******************************************************************************/
	DVDSDK_DIR DVDSDK_CreateDir(DVDDRV_HANDLE hDVD, char *szDirName);

	/*******************************************************************************
	* ����  : DVDSDK_CreateFile
	* ����  : �����ļ�����ʼд����
	* ����  :
	nDevNo : �豸�ţ�0-n
	pDir   : Ŀ¼�ڵ�ָ��, NULLΪ��Ŀ¼, DVDSDK_CreateDir�ķ���ֵ
	szFileName: �ļ�����
	filesize: Ĭ��0
	* ����ֵ: �ļ��ڵ�ָ�룬NULL������ʧ��
	* ����  : passion
	* ����  : 2017.3.31
	*******************************************************************************/
	DVDSDK_FILE DVDSDK_CreateFile(DVDDRV_HANDLE hDVD, DVDSDK_DIR pDir, char *szFileName, uint64_t filesize);

	/*******************************************************************************
	* ����  : DVDSDK_WriteData
	* ����  : ���ļ���д����, size = 32 * 1024
	* ����  :
	nDevNo : �豸�ţ�0-n
	pFile  : �ļ��ڵ�ָ�룬DVDSDK_CreateFile�ķ���ֵ
	pBuffer: ����buffer
	size   : ���ݴ�С�������� 32*1024 ��������
	* ����ֵ: 0: �ɹ�������Ϊ����ֵ
	* ����  : passion
	* ����  : 2017.3.31
	*******************************************************************************/
	int	DVDSDK_WriteData(DVDDRV_HANDLE hDVD, DVDSDK_FILE pFile, unsigned char *pBuffer, int size);

	/*******************************************************************************
	* ����  : DVDSDK_CloseFile
	* ����  : �ر��ļ�
	* ����  :
	nDevNo : �豸�ţ�0-n
	pFile  : �ļ��ڵ�ָ�룬DVDSDK_CreateFile�ķ���ֵ
	* ����ֵ: 0: �ɹ�������Ϊ����ֵ
	* ����  : passion
	* ����  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_CloseFile(DVDDRV_HANDLE hDVD, DVDSDK_FILE pFile);

	/*******************************************************************************
	* ����  : DVDSDK_fillAllDiscEmptyData
	* ����  : ����������ʣ��ռ�(�ڿ�¼ֹ֮ͣ�󣬷���֮ǰ����)
	* ����  :
	nDevNo : �豸�ţ�0-n
	* ����ֵ: 0: �ɹ�������Ϊ����ֵ
	* ����  : passion
	* ����  : 2017.3.31
	* �޸�  : 2017.4.17 modify by passion for FUNC
	*******************************************************************************/
	int DVDSDK_FillAllDiscEmptyData(DVDDRV_HANDLE hDVD);

	/*******************************************************************************
	* ����  : DVDSDK_CloseDisc
	* ����  : ��տ�¼���; ���øýӿں󣬹��̽�����д;
	* ����  :
	nDevNo : �豸�ţ�0-n
	* ����ֵ: 0: �ɹ�������Ϊ����ֵ
	* ����  : passion
	* ����  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_CloseDisc(DVDDRV_HANDLE hDVD);

	/*******************************************************************************
	* ����  : DVDSDK_CopyDisc
	* ����  : ���̸���,ֱ�ӹ�����������Ǳ������������
	* ����  :
	nSrcDevno : Դ�豸�ţ�0-n
	nDstDevno : Ŀ���豸�ţ�0-n
	* ����ֵ: 0: �ɹ�������Ϊ����ֵ
	* ����  : passion
	* ����  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_CopyDisc(DVDDRV_HANDLE HDVDSrc, DVDDRV_HANDLE HDVDDst);

	/*******************************************************************************
	* ����  : DVDSDK_ResumeDisc
	* ����  : ���ָ̻�,����ڿ�¼�����жϵ磬���øú������ָ��ļ�ϵͳ
	* ����  :
	nDevNo : �豸�ţ�0-n
	DiscName : ��������
	DirName : Ŀ¼����
	FileName : �ļ�����
	FillSize : ����������
	* ����ֵ: 0: �ɹ�������Ϊ����ֵ
	* ����  : passion
	* ����  : 2017.3.31
	* �޸�  : 2017.4.30 modify by passion for Complete Func
	*******************************************************************************/
	int DVDSDK_ResumeDisc(DVDDRV_HANDLE hDVD, char *DiscName, char *DirName, char *FileName, int FillSize);

	/*******************************************************************************
	* ����  : DVDSDK_GetReserveData
	* ����  : ��ñ����������, �����Ѿ����̵Ĺ��̲���Ч, ��ȡʱ����
	* ����  :
	nDevNo : �豸�ţ�0-n
	pBuffer: ���ر�������ָ��
	pSize  : ����Buffer����
	* ����ֵ: 0: �ɹ�������Ϊ����ֵ
	* ����  : passion
	* ����  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_GetReserveData(DVDDRV_HANDLE hDVD, unsigned char **pBuffer, int *pSize);

	/*******************************************************************************
	* ����  : DVDSDK_GetReserveBuffer
	* ����  : ��ñ����������ָ��, ��������ڱ������ǰ���ã�ֱ���޸�Buffer����
	����ʱд�������,��С 32K�� ��¼ʱ����
	* ����  :
	nDevNo : �豸�ţ�0-n
	pBuffer: ���ر�������ָ��
	pSize  : ����Buffer����
	* ����ֵ: 0: �ɹ�������Ϊ����ֵ
	* ����  : passion
	* ����  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_GetReserveBuffer(DVDDRV_HANDLE hDVD, unsigned char **pBuffer, int *pSize);

	/*******************************************************************************
	* ����  : DVDSDK_GetTotalWriteSize
	* ����  : ��ù���������д�ռ�
	* ����  :
	nDevNo    : �豸�ţ�0-n
	pTotalSize: ����������д�ռ�
	* ����ֵ: 0: �ɹ�������Ϊ����ֵ
	* ����  : passion
	* ����  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_GetTotalWriteSize(DVDDRV_HANDLE hDVD, unsigned long long *pTotalSize);

	/*******************************************************************************
	* ����  : DVDSDK_GetFreeWriteSize
	* ����  : ���ʣ���д�ռ�
	* ����  :
	nDevNo   : �豸�ţ�0-n
	pFreeSize: ����ʣ���д�ռ�
	* ����ֵ: 0: �ɹ�������Ϊ����ֵ
	* ����  : passion
	* ����  : 2017.3.31
	*******************************************************************************/
	int DVDSDK_GetFreeWriteSize(DVDDRV_HANDLE hDVD, unsigned long long *pFreeSize);

	/*******************************************************************************
	* ����  : DVDSDK_PrintProfile
	* ����  : ��ӡprofile
	* ����  :
	nDevNo   : �豸�ţ�0-n
	* ����ֵ: 0: �ɹ�
	* ����  : passion
	* ����  : 2017.4.14
	*******************************************************************************/
	int DVDSDK_PrintProfile(DVDDRV_HANDLE hDVD);

	/*******************************************************************************
	* ����  : DVDSDK_SetRecordStatus
	* ����  : ���ù�����¼״̬�����ڹ��̸�ʽ�����¼ʧ��δ���̣��ٴθ�ʽ��ǰ���ÿ�¼״̬��

	* ����  :
	hDVD   : DVD�������
	* ����ֵ:
	* ����  : passion
	* ����  : 2017.4.5
	*******************************************************************************/
	void DVDSDK_SetRecordStatus(DVDDRV_HANDLE hDVD, BOOL bRecordStatus);

};

#ifdef __cplusplus
}
#endif  //__cplusplus

#endif//__DVDSDK_H__
