
// MFC-OVPN-GUIDlg.cpp : 实现文件
//

#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "MFC-OVPN-GUI.h"
#include "MFC-OVPN-GUIDlg.h"
#include "afxdialogex.h"
#include "winsock2.h"
#include "ws2tcpip.h"
#include "iphlpapi.h"
#include "stdio.h"
#include "lerror.h"
#include "LTAPManager.h"
#include "LoadingFrmDlg.h"

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//全局变量
CString Server;
CString UserName;
CString UserPassword;
CString Protocol;
CString Port;
CString cvpnPath;
CString configFile;
CString ConfigfolderPath;
CString passFile;
CString CAFile,CAKeyFile;
CString orignal;
ULONGLONG ConnectTimeout;    //连接超时

int ListCount = 0;              //列表项目数的变量，每次重载之后都会刷新
int TotalRouteCount;
int SelectedItemIndex;
CString SelectedItemIP;
CString SelectedItemGateway;
CString SelectedItemMetric;
CString SelectedItemMask;

// CMFCOVPNGUIDlg 对话框

//=========================数据类型转换=========================
CString chartocstring(char* character)
{
	USES_CONVERSION;
	CString str = A2T(character);                 //CString str = A2W(pChar);  也可以
	return str;
}

char* cstringtochar(CString cstring)
{
	char* cstring_char;
	cstring_char = (char*)cstring.GetBuffer();
	return cstring_char;
}

CString dwordtocstring(DWORD Source)                   //将ip格式DWORD文本转化成CString
{
	CString output;
	output.Format(_T("%d.%d.%d.%d"), (Source >> 24) & 0xff, (Source >> 16) & 0xff, (Source >> 8) & 0xff, Source & 0xff);
	return output;
}

DWORD cstringtodword(CString Source)
{
	const wchar_t* wstr = (LPCTSTR)Source;     //一定得是unicode，否则这句话会错的
	char str[20] = { 0 };
	size_t   i;
	wcstombs_s(&i, str, wstr, 20);
	DWORD dwAddress = ntohl(inet_addr(str));
	return dwAddress;
}

TCHAR* StringToChar(CString& str)
{
	int len = str.GetLength();
	TCHAR* cr = str.GetBuffer(len);
	str.ReleaseBuffer();
	return cr;
}

//=========================数据类型转换=========================

int CMFCOVPNGUIDlg::TAPManager()
{
	LTAPManager objTAPManager;
	std::vector<LTAPManager::IF_INFO> vtIFInfo;
	if (err::NOERR != objTAPManager.GetAvailableAdapters(2, vtIFInfo, true))
	{
		MessageBox(_T("创建虚拟接口没有成功"), _T("提示"), MB_ICONWARNING);
		return 1;
	}
	return 0;
}

CString CMFCOVPNGUIDlg::GetModuleDir()
{
	HMODULE module = GetModuleHandle(0);
	wchar_t  pFileName[MAX_PATH];
	GetModuleFileName(module, pFileName, MAX_PATH);

	CString csFullPath(pFileName);
	int nPos = csFullPath.ReverseFind(_T('\\'));
	if (nPos < 0)
		return CString("");
	else
		return csFullPath.Left(nPos);
}

void CMFCOVPNGUIDlg::SetStatusTextBoxVisiable(bool Visiable)
{
	//CEdit *edit = (CEdit*)GetDlgItem(StatusText);
	CEdit *edit = (CEdit*)GetDlgItem(StatusText);
	edit->ShowWindow(Visiable);
	CEdit *edit1 = (CEdit*)GetDlgItem(RouteList);
	edit1->ShowWindow(!Visiable);
	CEdit *edit2 = (CEdit*)GetDlgItem(IPdest_Label);
	edit2->ShowWindow(!Visiable);
	CEdit *edit3 = (CEdit*)GetDlgItem(IPMask_Label);
	edit3->ShowWindow(!Visiable);
	CEdit *edit4 = (CEdit*)GetDlgItem(IPGateway_Label);
	edit4->ShowWindow(!Visiable);
	CEdit *edit5 = (CEdit*)GetDlgItem(IPMetric_Label);
	edit5->ShowWindow(!Visiable);
	CEdit *edit6 = (CEdit*)GetDlgItem(IP_IPdest);
	edit6->ShowWindow(!Visiable);
	CEdit *edit7 = (CEdit*)GetDlgItem(IP_IPMask);
	edit7->ShowWindow(!Visiable);
	CEdit *edit8 = (CEdit*)GetDlgItem(IP_IPGateway);
	edit8->ShowWindow(!Visiable);
	CEdit *edit9 = (CEdit*)GetDlgItem(Edit_IPMetric);
	edit9->ShowWindow(!Visiable);
	CEdit *edit10 = (CEdit*)GetDlgItem(CmdReset);
	edit10->ShowWindow(!Visiable);
	CEdit *edit11 = (CEdit*)GetDlgItem(PermanentCheck);
	edit11->ShowWindow(!Visiable);
	CEdit *edit12 = (CEdit*)GetDlgItem(CmdModifyRoute);
	edit12->ShowWindow(!Visiable);
	CEdit *edit13 = (CEdit*)GetDlgItem(CmdAddRoute);
	edit13->ShowWindow(!Visiable);
	CEdit *edit14 = (CEdit*)GetDlgItem(CmdDeleteRoute);
	edit14->ShowWindow(!Visiable);
	CEdit *edit15 = (CEdit*)GetDlgItem(CmdRefresh);
	edit15->ShowWindow(!Visiable);

	if (Visiable)
	{
		GetDlgItem(Cmd_Detail)->SetWindowText(_T("显示静态路由表"));
		GetDlgItem(Frame2)->SetWindowText(_T("连接状态"));
	}else{
		GetDlgItem(Cmd_Detail)->SetWindowText(_T("显示详情"));
		GetDlgItem(Frame2)->SetWindowText(_T("静态路由表"));
	}

}

