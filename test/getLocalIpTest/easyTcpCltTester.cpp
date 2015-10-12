#include <stdio.h>
#include <Windows.h>
#include <time.h>
#include "easyTcpCltTester.h"


int iItem = 0;

int CEasyTcpCltTester::readHandle(void)
{
    int iRet = 0;
    char rBuf[100] = {0};
    iRet = m_tcpclt.read(rBuf, 100);
    printf("item %d: read %d counts, content is %s\n", ++iItem, iRet, rBuf);

    return 0;
}

void CEasyTcpCltTester::test(void)
{
    testInit();

    testWrite();

    //testRead();

    testClear();
}

void CEasyTcpCltTester::testInit(void)
{
    printf("---------------------test init ------------------\n");
    int iRet = 0;
    iRet = m_tcpclt.open();
    if (iRet == -1) {
        printf("open socket failed\n");
        return ;
    }
    printf("open successful\n");

    TCPCLTAPPPARAS paras;
    strcpy(paras.szAddr, "127.0.0.1");
    paras.iPort = 1234;
    paras.p_app = this;

    iRet = m_tcpclt.init(&paras);
    if (iRet == -1) {
        printf("init failed\n");
        return ;
    }
    printf("init successful\n");
}

void CEasyTcpCltTester::testWrite(void)
{
    printf("---------------------test write------------------\n");

    char wBuf[100] = {0};
    int iRet = 0;

    for (int i = 0; i < 5; i++) {
        sprintf(wBuf, "%d times write\n", i + 1);
        iRet = m_tcpclt.write(wBuf, strlen(wBuf));
        printf("write %d counts\n", iRet);
    }  
}

void CEasyTcpCltTester::testRead(void)
{
    printf("---------------------test read------------------\n");

    char rBuf[100] = {0};
    int iRet = 0;

    for (int i = 0; i < 5; i++) {
        iRet = m_tcpclt.read(rBuf, 100);
        if (iRet > 0) {
            rBuf[iRet] = 0;
            printf("read %d counts, content is %s\n", iRet, rBuf);
        }
        else
            printf("no data, ret = %d, time = %d\n", iRet, time(NULL));
    }
}

void CEasyTcpCltTester::testClear(void)
{
    printf("---------------------test clear------------------\n");

    printf("before clear\n\n");
    int iRet = m_tcpclt.isClosed();
    if (iRet)
        printf("client close\n");
    else
        printf("client open\n");

    m_tcpclt.clear();

    printf("after clear\n\n");
    iRet = m_tcpclt.isClosed();
    if (iRet)
        printf("client close\n");
    else
        printf("client open\n");


}