#include <stdio.h>
#include <string.h>
#include "testRdbManager.h"


void CTestRdbManager::test(void)
{
    //testDevTblOp();

    //testRdbMgr();

    testRdb_updateDevValByGroupAddr();
}

void CTestRdbManager::testRdb_updateDevValByGroupAddr(void)
{
    printf("----------------test rdb_updateDevValByGroupAddr---------------\n");

    int ret = 0;
    char szAddr[24] = {0};
    char szVal[8] = {0};

    strcpy(szAddr, "10/1/11");
    strcpy(szVal, "1");
    ret = rdb_updateDevValByGroupAddr(szAddr, szVal);
    printf("update %s by value %s, ret = %d\n", szAddr, szVal, ret);

    strcpy(szAddr, "10/1/11");
    strcpy(szVal, "0");
    ret = rdb_updateDevValByGroupAddr(szAddr, szVal);
    printf("update %s by value %s, ret = %d\n", szAddr, szVal, ret);

    strcpy(szVal, "1");
    ret = rdb_updateDevValByGroupAddr(szAddr, szVal);
    printf("update %s by value %s, ret = %d\n", szAddr, szVal, ret);
}

void CTestRdbManager::testDevTblOp(void)
{
    testDevTblOp_insert();

    //testDevTblOp_find();

    testDevTblOp_findByIndex();

    /*
    testDevTblOp_update();

    testDevTblOp_updateByGrpAddr();

    testDevTblOp_selectByGroupAddr();
    //testDevTblOp_remove();
    testDevTblOp_getRecordCount();

    testDevTblOp_removeAll();
    */
}


void CTestRdbManager::testDevTblOp_insert(void)
{
    printf("----------------test insert---------------\n");
    m_devTblOp.printAllNode();
    TB001_DEVOBJ debobj = {0};

    debobj.iID = 1;
    strcpy(debobj.szDesc, "lamp");
    debobj.iDataType = 1;
    debobj.iProtocolID = 1;
    strcpy(debobj.szGroupAddr, "1/1/10");
    strcpy(debobj.szValue, "0");

    m_devTblOp.insert(&debobj);

    debobj.iID = 2;
    strcpy(debobj.szDesc, "window");
    debobj.iDataType = 2;
    debobj.iProtocolID = 3;
    strcpy(debobj.szGroupAddr, "1/1/11");
    strcpy(debobj.szValue, "1");

    m_devTblOp.insert(&debobj);

    m_devTblOp.printAllNode();
}

void CTestRdbManager::testDevTblOp_find(void)
{
    printf("----------------test find---------------\n");
    TB001_DEVOBJ devobj = {0};

    int key = 1;
    if (!m_devTblOp.find(&devobj, key)) {
        printf("key: %d, find devobj\n", key);
        printDevObj(&devobj);
    }
    else
        printf("key: %d, dev obj not find\n", key);

    key = 2;
    if (!m_devTblOp.find(&devobj, key)) {
        printf("key: %d, find devobj\n", key);
        printDevObj(&devobj);
    }
    else
        printf("key: %d, dev obj not find\n", key);

    key = 3;
    if (!m_devTblOp.find(&devobj, key)) {
        printf("key: %d, find devobj\n", key);
        printDevObj(&devobj);
    }
    else
        printf("key: %d, dev obj not find\n", key);
}

void CTestRdbManager::testDevTblOp_findByIndex(void)
{
    printf("----------------test findByIndex---------------\n");
    TB001_DEVOBJ devobj = {0};

    m_devTblOp.printAllNode();

    int index = 0;
    for (index = 0; index < 3; index++) {
        if (!m_devTblOp.findByIndex(&devobj, index)) {
            printf("index: %d, find devobj\n", index);
            printDevObj(&devobj);
        }
        else
            printf("index: %d, dev obj not find\n", index);
    }
}

void CTestRdbManager::testDevTblOp_update(void)
{
    printf("----------------test update---------------\n");
    m_devTblOp.printAllNode();
    TB001_DEVOBJ debobj = {0};

    debobj.iID = 1;
    strcpy(debobj.szDesc, "kkk");
    debobj.iDataType = 1;
    debobj.iProtocolID = 1;
    strcpy(debobj.szGroupAddr, "1/1/10");
    strcpy(debobj.szValue, "0");

    m_devTblOp.update(&debobj, 1);

    debobj.iID = 2;
    strcpy(debobj.szDesc, "llll");
    debobj.iDataType = 2;
    debobj.iProtocolID = 3;
    strcpy(debobj.szGroupAddr, "1/1/11");
    strcpy(debobj.szValue, "1");

    m_devTblOp.update(&debobj, 2);

    m_devTblOp.printAllNode();

}

