#include "mainDialog.h"
#include "options.h"

extern NOTIFYICONDATA m_tind;

CMainDialog::CMainDialog(CWnd *pParentWnd) : CDialog( IDD_MAIN, pParentWnd ){}

BOOL CMainDialog::OnInitDialog()
{
	CRect rc; 
	CPaintDC dc(this);	

	HICON hIcon, hIconSm;

	CAppOptions *appOptions = CAppOptions::GetSingleInstance();

	LOGFONT logfont; 
	CWnd *pTxtUsername = GetDlgItem( TXT_USERNAME );
	CWnd *pTxtPassword = GetDlgItem( TXT_PASSWORD );
	CFont *pfont = pTxtUsername-> GetFont(); 


	pfont->GetLogFont(&logfont); 
	logfont.lfHeight =  18; 
	m_font.CreateFontIndirect(&logfont); 
	pTxtUsername->SetFont( &m_font );
	pTxtPassword->SetFont( &m_font );

	hIcon = LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE(APP_ICON) );
	
	hIconSm = LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE(APP_ICON) );
	
	this->SetIcon( hIcon, TRUE );
	this->SetIcon( hIcon, FALSE);	
	this->ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW);

	// 绘制窗体
	GetClientRect(rc); 
	m_rgn.CreateRoundRectRgn(rc.left,   rc.top,   rc.right,   rc.bottom,   10,   10);
	::SetWindowRgn(GetSafeHwnd(),(HRGN)m_rgn,TRUE);		

	SetWindowLong( this->GetSafeHwnd(), GWL_EXSTYLE,
					GetWindowLong( this->GetSafeHwnd(), GWL_EXSTYLE )^0x80000 );

	HINSTANCE hInst = LoadLibrary( _T("User32.DLL") );
	if(hInst)
	{
		typedef BOOL (WINAPI *MYFUNC)(HWND,COLORREF,BYTE,DWORD);
		MYFUNC fun = NULL;
		fun=(MYFUNC)GetProcAddress(hInst, "SetLayeredWindowAttributes");
		if(fun)fun(this->GetSafeHwnd(),0,190,2);
		FreeLibrary(hInst);
	}

	TerminateProcess( appOptions->proc_info.hProcess,0 );

	return TRUE;
}

void CMainDialog::OnPaint()
{
	CRect rect; 
    CPaintDC dc(this); 
	CFont font;

	Image image( _T(".\\image\\XIII.png"), FALSE);

	// 绘制对话框背景
    GetClientRect(rect); 	
    dc.FillSolidRect(rect,RGB(160,200,236));	
	
	// 绘制LOGO
	Graphics graphics( dc.m_hDC);    
    graphics.DrawImage(&image, 6, 5);

	// 绘制菜单文字信息	
	font.CreateFontW( 14, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("黑体") );
	
	rect.OffsetRect( 32, 8 );
	dc.SelectObject( &font );
	dc.DrawText( _T("MyOpenVPN(绿色版)"), &rect, DT_SINGLELINE );

	rect.OffsetRect( 15, 80 );
	dc.DrawText( _T("帐号："), &rect, DT_SINGLELINE );

	rect.OffsetRect( 0, 50 );
	dc.DrawText( _T("密码："), &rect, DT_SINGLELINE );
	
	ReleaseDC(&dc);	
}

void CMainDialog::OnNcMouseMove( UINT nHitTest, CPoint point )
{
	CRect rect;	
	CBitmap bitmap; 

	GetWindowRect(rect);	
	ScreenToClient(&point);

	// 绘制"最小化"按钮图标
	if( point.x > 190 && point.x < 215 && point.y < 22 && point.y > 2 )
	{
		bitmap.LoadBitmapW( BMP_MINIMIZE_OVER );
		( (CStatic *)GetDlgItem(PIC_SYSTEM_MINIMIZE) )->SetBitmap( (HBITMAP)bitmap );
		bitmap.Detach();
	}
	else
	{
		bitmap.LoadBitmapW( BMP_MINIMIZE );
		( (CStatic *)GetDlgItem(PIC_SYSTEM_MINIMIZE) )->SetBitmap( (HBITMAP)bitmap );
		bitmap.Detach();
	}

	// 绘制"关闭"按钮图标
	if( point.x > 215 && point.x < 240 && point.y < 22 && point.y > 2 )
	{
		bitmap.LoadBitmapW( BMP_EXIT_OVER );
		( (CStatic *)GetDlgItem(PIC_SYSTEM_EXIT) )->SetBitmap( (HBITMAP)bitmap );
		bitmap.Detach();
	}
	else
	{
		bitmap.LoadBitmapW( BMP_EXIT );
		( (CStatic *)GetDlgItem(PIC_SYSTEM_EXIT) )->SetBitmap( (HBITMAP)bitmap );
		bitmap.Detach();
	}

	// 鼠标指针修改
	if( point.x > 76 && point.x < 186 && point.y < 220 && point.y > 200 )
	{		
		::SetCursor( AfxGetApp()-> LoadCursor(IDC_XIII_HAND) );
	}	
}

