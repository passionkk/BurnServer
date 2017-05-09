
// TestBurnServerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TestBurnServer.h"
#include "TestBurnServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTestBurnServerDlg �Ի���



CTestBurnServerDlg::CTestBurnServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestBurnServerDlg::IDD, pParent)
	, m_nServerPort(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//m_pHttpPage = NULL;
	//m_pUdpPage = NULL;
}

void CTestBurnServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_RECV, m_editRecvInfo);
	DDX_Control(pDX, IDC_EDIT_SEND, m_editSendInfo);
	DDX_Control(pDX, IDC_IPADDRESS_SERVERIP, m_IPAddrCtrl);
	DDX_Text(pDX, IDC_EDIT_SERVERPORT, m_nServerPort);
	DDX_Control(pDX, IDC_TAB_CTRL, m_tabCtrl);
}

BEGIN_MESSAGE_MAP(CTestBurnServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_TEST, &CTestBurnServerDlg::OnBnClickedBtnTest)
	ON_BN_CLICKED(IDC_BTN_UDP_TEST, &CTestBurnServerDlg::OnBnClickedBtnUdpTest)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CTRL, &CTestBurnServerDlg::OnSelchangeTabCtrl)
END_MESSAGE_MAP()


// CTestBurnServerDlg ��Ϣ�������

BOOL CTestBurnServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	InitCtrl();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CTestBurnServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTestBurnServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CTestBurnServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

CString CTestBurnServerDlg::GetServerIP()
{
	UpdateData(TRUE);
	BYTE bt1, bt2, bt3, bt4;
	m_IPAddrCtrl.GetAddress(bt1, bt2, bt3, bt4);
	CString str;
	str.Format(L"%d.%d.%d.%d", bt1, bt2, bt3, bt4);//�����nf�õ���ֵ��IPֵ��.
	return str;
}

void CTestBurnServerDlg::InitCtrl()
{
	m_IPAddrCtrl.SetAddress(0x7F000001);
	m_nServerPort = 90;

	m_tabCtrl.InsertItem(0, L"Http");
	m_tabCtrl.InsertItem(1, L"UDP");
	
	//���������Ի���
	m_HTTPPage.Create(CTestProtocolDlg::IDD, &m_tabCtrl);
	m_HTTPPage.SetProtocolMode(HTTP_MODE);

	m_UDPPage.Create(CTestProtocolDlg::IDD, &m_tabCtrl);
	m_UDPPage.SetProtocolMode(UDP_MODE);
	//�趨��Tab����ʾ�ķ�Χ
	CRect rc;
	m_tabCtrl.GetClientRect(rc);
	rc.top += 20;
	rc.bottom -= 0;
	rc.left += 0;
	rc.right -= 0;

	m_HTTPPage.MoveWindow(&rc);
	m_UDPPage.MoveWindow(&rc);

	m_HTTPPage.ShowWindow(SW_SHOW);
	m_UDPPage.ShowWindow(SW_HIDE);
	m_nCurTabSel = 0;

	UpdateData(FALSE);
}

void CTestBurnServerDlg::OnBnClickedBtnTest()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString strRecv;
	strRecv.Empty();
	m_httpClient.BurnServerConnect(strRecv);
	if (!strRecv.IsEmpty())
		m_editRecvInfo.SetWindowText(strRecv);
}


void CTestBurnServerDlg::OnBnClickedBtnUdpTest()
{
	CString strRecv;
	m_udpClient.TestUDPConnect(strRecv);
	m_editRecvInfo.Clear();
	m_editRecvInfo.SetWindowText(strRecv);
	/*UpdateData(TRUE);
	CString strIP = GetServerIP();
	int nPort = m_nServerPort;
	m_udpClient.Init(strIP, nPort);
	m_udpClient.ConnectServer();
	m_udpClient.Bind();
	std::string strSend;
	std::string strRecv;
	m_udpClient.SendProtocol(strSend, strRecv);*/
}


void CTestBurnServerDlg::OnSelchangeTabCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_nCurTabSel = m_tabCtrl.GetCurSel();
	if (m_nCurTabSel == 0)
	{
		m_UDPPage.ShowWindow(SW_HIDE);
		m_HTTPPage.ShowWindow(SW_SHOW);
	}
	else
	{
		m_HTTPPage.ShowWindow(SW_HIDE);
		m_UDPPage.ShowWindow(SW_SHOW);
	}
	*pResult = 0;
}
