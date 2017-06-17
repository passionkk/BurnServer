// TestProtocolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestBurnServer.h"
#include "TestProtocolDlg.h"
#include "afxdialogex.h"

// CTestProtocolDlg 对话框

IMPLEMENT_DYNAMIC(CTestProtocolDlg, CDialogEx)

CTestProtocolDlg::CTestProtocolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestProtocolDlg::IDD, pParent)
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
	DDX_Control(pDX, IDC_EDIT_RECV, m_editRecvInfo);
	DDX_Control(pDX, IDC_EDIT_SEND, m_editSendInfo);
	DDX_Control(pDX, IDC_IPADDRESS_SERVERIP, m_IPAddrCtrl);
	DDX_Text(pDX, IDC_EDIT_SERVERPORT, m_nServerPort);

}


BEGIN_MESSAGE_MAP(CTestProtocolDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_GETCDROMLIST, &CTestProtocolDlg::OnBnClickedButtonGetcdromlist)
	ON_BN_CLICKED(IDC_BTN_STARTBURN, &CTestProtocolDlg::OnBnClickedBtnStartburn)
	ON_BN_CLICKED(IDC_BTN_PAUSEBURN, &CTestProtocolDlg::OnBnClickedBtnPauseburn)
	ON_BN_CLICKED(IDC_BTN_RESUMEBURN, &CTestProtocolDlg::OnBnClickedBtnResumeburn)
	ON_BN_CLICKED(IDC_BTN_STOPBURN, &CTestProtocolDlg::OnBnClickedBtnStopburn)
	ON_BN_CLICKED(IDC_BTN_GETCDROMINFO, &CTestProtocolDlg::OnBnClickedBtnGetcdrominfo)
	ON_BN_CLICKED(IDC_BTN_ADDBURNFILE, &CTestProtocolDlg::OnBnClickedBtnAddburnfile)
END_MESSAGE_MAP()


// CTestProtocolDlg 消息处理程序

void CTestProtocolDlg::SetProtocolMode(PROTOCOL_MODE nMode)
{
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
	if (m_nMode == 0)
	{
		m_httpClient.SendStartBurnProtocol(strSend, strRecv);
	}
	else
	{
		std::string sSend, sRecv;
		m_udpClient.SendStartBurnProtocol(sSend, sRecv);
		strSend = CharsetConvertMFC::CharsetStreamToCStringA(CharsetConvertSTD::ConstructCharsetStream((const unsigned char*)sSend.c_str(), sSend.length()));
		strRecv = CharsetConvertMFC::CharsetStreamToCStringA(CharsetConvertSTD::ConstructCharsetStream((const unsigned char*)sRecv.c_str(), sRecv.length()));
	}
	m_editSendInfo.SetWindowText(strSend);
	m_editRecvInfo.SetWindowText(strRecv);
}

void CTestProtocolDlg::OnBnClickedBtnPauseburn()
{
	m_editSendInfo.Clear();
	m_editRecvInfo.Clear();
	CString strRecv, strSend;
	if (m_nMode == 0)
	{
		m_httpClient.SendPauseBurnProtocol(strSend, strRecv);
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
	if (m_nMode == 0)
	{
		m_httpClient.SendRemuseBurnProtocol(strSend, strRecv);
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
	if (m_nMode == 0)
	{
		m_httpClient.SendStopBurnProtocol(strSend, strRecv);
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
	if (m_nMode == 0)
	{
		m_httpClient.SendGetCDRomInfoProtocol(strSend, strRecv);
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
	if (m_nMode == 0)
	{
		m_httpClient.SendAddBurnFileProtocol(strSend, strRecv);
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
	m_nServerPort = 90;

	Poco::Net::SocketAddress sa("127.0.0.1", 91);
	//m_socketAddr = sa;
	//m_udpSocket.connect(m_socketAddr);
	m_udpClient.Init("127.0.0.1", 91);
	m_udpClient.ConnectServer();
}
