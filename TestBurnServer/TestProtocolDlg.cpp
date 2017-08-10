// TestProtocolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestBurnServer.h"
#include "TestProtocolDlg.h"
#include "afxdialogex.h"
#include "Poco/Glob.h"

// CTestProtocolDlg 对话框

IMPLEMENT_DYNAMIC(CTestProtocolDlg, CDialogEx)

CTestProtocolDlg::CTestProtocolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestProtocolDlg::IDD, pParent)
	, m_strSrcFilePath(_T(""))
	, m_strDestFilePath(_T(""))
	, m_strSessionID(_T(""))
{

}

CTestProtocolDlg::~CTestProtocolDlg()
{
	m_udpClient.Close();
}

void CTestProtocolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_RECV, m_editRecvInfo);
	DDX_Control(pDX, IDC_EDIT_SEND, m_editSendInfo);
	DDX_Control(pDX, IDC_EDIT_SEND, m_editSendInfo);
	DDX_Control(pDX, IDC_IPADDRESS_SERVERIP, m_IPAddrCtrl);
	DDX_Text(pDX, IDC_EDIT_SERVERPORT, m_nServerPort);

	DDX_Control(pDX, IDC_LIST1, m_lcFileInfo);
	DDX_Text(pDX, IDC_EDIT3, m_strSrcFilePath);
	DDX_Text(pDX, IDC_EDIT4, m_strDestFilePath);
	DDX_Control(pDX, IDC_CMB_FILE_LOCATION, m_cmbFileLocation);
	DDX_Control(pDX, IDC_CMB_FILETYPE, m_comboFileType);
	DDX_Control(pDX, IDC_CHECK_NEED_FEEDBACK, m_checkNeedFeedback);
	DDX_Control(pDX, IDC_IPADDRESS_SERVERIP2, m_ipCtrlFeedback);
	DDX_Control(pDX, IDC_EDIT_SERVERPORT2, m_editFeedbackPort);
	DDX_Control(pDX, IDC_COMBO_FEEDBACK_TRANSMODE, m_comboFeedbackMode);
	DDX_Control(pDX, IDC_COMBO3, m_comboBurnMode);
	DDX_Control(pDX, IDC_COMBO2, m_comboBurnType);
	DDX_Text(pDX, IDC_EDIT_SESSIONID, m_strSessionID);
	DDX_Control(pDX, IDC_LIST2, m_lcSaveTask);
	DDX_Control(pDX, IDC_COMBO_CDROMID, m_comboCDRomID);
}


BEGIN_MESSAGE_MAP(CTestProtocolDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_GETCDROMLIST, &CTestProtocolDlg::OnBnClickedButtonGetcdromlist)
	ON_BN_CLICKED(IDC_BTN_STARTBURN, &CTestProtocolDlg::OnBnClickedBtnStartburn)
	ON_BN_CLICKED(IDC_BTN_PAUSEBURN, &CTestProtocolDlg::OnBnClickedBtnPauseburn)
	ON_BN_CLICKED(IDC_BTN_RESUMEBURN, &CTestProtocolDlg::OnBnClickedBtnResumeburn)
	ON_BN_CLICKED(IDC_BTN_STOPBURN, &CTestProtocolDlg::OnBnClickedBtnStopburn)
	ON_BN_CLICKED(IDC_BTN_GETCDROMINFO, &CTestProtocolDlg::OnBnClickedBtnGetcdrominfo)
	ON_BN_CLICKED(IDC_BTN_ADDBURNFILE, &CTestProtocolDlg::OnBnClickedBtnAddburnfile)
	ON_BN_CLICKED(IDC_BTN_CONNECT_SERVER, &CTestProtocolDlg::OnBnClickedBtnConnectServer)
	ON_BN_CLICKED(IDC_BTN_ADDTOLIST, &CTestProtocolDlg::OnBnClickedBtnAddtolist)
	ON_BN_CLICKED(IDC_BTN_CLEAR_LIST, &CTestProtocolDlg::OnBnClickedBtnClearList)
	ON_BN_CLICKED(IDC_BTN_CLEAR_RECVJSON, &CTestProtocolDlg::OnBnClickedBtnClearRecvjson)
	ON_BN_CLICKED(IDC_BTN_CLEAR_SENDJSON, &CTestProtocolDlg::OnBnClickedBtnClearSendjson)
	ON_BN_CLICKED(IDC_BTN_MOVE_LIST, &CTestProtocolDlg::OnBnClickedBtnMoveList)
	ON_BN_CLICKED(IDC_BTN_CLEAR_LIST2, &CTestProtocolDlg::OnBnClickedBtnClearList2)
