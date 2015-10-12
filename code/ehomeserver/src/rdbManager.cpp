/********************************************************************
 File: rdbManager.cpp
 Function: define data type used in ehome
 Author: Luobihe
 Date: 2015-7-30
********************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "rdbManager.h"
#include "ehDataPushOutor.h"
#include "ehdbManager.h"
#include "logsys.h"

static DevTbl s_aDevInfo[] = {
    {1, "lamp", DEV_TYPE_BOOL, "10/1/11"},
    {2, "lamp", DEV_TYPE_BOOL, "10/1/12"},
    {3, "lamp", DEV_TYPE_BOOL, "10/1/13"},
    {4, "lamp", DEV_TYPE_BOOL, "10/1/31"},
    {5, "lamp", DEV_TYPE_BOOL, "10/1/32"},
    {6, "lamp", DEV_TYPE_BOOL, "10/1/33"},
    {7, "lamp", DEV_TYPE_BOOL, "10/2/11"},
    {8, "lamp", DEV_TYPE_BOOL, "10/2/12"},

    {9, "lamp", DEV_TYPE_BOOL, "10/2/13"},
    {10, "lamp", DEV_TYPE_BOOL, "10/2/14"},
    {11, "lamp", DEV_TYPE_BYTE, "10/2/16"},
    {12, "lamp", DEV_TYPE_BOOL, "10/2/18"},
    {13, "lamp", DEV_TYPE_BOOL, "10/2/31"},
    {14, "lamp", DEV_TYPE_BOOL, "10/2/32"},
    {15, "lamp", DEV_TYPE_BOOL, "10/2/33"},
    {16, "lamp", DEV_TYPE_BOOL, "10/2/34"},

    {17, "curtain", DEV_TYPE_BOOL, "10/2/61"},
    {18, "curtain", DEV_TYPE_BOOL, "10/2/62"},
    {19, "curtain", DEV_TYPE_BOOL, "10/2/63"},
    {20, "curtain", DEV_TYPE_BOOL, "10/2/64"},
} ;

static CDevTblOperator s_devTblOp;  //device table operator

static TBLOBJ s_aTblObj[] = {
    {TBL_DEV, &s_devTblOp}
};

static CRDBManager s_rdbmgr;    //tabel manager

int getSizeofDataType(int iDataType)
{
    int iRet = 0;
    switch (iDataType) {
        case DEV_TYPE_BOOL:
        case DEV_TYPE_BYTE:
            iRet = 1;
            break;

        case DEV_TYPE_SHORT:
            iRet = sizeof(short);
            break;

        case DEV_TYPE_INT:
            iRet = sizeof(int);
            break;

        case DEV_TYPE_LONG:
            iRet = sizeof(long);
            break;

        case DEV_TYPE_FLOAT:
            iRet = sizeof(float);
            break;

        case DEV_TYPE_DOUBLE:
            iRet = sizeof(double);
            break;

        default:
            break;
    }

    return iRet;
}


/*-------------------------------------------------------------------
 Function: rdb_init()
 Purpose: initial table manager
 Parameters: 
 return: 0 -- success
         -1 -- failed
-------------------------------------------------------------------*/
int rdb_init(void)
{
    if (s_rdbmgr.initDev())
        return -1;

    return 0;
}

/*-------------------------------------------------------------------
 Function: rdb_deinit()
 Purpose: 
 Parameters: 
 return: 0 -- success
         -1 -- failed
-------------------------------------------------------------------*/
int rdb_deinit(void)
{
    s_rdbmgr.clear();

    return 0;
}

/*-------------------------------------------------------------------
 Function: rdb_getRecordCount(szTblName)
 Purpose: get the record count of a table with a table name
 Parameters: szTblName -- [IN], table name
 return: -1 -- table is not found
         >=0 -- record count
-------------------------------------------------------------------*/
int rdb_getRecordCount(const char* szTblName)
{
    return s_rdbmgr.getRecordCount(szTblName);
}

/*-------------------------------------------------------------------
 Function: rdb_find()
 Purpose: 
 Parameters: szTblName -- [IN], table name
             p_node -- [OUT], result
             key -- [IN], index
 return: 0 -- success
         -1 -- failed
-------------------------------------------------------------------*/
int rdb_find(const char* szTblName, void* p_node, int key)
{
    return s_rdbmgr.find(szTblName, p_node, key);
}

