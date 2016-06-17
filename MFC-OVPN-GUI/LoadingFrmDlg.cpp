// LoadingFrmDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFC-OVPN-GUI.h"
#include "LoadingFrmDlg.h"
#include "afxdialogex.h"
#include "LTAPManager.h"
#include "lerror.h"

// LoadingFrmDlg 对话框

IMPLEMENT_DYNAMIC(LoadingFrmDlg, CDialogEx)
const DWORD LoadingFrmDlg::WM_DOINIT = WM_USER + 9012;

LoadingFrmDlg::LoadingFrmDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(LoadingFrm, pParent)
	, m_blLoaded(false)
{

}

LoadingFrmDlg::~LoadingFrmDlg()
{
}

void LoadingFrmDlg::DoDataExchange(CDataExchange * pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(LoadingFrmDlg, CDialogEx)
	ON_WM_WINDOWPOSCHANGED()
	ON_MESSAGE(WM_DOINIT, &LoadingFrmDlg::OnDoinit)
END_MESSAGE_MAP()



BOOL LoadingFrmDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void LoadingFrmDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CDialogEx::OnWindowPosChanged(lpwndpos);

	if ((lpwndpos->flags & SWP_SHOWWINDOW) && !m_blLoaded)
	{
		m_blLoaded = true;
		PostMessage(WM_DOINIT);
	}
}

afx_msg LRESULT LoadingFrmDlg::OnDoinit(WPARAM wParam, LPARAM lParam)
{
	GetDlgItem(StatusLabel)->SetWindowText(_T("正在获取网卡信息. . ."));

	LTAPManager objTAPManager;
	std::vector<LTAPManager::IF_INFO> vtIFInfo;
	if (err::NOERR != objTAPManager.GetAvailableAdapters(2, vtIFInfo, true))
	{
		GetDlgItem(StatusLabel)->SetWindowText(_T("创建虚拟网卡失败"));
		MessageBox(_T("创建虚拟网卡失败"), _T("错误"), MB_OK+MB_ICONWARNING);
	}

	GetDlgItem(StatusLabel)->SetWindowText(_T("成功获取到网卡信息"));

	EndDialog(0);

	return 0;
}