#ifndef __TESTRDBMANAGER_H__
#define __TESTRDBMANAGER_H__

#include "rdbManager.h"


class CTestRdbManager
{
private:
    CDevTblOperator m_devTblOp;

    CRDBManager m_rdbMgr;

    void testDevTblOp_insert(void);
    void testDevTblOp_find(void);
    void testDevTblOp_findByIndex(void);
    void testDevTblOp_update(void);
    void testDevTblOp_remove(void);
    void testDevTblOp_removeAll(void);
    void testDevTblOp_getRecordCount(void);
    void testDevTblOp_updateByGrpAddr(void);
    void testDevTblOp_selectByGroupAddr(void);

    void testDevTblOp(void);

    void printDevObj(TB001_DEVOBJ* p_obj);


    void testRdbRegisterTbl(void);
    void testRdbInsert(void);
    void testRdbUpdate(void);
    void testRdbRemove(void);
    void testRdbFind(void);
    void testRdbFindByIndex(void);

    void testRdbInitDev(void);
    void testRdbClear(void);
    void testRdbGetRecordCount(void);

    void testRdbMgr(void);

    void testRdb_updateDevValByGroupAddr(void);


public:
    void test(void);
};

#endif  //__TESTRDBMANAGER_H__