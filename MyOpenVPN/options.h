#include "stdafx.h"

class CAppOptions
{
public:
	static CAppOptions * GetSingleInstance();

private:
	static CAppOptions *m_AppOptions;
	CAppOptions::CAppOptions();
	CAppOptions::~CAppOptions();

	void InitPath();		// 初始化程序路径

	void BuildFileList();		// 创建字段列表

	// 文件后缀名匹配
	bool match (const WIN32_FIND_DATA *find, const wchar_t *ext);	

public:
	TCHAR exe_path[MAX_PATH];	// 程序执行完整路径

	TCHAR bin_path[MAX_PATH];	// 程序所在完整目录

	TCHAR vpn_path[MAX_PATH];	// openvpn执行路径

	TCHAR link_path[MAX_PATH];	// link文件夹的路径

	TCHAR config_dir[MAX_PATH];	// 程序配置文件目录

	TCHAR config_file[MAX_PATH];// 程序配置文件名称

	PROCESS_INFORMATION proc_info; // openvpn执行线程
};