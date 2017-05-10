#pragma once
#include "HttpClient.h"
#include "UDPClient.h"

#include "CommonDefine.h"
// CTestProtocolDlg �Ի���

class CTestProtocolDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTestProtocolDlg)

public:
	CTestProtocolDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTestProtocolDlg();

// �Ի�������
	enum { IDD = IDD_DLG_PROTOCOL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonGetcdromlist();
	afx_msg void OnBnClickedBtnStartburn();
	afx_msg void OnBnClickedBtnPauseburn();
	afx_msg void OnBnClickedBtnResumeburn();
	afx_msg void OnBnClickedBtnStopburn();
	afx_msg void OnBnClickedBtnGetcdrominfo();
	afx_msg void OnBnClickedBtnAddburnfile();

	CString GetServerIP();
	void	Init();
	void	SetShowEdit(CEdit& editSend, CEdit& editRecv);
	void	SetProtocolMode(PROTOCOL_MODE nMode);
	void	SendGetCDRomListProtocol(CString& strRecv);

	PROTOCOL_MODE		m_nMode;		//0,http 1,udp

	CHttpClient			m_httpClient;
	UDPClient			m_udpClient;
	CEdit				m_editRecvInfo;
	CEdit				m_editSendInfo;
	CIPAddressCtrl		m_IPAddrCtrl;
	int					m_nServerPort;
};
