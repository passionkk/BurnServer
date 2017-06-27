#pragma once

#include "CommonDefine.h"
// CTestPtoServerDlg �Ի���

class CTestPtoServerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTestPtoServerDlg)

public:
	CTestPtoServerDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTestPtoServerDlg();

// �Ի�������
	enum { IDD = IDD_DLG_PROTOCOL_SERVER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	afx_msg void OnBnClickedBtnStartserver();
	afx_msg void OnBnClickedBtnAddburnfile();

	DECLARE_MESSAGE_MAP()

public:
	
	static void SetFeedbackState(LPVOID pVoid, std::string strFeedback, int nShowIndex);
	void DoSetFeedbackState(std::string strFeedback, int nShowIndex);
	void	SetProtocolMode(PROTOCOL_MODE nMode);
	PROTOCOL_MODE		m_nMode;		//0,http 1,udp
	
	int					m_nServerPort;
	CString				m_strSessionID;
};
