
// TestBurnServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestBurnServer.h"
#include "TestBurnServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CTestBurnServerDlg 对话框



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


// CTestBurnServerDlg 消息处理程序

BOOL CTestBurnServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	InitCtrl();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTestBurnServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
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
	str.Format(L"%d.%d.%d.%d", bt1, bt2, bt3, bt4);//这里的nf得到的值是IP值了.
	return str;
}

void CTestBurnServerDlg::InitCtrl()
{
	m_IPAddrCtrl.SetAddress(0x7F000001);
	m_nServerPort = 90;

	m_tabCtrl.InsertItem(0, L"Http");
	m_tabCtrl.InsertItem(1, L"UDP");
	
	//创建两个对话框
	m_HTTPPage.Create(CTestProtocolDlg::IDD, &m_tabCtrl);
	m_HTTPPage.SetProtocolMode(HTTP_MODE);

	m_UDPPage.Create(CTestProtocolDlg::IDD, &m_tabCtrl);
	m_UDPPage.SetProtocolMode(UDP_MODE);
	//设定在Tab内显示的范围
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
	// TODO:  在此添加控件通知处理程序代码
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