/*-------------------------------------------------------------------
 Function: rdb_findByIndex()
 Purpose: 
 Parameters: szTblName -- [IN], table name
             p_node -- [OUT], result
             index -- [IN], index
 return: 0 -- success
         -1 -- failed
-------------------------------------------------------------------*/
int rdb_findByIndex(const char* szTblName, void* p_node, int index)
{
    return s_rdbmgr.findByIndex(szTblName, p_node, index);
}

/*-------------------------------------------------------------------
 Function: rdb_update()
 Purpose: 
 Parameters: szTblName -- [IN], table name
             p_node -- [OUT], result
             key -- [IN], index
 return: 0 -- success
         -1 -- failed
-------------------------------------------------------------------*/
int rdb_update(const char* szTblName, void* p_node, int key)
{
    return s_rdbmgr.update(szTblName, p_node, key);
}

/*-------------------------------------------------------------------
 Function: rdb_updateDevValByGroupAddr()
 Purpose: updata table 'device' by group address
 Parameters: szGroupAddr -- [IN], group address
             szVal -- [IN], new value to update
 return: 0 -- success
         -1 -- failed
-------------------------------------------------------------------*/
int rdb_updateDevValByGroupAddr(const char* szGroupAddr, 
                                const char* szVal)
{
    return s_devTblOp.updateByGroupAddr(szGroupAddr, szVal);
}

/*-------------------------------------------------------------------
 Function: rdb_selectDevNodeByGroupAddr()
 Purpose: select a dev node by group address
 Parameters: szGroupAddr -- [IN], group address
             p_node -- [OUT], result
 return: 0 -- success
         -1 -- failed
-------------------------------------------------------------------*/
int rdb_selectDevNodeByGroupAddr(const char* szGroupAddr, void* p_node)
{
    return s_devTblOp.selectByGroupAddr(szGroupAddr, p_node);
}


/////////////////////////////////////////////////////////////////////
//CDevTblOperator

CDevTblOperator::CDevTblOperator(void)
{
    m_DevTblLock.init();
}

CDevTblOperator::~CDevTblOperator(void)
{
    m_DevTblLock.destroy();
}

/*-------------------------------------------------------------------
 Function: insert(p_node)
 Purpose: insert a node to device table
 Parameters: p_node -- [IN], node to insert
 return: 0 -- success
         -1 -- failed
-------------------------------------------------------------------*/
int CDevTblOperator::insert(void* p_node)
{
    PTB001_DEVOBJ p_devobj = (PTB001_DEVOBJ)p_node;

    //check key
    int key = p_devobj->iID;
    TB001_DEVOBJ devobj = {0};
    if (find(&devobj, key) == 0)
        return -1;

    //add to list
    PTB001_DEVOBJ p_objToAdd = new TB001_DEVOBJ;
    if (!p_objToAdd) 
        return -1;

    memcpy(p_objToAdd, p_node, sizeof(TB001_DEVOBJ));

    m_DevTblLock.lock();
    m_listDev.push_back(p_objToAdd);
    m_DevTblLock.unlock();

    return 0;
}

/*-------------------------------------------------------------------
 Function: remove(key)
 Purpose: remove device obj from table index by a key
 Parameters: key -- [IN], index
 return: 0 -- find the object
         -1 -- not find
-------------------------------------------------------------------*/
int CDevTblOperator::remove(int key)
{
    PTB001_DEVOBJ p_devTemp = NULL;
    int iRet = -1;

    m_DevTblLock.lock();

    list <PTB001_DEVOBJ>::iterator pos = m_listDev.begin();
    list <PTB001_DEVOBJ>::iterator last = m_listDev.end();

    while (pos != last) {
        p_devTemp = *pos;
        if (p_devTemp->iID == key) {
            delete p_devTemp;
            p_devTemp = NULL;

            pos = m_listDev.erase(pos);
            iRet = 0;
            break;
        }
        else
            pos++;
    }

    m_DevTblLock.unlock();

    return iRet;
}

