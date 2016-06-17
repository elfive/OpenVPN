#include "mainApp.h"
#include "mainWnd.h"
#include "resource.h"

CMainApp mainApp;
NOTIFYICONDATA m_tind;

CMainWnd::CMainWnd()
{
	m_popupmenuDialog = NULL;
	m_mainDialog = NULL;	
	Create( NULL, _T("MyOpenVPN") );		
}

int CMainWnd::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	m_tind.cbSize = sizeof(NOTIFYICONDATA);
	m_tind.hWnd = this->m_hWnd;
	m_tind.uID = APP_ICON;
	m_tind.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_INFO;
	m_tind.uCallbackMessage = WM_NC;
	m_tind.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(APP_ICON));	
	wcscpy_s(m_tind.szTip, L"MyOpenVPN");

	Shell_NotifyIcon(NIM_ADD, &m_tind); 

	m_mainDialog = new CMainDialog();	
	m_mainDialog->Create( IDD_MAIN );	
	m_mainDialog->SetWindowText( _T("MyOpenVPN") );
	m_mainDialog->ShowWindow(SW_SHOW);

	return 0;
}

LRESULT CMainWnd::OnNotifyIcon(WPARAM wParam, LPARAM lParam)
{
	if( lParam == WM_RBUTTONUP )
	{
		if( m_popupmenuDialog != NULL ) 
		{
			m_popupmenuDialog->ShowWindow( SW_SHOW );
			m_popupmenuDialog->SetFocus();
		}
		else
		{
			m_popupmenuDialog = new CPopupmenuDialog();			
			CPoint point;
			GetCursorPos(&point);		
			m_popupmenuDialog->Create( IDD_POPUPMENU );
			m_popupmenuDialog->SetWindowText( _T("popupMenu") );
			m_popupmenuDialog->MoveWindow(point.x + 2, point.y - 26, 100, 50);			 
			m_popupmenuDialog->ShowWindow( SW_SHOW );			
		}

		// 让主窗口重新获得焦点
		m_mainDialog->SetForegroundWindow();
		AfxGetMainWnd()->SetCapture();
	}/* if( lParam == WM_RBUTTONDOWN ); */

	if( lParam == WM_LBUTTONDBLCLK )
	{
		m_mainDialog->SendMessageW(WM_SYSCOMMAND, SC_RESTORE);	
		m_mainDialog->ShowWindow( SW_SHOW );
		m_mainDialog->SetForegroundWindow();
	}

	return 1; 
}

void CMainWnd::OnLButtonUp( UINT nFlags, CPoint point )
{
	if( m_popupmenuDialog != NULL ) m_popupmenuDialog->ShowWindow( SW_HIDE );

	ReleaseCapture();
}

void CMainWnd::OnLButtonDown( UINT nFlags, CPoint point )
{
	if( m_popupmenuDialog != NULL ) m_popupmenuDialog->ShowWindow( SW_HIDE );

	ReleaseCapture();
}

void CMainWnd::OnRButtonUp( UINT nFlags, CPoint point )
{
	if( m_popupmenuDialog != NULL ) m_popupmenuDialog->ShowWindow( SW_HIDE );

	ReleaseCapture();
}

void CMainWnd::OnRButtonDown( UINT nFlags, CPoint point )
{
	if( m_popupmenuDialog != NULL ) m_popupmenuDialog->ShowWindow( SW_HIDE );

	ReleaseCapture();
}

void CMainWnd::OnMouseMove( UINT nFlags, CPoint point )
{
	m_popupmenuDialog->OnMouseMove( nFlags, point );
}

BEGIN_MESSAGE_MAP( CMainWnd, CFrameWnd )
	ON_WM_CREATE()	
	ON_MESSAGE(WM_NC, OnNotifyIcon)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()