END_MESSAGE_MAP()


// CTestProtocolDlg 消息处理程序
#include "DirectoryUtil.h"
void CTestProtocolDlg::SetProtocolMode(PROTOCOL_MODE nMode)
{
	std::set<std::string>   setFiles;
	std::string strParse = "D:\\download\\";
	strParse += "*";

	Poco::Glob::glob(strParse, setFiles);
	if (setFiles.size() <= 0)
	{
		TRACE(L"No file");
	}

	strParse = "D:\\download";
	std::string strDestDir = "D:\\abc";
	strParse = DirectoryUtil::EnsureSlashEnd(strParse);
	strDestDir = DirectoryUtil::EnsureSlashEnd(strDestDir);

	std::vector<std::string> vecFileDstPath;
	DirectoryUtil::GetFileList(strParse, vecFileDstPath);
	int nIndex = -1;
	for (int i = 0; i < vecFileDstPath.size(); i++)
	{
		nIndex = vecFileDstPath.at(i).find(strParse);
		if (nIndex > -1)
		{	//find success
			vecFileDstPath.at(i).replace(nIndex, strParse.length(), strDestDir);
		}
	}

	m_nMode = nMode;
}

void CTestProtocolDlg::OnBnClickedButtonGetcdromlist()
{
	m_editSendInfo.Clear();
	m_editRecvInfo.Clear();
	CString strRecv, strSend;
	if (m_nMode == 0)
	{
		m_httpClient.SendGetCDRomListProtocol(strSend, strRecv);
	}
	else
	{
		std::string sSend, sRecv;
		m_udpClient.SendGetCDRomListProtocol(sSend, sRecv);
		//strSend = CharsetConvertMFC::CharsetStreamToCStringA(CharsetConvertSTD::ConstructCharsetStream((const unsigned char*)sSend.c_str(), sSend.length()));
		//strRecv = CharsetConvertMFC::CharsetStreamToCStringA(CharsetConvertSTD::ConstructCharsetStream((const unsigned char*)sRecv.c_str(), sRecv.length()));
		strSend = CharsetConvertMFC::UTF8ToUTF16(CharsetConvertMFC::CharsetStreamToCStringA(CharsetConvertSTD::ConstructCharsetStream((const unsigned char*)sSend.c_str(), sSend.length())));
		strRecv = CharsetConvertMFC::UTF8ToUTF16(CharsetConvertMFC::CharsetStreamToCStringA(CharsetConvertSTD::ConstructCharsetStream((const unsigned char*)sRecv.c_str(), sRecv.length())));
	}
	m_editSendInfo.SetWindowText(strSend);
	m_editRecvInfo.SetWindowText(strRecv);
}