/*-------------------------------------------------------------------
 Function: update(p_node, key)
 Purpose: update device obj from table index by a key
 Parameters: p_node -- [IN], new data
             key -- [IN], index
 return: 0 -- find the object
         -1 -- not find
-------------------------------------------------------------------*/
int CDevTblOperator::update(void* p_node, int key)
{
    PTB001_DEVOBJ p_devTemp = NULL;
    int iRet = -1;

    m_DevTblLock.lock();

    list <PTB001_DEVOBJ>::iterator pos = m_listDev.begin();
    list <PTB001_DEVOBJ>::iterator last = m_listDev.end();

    while (pos != last) {
        p_devTemp = *pos;
        if (p_devTemp->iID == key) {
            memcpy(p_devTemp, p_node, sizeof(TB001_DEVOBJ));
            iRet = 0;
            break;
        }

        pos++;
    }

    m_DevTblLock.unlock();

    return iRet;
}

/*-------------------------------------------------------------------
 Function: find(p_node, key)
 Purpose: find device obj from table index by a key
 Parameters: p_node -- [OUT], result
             key -- [IN], index
 return: 0 -- find the object
         -1 -- not find
-------------------------------------------------------------------*/
int CDevTblOperator::find(void* p_node, int key)
{
    PTB001_DEVOBJ p_devTemp = NULL;
    int iRet = -1;

    m_DevTblLock.lock();

    list <PTB001_DEVOBJ>::iterator pos = m_listDev.begin();
    list <PTB001_DEVOBJ>::iterator last = m_listDev.end();

    while (pos != last) {
        p_devTemp = *pos;
        if (p_devTemp->iID == key) {
            memcpy(p_node, p_devTemp, sizeof(TB001_DEVOBJ));
            iRet = 0;
            break;
        }

        pos++;
    }

    m_DevTblLock.unlock();

    return iRet;
}

/*-------------------------------------------------------------------
 Function: findByIndex(p_node, index)
 Purpose: find device obj from table index by index
 Parameters: p_node -- [OUT], result
             index -- [IN], index
 return: 0 -- find the object
         -1 -- not find
-------------------------------------------------------------------*/
int CDevTblOperator::findByIndex(void* p_node, int index)
{
    PTB001_DEVOBJ p_devTemp = NULL;
    int iRet = -1;

    if (index >= getRecordCount())
        return -1;

    m_DevTblLock.lock();

    list <PTB001_DEVOBJ>::iterator pos = m_listDev.begin();
    list <PTB001_DEVOBJ>::iterator last = m_listDev.end();
    int iPos = 0;

    while (pos != last) {
        if (iPos == index) {
            p_devTemp = *pos;
            memcpy(p_node, p_devTemp, sizeof(TB001_DEVOBJ));
            iRet = 0;
            break;
        }

        pos++;
        iPos++;
    }

    m_DevTblLock.unlock();

    return iRet;
}

void CDevTblOperator::removeAll(void)
{
    m_DevTblLock.lock();

    list <PTB001_DEVOBJ>::iterator pos = m_listDev.begin();
    list <PTB001_DEVOBJ>::iterator last = m_listDev.end();

    while (pos != last) {
        PTB001_DEVOBJ p_devobj = *pos;
        delete p_devobj;

        pos = m_listDev.erase(pos);              
    }

    m_DevTblLock.unlock();
}

int CDevTblOperator::getRecordCount(void)
{
    int size = 0;

    m_DevTblLock.lock();
    size = m_listDev.size();
    m_DevTblLock.unlock();

    return size;
}

//0 -- success, -1 -- failed
int CDevTblOperator::updateByGroupAddr(const char* szGroupAddr, 
                                       const char* szVal)
{
    PTB001_DEVOBJ p_devTemp = NULL;
    int iRet = -1;
    char szLogout[1024] = {0};

    m_DevTblLock.lock();
    
    list <PTB001_DEVOBJ>::iterator pos = m_listDev.begin();
    list <PTB001_DEVOBJ>::iterator last = m_listDev.end();

    while (pos != last) {
        p_devTemp = *pos;
        if (strcmp(p_devTemp->szGroupAddr, szGroupAddr) == 0) {
            if (strcmp(p_devTemp->szValue, szVal) != 0) {
                strcpy(p_devTemp->szValue, szVal);       
                int value = atoi(szVal);

                sprintf(szLogout, "pushout data, addr: %s, value = %s", szGroupAddr, szVal);
                //logout(LOG_ALARM, szLogout, __FILE__, __LINE__);

                pushOutDevData(p_devTemp->iID, &value, p_devTemp->iDataType);
            }

            iRet = 0;
            break;
        }

        pos++;
    }

    m_DevTblLock.unlock();

    return iRet;
}

