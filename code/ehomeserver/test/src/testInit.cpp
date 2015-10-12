#include <stdio.h>
#include "testInit.h"

#include "rdbManager.h"
#include "platcomm.h"
#include "logsys.h"
#include "ehDataPushOutor.h"

int testInit(void)
{
    if (initSocket())
        return -1;

    char szFilePath[256] = {0};
    char szFileName[256] = {0};
    char szLogFilePath[256] = {0};    
    if (getExePath(szFilePath, szFileName, 256))
        return -1;

    sprintf(szLogFilePath, "%s/%s", szFilePath, "log.dat");

    if (loginit(szLogFilePath, LOG_INFO))
        return -1;

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

    return 0;
}

int testDeinit(void)
{
    rdb_deinit();
    deinitSocket();

    return 0;
}