void CMainDialog::OnNcLButtonUp( UINT nFlags, CPoint point )
{
	CBitmap bitmap; 

	MSG pMsg;
	pMsg.message = WM_KEYDOWN;
	pMsg.wParam = VK_RETURN;
	pMsg.hwnd = ( (CEdit *)GetDlgItem(TXT_USERNAME) )->m_hWnd;

	// 隐藏托盘弹出菜单
	if( FindWindow( NULL, _T("popupMenu") ) != NULL )
	{
		FindWindow( NULL, _T("popupMenu") )->ShowWindow(SW_HIDE);
	}

	ScreenToClient(&point);

	// "最小化"按钮
	if( point.x > 190 && point.x < 215 && point.y < 22 && point.y > 2 )
	{
		this->SendMessageW(WM_SYSCOMMAND, SC_MINIMIZE);
		this->ShowWindow( SW_HIDE );
		bitmap.LoadBitmapW( BMP_MINIMIZE );
		( (CStatic *)GetDlgItem(PIC_SYSTEM_MINIMIZE) )->SetBitmap( (HBITMAP)bitmap );
		bitmap.Detach();
	}

	// "关闭"按钮
	if( point.x > 215 && point.x < 240 && point.y < 22 && point.y > 2 )
	{
		CAppOptions *appOptions = CAppOptions::GetSingleInstance();
		Shell_NotifyIcon(NIM_DELETE, &m_tind);
		TerminateProcess( appOptions->proc_info.hProcess,0 );
		AfxGetMainWnd()->DestroyWindow();
	}

	// "连接"按钮
	if( point.x > 76 && point.x < 186 && point.y < 220 && point.y > 200 )
	{		
		bitmap.LoadBitmapW( BMP_CONNECT );
		( (CStatic *)GetDlgItem(PIC_CONNECT) )->SetBitmap( (HBITMAP)bitmap );
		bitmap.Detach();
		CMainDialog::PreTranslateMessage( &pMsg );
	}	
}

void CMainDialog::OnNcLButtonDown( UINT nFlags, CPoint point )
{
	CBitmap bitmap; 

	// 隐藏托盘弹出菜单
	if( FindWindow( NULL, _T("popupMenu") ) != NULL )
	{
		FindWindow( NULL, _T("popupMenu") )->ShowWindow(SW_HIDE);
	}

	ScreenToClient(&point);

	// "最小化"按钮
	if( point.x > 190 && point.x < 215 && point.y < 22 && point.y > 2 )
	{		
		this->SendMessageW(WM_SYSCOMMAND, SC_MINIMIZE);	
		this->ShowWindow( SW_HIDE );
		bitmap.LoadBitmapW( BMP_MINIMIZE );
		( (CStatic *)GetDlgItem(PIC_SYSTEM_MINIMIZE) )->SetBitmap( (HBITMAP)bitmap );
		bitmap.Detach();
		return;
	}

	// "关闭"按钮
	if( point.x > 215 && point.x < 240 && point.y < 22 && point.y > 2 )
	{
		CAppOptions *appOptions = CAppOptions::GetSingleInstance();
		Shell_NotifyIcon(NIM_DELETE, &m_tind);
		TerminateProcess( appOptions->proc_info.hProcess,0 );		
		AfxGetMainWnd()->DestroyWindow();
		return;
	}

	// "连接"按钮
	if( point.x > 76 && point.x < 186 && point.y < 220 && point.y > 200 )
	{		
		::SetCursor( AfxGetApp()-> LoadCursor(IDC_XIII_HAND) );
		bitmap.LoadBitmapW( BMP_CONNECT_DOWN );
		( (CStatic *)GetDlgItem(PIC_CONNECT) )->SetBitmap( (HBITMAP)bitmap );
		bitmap.Detach();			
		return;
	}	

	CDialog::OnNcLButtonDown( nFlags, point );
}

void CMainDialog::OnNcRButtonUp( UINT nFlags, CPoint point )
{
	if( FindWindow( NULL, _T("popupMenu") ) != NULL )
	{
		FindWindow( NULL, _T("popupMenu") )->ShowWindow(SW_HIDE);
	}
}

LRESULT CMainDialog::OnNcHitTest( CPoint point )
{
	CRect rect;
	CBitmap bitmap;
	GetClientRect(&rect);
	ClientToScreen(&rect);

	return rect.PtInRect(point) ? HTCAPTION : CDialog::OnNcHitTest(point);
}

