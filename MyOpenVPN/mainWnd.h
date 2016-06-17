#include "popupmenuDialog.h"
#include "mainDialog.h"

#define WM_NC (WM_USER + 1001)

class CMainWnd : public CFrameWnd
{
public:
	CMainWnd();

	CPopupmenuDialog *m_popupmenuDialog;

protected:	
	CMainDialog *m_mainDialog;

public:
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnLButtonUp( UINT nFlags, CPoint point );
	afx_msg void OnLButtonDown( UINT nFlags, CPoint point );
	afx_msg void OnRButtonUp( UINT nFlags, CPoint point );
	afx_msg void OnRButtonDown( UINT nFlags, CPoint point );
	afx_msg void OnMouseMove( UINT nFlags, CPoint point );
	afx_msg LRESULT OnNotifyIcon( WPARAM wParam,LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};