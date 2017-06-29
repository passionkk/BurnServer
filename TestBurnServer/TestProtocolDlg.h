#pragma once
#include "HttpClient.h"
#include "UDPClient.h"

#include "CommonDefine.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "ListCtrlEdit.h"
// CTestProtocolDlg 对话框

class CTestProtocolDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTestProtocolDlg)

public:
	CTestProtocolDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTestProtocolDlg();

// 对话框数据
	enum { IDD = IDD_DLG_PROTOCOL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
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
	afx_msg void OnBnClickedBtnConnectServer();
	afx_msg void OnBnClickedBtnAddtolist();
	afx_msg void OnBnClickedBtnClearList();
	afx_msg void OnBnClickedBtnClearRecvjson();
	afx_msg void OnBnClickedBtnClearSendjson();

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
	std::string			m_strServerIP;
	/*CListCtrl*/CListCtrlEdit			m_lcFileInfo;
	CString				m_strSrcFilePath;
	CString				m_strDestFilePath;
	CComboBox m_cmbFileLocation;
	CComboBox m_comboFileType;
	std::vector<FileInfo> m_vecFileInfo;
	CButton m_checkNeedFeedback;
	// 反馈server IP
	CIPAddressCtrl m_ipCtrlFeedback;
	// 反馈服务器Port
	CEdit m_editFeedbackPort;
	CComboBox m_comboFeedbackMode;
	CComboBox m_comboBurnMode;
	CComboBox m_comboBurnType;
	CString m_strSessionID;
	CListCtrl m_lcSaveTask;
	afx_msg void OnBnClickedBtnMoveList();
	afx_msg void OnBnClickedBtnClearList2();
};
