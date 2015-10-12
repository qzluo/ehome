#ifndef __TESTEHDBMANAGER_H__
#define __TESTEHDBMANAGER_H__

#include "cJSON.h"
#include "ehdbManager.h"

class CTestEhdbManager
{
private:
    CEhJsonReader m_jsonReader;

    void testGenFromDevArrayJson(void);
    void testGenTransJsonFromText(void);
    void testSetTbl(void);
    void testGetDataCount(void);
    void testGetDataTypeFromStr(void);
    void testFetchData(void);

public:
    void test(void);
};

#endif  //__TESTEHDBMANAGER_H__