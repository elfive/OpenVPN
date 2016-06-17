/*
* Created on Dec 5, 2015
*
* @author: luodichen
*/

#include "stdafx.h"
#include "LVPNConnector.h"
#include "lerror.h"

LVPNConnector::LVPNConnector(const char *szServer, Callback &callback)
    : m_strServer(szServer)
    , m_strUsername("")
    , m_strPassword("")
    , m_enStatus(READY)
    , m_objCallback(callback)
    , m_hConnectThread(INVALID_HANDLE_VALUE)
    , m_hDisconnectThread(INVALID_HANDLE_VALUE)
    , m_nErrorCode(err::NOERR)
{

}

LVPNConnector::~LVPNConnector()
{
    SyncDisconnect();
}

int LVPNConnector::Connect(const char *szUser, const char *szPassword)
{
    if (READY != m_enStatus && DISCONNECTED != m_enStatus)
        return err::OPERATION_NOT_ALLOWED_IN_CURRENT_STATUS;

    if (INVALID_HANDLE_VALUE != m_hConnectThread)
        return err::OPERATION_NOT_ALLOWED_IN_CURRENT_STATUS;

    m_strUsername = szUser;
    m_strPassword = szPassword;

    m_hConnectThread = (HANDLE)_beginthread(ConnectThread, 0, this);

    return err::NOERR;
}

int LVPNConnector::Disconnect()
{
    if (DISCONNECTING == m_enStatus || READY == m_enStatus || DISCONNECTED == m_enStatus)
        return err::OPERATION_NOT_ALLOWED_IN_CURRENT_STATUS;

    if (INVALID_HANDLE_VALUE != m_hDisconnectThread)
        return err::OPERATION_NOT_ALLOWED_IN_CURRENT_STATUS;

    m_hDisconnectThread = (HANDLE)_beginthread(DisconnectThread, 0, this);

    return err::NOERR;
}

int LVPNConnector::SyncDisconnect()
{
    Disconnect();

    if (INVALID_HANDLE_VALUE != m_hDisconnectThread)
        WaitForSingleObject(m_hDisconnectThread, INFINITE);
    if (INVALID_HANDLE_VALUE != m_hConnectThread)
        WaitForSingleObject(m_hConnectThread, INFINITE);

    return 0;
}

LVPNConnector::STATUS LVPNConnector::GetStatus()
{
    return m_enStatus;
}

void LVPNConnector::SetStatus(LVPNConnector::STATUS status)
{
    m_enStatus = status;
    m_objCallback.OnStatusChanged(*this);
}

int LVPNConnector::GetError()
{
    return m_nErrorCode;
}

void LVPNConnector::SetError(int nErrorCode)
{
    m_nErrorCode = nErrorCode;
}

void LVPNConnector::ConnectThread(void *pData)
{
    LVPNConnector *pSelf = (LVPNConnector *)pData;
    pSelf->RealConnect();

    //pSelf->m_hConnectThread = INVALID_HANDLE_VALUE;
}

void LVPNConnector::DisconnectThread(void *pData)
{
    LVPNConnector *pSelf = (LVPNConnector *)pData;
    pSelf->RealDisconnect();

    //pSelf->m_hDisconnectThread = INVALID_HANDLE_VALUE;
}