void CTestProtocolDlg::OnBnClickedBtnStartburn()
{
	m_editSendInfo.Clear();
	m_editRecvInfo.Clear();
	CString strRecv, strSend;
	m_vecFileInfo.clear();
	USES_CONVERSION;
	CString strCDRomID;
	GetDlgItemText(IDC_COMBO_CDROMID, strCDRomID);
	std::string sCDRomID = T2A(strCDRomID);

	for (int i = 0; i < m_lcFileInfo.GetItemCount(); i++)
	{
		FileInfo fInfo;
		fInfo.m_strSrcUrl = T2A(m_lcFileInfo.GetItemText(i, 1));
		fInfo.m_strDestFilePath = T2A(m_lcFileInfo.GetItemText(i, 2));
		fInfo.m_strDescription = T2A(m_lcFileInfo.GetItemText(i, 3));
		fInfo.m_strFileLocation = T2A(m_lcFileInfo.GetItemText(i, 4));
		fInfo.m_strType = T2A(m_lcFileInfo.GetItemText(i, 5));
		m_vecFileInfo.push_back(fInfo);
	}
	BurnStateFeedbcak feedback;
	if (m_checkNeedFeedback.GetCheck() == BST_CHECKED)
	{
		feedback.m_strNeedFeedback = "yes";
	}
	else
		feedback.m_strNeedFeedback = "no";
	
	CString strIP;
	m_ipCtrlFeedback.GetWindowText(strIP);
	feedback.m_strFeedbackIP = T2A(strIP);
	feedback.m_nFeedbackPort = GetDlgItemInt(IDC_EDIT_SERVERPORT2);
	feedback.m_transType = m_comboFeedbackMode.GetCurSel() == 0 ? "http" : "udp";
	feedback.m_nFeedbackInterval = GetDlgItemInt(IDC_EDIT_FEEDBACK_INTERVAL);

	CString strBurnType;
	m_comboBurnType.GetWindowText(strBurnType);
	std::string sBurnType = T2A(strBurnType);

	CString strBurnMode;
	m_comboBurnMode.GetWindowText(strBurnMode);
	std::string sBurnMode = T2A(strBurnMode);

	int nAlarmSize = GetDlgItemInt(IDC_EDIT_ALARM_SIZE);

	if (m_nMode == 0)
	{
		m_httpClient.SendStartBurnProtocol(sCDRomID, sBurnType, sBurnMode, nAlarmSize, m_vecFileInfo, feedback, strSend, strRecv);
	}
	else
	{
		std::string sSend, sRecv;
		m_udpClient.SendStartBurnProtocol(sCDRomID, sBurnType, sBurnMode, nAlarmSize, m_vecFileInfo, feedback, sSend, sRecv);
		strSend = CharsetConvertMFC::CharsetStreamToCStringA(CharsetConvertSTD::ConstructCharsetStream((const unsigned char*)sSend.c_str(), sSend.length()));
		strRecv = CharsetConvertMFC::CharsetStreamToCStringA(CharsetConvertSTD::ConstructCharsetStream((const unsigned char*)sRecv.c_str(), sRecv.length()));
	}
	m_editSendInfo.SetWindowText(strSend);
	m_editRecvInfo.SetWindowText(strRecv);
	int nIndex = strRecv.Find(L"sessionID");
	if (nIndex >= 0)
	{
		CString strSessionID = strRecv.Mid(nIndex + 14, 36);
		SetDlgItemText(IDC_EDIT_SESSIONID, strSessionID);
	}
	//立即发送 addBurnFile stopBurn协议
}

void CTestProtocolDlg::OnBnClickedBtnPauseburn()
{
	m_editSendInfo.Clear();
	m_editRecvInfo.Clear();
	CString strRecv, strSend;
	USES_CONVERSION;
	CString strUUID;
	GetDlgItemText(IDC_EDIT_SESSIONID, strUUID);
	std::string sessionID = T2A(strUUID);

	if (m_nMode == 0)
	{
		m_httpClient.SendPauseBurnProtocol(sessionID, strSend, strRecv);
	}
	else
	{
		std::string sSend, sRecv;
		m_udpClient.SendPauseBurnProtocol(sSend, sRecv);
		strSend = CharsetConvertMFC::CharsetStreamToCStringA(CharsetConvertSTD::ConstructCharsetStream((const unsigned char*)sSend.c_str(), sSend.length()));
		strRecv = CharsetConvertMFC::CharsetStreamToCStringA(CharsetConvertSTD::ConstructCharsetStream((const unsigned char*)sRecv.c_str(), sRecv.length()));
	}
	m_editSendInfo.SetWindowText(strSend);
	m_editRecvInfo.SetWindowText(strRecv);
}

