/********************************************************************
 File: devMonitor.h
 Function: monitor all the device periodically
 Author: Luobihe
 Date: 2015-8-24
********************************************************************/

#ifndef __DEVMONITOR_H__
#define __DEVMONITOR_H__

#define MONITOR_PERIOD  10  //read dev per 10 seconds

//start dev monitor thread
int startDevMonitor(void);

#endif  //__DEVMONITOR_H__