CString GetIPFromHostname(CString hostname)
{
	int status;
	WSADATA WSAData;
	char szTemp1[80];
	struct in_addr addr;
	if ((status = WSAStartup(MAKEWORD(1, 1), &WSAData)) == 0)
	{
		CString str_Description = chartocstring(WSAData.szDescription);
		CString str_SystemStatus = chartocstring(WSAData.szSystemStatus);
		//MessageBox(str_Description, str_SystemStatus, MB_OK);
	}
	else {
		CString str_szTemp1 = chartocstring(szTemp1);
		AfxMessageBox(str_szTemp1, MB_OK);
	}
	PHOSTENT phe;

	phe = gethostbyname(cstringtochar(hostname));
	if (phe == NULL) {
		char szTemp[200];
		CString str_szTemp1 = chartocstring(szTemp1);
		//AfxMessageBox(str_szTemp1, MB_OK);
		return _T("域名解析失败");
	}
	else {
		addr.s_addr = *(u_long *)phe->h_addr_list[0];
		CString returnstr = chartocstring(inet_ntoa(addr));
		return returnstr;
	}
}

void CMFCOVPNGUIDlg::Reset(bool ClearList)
{
	if (ClearList)
	{ 
		m_RouteList.DeleteAllItems(); 
	}           //清空列表
	ListCount = 0;                          //重置计数器
	m_IP_IPdest.ClearAddress();
	m_IP_IPGateway.ClearAddress();
	m_IP_IPMask.ClearAddress();
	GetDlgItem(Edit_IPMetric)->SetWindowText(_T(""));
	SelectedItemIP = "";
	SelectedItemGateway = "";
	SelectedItemMetric = "";
	SelectedItemMask = "";
	SelectedItemIndex = 0;
}

CString ExecuteCmd(CString str)
{
	SECURITY_ATTRIBUTES sa;
	HANDLE hRead, hWrite;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	if (!CreatePipe(&hRead, &hWrite, &sa, 0))
	{
		AfxMessageBox(_T("管道创建失败，操作终止!"),MB_OK);
		return NULL;
	}
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	si.hStdError = hWrite;
	si.hStdOutput = hWrite;
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	TCHAR* cmdline = StringToChar(str);
	if (!CreateProcess(NULL, cmdline, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi))
	{
		AfxMessageBox(_T("管道进程创建失败，操作终止!"), MB_OK);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return NULL;
	}
	CloseHandle(hWrite);

	char buffer[4096];
	memset(buffer, 0, 4096);
	CString output;
	DWORD byteRead;
	while (true)
	{
		if (ReadFile(hRead, buffer, 4095, &byteRead, NULL) == NULL)
		{
			break;
		}
		output += buffer;
	}
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return output;
}      

