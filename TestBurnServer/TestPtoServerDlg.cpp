// TestPtoServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestBurnServer.h"
#include "TestPtoServerDlg.h"
#include "afxdialogex.h"
#include "Net/HttpServerModule.h"
#include "Net/UDPServerModule.h"

// CTestPtoServerDlg 对话框

IMPLEMENT_DYNAMIC(CTestPtoServerDlg, CDialogEx)

CTestPtoServerDlg::CTestPtoServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestPtoServerDlg::IDD, pParent)
	, m_nServerPort(0)
	, m_strSessionID(_T(""))
{

}

CTestPtoServerDlg::~CTestPtoServerDlg()
{
}

void CTestPtoServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SERVERPORT, m_nServerPort);
	//DDX_Text(pDX, IDC_EDIT_SESSIONID, m_strSessionID);
}


BEGIN_MESSAGE_MAP(CTestPtoServerDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_STARTSERVER, &CTestPtoServerDlg::OnBnClickedBtnStartserver)
	ON_BN_CLICKED(IDC_BTN_ADDBURNFILE, &CTestPtoServerDlg::OnBnClickedBtnAddburnfile)
END_MESSAGE_MAP()


// CTestPtoServerDlg 消息处理程序
void CTestPtoServerDlg::SetProtocolMode(PROTOCOL_MODE nMode)
{
	m_nMode = nMode;
	if (m_nMode == HTTP_MODE)
	{
		m_nServerPort = 1001;
	}
	else
		m_nServerPort = 1002;

	UpdateData(FALSE);
	//OnBnClickedBtnStartserver();
}


void CTestPtoServerDlg::OnBnClickedBtnStartserver()
{
	if (m_nMode == HTTP_MODE)
	{
		HttpServerModule::Initialize();
		HttpServerModule::GetInstance()->SetCallback(CTestPtoServerDlg::SetFeedbackState, this);
	}
	else
		UDPServerModule::Initialize();
}


void CTestPtoServerDlg::OnBnClickedBtnAddburnfile()
{
	// TODO:  在此添加控件通知处理程序代码
}

void CTestPtoServerDlg::SetFeedbackState(LPVOID pVoid, std::string strFeedback, int nShowIndex)
{
	if (pVoid != NULL)
	{
		CTestPtoServerDlg* pThis = (CTestPtoServerDlg*)pVoid;
		pThis->DoSetFeedbackState(strFeedback, nShowIndex);
	}
}

void CTestPtoServerDlg::DoSetFeedbackState(std::string strFeedback, int nShowIndex)
{
	CString str(strFeedback.c_str());
	if (nShowIndex == 1)
		SetDlgItemText(IDC_EDIT_RECV, str);
	else
		SetDlgItemText(IDC_EDIT_SEND, str);
}