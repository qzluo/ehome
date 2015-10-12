/********************************************************************
 File: objManager.cpp
 Function: 对象管理
 Author: Luobihe
 Date: 2015-7-22
********************************************************************/

#include <stdio.h>

#include "objManager.h"
#include "rdbManager.h"
#include "ptlManager.h"
#include "platcomm.h"
#include "logsys.h"
#include "ehDataPushOutor.h"

int init_obj(void)
{    
    if (initSocket())
        return -1;

    char szFilePath[256] = {0};
    char szFileName[256] = {0};
    char szLogFilePath[256] = {0};    
    if (getExePath(szFilePath, szFileName, 256))
        return -1;

    sprintf(szLogFilePath, "%s/%s", szFilePath, "log.dat");

#ifdef _MYDEBUG
    if (loginit(szLogFilePath, LOG_INFO))
        return -1;
#else
    if (loginit(szLogFilePath, LOG_WARNING))
        return -1;
#endif

    if (rdb_init()) {
        logout(LOG_EMERGENCY, "Failed to init protocol", 
           __FILE__, __LINE__);

        return -1;
    }

    if (pushOut_init()) {
        logout(LOG_ERR, "Failed to init push out", 
           __FILE__, __LINE__);

        return -1;
    }

    if (ptl_init()) {
        logout(LOG_EMERGENCY, "Failed to init protocol", 
           __FILE__, __LINE__);

        return -1;
    }
    
    return 0;
}

int deinit_obj(void)
{
    rdb_deinit();
    deinitSocket();

    return 0;
}