bool CMFCOVPNGUIDlg::Writeovpnconfig(CString path)                //写入文件
{
	if (!PathIsDirectory(ConfigfolderPath))                        //判断文件夹是否存在，这里其实不需要创建文件夹，为了以后能够直接解压资源，还是写上
	{
		if (!CreateDirectory(ConfigfolderPath, NULL))              //创建文件夹是否成功
		{                                                          //路径创建失败
			MessageBox(_T("  路径：\n" + ConfigfolderPath + "创建失败\n\n  请检查权限是否正确"), _T("错误"), MB_OK + MB_ICONERROR);
			return false;
		}
	}
	char* fileName;
	fileName = (char*)path.GetBuffer();
	CStdioFile myFile;
	CFileException fileException;
	if (myFile.Open((LPCTSTR)fileName, CFile::typeText | CFile::modeCreate | CFile::modeReadWrite | CFile::modeNoInherit), &fileException)
	{
		myFile.WriteString(_T("client\n"));
		myFile.WriteString(_T("dev tun\n"));
		myFile.WriteString(_T("proto " + Protocol + "\n"));
		myFile.WriteString(_T("remote " + Server +_T(" ")+ Port + "\n"));
		myFile.WriteString(_T("ca ca.crt\n"));
		myFile.WriteString(_T("keepalive 10 120\n"));
		myFile.WriteString(_T("cipher AES-256-CBC\n"));
		myFile.WriteString(_T("auth SHA1\n"));
		myFile.WriteString(_T("auth-user-pass pass.txt\n"));
		myFile.WriteString(_T("nobind\n"));
		myFile.WriteString(_T("persist-key\n"));
		myFile.WriteString(_T("auth-nocache\n"));
		myFile.WriteString(_T("verb 5\n"));
		//myFile.WriteString(_T("up up.bat\n"));

	}
	else
	{
		TRACE(_T("无法写入ovpn配置文件 %s,错误信息：%u\n"), path, fileException.m_cause);
	}
	return true;
}

bool CMFCOVPNGUIDlg::WriteTxtPassFile(CString path)                //写入文件
{
	if (!PathIsDirectory(ConfigfolderPath))                        //判断文件夹是否存在，这里其实不需要创建文件夹，为了以后能够直接解压资源，还是写上
	{
		if (!CreateDirectory(ConfigfolderPath, NULL))              //创建文件夹是否成功
		{                                                          //路径创建失败
			MessageBox(_T("  路径：\n" + ConfigfolderPath + "创建失败\n\n  请检查权限是否正确"), _T("错误"), MB_OK + MB_ICONERROR);
			return false;
		}
	}
	char* fileName;
	fileName = (char*)path.GetBuffer();
	CStdioFile myFile;
	CFileException fileException;
	if (myFile.Open((LPCTSTR)fileName, CFile::typeText | CFile::modeCreate | CFile::modeReadWrite | CFile::modeNoInherit), &fileException)
	{
		myFile.WriteString(UserName + _T("\n"));
		myFile.WriteString(UserPassword);
	}
	else
	{
		TRACE(_T("无法写入ovpn密码文件 %s,错误信息：%u\n"), path, fileException.m_cause);
	}
	return true;
}

//=========================编辑路由表=========================

void CMFCOVPNGUIDlg::CmdDeleteRoute_Click()                  //删除路由项
{
	if (SelectedItemIndex == 0) 
	{
		MessageBox(_T("请选择一个删除项目！"), _T("提示"),MB_OK + MB_ICONERROR);
		return;
	}
	int res;
	CString index;
	index.Format(_T("%d"), SelectedItemIndex+1);
	res = MessageBox(_T("是否要删除第 "+ index +_T(" 项，目标网络为： ") + SelectedItemIP + _T(" 的路由表项目？")), _T("删除确认"), MB_YESNOCANCEL + MB_ICONQUESTION);
	if(res == IDYES){
		CString cmd;
		cmd = _T("route delete ")+SelectedItemIP;
		CString Result = ExecuteCmd(cmd);
		GetRouteTable(false);
		MessageBox(Result,_T("提示"),MB_OK);
	}
}

void CMFCOVPNGUIDlg::CmdModifyRoute_Click()                  //修改路由项
{
	if (SelectedItemIndex == 0)
	{
		MessageBox(_T("请选择一个删除项目！"), _T("提示"), MB_OK + MB_ICONERROR);
		return;
	}

	CString Metric;
	GetDlgItem(Edit_IPMetric)->GetWindowText(Metric);
	int int_TmpMetric = _wtoi(Metric.GetBuffer());
	if (int_TmpMetric<1 || int_TmpMetric >9999) 
	{
		MessageBox(_T("请输入一个介于1至9999之间的跃点数！"), _T("提示"), MB_OK + MB_ICONERROR);
		return;
	}
	if (m_IP_IPdest.IsBlank() || m_IP_IPGateway.IsBlank() || Metric == "")
	{
		MessageBox(_T("填写完整需要修改的路由项目！"),_T("提示"),MB_OK+MB_ICONERROR);
		return;
	}else{
		CString IP, Gateway,Mask;
		DWORD DW_IP, DW_Gateway,DW_Mask;
		m_IP_IPdest.GetAddress(DW_IP);                   //将IP Control中的DWORD数据类型转换成CString
		m_IP_IPGateway.GetAddress(DW_Gateway);
		m_IP_IPMask.GetAddress(DW_Mask);

		IP = dwordtocstring(DW_IP);
		Gateway = dwordtocstring(DW_Gateway);
		Mask = dwordtocstring(DW_Mask);

		CString cmd;

		cmd = _T("Route change ") + SelectedItemIP + _T(" mask ") + SelectedItemMask + " " + Gateway + _T(" metric ") + Metric;		//只能修改网关和跃点数
		//需要对跃点数进行判断，判断是否在1-9999之间。

		CString Result = ExecuteCmd(cmd);
		GetRouteTable(false);
		MessageBox(Result, _T("提示"), MB_OK);
	}
}

