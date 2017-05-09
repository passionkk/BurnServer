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
}

void CTestProtocolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_RECV, m_editRecvInfo);
	DDX_Control(pDX, IDC_EDIT_SEND, m_editSendInfo);

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
	}
	m_editSendInfo.SetWindowText(strSend);
	m_editRecvInfo.SetWindowText(strRecv);
}