void CTestProtocolDlg::OnBnClickedBtnResumeburn()
{
	m_editSendInfo.Clear();
	m_editRecvInfo.Clear();
	CString strRecv, strSend;
	USES_CONVERSION;
	CString strUUID;
	GetDlgItemText(IDC_EDIT_SESSIONID, strUUID);
	std::string sessionID = T2A(strUUID);
	if (m_nMode == 0)
	{
		m_httpClient.SendRemuseBurnProtocol(sessionID, strSend, strRecv);
	}
	else
	{
		std::string sSend, sRecv;
		m_udpClient.SendRemuseBurnProtocol(sSend, sRecv);
		strSend = CharsetConvertMFC::CharsetStreamToCStringA(CharsetConvertSTD::ConstructCharsetStream((const unsigned char*)sSend.c_str(), sSend.length()));
		strRecv = CharsetConvertMFC::CharsetStreamToCStringA(CharsetConvertSTD::ConstructCharsetStream((const unsigned char*)sRecv.c_str(), sRecv.length()));
	}
	m_editSendInfo.SetWindowText(strSend);
	m_editRecvInfo.SetWindowText(strRecv);
}


void CTestProtocolDlg::OnBnClickedBtnStopburn()
{
	m_editSendInfo.Clear();
	m_editRecvInfo.Clear();
	CString strRecv, strSend;
	std::string sessionID;
	USES_CONVERSION;
	CString strUUID;
	GetDlgItemText(IDC_EDIT_SESSIONID, strUUID);
	sessionID = T2A(strUUID);
	if (m_nMode == 0)
	{
		m_httpClient.SendStopBurnProtocol(sessionID, strSend, strRecv);
	}
	else
	{
		std::string sSend, sRecv;
		m_udpClient.SendStopBurnProtocol(sSend, sRecv);
		strSend = CharsetConvertMFC::CharsetStreamToCStringA(CharsetConvertSTD::ConstructCharsetStream((const unsigned char*)sSend.c_str(), sSend.length()));
		strRecv = CharsetConvertMFC::CharsetStreamToCStringA(CharsetConvertSTD::ConstructCharsetStream((const unsigned char*)sRecv.c_str(), sRecv.length()));
	}
	m_editSendInfo.SetWindowText(strSend);
	m_editRecvInfo.SetWindowText(strRecv);
}


void CTestProtocolDlg::OnBnClickedBtnGetcdrominfo()
{
	m_editSendInfo.Clear();
	m_editRecvInfo.Clear();
	CString strRecv, strSend;
	USES_CONVERSION;
	CString strUUID;
	GetDlgItemText(IDC_EDIT_SESSIONID, strUUID);
	std::string sessionID = T2A(strUUID);
	
	CString strCDRomId;
	GetDlgItemText(IDC_COMBO_CDROMID, strCDRomId);
	std::string strCDRomID = T2A(strCDRomId);

	if (m_nMode == 0)
	{
		m_httpClient.SendGetCDRomInfoProtocol(strCDRomID/*sessionID*/, strSend, strRecv);
	}
	else
	{
		std::string sSend, sRecv;
		m_udpClient.SendGetCDRomInfoProtocol(sSend, sRecv);
		strSend = CharsetConvertMFC::CharsetStreamToCStringA(CharsetConvertSTD::ConstructCharsetStream((const unsigned char*)sSend.c_str(), sSend.length()));
		strRecv = CharsetConvertMFC::CharsetStreamToCStringA(CharsetConvertSTD::ConstructCharsetStream((const unsigned char*)sRecv.c_str(), sRecv.length()));
	}
	m_editSendInfo.SetWindowText(strSend);
	m_editRecvInfo.SetWindowText(strRecv);
}