void CMFCOVPNGUIDlg::CmdAddRoute_Click()
{

	// TODO: 在此添加控件通知处理程序代码

	if (m_IP_IPdest.IsBlank() || m_IP_IPGateway.IsBlank() || m_IP_IPMask.IsBlank())
	{
		MessageBox(_T("填写完整需要增加的路由项目！"), _T("提示"), MB_OK + MB_ICONERROR);
		return;
	}else{
		CString IP, Gateway, Metric, Mask;
		CString Cmd_Permanent,Cmd_Mask;
		DWORD DW_IP, DW_Gateway, DW_Mask;

		m_IP_IPdest.GetAddress(DW_IP);                   //将IP Control中的DWORD数据类型转换成CString
		m_IP_IPGateway.GetAddress(DW_Gateway);
		m_IP_IPMask.GetAddress(DW_Mask);

		IP = dwordtocstring(DW_IP);
		Gateway = dwordtocstring(DW_Gateway);
		Mask = dwordtocstring(DW_Mask);

		GetDlgItem(Edit_IPMetric)->GetWindowText(Metric);
		int int_TmpMetric = _wtoi(Metric.GetBuffer());
		if (int_TmpMetric<1 || int_TmpMetric >9999)
		{
			MessageBox(_T("请输入一个介于1至9999之间的跃点数！"), _T("提示"), MB_OK+ MB_ICONERROR);
			return;
		}
		CString cmd;
		//只能修改网关和跃点数
		//Route change 157.0.0.0 mask 255.0.0.0 157.55.80.5 metric 2 if 2

		CButton* pBtn = (CButton*)GetDlgItem(PermanentCheck);
		int state = pBtn->GetCheck();               //返回1代表被选中，返回0表示没被选中
		if (state == 1) 
		{
			Cmd_Permanent = " -p";
		}else if(state==0)
		{
			Cmd_Permanent = "";
		}

		if (Metric == "") 
		{
			Cmd_Mask = "";
		}else{
			Cmd_Mask = _T(" metric ") + Metric;
			//需要对跃点数进行判断，判断是否在1-9999之间。
		}

		cmd = _T("Route") + Cmd_Permanent + _T(" add ") + IP + _T(" mask ") + Mask + " " + Gateway + " " + Cmd_Mask;
		CString Result = ExecuteCmd(cmd);
		GetRouteTable(false);
		MessageBox(Result, _T("提示"), MB_OK + MB_ICONINFORMATION);
	}

}

//=========================编辑路由表=========================

//=========================获取路由表=========================
CString CMFCOVPNGUIDlg::GetLocalIp()                  //获取本机内网ip地址
{
	WORD wVersionRequested;
	WSADATA wsaData;
	char name[255];
	CString ip;
	PHOSTENT hostinfo;
	wVersionRequested = MAKEWORD(2, 0);

	if (WSAStartup(wVersionRequested, &wsaData) == 0)
	{
		if (gethostname(name, sizeof(name)) == 0)
		{
			if ((hostinfo = gethostbyname(name)) != NULL)
			{
				ip = inet_ntoa(*(struct in_addr *)*hostinfo->h_addr_list);
			}
		}
		WSACleanup();
	}
	return ip;
}

void CMFCOVPNGUIDlg::SetAutoWidth()                  //自动列宽
{
	m_RouteList.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);//第一个参数是列索引
	m_RouteList.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
	m_RouteList.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);
	m_RouteList.SetColumnWidth(3, LVSCW_AUTOSIZE_USEHEADER);
	m_RouteList.SetColumnWidth(4, LVSCW_AUTOSIZE_USEHEADER);
	m_RouteList.SetColumnWidth(5, LVSCW_AUTOSIZE_USEHEADER);
}

void CMFCOVPNGUIDlg::AddtoList(CString _DestIp, CString _MaskIp, CString _GatewayIp, CString _ForwardType, CString _ForwardMetric)
{
	//序号、网络目标、网络掩码、网关、接口、跃点数
	CString StrIndex;
	StrIndex.Format(_T("%d"), ListCount+1);
	m_RouteList.InsertItem(ListCount, StrIndex);
	m_RouteList.SetItemText(ListCount, 1, _DestIp);
	m_RouteList.SetItemText(ListCount, 2, _MaskIp);
	m_RouteList.SetItemText(ListCount, 3, _GatewayIp);
	m_RouteList.SetItemText(ListCount, 4, _ForwardType);
	m_RouteList.SetItemText(ListCount, 5, _ForwardMetric);
	ListCount++;                   //计数器+1
}