BOOL CMainDialog::PreTranslateMessage( MSG *pMsg )
{
	HWND hTxtUsername = ( (CEdit *)GetDlgItem(TXT_USERNAME) )->m_hWnd;
	HWND hTxtPassword = ( (CEdit *)GetDlgItem(TXT_PASSWORD) )->m_hWnd;
	
	CString username;
	CString password;	
	
	if( pMsg->message == WM_KEYDOWN && pMsg->hwnd == hTxtUsername && pMsg->wParam == VK_RETURN )
	{	
		( (CEdit *)GetDlgItem(TXT_USERNAME) )->GetWindowTextW( username );
		( (CEdit *)GetDlgItem(TXT_PASSWORD) )->GetWindowTextW( password );

		if( username == "" || password == "" )
		{
			AfxMessageBox( _T("请输入用户名和密码！") );
			( (CEdit *)GetDlgItem(TXT_USERNAME) )->SetFocus();
			return TRUE;
		}

		this->SendMessageW(WM_SYSCOMMAND, SC_MINIMIZE);
		this->ShowWindow( SW_HIDE );

		( (CEdit *)GetDlgItem(TXT_USERNAME) )->EnableWindow( FALSE );
		( (CEdit *)GetDlgItem(TXT_PASSWORD) )->EnableWindow( FALSE );
		( (CStatic *)GetDlgItem(PIC_CONNECT) )->ShowWindow( SW_HIDE );


		StartOpenVPN( username, password );
		return TRUE;
	}

	if( pMsg->message == WM_KEYDOWN && pMsg->hwnd == hTxtPassword && pMsg->wParam == VK_RETURN )
	{
		( (CEdit *)GetDlgItem(TXT_USERNAME) )->GetWindowTextW( username );
		( (CEdit *)GetDlgItem(TXT_PASSWORD) )->GetWindowTextW( password );		

		if( username == "" || password == "" )
		{
			AfxMessageBox( _T("请输入用户名和密码！") );
			( (CEdit *)GetDlgItem(TXT_USERNAME) )->SetFocus();
			return TRUE;
		}

		this->SendMessageW(WM_SYSCOMMAND, SC_MINIMIZE);
		this->ShowWindow( SW_HIDE );

		( (CEdit *)GetDlgItem(TXT_USERNAME) )->EnableWindow( FALSE );
		( (CEdit *)GetDlgItem(TXT_PASSWORD) )->EnableWindow( FALSE );
		( (CStatic *)GetDlgItem(PIC_CONNECT) )->ShowWindow( SW_HIDE );


		StartOpenVPN( username, password );
		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CMainDialog::StartOpenVPN( CString username, CString password )
{	
	HANDLE hOutputReadTmp = NULL;
	HANDLE hInputWriteTmp = NULL;

	HANDLE hInputRead = NULL;
	HANDLE hInputWrite = NULL;

	HANDLE hOutputRead = NULL;
	HANDLE hOutputWrite = NULL;

	HANDLE exit_event;
	int iLoginResult;

	SECURITY_ATTRIBUTES sa;
	SECURITY_DESCRIPTOR sd;

	STARTUPINFO start_info;
	CAppOptions *appOptions = CAppOptions::GetSingleInstance();
	

	wchar_t commandline[MAX_PATH];
	wchar_t outline[MAX_LINELEN];

	memset( &(start_info), 0, sizeof(start_info) );
	memset( &(appOptions->proc_info), 0, sizeof(appOptions->proc_info) );
	memset( &(sa), 0, sizeof(sa) );
	memset( &(sd), 0, sizeof(sd) );

	m_tind.uTimeout = 30000;
	m_tind.dwInfoFlags = NIIF_INFO;
	wcscpy_s( m_tind.szInfoTitle, _T("提示") );
	wcscpy_s( m_tind.szInfo, _T("正在连接，请稍候...") );
	Shell_NotifyIcon( NIM_MODIFY, &m_tind );

	exit_event = CreateEvent( NULL, TRUE, FALSE, _T("openvpn_exit") );

	sa.nLength = sizeof (sa);
	sa.lpSecurityDescriptor = &sd;
	sa.bInheritHandle = TRUE;

	InitializeSecurityDescriptor( &sd, SECURITY_DESCRIPTOR_REVISION );
	
	SetSecurityDescriptorDacl( &sd, TRUE, NULL, FALSE );
	CreatePipe( &hInputRead, &hInputWriteTmp, &sa, 0 );
	CreatePipe( &hOutputReadTmp, &hOutputWrite, &sa, 0 );
	DuplicateHandle( GetCurrentProcess(), hOutputReadTmp, GetCurrentProcess(), &hOutputRead, 0, FALSE, DUPLICATE_SAME_ACCESS );
	DuplicateHandle( GetCurrentProcess(), hInputWriteTmp, GetCurrentProcess(), &hInputWrite, 0, FALSE, DUPLICATE_SAME_ACCESS );
	 
	GetStartupInfo(&start_info);
	start_info.cb = sizeof(start_info);
	start_info.dwFlags = STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
	start_info.wShowWindow = SW_HIDE;
	start_info.hStdInput = hInputRead;
	start_info.hStdOutput = hOutputWrite;
	start_info.hStdError = hOutputWrite;

	swprintf_s( commandline, _T("openvpn --config %s"), appOptions->config_file );

	CreateProcess(appOptions->vpn_path, commandline, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL,appOptions->link_path, &start_info, &appOptions->proc_info);

	iLoginResult = Login( hInputWriteTmp, hOutputRead, outline, username, password );

	if( iLoginResult == 1 )
	{
		m_tind.uTimeout = 5000;
		m_tind.dwInfoFlags = NIIF_INFO;
		wcscpy_s( m_tind.szInfoTitle, _T("提示") );
		wcscpy_s( m_tind.szInfo, _T("连接成功！") );
		Shell_NotifyIcon( NIM_MODIFY, &m_tind );
	} 
	else if ( iLoginResult == 0 )
	{
		m_tind.dwInfoFlags = NIIF_INFO;
		wcscpy_s( m_tind.szInfoTitle, _T("") );
		wcscpy_s( m_tind.szInfo, _T("") );
		Shell_NotifyIcon( NIM_MODIFY, &m_tind );

		SendMessageW(WM_SYSCOMMAND, SC_RESTORE);	
		ShowWindow( SW_SHOW );
		SetForegroundWindow();

		AfxMessageBox( _T("用户名或密码错误！") );
	}

	CloseHandle ( appOptions->proc_info.hThread );
	CloseHandle (hOutputWrite);
	CloseHandle (hInputRead);
	CloseHandle(exit_event);
}/* void StartOpenVPN( CString username, CString password ); */

int CMainDialog::Login( HANDLE hStdIn, HANDLE hStdOut, wchar_t *line, CString username, CString password )
{	
	CHAR lpBuffer[MAX_LINELEN];
	char *p;	
	DWORD nBytesRead = 0, nCharsWritten= 0;
	size_t wcConverted = 0, wcLastLine = 0;;

	while(true)
	{
		memset(lpBuffer, 0, sizeof(lpBuffer) );
		if ( !ReadFile( hStdOut, lpBuffer, sizeof(lpBuffer) - 1, &nBytesRead, NULL ) || !nBytesRead )
		{
			AfxMessageBox( _T("程序初始化失败，请联系管理员！\nmainDialog.cpp:line:315") );
			return (-1);
		}		

		lpBuffer[nBytesRead] = '\0';
		p=strchr(lpBuffer, '\n');
	
		if( p == NULL ) 
			p = lpBuffer;
		else
		{
			while ( p != NULL )
			{
				wcLastLine = (p - lpBuffer + 1);
				p = strchr(p+1, '\n');			
			}
			p = &lpBuffer[wcLastLine];
		}/* if( p == NULL ); */
		
		mbstowcs_s(&wcConverted, line, (strlen(p) + 1), p, _TRUNCATE); 		

	   if( strncmp(p, "Enter Auth Username:", 22) == 0 )
		{
			WriteFile( hStdIn, _com_util::ConvertBSTRToString(username.GetBuffer(username.GetLength())), username.GetLength(), &nCharsWritten, NULL );
			password.ReleaseBuffer();
		}

		if( strncmp(p, "Enter Auth Password:", 22) == 0 )
		{
			WriteFile( hStdIn, _com_util::ConvertBSTRToString(password.GetBuffer(username.GetLength())), password.GetLength(), &nCharsWritten, NULL );
			password.ReleaseBuffer();
		}

		// 连接成功
		p = strstr( lpBuffer, "Initialization Sequence Completed" );

		if ( p != NULL ) return 1;

		// 用户名或密码错误
		p = strstr( lpBuffer, "AUTH_FAILED" );

		if ( p != NULL ) return 0;

		::Sleep(2000);
	}/* while(true); */	
}/* int Login( HANDLE hStdIn, HANDLE hStdOut, wchar_t *line, CString username, CString password ) */


BEGIN_MESSAGE_MAP(CMainDialog, CDialog)
	ON_WM_PAINT()
	ON_WM_NCLBUTTONUP()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCRBUTTONUP()
	ON_WM_NCMOUSEMOVE()
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()