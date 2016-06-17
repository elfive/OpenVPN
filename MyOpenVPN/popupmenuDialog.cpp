#include "popupmenuDialog.h"
#include "options.h"

extern NOTIFYICONDATA m_tind;

CPopupmenuDialog::CPopupmenuDialog(CWnd *pParentWnd) : CDialog( IDD_POPUPMENU, pParentWnd ){}

BOOL CPopupmenuDialog::OnInitDialog()
{
	CRect rc; 
	CPaintDC dc(this);

	// 绘制窗体
	GetClientRect(rc); 
	m_rgn.CreateRoundRectRgn(rc.left,   rc.top,   rc.right,   rc.bottom,   10,   10);
	::SetWindowRgn(GetSafeHwnd(),(HRGN)m_rgn,TRUE);	
	
	return TRUE;
}

void CPopupmenuDialog::OnPaint()
{
	CRect rect; 
    CPaintDC dc(this); 
	CFont font;

	// 绘制对话框背景
    GetClientRect(rect); 	
    dc.FillSolidRect(rect,RGB(246,246,246));	
	
	// 绘制菜单文字信息	
	font.CreatePointFont( 100, _T("黑体") );
	rect.OffsetRect( 33, 11 );
	dc.SelectObject( &font );
	dc.DrawText( _T("退出"), &rect, DT_SINGLELINE );

	ReleaseDC(&dc);	
}

/**
 * 客户区鼠标移动事件
 */
void CPopupmenuDialog::OnMouseMove(UINT nFlags, CPoint point)
{
	CRect rect;
	CClientDC dc(this); 	
	CFont font;
	CBitmap bitmap; 

	GetWindowRect(rect);
	GetCursorPos(&point);

	if ( point.x <= rect.left || point.x >= rect.right || point.y <= rect.top || point.y >= rect.bottom )
	{
		AfxGetMainWnd()->SetCapture();
		return;
	}
	else
	{
		ReleaseCapture();
		ScreenToClient(&point);

	}

	if( point.y > 10 && point.y < 20 && point.x > 8 && point.x < 60 )
	{		
		// 绘制对话框背景
		GetClientRect(rect); 
		dc.FillSolidRect(rect,RGB(52,138,231));

		// 绘制菜单文字信息	
		font.CreatePointFont( 100, _T("黑体") );
		GetClientRect(rect); 
		rect.OffsetRect( 33, 11 );
		dc.SelectObject( &font );
		dc.SetTextColor( RGB(255,255,255) );
		dc.DrawText( _T("退出"), &rect, DT_SINGLELINE );		

		// 绘制菜单图标
		bitmap.LoadBitmapW( BMP_POPUPEXIT_OVER );
		( (CStatic *)GetDlgItem(PIC_EXIT) )->SetBitmap( (HBITMAP)bitmap );
		bitmap.Detach(); 		
	}
	else
	{
		// 绘制对话框背景
		GetClientRect(rect); 
		dc.FillSolidRect(rect,RGB(246,246,246));	
		
		// 绘制菜单文字信息	
		font.CreatePointFont( 100, _T("黑体") );
		rect.OffsetRect( 33, 11 );
		dc.SelectObject( &font );
		dc.DrawText( _T("退出"), &rect, DT_SINGLELINE );

		// 绘制菜单图标
		bitmap.LoadBitmapW( BMP_POPUPEXIT );
		( (CStatic *)GetDlgItem(PIC_EXIT) )->SetBitmap( (HBITMAP)bitmap );
		bitmap.Detach();
	}

	ReleaseDC(&dc);
}

/**
 * 客户区鼠标左键弹起事件
 */
void CPopupmenuDialog::OnLButtonUp(UINT nFlags, CPoint point)
{
		CAppOptions *appOptions = CAppOptions::GetSingleInstance();
		Shell_NotifyIcon(NIM_DELETE, &m_tind);
		TerminateProcess( appOptions->proc_info.hProcess,0 );
		AfxGetMainWnd()->DestroyWindow();
}

BEGIN_MESSAGE_MAP(CPopupmenuDialog, CDialog)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()