CString CMFCOVPNGUIDlg::GetRouteTable(bool OnlyFirst=false)
{
	Reset(true);

	// Declare and initialize variables.

	/* variables used for GetIfForwardTable */
	PMIB_IPFORWARDTABLE pIpForwardTable;
	DWORD dwSize = 0;
	DWORD dwRetVal = 0;

	char szDestIp[128];
	char szMaskIp[128];
	char szGatewayIp[128];

	struct in_addr IpAddr;

	int i;
	pIpForwardTable =(MIB_IPFORWARDTABLE *)MALLOC(sizeof(MIB_IPFORWARDTABLE));
	if (pIpForwardTable == NULL) {
		return NULL;       //分配内存错误，返回NULL
	}

	if (GetIpForwardTable(pIpForwardTable, &dwSize, 0) ==
		ERROR_INSUFFICIENT_BUFFER) {
		FREE(pIpForwardTable);
		pIpForwardTable = (MIB_IPFORWARDTABLE *)MALLOC(dwSize);
		if (pIpForwardTable == NULL) {
			return NULL;       //分配内存错误，返回NULL
		}
	}

	if ((dwRetVal = GetIpForwardTable(pIpForwardTable, &dwSize, 0)) == NO_ERROR) {

		TotalRouteCount = (int)pIpForwardTable->dwNumEntries;           //路由项总数

		for (i = 0; i < (int)pIpForwardTable->dwNumEntries; i++) {
			char IPdotdec[20];
			IpAddr.S_un.S_addr = (u_long)pIpForwardTable->table[i].dwForwardDest;
			inet_ntop(AF_INET, &IpAddr, IPdotdec, 16);
			strcpy_s(szDestIp, sizeof(szDestIp), IPdotdec);
			IpAddr.S_un.S_addr = (u_long)pIpForwardTable->table[i].dwForwardMask;
			inet_ntop(AF_INET, &IpAddr, IPdotdec, 16);
			strcpy_s(szMaskIp, sizeof(szMaskIp), IPdotdec);
			IpAddr.S_un.S_addr = (u_long)pIpForwardTable->table[i].dwForwardNextHop;
			inet_ntop(AF_INET, &IpAddr, IPdotdec, 16);
			strcpy_s(szGatewayIp, sizeof(szGatewayIp), IPdotdec);

			CString Str_DestIp;
				Str_DestIp = szDestIp;
			CString Str_MaskIp;
				Str_MaskIp = szMaskIp;
			CString Str_GatewayIp;
				Str_GatewayIp = szGatewayIp;
			CString StrMetric;
				StrMetric.Format(_T("%ld"), pIpForwardTable->table[i].dwForwardMetric1);
			CString StrForwardType;
				StrForwardType.Format(_T("%ld"), pIpForwardTable->table[i].dwForwardType);

				 //是否显示在链路上
				
				if (StrForwardType == "3")
				{
					StrForwardType = (_T("在链路上"));
				}else if(StrForwardType == "4"){
					StrForwardType = GetLocalIp();
				}
				
			CString StrForwardAge;
				StrForwardAge.Format(_T("%ld"), pIpForwardTable->table[i].dwForwardAge);
				if (OnlyFirst) 
				{
					//MessageBox(StrForwardType);
					return StrForwardType;
				}
			AddtoList(
				Str_DestIp,
				Str_MaskIp,
				Str_GatewayIp,
				StrForwardType,
				StrMetric);
		}

		SetAutoWidth();

		FREE(pIpForwardTable);
		return 0;                //获取路由表成功，返回0
	}
	else {
		FREE(pIpForwardTable);
		return NULL;                 //获取路由表失败，返回1
	}

}
//=========================获取路由表=========================

