/*
* Created on Dec 5, 2015
*
* @author: luodichen
*/

#pragma once

#include <string>

class LVPNConnector
{
public:
    typedef enum _STATUS
    {
        READY = 0,
        CONNECTING = 1,
        CONNECTED = 2,
        DISCONNECTING = 3,
        DISCONNECTED = 4
    } STATUS;

    typedef class _Callback
    {
    public:
        _Callback() {}
        virtual void OnSuccess(LVPNConnector &connector) = 0;
        virtual void OnFailed(LVPNConnector &connector) = 0;
        virtual void OnStatusChanged(LVPNConnector &connector) = 0;
        virtual void MsgReceived(std::string strMsg) = 0;
    } Callback;

public:
    LVPNConnector(const char *szServer, Callback &callback);
    virtual ~LVPNConnector();

public:
    virtual STATUS GetStatus();
    int GetError();
    int Connect(const char *szUser, const char *szPassword);
    int Disconnect();
    int SyncDisconnect();

protected:
    void SetStatus(STATUS status);
    void SetError(int nErrorCode);

    virtual void RealConnect() = 0;
    virtual void RealDisconnect() = 0;

private:
    static void ConnectThread(void *pData);
    static void DisconnectThread(void *pData);

protected:
    std::string m_strServer;
    std::string m_strUsername;
    std::string m_strPassword;
    Callback &m_objCallback;

private:
    STATUS m_enStatus;
    HANDLE m_hConnectThread;
    HANDLE m_hDisconnectThread;
    int m_nErrorCode;
};

