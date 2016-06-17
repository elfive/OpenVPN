#include "stdafx.h"
#include "resource.h"

#define MAX_LINELEN 2048

class CMainDialog : public CDialog
{
public:	
	CRgn   m_rgn;
	CFont m_font;

	CMainDialog( CWnd *pParentWnd = NULL );

	virtual BOOL OnInitDialog();	
	afx_msg void OnPaint();
	afx_msg void OnNcLButtonUp( UINT nFlags, CPoint point );
	afx_msg void OnNcLButtonDown( UINT nFlags, CPoint point );
	afx_msg void OnNcRButtonUp( UINT nFlags, CPoint point );
	afx_msg void OnNcMouseMove( UINT nHitTest, CPoint point );

	BOOL PreTranslateMessage( MSG *pMsg );

	LRESULT OnNcHitTest( CPoint point );

	DECLARE_MESSAGE_MAP();

private:
	void StartOpenVPN( CString username, CString password );

	// µÇÂ¼
	int Login( HANDLE hStdIn, HANDLE hStdOut, wchar_t *line, CString username, CString password );
};