void CTestProtocolDlg::OnBnClickedBtnAddburnfile()
{
	m_editSendInfo.Clear();
	m_editRecvInfo.Clear();
	CString strRecv, strSend;
	USES_CONVERSION;
	CString strUUID;
	GetDlgItemText(IDC_EDIT_SESSIONID, strUUID);
	std::string sessionID = T2A(strUUID);

	m_vecFileInfo.clear();
	for (int i = 0; i < m_lcFileInfo.GetItemCount(); i++)
	{
		FileInfo fInfo;
		fInfo.m_strSrcUrl = T2A(m_lcFileInfo.GetItemText(i, 1));
		fInfo.m_strDestFilePath = T2A(m_lcFileInfo.GetItemText(i, 2));
		fInfo.m_strDescription = T2A(m_lcFileInfo.GetItemText(i, 3));
		fInfo.m_strFileLocation = T2A(m_lcFileInfo.GetItemText(i, 4));
		fInfo.m_strType = T2A(m_lcFileInfo.GetItemText(i, 5));
		m_vecFileInfo.push_back(fInfo);
	}

	if (m_nMode == 0)
	{
		m_httpClient.SendAddBurnFileProtocol(sessionID, m_vecFileInfo, strSend, strRecv);
	}
	else
	{
		std::string sSend, sRecv;
		m_udpClient.SendAddBurnFileProtocol(sSend, sRecv);
		strSend = CharsetConvertMFC::CharsetStreamToCStringA(CharsetConvertSTD::ConstructCharsetStream((const unsigned char*)sSend.c_str(), sSend.length()));
		strRecv = CharsetConvertMFC::CharsetStreamToCStringA(CharsetConvertSTD::ConstructCharsetStream((const unsigned char*)sRecv.c_str(), sRecv.length()));
	}
	m_editSendInfo.SetWindowText(strSend);
	m_editRecvInfo.SetWindowText(strRecv);
}


BOOL CTestProtocolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  在此添加额外的初始化
	Init();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

CString CTestProtocolDlg::GetServerIP()
{
	UpdateData(TRUE);
	BYTE bt1, bt2, bt3, bt4;
	m_IPAddrCtrl.GetAddress(bt1, bt2, bt3, bt4);
	CString str;
	str.Format(L"%d.%d.%d.%d", bt1, bt2, bt3, bt4);//这里的nf得到的值是IP值了.
	return str;
}

void CTestProtocolDlg::Init()
{
	m_IPAddrCtrl.SetAddress(0x7F000001);
	if (m_nMode == HTTP_MODE)
		m_nServerPort = 90;
	else
		m_nServerPort = 91;
	SetDlgItemInt(IDC_EDIT_SERVERPORT, m_nServerPort);

	Poco::Net::SocketAddress sa("127.0.0.1", 91);
	//m_socketAddr = sa;
	//m_udpSocket.connect(m_socketAddr);
	m_udpClient.Init("127.0.0.1", 91);
	m_udpClient.ConnectServer();
	
	m_comboCDRomID.InsertString(0, L"CDRom_1");
	m_comboCDRomID.InsertString(1, L"CDRom_2");
	m_comboCDRomID.SetCurSel(0);

	DWORD dwStyle = m_lcFileInfo.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	m_lcFileInfo.SetExtendedStyle(dwStyle);

	CRect rect;
	m_lcFileInfo.GetClientRect(&rect);
	int nWidth = rect.Width();
	m_lcFileInfo.InsertColumn(0, L"Index", 0, nWidth * 1/ 12);
	m_lcFileInfo.InsertColumn(1, L"源路径", 0, nWidth * 4 /12);
	m_lcFileInfo.InsertColumn(2, L"目标路径", 0, nWidth * 3 / 12);
	m_lcFileInfo.InsertColumn(3, L"描述", 0, nWidth * 1 / 12);
	m_lcFileInfo.InsertColumn(4, L"本地/远端", 0, nWidth * 1.8 / 12);
	m_lcFileInfo.InsertColumn(5, L"文件/文件夹", 0, nWidth * 2 / 12);

	//save send file info
	m_lcSaveTask.SetExtendedStyle(dwStyle);
	m_lcSaveTask.GetClientRect(&rect);
	nWidth = rect.Width();
	m_lcSaveTask.InsertColumn(0, L"SessionID", 0, nWidth * 1 / 12);
	m_lcSaveTask.InsertColumn(1, L"源路径", 0, nWidth * 4 / 12);
	m_lcSaveTask.InsertColumn(2, L"目标路径", 0, nWidth * 3 / 12);
	m_lcSaveTask.InsertColumn(3, L"描述", 0, nWidth * 1 / 12);
	m_lcSaveTask.InsertColumn(4, L"本地/远端", 0, nWidth * 1.8 / 12);
	m_lcSaveTask.InsertColumn(5, L"文件/文件夹", 0, nWidth * 2 / 12);

	m_IPAddrCtrl.SetWindowText(L"192.168.1.11");
	m_ipCtrlFeedback.SetWindowText(L"192.168.1.149");

	m_comboBurnMode.InsertString(m_comboBurnMode.GetCount(), L"singleBurn");
	m_comboBurnMode.InsertString(m_comboBurnMode.GetCount(), L"doubleRelayBurn");
	m_comboBurnMode.InsertString(m_comboBurnMode.GetCount(), L"doubleParallelBurn");

	m_comboFileType.SetCurSel(0);
	m_cmbFileLocation.SetCurSel(0);

	m_comboBurnType.SetCurSel(0);
	m_comboBurnMode.SetCurSel(0);
	SetDlgItemInt(IDC_EDIT_ALARM_SIZE, 200);
	
	m_checkNeedFeedback.SetCheck(BST_UNCHECKED);
	SetDlgItemInt(IDC_EDIT_FEEDBACK_INTERVAL, 2000);
	SetDlgItemInt(IDC_EDIT_FEEDBACKPORT, 1001);
	m_comboFeedbackMode.SetCurSel(0);

	UpdateData(FALSE);
}


