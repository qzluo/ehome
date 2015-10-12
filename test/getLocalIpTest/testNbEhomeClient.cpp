#include <Windows.h>
#include <stdio.h>
#include "testNbEhomeClient.h"

void CNbEhomeClientTester::test(void)
{
    testInit();

    //testWriteDev();
    Sleep(1000);

    testReadDev();
}

void CNbEhomeClientTester::testInit(void)
{
    printf("------------------test init-------------------\n");
    int iRet = 0;
    iRet = m_client.init("127.0.0.1", 2222);

    if (iRet)
        printf("init failed\n");
    else
        printf("init success\n");
}

void CNbEhomeClientTester::testWriteDev(void)
{
    printf("------------------test writeDev-------------------\n");
    int iRet = 0;

    int id = 1;
    printf("close %d\n", id);
    m_client.writeDev(id, 0);

    Sleep(3000);

    printf("open %d\n", id);
    m_client.writeDev(id, 1);
}

void CNbEhomeClientTester::testReadDev(void)
{
    printf("------------------test readDev-------------------\n");

    for (int i = 0; i < 20; i++) {
        printf("read dev %d\n", i + 1);
        m_client.readDev(i + 1);
        Sleep(1);
    }
}

   