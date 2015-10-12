#include <stdio.h>
#include <string.h>

#include "testCIpKnxPtl.h"
#include "rdbManager.h"
#include "ptlManager.h"
#include "platcomm.h"

void CTestIpxKnxPtl::testReadDev(void)
{
    printf("-------------test readDev---------------\n");

    testReadDevWithAddr("10/1/11");
    testReadDevWithAddr("10/1/12");
    testReadDevWithAddr("10/1/13");
    testReadDevWithAddr("10/1/31");
    testReadDevWithAddr("10/1/32");
    testReadDevWithAddr("10/1/33");
}

void CTestIpxKnxPtl::testReadDevWithAddr(const char* saAddr)
{
    PtllDataType result;
    if (m_ptl.readDev(saAddr, &result, DEV_TYPE_BOOL) == 0) {
        printf("read %s successful, result = %d\n", saAddr, result.iVal);
    }
    else
        printf("read %s failed.\n", saAddr);
}

void CTestIpxKnxPtl::testWriteDev(void)
{
    printf("-------------test writeDev---------------\n");

    int val = 1;
    for (int i = 0; i < 10; i++) {
        testWriteDevWithAddr("10/2/11", val);
        testWriteDevWithAddr("10/2/12", val);
        testWriteDevWithAddr("10/2/13", val);
        testWriteDevWithAddr("10/2/14", val);
        testWriteDevWithAddr("10/2/16", val);

        val = val == 1 ? 0 : 1;

        platSleep(3);
    }
}

void CTestIpxKnxPtl::testWriteDevWithAddr(const char* saAddr, int iVal)
{
    PtllDataType result;
    result.iVal = iVal;
    if (m_ptl.writeDev(saAddr, &result, DEV_TYPE_BOOL) == 0){
        printf("write %s successful, result = %d\n", saAddr, result.iVal);
    }
    else
        printf("read %s failed.\n", saAddr);
}

void CTestIpxKnxPtl::test(void)
{
    printf("initial ipknx protocol\n");
    int iRet = m_ptl.init();
    printf("init return = %d\n", iRet);
    
    testReadDev();

    testWriteDev();
}