#include "stdafx.h"
#include "options.h"

class CMainApp : public CWinApp
{
public:
	GdiplusStartupInput m_gdiplusStartupInput;
	ULONG_PTR m_pGdiToken;

	virtual BOOL InitInstance();
};