//=========================子线程=========================
UINT __cdecl ConnecttoOVPNThread(LPVOID pParm)
{
	bool haveshowedmessagebox = false;
	CMFCOVPNGUIDlg *dlg = (CMFCOVPNGUIDlg*)pParm;
	SECURITY_ATTRIBUTES sa;
	HANDLE hRead, hWrite;

	//CMD_Str = _T("openvpn --config \"") + ConfigfolderPath + _T("\\client1.ovpn\"  --log \"") + ConfigfolderPath + _T("\\Client.log\" --auth-retry interact --management 127.0.0.1 25340 stdin --management-query-passwords --management-hold");
	CString CMD_Str = _T("openvpn --config \"") + ConfigfolderPath + _T("\\client1.ovpn\" --script-security 3  --management 127.0.0.1 25340");
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	if (!CreatePipe(&hRead, &hWrite, &sa, 0))
	{
		AfxMessageBox(_T("管道创建失败，操作终止!"), MB_OK | MB_ICONERROR);
		return 0;
	}
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	si.hStdError = hWrite;
	si.hStdOutput = hWrite;
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	int len = CMD_Str.GetLength();
	TCHAR* cmdline = CMD_Str.GetBuffer(len);
	CMD_Str.ReleaseBuffer();
	if (!CreateProcess(NULL, cmdline, NULL, NULL, TRUE, NULL, NULL, ConfigfolderPath, &si, &pi))
	{
		AfxMessageBox(_T("管道进程创建失败，操作终止!"), MB_OK | MB_ICONERROR);
		return 0;
	}
	CloseHandle(hWrite);

	char buffer[51200];
	memset(buffer, 0, 51200);
	CString output;
	DWORD byteRead;
	ULONGLONG Start_Time = GetTickCount64();                //记录下程序开始连接的启动时间（ms）
	while (true)
	{
		/*ULONGLONG During_Time = GetTickCount64() - Start_Time;                //记录下程序从启动到目前的CPU时间（ms）用于超时检测。
		if (During_Time > ConnectTimeout)       //如果超时还没连接上
		{
			ShellExecute(NULL, _T("open"), _T("taskkill.exe"), _T("/f /im openvpn.exe"), NULL, SW_HIDE);
			dlg->m_Ctl_CmdConnect.SetWindowText(_T("连  接"));//使用control变量
		}
		*/
		CString tmp;
		dlg->m_Ctl_StatusText.GetWindowText(tmp);
		if (tmp == "连  接") { return 1; }
		if (ReadFile(hRead, buffer, 51199, &byteRead, NULL) == NULL)
		{
			return 1;
		}
		output += buffer;
		//Sleep(1000);
		dlg->m_StatusText.Format(_T("%s"),output);
		dlg->m_Ctl_StatusText.SetWindowText(dlg->m_StatusText);//使用control变量
		dlg->m_Ctl_StatusText.LineScroll(dlg->m_Ctl_StatusText.GetLineCount());
		CString FindStr = _T("Successful ARP Flush on interface");

		//if (orignal != dlg->GetRouteTable(true) && !haveshowedmessagebox)       
			if ((orignal != dlg->GetRouteTable(true) || output.Find(FindStr) != -1) && !haveshowedmessagebox)                    //修改判定条件
		{
			dlg->m_Ctl_CmdConnect.SetWindowText(_T("断  开"));//使用control变量
			haveshowedmessagebox = true;
			AfxMessageBox(_T("连接成功！"), MB_OK + MB_ICONINFORMATION);
		}
	}
	//接判断监测openvpn.exe是否退出的代码
	// 等待子进程的退出
	WaitForSingleObject(pi.hProcess, INFINITE);
	// 获取子进程的退出码
	DWORD dwExitCode;
	GetExitCodeProcess(pi.hProcess, &dwExitCode);
	PostMessage(dlg->m_hWnd, WM_UPDATEDATA, FALSE, FALSE);//发送给主进程更新按钮标题变更的消息
	// 关闭子进程句柄
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return 1;
}

UINT __cdecl ServerTextTread(LPVOID pParm)
{
	CMFCOVPNGUIDlg *dlg = (CMFCOVPNGUIDlg*)pParm;
	CString tmp;
	dlg->GetDlgItem(ServerText)->GetWindowText(tmp);
	tmp = GetIPFromHostname(tmp);           //调用GetIPFromHostname
	dlg->m_ServerIPText = tmp;
	dlg->m_Ctl_ServerIPText.SetWindowText(dlg->m_ServerIPText);//使用control变量
	return 1;
}
//=========================子线程=========================

CMFCOVPNGUIDlg::CMFCOVPNGUIDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFCOVPNGUI_DIALOG, pParent)
	, m_StatusText(_T(""))
	, m_ServerIPText(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCOVPNGUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, RouteList, m_RouteList);
	DDX_Control(pDX, IP_IPdest, m_IP_IPdest);
	DDX_Control(pDX, IP_IPGateway, m_IP_IPGateway);
	DDX_Control(pDX, ProtocolList, m_ProtocolList);
	DDX_Control(pDX, IP_IPMask, m_IP_IPMask);
	//  DDX_Control(pDX, StatusText, m_StatusText);
	DDX_Text(pDX, StatusText, m_StatusText);
	DDX_Control(pDX, StatusText, m_Ctl_StatusText);
	DDX_Control(pDX, ServerIPText, m_Ctl_ServerIPText);
	DDX_Text(pDX, ServerIPText, m_ServerIPText);
	DDX_Control(pDX, CmdConnect, m_Ctl_CmdConnect);
}

