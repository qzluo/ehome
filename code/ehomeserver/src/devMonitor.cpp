/********************************************************************
 File: devMonitor.cpp
 Function: monitor all the device periodically
 Author: Luobihe
 Date: 2015-8-24
********************************************************************/

#include "devMonitor.h"
#include "platcomm.h"
#include "rdbManager.h"
#include "ptlManager.h"

static void devMonitorLoopBody(void)
{
    int iRecordCount = 0;
    TB001_DEVOBJ devobj = {0}; 

    iRecordCount = rdb_getRecordCount(TBL_DEV);
    for (int i = 0; i < iRecordCount; i++) {
        if (rdb_findByIndex(TBL_DEV, &devobj, i))
            break;

        //monitor dev
        monitorDev(devobj.iProtocolID, 
            devobj.szGroupAddr, 
            devobj.iDataType);

        platMSleep(50);
    }
}

static void* DevMonitorThreadFun(void* pM)  
{
    while (1) {
        devMonitorLoopBody();

        //break;
        platSleep(MONITOR_PERIOD);
    }

    return 0;
}

/*-------------------------------------------------------------------
 Function: startDevMonitor(void);
           start device monitor thread
 Parameters:
 Return: 0 -- success
         -1 -- failed
-------------------------------------------------------------------*/
int startDevMonitor(void)
{
    return easyCreateThread(DevMonitorThreadFun, NULL);
}
