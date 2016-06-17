/*
* Created on Dec 4, 2015
*
* @author: luodichen
*/

#pragma once

namespace err
{
    const int NOERR = 0x0000;
    const int BAD_RESPONSE = 0x0001 << 16;
    const int KEY_ERROR = 0x0002 << 16;
    const int NETWORK_FAIL = 0x0003 << 16;
    const int GET_IF_TABLE_FAIL = 0x0004 << 16;
    const int MEMORY_ALLOC_FAIL = 0x0005 << 16;
    const int CREATE_PROCESS_FAIL = 0x0006 << 16;
    const int PROCESS_RETURN_FAILED = 0x0007 << 16;
    const int OPERATION_NOT_ALLOWED_IN_CURRENT_STATUS = 0x008 << 16;
    const int CREATE_HANDLE_FAIL = 0x0009 << 16;
    const int USER_PASS_ERROR = 0x000a << 16;
    const int NO_AVAILABLE_ADAPTER = 0x000b << 16;
    const int GET_ADAPTER_ADDRESS_FAIL = 0x000c << 16;
    const int ADAPTER_NOT_FOUND = 0x000d << 16;
    const int ADDRESS_NOT_FOUND = 0x000e << 16;
    const int ADAPTER_COUNT_NOT_ENOUGH = 0x000f << 16;
    const int GET_BEST_INTERFACE_FAIL = 0x0010 << 16;
    const int READ_ROUTE_TABLE_FAIL = 0x0011 << 16;
    const int CREATE_ROUTE_FAIL = 0x0012 << 16;
    const int GET_INTERFACE_ENTRY_FAIL = 0x0013 << 16;
    const int IN_PROGRESS = 0x0014 << 16;
    const int MYSQL_INIT_FAIL = 0x0015 << 16;
    const int MYSQL_CONNECT_FAIL = 0x0016 << 16;
    const int ILLEGAL_QUERY = 0x0017 << 16;
    const int MYSQL_QUERY_FAIL = 0x0018 << 16;
    const int CREATE_PPTP_ENTRY_FAIL = 0x0019 << 16;
    const int PPTP_CONNECT_FAIL = 0x0020 << 16;
    const int GET_FILE_PATH_FAIL = 0x0021 << 16;
    const int WRITE_ENTRY_PROFILE_FAIL = 0x0022 << 16;
    const int RAS_CONN_NOT_FOUND = 0x0023 << 16;
    const int WEB_INTERFACE_REQUEST_FAIL = 0x0024 << 16;
    const int WEB_INTERFACE_RETURNS_ERROR = 0x0025 << 16;
    const int WEB_INTERFACE_ERROR = 0x0026 << 16;
}
