
// TestBurnServerDlg.h : 头文件
//

#pragma once
#include "HttpClient.h"
#include "UDPClient.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "TestProtocolDlg.h"
#include "TestPtoServerDlg.h"

// CTestBurnServerDlg 对话框
class CTestBurnServerDlg : public CDialogEx
{
// 构造
public:
	CTestBurnServerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TESTBURNSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
