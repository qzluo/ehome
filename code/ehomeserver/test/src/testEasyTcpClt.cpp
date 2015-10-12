#ifdef _WIN32
#include <Windows.h>
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "testEasyTcpClt.h"


void CTestEasyTcpClt::testInit(void)
{
    printf("----------------------test init-----------------\n");
    int iRet = m_easyTcpClt.open();
    if (iRet == -1) {
        printf("open failed\n");
        return;
    }

    printf("open success\n");

    TCPCLTAPPPARAS paras = {0};
    strcpy(paras.szAddr, "192.168.1.148");
    paras.iPort = 2222;

    printf("before init, time = %d\n", time(NULL));
    iRet = m_easyTcpClt.init(&paras);
    if (iRet == -1) {
        printf("init failed\n");
        printf("after init, time = %d\n", time(NULL));
        return ;
    }
    printf("init success\n");
    printf("after init, time = %d\n", time(NULL));
}

void CTestEasyTcpClt::testWrite(void)
{
    printf("----------------------test write-----------------\n");
    
    char wBuf[100] = {0};
    int iRet = 0;

    for (int i = 0; i < 5; i++) {
        sprintf(wBuf, "%d times write\n", i + 1);
        iRet = m_easyTcpClt.write(wBuf, strlen(wBuf));
        printf("write %d counts\n", iRet);
    }
}

void CTestEasyTcpClt::testRead(void)
{
    printf("----------------------test read-----------------\n");
    
    char rBuf[100] = {0};
    int iRet = 0;

    for (int i = 0; i < 5; i++) {
        iRet = m_easyTcpClt.read(rBuf, 100);
        if (iRet > 0) {
            rBuf[iRet] = 0;
            printf("read %d counts, content is %s\n", iRet, rBuf);
        }
        else
            printf("no data, ret = %d, time = %d\n", iRet, time(NULL));
    }
}

void CTestEasyTcpClt::test(void)
{
    testInit();

    testWrite();

    testRead();
}