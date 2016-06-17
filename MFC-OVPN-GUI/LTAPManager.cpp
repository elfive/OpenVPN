/*
* Created on Dec 4, 2015
*
* @author: luodichen
*/

#include "stdafx.h"
#include "LTAPManager.h"
#include "lerror.h"
#include "MFC-OVPN-GUIDlg.h"

#include <atlbase.h>
#include <atlconv.h>

const CString LTAPManager::TARGET_INTERFACE = _T("TAP-Windows Adapter V9");

LTAPManager::LTAPManager()
{

}

LTAPManager::~LTAPManager()
{

}


CString LTAPManager::GetModuleDir()
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

int LTAPManager::DetectFreeAdapter(PMIB_IFROW pTable, bool *pFound)
{
	int ret = err::NOERR;
	*pFound = false;

	ULONG ulSize = 0;
	if (ERROR_INSUFFICIENT_BUFFER != GetIfTable(NULL, &ulSize, TRUE))
		return err::GET_IF_TABLE_FAIL;

	PMIB_IFTABLE pIfTable = (PMIB_IFTABLE)(new char[ulSize]);
	if (NULL == pIfTable)
		return err::MEMORY_ALLOC_FAIL;

	do
	{
		if (NOERROR != GetIfTable(pIfTable, &ulSize, TRUE))
		{
			ret = err::GET_IF_TABLE_FAIL;
			break;
		}

		auto count = pIfTable->dwNumEntries;
		for (int i = 0; i < (int)count; i++)
		{
			CString strDescription(pIfTable->table[i].bDescr);
			if (strDescription.FindOneOf(TARGET_INTERFACE) == -1)
				continue;

			if (pIfTable->table[i].dwOperStatus == IF_OPER_STATUS_NON_OPERATIONAL)
			{
				*pFound = true;
				memcpy((void *)pTable, (void *)&(pIfTable->table[i]), sizeof(MIB_IFROW));

				return ret;
			}
		}

	} while (false);

	delete[](char*)(pIfTable);
	pIfTable = NULL;

	return ret;
}

int LTAPManager::DetectFreeAdapter(std::vector<IF_INFO> &vtAdapters)
{
	int ret = err::NOERR;
	ULONG ulSize = 0;

	vtAdapters.clear();

	if (ERROR_BUFFER_OVERFLOW != GetAdaptersAddresses(AF_INET, 0, NULL, NULL, &ulSize))
		return err::GET_ADAPTER_ADDRESS_FAIL;

	auto pAddresses = (PIP_ADAPTER_ADDRESSES)(new uint8_t[ulSize]);
	if (NULL == pAddresses)
		return err::MEMORY_ALLOC_FAIL;

	do
	{
		if (NO_ERROR != GetAdaptersAddresses(AF_INET, 0, NULL, pAddresses, &ulSize))
		{
			ret = err::GET_ADAPTER_ADDRESS_FAIL;
			break;
		}

		auto pCur = pAddresses;
		while (NULL != pCur)
		{
			CString strDescription = CString(CW2A(pCur->Description));
			if (strDescription.Find(TARGET_INTERFACE) != -1
				&& pCur->OperStatus == IfOperStatusDown)
			{
				IF_INFO info(
					(NET_IFINDEX)(pCur->IfIndex),
					pCur->AdapterName
				);
				vtAdapters.push_back(info);
			}
			pCur = pCur->Next;
		}
	} while (false);

	delete[] pAddresses;

	return ret;
}

int LTAPManager::CreateNewAdapter()
{
	int ret = err::NOERR;
	CString szAppPath = GetModuleDir();
	CString strDriverPath;
	CString strInstaller;
	CString strCommand;


	//获得操作系统版本
	DWORD dwVersion = GetVersion();
	DWORD dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));
	if (dwMajorVersion > 5)          //Win Vista及以上系统
	{
		strDriverPath = szAppPath + _T("\\tap-windows");
		strInstaller = strDriverPath + _T("\\tapinstall.exe");
		strCommand = _T("tapinstall.exe install OemVista.inf tap0901");
	}
	else          //Win XP系统
	{
		strDriverPath = szAppPath + _T("\\tap-windows-xp");
		strInstaller = strDriverPath + _T("\\devcon.exe");
		strCommand = _T("devcon.exe install OemWin2k.inf tap0901");
	}

	STARTUPINFO startupinfo = { 0 };
	startupinfo.cb = sizeof(startupinfo);

	PROCESS_INFORMATION pi = { 0 };

	if (!CreateProcess((strInstaller.GetBuffer()),
		(strCommand.GetBuffer()),
		NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL,
		(strDriverPath.GetBuffer()), &startupinfo, &pi))
	{
		ret = err::CREATE_PROCESS_FAIL;
		return ret;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);

	DWORD dwRetCode = 0;
	if (!GetExitCodeProcess(pi.hProcess, &dwRetCode))
		return err::PROCESS_RETURN_FAILED;

	ret = dwRetCode;

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	return ret;
}

int LTAPManager::GetAvailableAdapter(PMIB_IFROW pTable)
{
	int ret = 0;
	bool bFound = false;

	if (0 != (ret = DetectFreeAdapter(pTable, &bFound)) || bFound)
		return ret;

	if (0 != (ret = CreateNewAdapter()))
		return ret;

	if (0 != (ret = DetectFreeAdapter(pTable, &bFound)) || bFound)
		return ret;

	ret = err::NO_AVAILABLE_ADAPTER;

	return ret;
}

int LTAPManager::GetAvailableAdapterName(CString &strName)
{
	int ret = err::NOERR;
	MIB_IFROW ifrow = { 0 };

	if (err::NOERR != (ret = GetAvailableAdapter(&ifrow)))
		return ret;

	strName = GetAdapterNameFromIfRow(&ifrow);

	return ret;
}

CString LTAPManager::GetAdapterNameFromIfRow(const PMIB_IFROW pRow)
{
	CString strOriginalName = (pRow->wszName);
	return strOriginalName.Mid(strOriginalName.Find(_T("{")));
}

CString LTAPManager::GetAdapterNameFromIndex(NET_IFINDEX ulIndex)
{
	NET_LUID luid = { 0 };
	ConvertInterfaceIndexToLuid(ulIndex, &luid);
	char szInterfaceName[128] = { 0 };

	ConvertInterfaceLuidToNameA(&luid, szInterfaceName, sizeof(szInterfaceName));

	return CString(szInterfaceName);
}

int LTAPManager::GetAvailableAdapters(int nCount,
	std::vector<IF_INFO> &vtAdapters, bool blForce)
{
	int nErrCode = err::NOERR;
	std::vector<IF_INFO> vtDetectedAdapter;

	if (err::NOERR != (nErrCode = DetectFreeAdapter(vtDetectedAdapter)))
		return nErrCode;

	if (vtDetectedAdapter.size() < nCount && blForce)
	{
		int nCreateCount = nCount - vtDetectedAdapter.size();
		vtDetectedAdapter.clear();

		while (nCreateCount--)
			CreateNewAdapter();

		Sleep(2000);

		if (err::NOERR != (nErrCode = DetectFreeAdapter(vtDetectedAdapter)))
			return nErrCode;
	}

	if (vtDetectedAdapter.size() < nCount)
		nErrCode = err::ADAPTER_COUNT_NOT_ENOUGH;

	int nReturnCount = vtDetectedAdapter.size() < nCount
		? vtDetectedAdapter.size() : nCount;

	for (int i = 0; i < nReturnCount; i++)
		vtAdapters.push_back(vtDetectedAdapter[i]);

	return nErrCode;
}


