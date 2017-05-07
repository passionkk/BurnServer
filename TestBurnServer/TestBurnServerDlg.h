
// TestBurnServerDlg.h : ͷ�ļ�
//

#pragma once
#include "HttpClient.h"
#include "UDPClient.h"
#include "afxwin.h"
#include "afxcmn.h"

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
	CEdit		m_editRecvInfo;
	UDPClient	m_udpClient;
public:
	CIPAddressCtrl m_IPAddrCtrl;
	int m_nServerPort;
};
