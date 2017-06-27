
// TestBurnServerDlg.h : ͷ�ļ�
//

#pragma once
#include "HttpClient.h"
#include "UDPClient.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "TestProtocolDlg.h"
#include "TestPtoServerDlg.h"

// CTestBurnServerDlg �Ի���
class CTestBurnServerDlg : public CDialogEx
{
// ����
public:
	CTestBurnServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TESTBURNSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedBtnTest();
	afx_msg void OnBnClickedBtnUdpTest();

	DECLARE_MESSAGE_MAP()


	CString GetServerIP();
	void	InitCtrl();
private:
	CHttpClient m_httpClient;
	UDPClient	m_udpClient;

	CTestProtocolDlg	m_UDPPage;
	CTestProtocolDlg	m_HTTPPage;

	CTestPtoServerDlg	m_DlgUDPServer;
	CTestPtoServerDlg	m_DlgHttpServer;

	CIPAddressCtrl		m_IPAddrCtrl;
	CEdit				m_editRecvInfo;
	CEdit				m_editSendInfo;
	int					m_nServerPort;
	CTabCtrl			m_tabCtrl;
	int					m_nCurTabSel;
	bool				bInit;
public:
	afx_msg void OnSelchangeTabCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
