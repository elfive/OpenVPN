#include "stdafx.h"
#include "resource.h"

class CPopupmenuDialog : public CDialog
{
public:
	CRgn m_rgn;

	CPopupmenuDialog( CWnd *pParentWnd = NULL );
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnMouseMove( UINT nFlags, CPoint point );
	afx_msg void OnLButtonUp( UINT nFlags, CPoint point );
	DECLARE_MESSAGE_MAP()
};