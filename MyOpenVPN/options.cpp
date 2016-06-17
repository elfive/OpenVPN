#include "options.h"

CAppOptions *CAppOptions::m_AppOptions = NULL;

CAppOptions::CAppOptions()
{
	InitPath();

	BuildFileList();
}

CAppOptions* CAppOptions::GetSingleInstance()
{
	if( NULL == m_AppOptions ) m_AppOptions = new CAppOptions();  
    return m_AppOptions;  
}

void CAppOptions::InitPath()
{
	GetModuleFileName( NULL, exe_path, MAX_PATH );

	// 计算出程序所在执行目录
	wcsncpy_s( bin_path, exe_path, MAX_PATH );

	for (int i=_tcslen(bin_path) - 1; i > 0; i--)
	{
		if (bin_path[i] == '\\')
		{
			bin_path[i] = '\0';
			break;
		}
		bin_path[i] = '\0';
	}

	// 计算link目录所在路径
	wcsncpy_s( link_path, bin_path, MAX_PATH );

	wcscat_s( link_path, MAX_PATH, _T("\\link") );

	// 计算openvpn程序完整路径
	wcsncpy_s( vpn_path, link_path, MAX_PATH );

	wcscat_s( vpn_path, MAX_PATH, _T("\\openvpn.exe") );
}

void CAppOptions::BuildFileList()
{
	WIN32_FIND_DATA find_obj;
	HANDLE find_handle;
	BOOL more_files;
	wchar_t find_string[MAX_PATH];

	swprintf_s( find_string, _T("%s\\*"), link_path );
	
	find_handle = FindFirstFile (find_string, &find_obj);	

	 // 查找配置文件
	 do{
		if( match (&find_obj, _T("ovpn")) )
		{
			wcsncpy_s( config_file, MAX_PATH, find_obj.cFileName, wcslen(find_obj.cFileName) );
		}/* if (match (&find_obj, o.ext_string)); */
		more_files = FindNextFile (find_handle, &find_obj);
	 }while (more_files);

	 FindClose (find_handle);
}

bool CAppOptions::match (const WIN32_FIND_DATA *find, const wchar_t *ext)
{
  int i;

  if (find->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    return false;

  if (!wcslen (ext))
    return true;

  i = wcslen (find->cFileName) - wcslen (ext) - 1;
  if (i < 1)
    return false;

  return find->cFileName[i] == '.' && !wcscmp (find->cFileName + i + 1, ext);
}

CAppOptions::~CAppOptions()
{  
    if( NULL != m_AppOptions )
    {  
        delete m_AppOptions;  
        m_AppOptions = NULL;  
    }  
}