BEGIN_MESSAGE_MAP(CMFCOVPNGUIDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(CmdConnect, &CMFCOVPNGUIDlg::Connect_Click)
	ON_BN_CLICKED(CmdDeleteRoute, &CMFCOVPNGUIDlg::CmdDeleteRoute_Click)
	ON_NOTIFY(NM_CLICK, RouteList, &CMFCOVPNGUIDlg::RouteList_Click)
	ON_BN_CLICKED(CmdModifyRoute, &CMFCOVPNGUIDlg::CmdModifyRoute_Click)
	ON_BN_CLICKED(CmdAddRoute, &CMFCOVPNGUIDlg::CmdAddRoute_Click)
	ON_BN_CLICKED(CmdRefresh, &CMFCOVPNGUIDlg::CmdRefresh_Click)
	ON_CBN_SELCHANGE(ProtocolList, &CMFCOVPNGUIDlg::ProtocolList_Change)
	ON_BN_CLICKED(CmdReset, &CMFCOVPNGUIDlg::CmdReset_Click)
	ON_BN_CLICKED(Cmd_Detail, &CMFCOVPNGUIDlg::Cmd_Detail_Click)

END_MESSAGE_MAP()

// CMFCOVPNGUIDlg 消息处理程序

BOOL CMFCOVPNGUIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//载入安装驱动界面
	LoadingFrmDlg objLoadDlg;
	objLoadDlg.DoModal();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//窗体固定大小
	ModifyStyle(m_hWnd, WS_THICKFRAME, 0, 0);

	//路径初始化
	cvpnPath = GetModuleDir();
	ConfigfolderPath = cvpnPath + _T("\\config");
	configFile = ConfigfolderPath +  _T("\\client1.ovpn");
	CAFile = ConfigfolderPath + _T("\\ca.crt");
	CAKeyFile = ConfigfolderPath + _T("\\ca.key");
	passFile = ConfigfolderPath + _T("\\pass.txt");

	//初始化全局连接所需变量
	Server = _T("ros0001.9966.org");
	Protocol = _T("tcp");
	Port = _T("1987");
	UserName = _T("cnkiovpn");
	UserPassword = _T("cnkiovpn");

	//初始服务器地址
	GetDlgItem(usernameText)->SetWindowText(_T("cnki"));
	GetDlgItem(userpasswordText)->SetWindowText(_T("cnki2"));

	//初始服务器地址
	GetDlgItem(ServerText)->SetWindowText(_T("ros0001.9966.org"));

	//初始化端口号设置
	GetDlgItem(PortText)->SetWindowText(_T("2016"));

	//初始化combo box控件
	((CComboBox*)GetDlgItem(ProtocolList))->ResetContent();//消除现有所有内容
	((CComboBox*)GetDlgItem(ProtocolList))->AddString(_T("udp"));
	((CComboBox*)GetDlgItem(ProtocolList))->AddString(_T("tcp"));
	((CComboBox*)GetDlgItem(ProtocolList))->SetCurSel(0);
	Protocol = "tcp";

	//初始化List Control
	m_RouteList.SetExtendedStyle(m_RouteList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_RouteList.InsertColumn(0, _T("序号"), LVCFMT_LEFT, 40, - 1);
	m_RouteList.InsertColumn(1, _T("目标网络"), LVCFMT_LEFT, 120, -1);
	m_RouteList.InsertColumn(2, _T("网络掩码"), LVCFMT_LEFT, 120, -1);
	m_RouteList.InsertColumn(3, _T("网关"), LVCFMT_LEFT, 120, -1);
	m_RouteList.InsertColumn(4, _T("接口"), LVCFMT_LEFT, 60, -1);
	m_RouteList.InsertColumn(5, _T("跃点数"), LVCFMT_LEFT, 60, -1);

	//初始化连接超时设置
	GetDlgItem(TimeoutText)->SetWindowText(_T("60000"));

	if (GetRouteTable() == 1) 
	{
		MessageBox(_T("路由表获取失败！请刷新重试。"), _T("错误"), MB_OK);
	}

	SetStatusTextBoxVisiable(true);

	ShellExecute(NULL, _T("open"), _T("taskkill.exe"), _T("/f /im openvpn.exe"), NULL, SW_HIDE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}
//禁止从窗口响应回车和ESC退出
BOOL CMFCOVPNGUIDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) { return TRUE; }
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) { return TRUE; }
	else
		return CDialog::PreTranslateMessage(pMsg);
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCOVPNGUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。