void CTestRdbManager::testDevTblOp_remove(void)
{
    printf("----------------test remove---------------\n");

    m_devTblOp.printAllNode();

    int key = 1;
    if (!m_devTblOp.remove(key)) {
        printf("key: %d, remove\n", key);
        m_devTblOp.printAllNode();
    }
    else
        printf("key: %d, dev obj not find\n", key);

    key = 3;
    if (!m_devTblOp.remove(key)) {
        printf("key: %d, remove\n", key);
        m_devTblOp.printAllNode();
    }
    else
        printf("key: %d, dev obj not find\n", key);
}

void CTestRdbManager::testDevTblOp_removeAll(void)
{
    printf("----------------test removeAll---------------\n");

    m_devTblOp.printAllNode();
    m_devTblOp.removeAll();
    m_devTblOp.printAllNode();
}

void CTestRdbManager::testDevTblOp_getRecordCount(void)
{
    printf("----------------test getRecordCount---------------\n");

    int iSize = 0;

    m_devTblOp.printAllNode();
    iSize = m_devTblOp.getRecordCount();
    printf("record count = %d\n", iSize);

    m_devTblOp.removeAll();
    m_devTblOp.printAllNode();
    iSize = m_devTblOp.getRecordCount();
    printf("after remove all, record count = %d\n", iSize);
}

void CTestRdbManager::testDevTblOp_updateByGrpAddr(void)
{
    printf("----------------test updateByGrpAddr---------------\n");
    m_devTblOp.printAllNode();
   
    int ret = m_devTblOp.updateByGroupAddr("1/1/10", "3");
    printf("update 1/1/10, ret = %d\n", ret);
    ret = m_devTblOp.updateByGroupAddr("1/1/11", "30");
    printf("update 1/1/11, ret = %d\n", ret);
    ret = m_devTblOp.updateByGroupAddr("1/1/12", "30");
    printf("update 1/1/12, ret = %d\n", ret);

    m_devTblOp.printAllNode();
}

void CTestRdbManager::testDevTblOp_selectByGroupAddr(void)
{
    printf("----------------test selectByGroupAddr---------------\n");
    TB001_DEVOBJ devobj = {0};

    if (!m_devTblOp.selectByGroupAddr("1/1/10", &devobj)) {
        printDevObj(&devobj);
    }
    else
        printf("addr: 1/1/10, dev obj not find\n");

    if (!m_devTblOp.selectByGroupAddr("1/1/11", &devobj)) {
        printDevObj(&devobj);
    }
    else
        printf("addr: 1/1/11, dev obj not find\n");

    if (!m_devTblOp.selectByGroupAddr("1/1/12", &devobj)) {
        printDevObj(&devobj);
    }
    else
        printf("addr: 1/1/12, dev obj not find\n");
}

void CTestRdbManager::printDevObj(TB001_DEVOBJ* p_devobj)
{
    printf("Item info:\n");
    printf("id: %d\n", p_devobj->iID);
    printf("desc: %s\n", p_devobj->szDesc);
    printf("DataType: %d\n", p_devobj->iDataType);
    printf("ProtocolID: %d\n", p_devobj->iProtocolID);
    printf("GroupAddr: %s\n", p_devobj->szGroupAddr);
    printf("Value: %s\n\n", p_devobj->szValue);
}

void CTestRdbManager::testRdbMgr(void)
{
    //testRdbRegisterTbl();

    //testRdbInsert();

    //testRdbFind();

    //testRdbUpdate();

    //testRdbRemove();

    testRdbInitDev();

    //testRdbGetRecordCount();

    testRdbFindByIndex();

    testRdbClear();

}

void CTestRdbManager::testRdbRegisterTbl(void)
{
    printf("----------------test RegisterTbl---------------\n");
    m_rdbMgr.printTblInfo();


    m_rdbMgr.registerTbl("aaa", &m_devTblOp);

    m_rdbMgr.printTblInfo();
}

void CTestRdbManager::testRdbUpdate(void)
{
    printf("----------------test rdbmanager Update---------------\n");

    m_rdbMgr.printTblInfo();

    TB001_DEVOBJ debobj = {0};

    debobj.iID = 1;
    strcpy(debobj.szDesc, "kkk");
    debobj.iDataType = 1;
    debobj.iProtocolID = 1;
    strcpy(debobj.szGroupAddr, "1/1/10");
    strcpy(debobj.szValue, "0");

    m_rdbMgr.update("device", &debobj, 1);

    debobj.iID = 2;
    strcpy(debobj.szDesc, "llll");
    debobj.iDataType = 2;
    debobj.iProtocolID = 3;
    strcpy(debobj.szGroupAddr, "1/1/11");
    strcpy(debobj.szValue, "1");

    m_rdbMgr.update("device", &debobj, 2);


    m_rdbMgr.printTblInfo();
}

