#include <stdio.h>
#include <string.h>
#include "testDataPushoutor.h"
#include "ehDataPushOutor.h"

#include "platcomm.h"

void CDataPushOutorTester::testSendUdpData(void)
{
    printf("----------------test sendUdpData--------------------\n");

    char szData[] = { 0x00, 0x01, 0x02, 0x03,
                      0x04, 0x05, 0x06, 0x07,
                      0x08, 0x09, 0x0A, 0x0B };

    char szData2[] = { 0x10, 0x11, 0x12, 0x13,
                      0x14, 0x15, 0x16, 0x17,
                      0x18, 0x19, 0x1A, 0x1B };

    strcpy(szData, "a test");
    int ret = 0;

    for (int i = 0; i < 10; i++) {
        //ret = sendUdpData("192.168.1.169", 5678, 
        //    szData, sizeof(szData));

        ret = sendUdpData("192.168.1.255", 5678, 
            szData, sizeof(szData));
        
        //sendUdpData("255.255.255.255", 5678, 
        //    szData2, sizeof(szData2));
    }

    /*
    ret = sendUdpData("192.168.1.166", 1234, 
                szData, sizeof(szData));
    if (ret)
        printf("send data to %s failed\n", "192.168.1.166");
    else
        printf("send data to %s success\n", "192.168.1.166");

    ret = sendUdpData("192.168.1.166", 1234, 
                szData, sizeof(szData2));
    if (ret)
        printf("send data to %s failed\n", "192.168.1.166");
    else
        printf("send data to %s success\n", "192.168.1.166");
    
    ret = sendUdpData("192.168.1.255", 1234, 
                szData, sizeof(szData2));
    if (ret)
        printf("send data to %s failed\n", "192.168.1.255");
    else
        printf("send data to %s success\n", "192.168.1.255");

    ret = sendUdpData("255.255.255.255", 1234, 
                szData, sizeof(szData2));
    if (ret)
        printf("send data to %s failed\n", "255.255.255.255");
    else
        printf("send data to %s success\n", "255.255.255.255");
        */

}

void CDataPushOutorTester::testBroadCastUdpData(void)
{
    printf("----------------test broadCastUdpData--------------------\n");

    char szData[] = { 0x00, 0x01, 0x02, 0x03,
                      0x04, 0x05, 0x06, 0x07,
                      0x08, 0x09, 0x0A, 0x0B };

    strcpy(szData, "broad test");
    int ret = 0;
    ret = broadCastUdpData(1234, 
                szData, sizeof(szData));
    if (ret)
        printf("broadcast data failed\n");
    else
        printf("broadcast data success\n");
}

void CDataPushOutorTester::testPushOutDevData(void)
{
    printf("----------------test pushOutDevData--------------------\n");

    int ret = 0;
    int data = 1;
    ret = pushOutDevData(1, &data, 2);
    printf("ret = %d\n", ret);

    data = 2;
    ret = pushOutDevData(18, &data, 2);
    printf("ret = %d\n", ret);
}

void CDataPushOutorTester::test(void)
{
    testSendUdpData();

    //testBroadCastUdpData();
    //testPushOutDevData();
}

