#include "stdafx.h"
#include "mainApp.h"
#include "mainWnd.h"

BOOL CMainApp::InitInstance()
{  
	m_pMainWnd = new CMainWnd();	

	GdiplusStartup(&m_pGdiToken,&m_gdiplusStartupInput,NULL);
	return TRUE;
}