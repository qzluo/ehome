/********************************************************************
 File: testCIpKnxApp.h
 Function: 读写设备的应用
 Author: Luobihe
 Date: 2015-7-16
********************************************************************/

#ifndef __TESTCIPKNXAPP_H__
#define __TESTCIPKNXAPP_H__

#include "ip_knxapp.h"

//test CIpKnxApp
class CTestIpKnxApp {
private:
    CIpKnxApp testInst;

public:
    void test(void);

    void testWriteGroupAddr(char* szAddr, int value, int len);
    void testReadGroupAddr(char* szAddr, int len);
    void testMonitorGroupAddr(char* szAddr, int len);

    void testWriteGroupAddr();
    void testReadGroupAddr();
    void testReadCmd();
    void testMonitorGroupAddr(void);

};

#endif  //__TESTCIPKNXAPP_H__