void CTestProtocolDlg::OnBnClickedBtnConnectServer()
{
	UpdateData();
	USES_CONVERSION;
	m_strServerIP = W2A(GetServerIP());
	if (m_nMode == HTTP_MODE)
		m_httpClient.SetServerInfo(m_strServerIP, m_nServerPort);
}


void CTestProtocolDlg::OnBnClickedBtnAddtolist()
{
	UpdateData();
	CString strFileDesp;
	GetDlgItemText(IDC_EDIT_FILE_DESCRIPTION, strFileDesp);
	CString strInsert;
	strInsert.Format(L"%d", m_lcFileInfo.GetItemCount() + 1);
	int nItem = m_lcFileInfo.InsertItem(m_lcFileInfo.GetItemCount(), strInsert);
	m_lcFileInfo.SetItemText(nItem, 1, m_strSrcFilePath);
	m_lcFileInfo.SetItemText(nItem, 2, m_strDestFilePath);
	m_lcFileInfo.SetItemText(nItem, 3, strFileDesp);
	m_lcFileInfo.SetItemText(nItem, 4, (m_cmbFileLocation.GetCurSel() == 0) ? L"local" : L"remote");
	m_lcFileInfo.SetItemText(nItem, 5, (m_comboFileType.GetCurSel() == 0) ? L"file" : L"dir");
}


void CTestProtocolDlg::OnBnClickedBtnClearList()
{
	m_lcFileInfo.DeleteAllItems();
}


void CTestProtocolDlg::OnBnClickedBtnClearRecvjson()
{
	SetDlgItemText(IDC_EDIT_SEND, L"");
}


void CTestProtocolDlg::OnBnClickedBtnClearSendjson()
{
	SetDlgItemText(IDC_EDIT_RECV, L"");
}

void CTestProtocolDlg::OnBnClickedBtnMoveList()
{
	CString strSessionID;
	GetDlgItemText(IDC_EDIT_SESSIONID, strSessionID);
	for (int i = 0; i < m_lcFileInfo.GetItemCount(); i++)
	{
		int nItem = m_lcSaveTask.InsertItem(m_lcSaveTask.GetItemCount(), strSessionID);
		m_lcSaveTask.SetItemText(nItem, 1, m_lcFileInfo.GetItemText(i, 1));
		m_lcSaveTask.SetItemText(nItem, 2, m_lcFileInfo.GetItemText(i, 2));
		m_lcSaveTask.SetItemText(nItem, 3, m_lcFileInfo.GetItemText(i, 3));
		m_lcSaveTask.SetItemText(nItem, 4, m_lcFileInfo.GetItemText(i, 4));
		m_lcSaveTask.SetItemText(nItem, 5, m_lcFileInfo.GetItemText(i, 5));
	}
}


void CTestProtocolDlg::OnBnClickedBtnClearList2()
{
	m_lcSaveTask.DeleteAllItems();
}