int CDevTblOperator::selectByGroupAddr(const char* szGroupAddr, void* p_node)
{
    PTB001_DEVOBJ p_devTemp = NULL;
    int iRet = -1;

    m_DevTblLock.lock();

    list <PTB001_DEVOBJ>::iterator pos = m_listDev.begin();
    list <PTB001_DEVOBJ>::iterator last = m_listDev.end();

    while (pos != last) {
        p_devTemp = *pos;
        if (strcmp(p_devTemp->szGroupAddr, szGroupAddr) == 0) {
            memcpy(p_node, p_devTemp, sizeof(TB001_DEVOBJ));
            iRet = 0;
            break;
        }

        pos++;
    }

    m_DevTblLock.unlock();

    return iRet;
}

void CDevTblOperator::printAllNode(void)
{
    int num = m_listDev.size();

    printf("node number: %d\n", num);

    list <PTB001_DEVOBJ>::iterator pos = m_listDev.begin();
    list <PTB001_DEVOBJ>::iterator last = m_listDev.end();

    int i = 0;

    while (pos != last) {
        PTB001_DEVOBJ p_devobj = *pos;

        printf("Item %d info:\n", ++i);
        printf("id: %d\n", p_devobj->iID);
        printf("desc: %s\n", p_devobj->szDesc);
        printf("DataType: %d\n", p_devobj->iDataType);
        printf("ProtocolID: %d\n", p_devobj->iProtocolID);
        printf("GroupAddr: %s\n", p_devobj->szGroupAddr);
        printf("Value: %s\n\n", p_devobj->szValue);

        pos++;              
    }
}


/////////////////////////////////////////////////////////////////////
//CRDBManager

CRDBManager::CRDBManager(void)
{
    m_MutexLock.init();
}

CRDBManager::~CRDBManager(void)
{
    clear();

    m_MutexLock.destroy();
}

void CRDBManager::clear(void)
{
    m_MutexLock.lock();

    typedef map<string, CTblOperator*>::iterator IT;
    for (IT it = m_mapTbl.begin (); it != m_mapTbl.end (); ++it) {
        it->second->removeAll();
    }

    m_mapTbl.clear();

    m_MutexLock.unlock();
}

int CRDBManager::initDev(void)
{
    clear();

    PTBLOBJ p_result = NULL;

    for (int i = 0; i < sizeof(s_aTblObj) / sizeof(TBLOBJ); i++) {
        p_result = &s_aTblObj[i];
        registerTbl(p_result->szTblName, p_result->p_op);
    }

    //insert table information
    TB001_DEVOBJ devObj = {0};
    CEhJsonReader jsonReader;

    char szFilePath[256] = {0};
    char szFileName[256] = {0};
    char szCfgFilePath[256] = {0};

    if (getExePath(szFilePath, szFileName, 256))
        return -1;

    sprintf(szCfgFilePath, "%s/%s", szFilePath, JSON_FILENAME);
    if (jsonReader.setTbl(szCfgFilePath)) {
        logout(LOG_ALARM, "read json file failed", __FILE__, __LINE__);
        return -1;
    }

    logout(LOG_INFO, "read json file success", __FILE__, __LINE__);
    char szLogOut[2048] = {0};

    int devCount = jsonReader.getDataCount();
    for (int i = 0; i < devCount; i++) {
        if (jsonReader.fetchData(i, &devObj)) {
            logout(LOG_ALARM, "read json data failed", __FILE__, __LINE__);
            return -1;
        }

        sprintf(szLogOut, "id = %d, desc = %s, dataype = %d, protocolID = %d,"
            " groupAddr = %s, status groupAddr = %s, value = %s\n",
            devObj.iID, devObj.szDesc, devObj.iDataType,
            devObj.iProtocolID, devObj.szGroupAddr, devObj.szGroupAddr2,
            devObj.szValue);

        logout(LOG_INFO, szLogOut, __FILE__, __LINE__);

        if (insert(s_aTblObj[0].szTblName, &devObj)) {
            logout(LOG_ALARM, "insert data failed", __FILE__, __LINE__);
            return -1;
        }
    }

    logout(LOG_INFO, "init device successful", __FILE__, __LINE__);
    
    return 0;
}

