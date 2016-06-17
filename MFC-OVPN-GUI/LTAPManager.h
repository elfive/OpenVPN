/*
* Created on Dec 4, 2015
*
* @author: luodichen
*/

#pragma once

#include <Windows.h>
#include <Iphlpapi.h>
#include <string>
#include <vector>


class LTAPManager
{
public:
    LTAPManager();
    virtual ~LTAPManager();

public:
    typedef struct _IF_INFO
    {
        NET_IFINDEX m_index;
        CString m_name;

        _IF_INFO() {}

        _IF_INFO(NET_IFINDEX index, const char *szName)
            : m_index(index), m_name(szName)
        {

        }
    } IF_INFO;

public:
    int GetAvailableAdapter(PMIB_IFROW pTable);
    int GetAvailableAdapterName(CString &strName);

    int GetAvailableAdapters(int nCount, 
        std::vector<IF_INFO> &vtAdapters, bool blForce);

    static CString GetAdapterNameFromIfRow(const PMIB_IFROW pRow);
    static CString GetAdapterNameFromIndex(NET_IFINDEX ulIndex);

private:
	CString GetModuleDir();
	int DetectFreeAdapter(PMIB_IFROW pTable, bool *pFound);
    int DetectFreeAdapter(std::vector<IF_INFO> &vtAdapters);
    int CreateNewAdapter();

private:
    static const CString TARGET_INTERFACE;
};
