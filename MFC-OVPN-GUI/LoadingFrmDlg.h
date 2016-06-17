#pragma once


// LoadingFrmDlg 对话框

class LoadingFrmDlg : public CDialogEx
{
	DECLARE_DYNAMIC(LoadingFrmDlg)

public:
	LoadingFrmDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~LoadingFrmDlg();


// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = LoadingFrm };
#endif
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);

private:
	bool m_blLoaded;
	static const DWORD WM_DOINIT;

protected:
	afx_msg LRESULT OnDoinit(WPARAM wParam, LPARAM lParam);
};
