#include <stdio.h>
#include <time.h>
#include "testCIpKnxApp.h"
#include "platcomm.h"



/////////////////////////////////////////////////////////////////////
void CTestIpKnxApp::test()
{
    printf("------------------test CIpKnxApp------------------------\n");
    int iRet = 0;
    int i = 0;

    //init
    iRet = testInst.init("192.168.1.55", 1000);

    if (iRet < 0) {
        printf("time = %d\n", time(NULL));
        printf("init faile\n");
    }
    else
        printf("init success\n");
    
    //testWriteGroupAddr();

    testMonitorGroupAddr();
    //testReadGroupAddr();

}

void CTestIpKnxApp::testWriteGroupAddr()
{
    int value = 1;

    for (int i = 0; i < 100; i++) {
        /* testWriteGroupAddr("10/1/12", value, 1);
        testWriteGroupAddr("10/1/13", value, 1);
        testWriteGroupAddr("10/1/31", value, 1);
        testWriteGroupAddr("10/1/32", value, 1);
        testWriteGroupAddr("10/1/33", value, 1);
        */ 
        testWriteGroupAddr("10/2/11", value, 1);
        testWriteGroupAddr("10/2/12", value, 1);

        testWriteGroupAddr("10/2/13", value, 1);
        testWriteGroupAddr("10/2/14", value, 1);
        testWriteGroupAddr("10/2/16", value, 1);

        /* µ÷¹âµÆ
        testWriteGroupAddr("10/2/18", value, 2);
        value += 10;
        if (value > 255)
        value = 0;
        */

        /*
        testWriteGroupAddr("10/2/31", value, 1);
        testWriteGroupAddr("10/2/32", value, 1);
        testWriteGroupAddr("10/2/33", value, 1);
        testWriteGroupAddr("10/2/34", value, 1);
        */

        //´°Á±
        //testWriteGroupAddr("10/2/61", value, 1);
        //testWriteGroupAddr("10/2/62", value, 1);
        //testWriteGroupAddr("10/2/63", value, 1);
        //  testWriteGroupAddr("10/2/64", value, 1);


        platSleep(5);
        value = value == 1 ? 0 : 1;      
    }
}

void CTestIpKnxApp::testWriteGroupAddr(char* szAddr, int value, int len)
{
    int iRet = 0;
    iRet = testInst.writeGroupAddr(szAddr, value, len);

    if (iRet < 0) {
        printf("time = %d\n", time(NULL));
        printf("writeGroupAddr %s faile\n", szAddr);
    }
    else
        printf("writeGroupAddr %s success\n", szAddr); 

}

void CTestIpKnxApp::testReadGroupAddr()
{
    testReadGroupAddr("10/1/11", 1);

    platSleep(1);
    testReadGroupAddr("10/1/12", 1);
    platSleep(1);
    testReadGroupAddr("10/1/13", 1);
    platSleep(1);
    testReadGroupAddr("10/1/31", 1);
    platSleep(1);
    testReadGroupAddr("10/1/32", 1);
    platSleep(1);
    testReadGroupAddr("10/1/33", 1);
    platSleep(1);
}

void CTestIpKnxApp::testReadGroupAddr(char* szAddr, int len)
{
    int iRet = 0;
    int value = -1;
    iRet = testInst.readGroupAddr(szAddr, &value, len);

    if (iRet < 0) {
        printf("time = %d\n", time(NULL));
        printf("readGroupAddr %s faile\n", szAddr);
    }
    else
        printf("readGroupAddr %s success, val = %d\n", 
        szAddr, value); 
}

void CTestIpKnxApp::testReadCmd()
{
    //testInst.sendReadCmd("10/1/11", 1);
    //testInst.sendReadCmd("10/1/12", 1);
    //testInst.sendReadCmd("10/1/13", 1);
    //testInst.sendReadCmd("10/1/31", 1);
    //testInst.sendReadCmd("10/1/32", 1);
    //testInst.sendReadCmd("10/1/33", 1);
}

void CTestIpKnxApp::testMonitorGroupAddr(void)
{
    printf("-------------------test monitor groupAddr--------------------\n");
    for (int i = 0; i < 5; i++) {
        platSleep(2);
        testMonitorGroupAddr("10/2/11", 1);
        platMSleep(50);
        testMonitorGroupAddr("10/2/12", 1);
        platMSleep(50);
        /*
        testMonitorGroupAddr("10/2/13", 1);
        Sleep(500);
        testMonitorGroupAddr("10/2/31", 1);
        Sleep(500);
        testMonitorGroupAddr("10/2/32", 1);
        Sleep(500);
        testMonitorGroupAddr("10/2/33", 1);
        Sleep(500);
        */
    }
}

void CTestIpKnxApp::testMonitorGroupAddr(char* szAddr, int len)
{
    int ret = 0;
    ret = testInst.monitorGroupAddr(szAddr, len);
    if (ret == -1)
        printf("fail to send monitor cmd\n");
    else
        printf("success to send monitor cmd\n");
}