#include <stdio.h>
#include <stdlib.h>
         

#include "logsys.h"
#include "objManager.h"
#include "tcplistener.h"
#include "devMonitor.h"
#include "platcomm.h"

#define VERSION "1.0"

//#define _MYDEBUG

#ifdef _MYDEBUG

#include "testEHSCmdParser.h"
#include "testRdbManager.h"
#include "testEasyTcpClt.h"
#include "testCIpKnxPtl.h"
#include "testCIpKnxApp.h"
#include "testInit.h"
#include "testDataPushoutor.h"
#include "testEhdbManager.h"

int main(int argc, char** argv)
{
    testInit();

/*
    CTestIpxKnxPtl ipxKnxPtlTester;
    ipxKnxPtlTester.test();
*/

/*    
    CTestIpKnxApp ipKnxAppTester;
    ipKnxAppTester.test();
*/    

/*
    CTestEasyTcpClt tcpcltTester;
    tcpcltTester.test();

    while (1) 
        Sleep(1000);
*/

/*
    CTestEHSCmdParser tester;
    tester.test();    
/*/

/*
    CTestRdbManager rdbManagerTester;
    rdbManagerTester.test();
    */

//*
    CDataPushOutorTester dataPushOutorTester;
    dataPushOutorTester.test();
//*/

    /*
    CTestEhdbManager ehdbManagerTester;
    ehdbManagerTester.test();
    */
    testDeinit();

    return 0;
}

#else

int main(int argc, char** argv)
{    
    printf("Ehome server, version: %s\n", VERSION);
    if (init_obj())
        return 1;

    printf("Waiting initialization ...\n");
    platSleep(3);

    if (-1 == startDevMonitor())
        return 1;

    printf("Init success\n");

    if (-1 == startTcpListen())
        return 1;
    
    while(1) {
        platSleep(1);
    }

    deinit_obj();

    return 0;
}

#endif