void CTestRdbManager::testRdbFind(void)
{
    printf("----------------test rdbmanager find---------------\n");

    TB001_DEVOBJ devobj = {0};

    int key = 1;
    if (!m_rdbMgr.find("device", &devobj, key)) {
        printf("key: %d, find devobj\n", key);
        printDevObj(&devobj);
    }
    else
        printf("key: %d, dev obj not find\n", key);

    key = 2;
    if (!m_rdbMgr.find("device", &devobj, key)) {
        printf("key: %d, find devobj\n", key);
        printDevObj(&devobj);
    }
    else
        printf("key: %d, dev obj not find\n", key);

    key = 3;
    if (!m_rdbMgr.find("device", &devobj, key)) {
        printf("key: %d, find devobj\n", key);
        printDevObj(&devobj);
    }
    else
        printf("key: %d, dev obj not find\n", key);
}

void CTestRdbManager::testRdbFindByIndex(void)
{
    printf("----------------test rdbmanager findByIndex---------------\n");

    TB001_DEVOBJ devobj = {0};

    int index = 0;
    for (index = 0; index < 3; index++) {
        if (!m_rdbMgr.findByIndex("device", &devobj, index)) {
            printf("index: %d, find devobj\n", index);
            printDevObj(&devobj);
        }
        else
            printf("index: %d, dev obj not find\n", index);
    }

    for (index = 18; index < 22; index++) {
        if (!m_rdbMgr.findByIndex("device", &devobj, index)) {
            printf("index: %d, find devobj\n", index);
            printDevObj(&devobj);
        }
        else
            printf("index: %d, dev obj not find\n", index);
    }
}

void CTestRdbManager::testRdbInsert(void)
{
    printf("----------------test rdbmanager insert---------------\n");
    m_rdbMgr.initDev();

    m_rdbMgr.printTblInfo();

    TB001_DEVOBJ debobj = {0};

    debobj.iID = 1;
    strcpy(debobj.szDesc, "lamp");
    debobj.iDataType = 1;
    debobj.iProtocolID = 1;
    strcpy(debobj.szGroupAddr, "1/1/10");
    strcpy(debobj.szValue, "0");

    if (m_rdbMgr.insert("device", &debobj))
        printf("insert faile, id = %d\n", debobj.iID);
    else
        printf("insert successful, id = %d\n", debobj.iID);


    debobj.iID = 2;
    strcpy(debobj.szDesc, "window");
    debobj.iDataType = 2;
    debobj.iProtocolID = 3;
    strcpy(debobj.szGroupAddr, "1/1/11");
    strcpy(debobj.szValue, "1");

    if (m_rdbMgr.insert("device", &debobj))
        printf("insert faile, id = %d\n", debobj.iID);
    else
        printf("insert successful, id = %d\n", debobj.iID);

    debobj.iID = 2;
    strcpy(debobj.szDesc, "window");
    debobj.iDataType = 2;
    debobj.iProtocolID = 3;
    strcpy(debobj.szGroupAddr, "1/1/11");
    strcpy(debobj.szValue, "1");

    if (m_rdbMgr.insert("device", &debobj))
        printf("insert faile, id = %d\n", debobj.iID);
    else
        printf("insert successful, id = %d\n", debobj.iID);

    m_rdbMgr.printTbl("device");

}

void CTestRdbManager::testRdbRemove(void)
{
    printf("----------------test rdbmanager remove---------------\n");
    
    m_rdbMgr.printTblInfo();

    int key = 1;

    if (m_rdbMgr.remove("device", key))
        printf("remove faile, key = %d\n", key);
    else
        printf("remove successful, key = %d\n", key);


    key = 3;

    if (m_rdbMgr.remove("device", key))
        printf("remove faile, key = %d\n", key);
    else
        printf("remove successful, key = %d\n", key);

    m_rdbMgr.printTbl("device");
}

void CTestRdbManager::testRdbInitDev(void)
{
    printf("----------------test RdbInitDev remove---------------\n");
    
    m_rdbMgr.printTblInfo();
    m_rdbMgr.initDev();
    m_rdbMgr.printTblInfo();
}

void CTestRdbManager::testRdbClear(void)
{
    printf("----------------test RdbClear ---------------\n");
    
    //m_rdbMgr.printTblInfo();
    m_rdbMgr.clear();
    m_rdbMgr.printTblInfo();
}

void CTestRdbManager::testRdbGetRecordCount(void)
{
    printf("----------------test rdb recordcount ---------------\n");
    
    int count = 0;

    count = m_rdbMgr.getRecordCount("device");
    printf("count = %d\n", count);

    printf("\n remove 3 records\n");
    m_rdbMgr.remove("device", 1);
    m_rdbMgr.remove("device", 2);
    m_rdbMgr.remove("device", 3);

    count = m_rdbMgr.getRecordCount("device");
    printf("count = %d\n", count);
}