/*-------------------------------------------------------------------
 Function: registerTbl(szTblName, p_tblop)
 Purpose: register table to map
 Parameters: szTblName -- [IN], table name
             p_tblop -- [IN], table pointer
 return: 0 -- success
         -1 -- failed
-------------------------------------------------------------------*/
int CRDBManager::registerTbl(const char* szTblName, CTblOperator* p_tblop)
{    
    pair<map<string, CTblOperator*>::iterator, bool> insert_pair;

    m_MutexLock.lock();
    insert_pair = m_mapTbl.insert(make_pair(szTblName, p_tblop));
    m_MutexLock.unlock();

    return insert_pair.second ? 0 : -1;
}

int CRDBManager::getRecordCount(const char* szTblName)
{
    int count = 0;
    map<string, CTblOperator*>::iterator it;

    m_MutexLock.lock();

    it = m_mapTbl.find(szTblName);
    if (it == m_mapTbl.end()) {
        m_MutexLock.unlock();
        return -1;
    }
    
    count = it->second->getRecordCount();

    m_MutexLock.unlock();

    return count;
}

int CRDBManager::insert(const char* szTblName, void* p_node)
{
    int ret = 0;
    map<string, CTblOperator*>::iterator it;

    m_MutexLock.lock();

    it = m_mapTbl.find(szTblName);
    if (it == m_mapTbl.end()) {
        m_MutexLock.unlock();
        return -1;
    }
    
    ret = it->second->insert(p_node);

    m_MutexLock.unlock();

    return ret;
}

int CRDBManager::remove(const char* szTblName, int key)
{
    int ret = 0;
    map<string, CTblOperator*>::iterator it;

    m_MutexLock.lock();
    it = m_mapTbl.find(szTblName);
    if (it == m_mapTbl.end()){
        m_MutexLock.unlock();
        return -1;
    }

    ret = it->second->remove(key);
    m_MutexLock.unlock();

    return ret;
}

int CRDBManager::update(const char* szTblName, void* p_node, int key)
{
    int ret = 0;
    map<string, CTblOperator*>::iterator it;

    m_MutexLock.lock();
    it = m_mapTbl.find(szTblName);
    if (it == m_mapTbl.end()){
        m_MutexLock.unlock();
        return -1;
    }

    ret = it->second->update(p_node, key);
    m_MutexLock.unlock();

    return ret;
}

int CRDBManager::find(const char* szTblName, void* p_node, int key)
{
    int ret = 0;
    map<string, CTblOperator*>::iterator it;

    m_MutexLock.lock();
    it = m_mapTbl.find(szTblName);
    if (it == m_mapTbl.end()) {
        m_MutexLock.unlock();
        return -1;
    }
    
    ret = it->second->find(p_node, key);
    m_MutexLock.unlock();

    return ret;
}

int CRDBManager::findByIndex(const char* szTblName, void* p_node, int index)
{
    int ret = 0;
    map<string, CTblOperator*>::iterator it;

    m_MutexLock.lock();
    it = m_mapTbl.find(szTblName);
    if (it == m_mapTbl.end()) {
        m_MutexLock.unlock();
        return -1;
    }
    
    ret = it->second->findByIndex(p_node, index);
    m_MutexLock.unlock();

    return ret;
}

void CRDBManager::printTbl(const char* szTblName)
{
    map<string, CTblOperator*>::iterator it;
    
    m_MutexLock.lock();
    it = m_mapTbl.find(szTblName);
    if (it == m_mapTbl.end()) {
        m_MutexLock.unlock();
        return ;
    }

    it->second->printAllNode();
    m_MutexLock.unlock();
}

void CRDBManager::printTblInfo(void)
{
    typedef map<string, CTblOperator*>::iterator IT;

    m_MutexLock.lock();
    for (IT it = m_mapTbl.begin (); 
        it != m_mapTbl.end (); ++it)
        it->second->printAllNode();
    m_MutexLock.unlock();
}