HCURSOR CMFCOVPNGUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMFCOVPNGUIDlg::Connect_Click()
{

	orignal = GetRouteTable(true);
	CString caption;
	GetDlgItem(CmdConnect)->GetWindowText(caption);


	if(caption=="连  接")                                                 //建立连接
	{
		// TODO: 在此添加控件通知处理程序代码
		CString TmpServer, TmpUsername, TmpPassword,TmpPort,TmpTimeout;

		GetDlgItem(ServerText)->GetWindowText(TmpServer);
		GetDlgItem(PortText)->GetWindowText(TmpPort);
		GetDlgItem(usernameText)->GetWindowText(TmpUsername);
		GetDlgItem(userpasswordText)->GetWindowText(TmpPassword);
		GetDlgItem(TimeoutText)->GetWindowText(TmpTimeout);

		if (TmpServer == "") {
			MessageBox(_T("请输入正确的服务器地址！"), _T("错误"), MB_OK);
			return;
		}
		else {
			char* ip_pointer;
			ip_pointer = (char*)TmpServer.GetBuffer();
			if (!~inet_addr(ip_pointer))                               //判断是不是合法的ip地址，若不是则解析出ip地址
			{
				GetDlgItem(ServerIPText)->SetWindowText(_T("正在解析域名..."));
				m_Host_Thread = AfxBeginThread(ServerTextTread, this);
			}
		}

		int int_TmpPort = _wtoi(TmpPort.GetBuffer());

		if (TmpPort == "" || (int_TmpPort<1 || int_TmpPort>65535)) {
			MessageBox(_T("请输入一个（1-65535之间）正确的端口号！"), _T("错误"), MB_OK);
			return;
		}

		if (TmpUsername == "" || TmpPassword == "") {
			MessageBox(_T("帐号或密码不能为空！"), _T("错误"), MB_OK);
			return;
		}
		GetDlgItem(CmdConnect)->SetWindowText(_T("正在连接"));
		GetDlgItem(StatusText)->SetWindowText(_T("正在准备连接服务器：" + TmpServer + "\n"));

		UserName = TmpUsername;
		UserPassword = TmpPassword;
		Server = TmpServer;
		Port = TmpPort;
		ConnectTimeout = 1000 * atol(cstringtochar(TmpTimeout));
		Writeovpnconfig(configFile);
		WriteTxtPassFile(passFile);

		//ConnecttoOVPN();//单线程的调用，会阻塞主线程，造成程序假死现象，因为OPVN连接之后，openvpn.exe是不会退出的。

		//单线程的调用
		GetRouteTable();
		m_Info.hWnd = m_hWnd;
		m_pThread = AfxBeginThread(ConnecttoOVPNThread, this);

		SetStatusTextBoxVisiable(true);

	}else if (caption == "断  开") {                                      //断开连接
		ShellExecute(NULL, _T("open"), _T("taskkill.exe"), _T("/f /im openvpn.exe"), NULL, SW_HIDE);
		GetDlgItem(CmdConnect)->SetWindowText(_T("连  接"));

		CString tmp;
		GetDlgItem(StatusText)->GetWindowText(tmp);
		tmp += _T("\n用户退出连接！");
		GetDlgItem(StatusText)->SetWindowText(tmp);
		GetRouteTable(false);
	}else if (caption == "正在连接") {
		return;
	}
}

void CMFCOVPNGUIDlg::RouteList_Click(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	CListCtrl*pList = (CListCtrl*)GetDlgItem(RouteList);
	POSITION pos = pList->GetFirstSelectedItemPosition();
	int nItem = pList->GetSelectionMark();
	SelectedItemIndex = nItem;
	SelectedItemIP = m_RouteList.GetItemText(nItem, 1);
	SelectedItemMask = m_RouteList.GetItemText(nItem, 2);
	SelectedItemGateway = m_RouteList.GetItemText(nItem, 3);
	SelectedItemMetric = m_RouteList.GetItemText(nItem, 5);

	//MessageBox(SelectedItemIP);
	*pResult = 0;
	GetDlgItem(Edit_IPMetric)->SetWindowText(SelectedItemMetric);

	//将CString 型IP地址在IPAddressCtrl中显示 
	m_IP_IPdest.SetAddress(cstringtodword(SelectedItemIP));
	m_IP_IPGateway.SetAddress(cstringtodword(SelectedItemGateway));
	m_IP_IPMask.SetAddress(cstringtodword(SelectedItemMask));
}

void CMFCOVPNGUIDlg::CmdRefresh_Click()
{
	// TODO: 在此添加控件通知处理程序代码
	GetRouteTable();

}

void CMFCOVPNGUIDlg::ProtocolList_Change()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ProtocolList.GetLBText(m_ProtocolList.GetCurSel(), Protocol);    //当前选中的字符串

}

void CMFCOVPNGUIDlg::CmdReset_Click()
{
	// TODO: 在此添加控件通知处理程序代码
	Reset(false);
}

void CMFCOVPNGUIDlg::Cmd_Detail_Click()
{
	// TODO: 在此添加控件通知处理程序代码
	CString caption;
	GetDlgItem(Cmd_Detail)->GetWindowText(caption);
	if (caption == "显示详情") 
	{
		SetStatusTextBoxVisiable(true);
		GetDlgItem(Cmd_Detail)->SetWindowText(_T("显示静态路由表"));
	}else{
		SetStatusTextBoxVisiable(false);
		GetRouteTable();
		GetDlgItem(Cmd_Detail)->SetWindowText(_T("显示详情"));
	}
}
