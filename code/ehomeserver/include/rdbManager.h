/********************************************************************
 File: rdbManager.h
 Function: define data type used in ehome
 Author: Luobihe
 Date: 2015-7-30
********************************************************************/

#ifndef __RDBMANAGER_H__
#define __RDBMANAGER_H__

#include <list>
#include <map>
#include <string>
using namespace std;

#include "platcomm.h"

#define TBL_DEV  "device"

#define JSON_FILENAME  "knx_json.dat"


//数据点的数据类型
enum DevDataType{
    DEV_TYPE_BOOL = 1,
    DEV_TYPE_BYTE,
    DEV_TYPE_SHORT,
    DEV_TYPE_INT,
    DEV_TYPE_LONG,
    DEV_TYPE_FLOAT,
    DEV_TYPE_DOUBLE,
};

//设备表
typedef struct {
    int iID;                      //id
    char szDesc[64];              //discription
    int iDataType;                //data type
    int iProtocolID;              //protocol ID
    char szGroupAddr[64];         //communictation address
    char szGroupAddr2[64];        //group address to get status
    char szValue[64];             //value
} TB001_DEVOBJ, *PTB001_DEVOBJ;

typedef struct {
    int iID;
    char* p_desc;
    int iType;
    char* p_groupAddr;
} DevTbl;

//user table
typedef struct {
    char szUserName[32];         //user name
    char szPassWord[32];         //password
    char szFileName[256];        //json file name
} TB004_USER, *PTB004_USER;


int getSizeofDataType(int iDataType);


/*-------------------------------------------------------------------
 Function: rdb_init()
 Purpose: initial table manager
 Parameters: 
 return: 0 -- success
         -1 -- failed
-------------------------------------------------------------------*/
int rdb_init(void);

/*-------------------------------------------------------------------
 Function: rdb_deinit()
 Purpose: 
 Parameters: 
 return: 0 -- success
         -1 -- failed
-------------------------------------------------------------------*/
int rdb_deinit(void);

int rdb_getRecordCount(const char* szTblName);

//0 -- find, -1 -- not find
int rdb_find(const char* szTblName, void* p_node, int key);

//0 -- find, -1 -- not find
int rdb_findByIndex(const char* szTblName, void* p_node, int index);

//0 -- success, -1 -- failed
int rdb_update(const char* szTblName, void* p_node, int key);

/*-------------------------------------------------------------------
 Function: rdb_updateDevValByGroupAddr()
 Purpose: updata table 'device' by group address
 Parameters: szGroupAddr -- [IN], group address
             szVal -- [IN], new value to update
 return: 0 -- success
         -1 -- failed
-------------------------------------------------------------------*/
int rdb_updateDevValByGroupAddr(const char* szGroupAddr, const char* szVal);

/*-------------------------------------------------------------------
 Function: rdb_selectDevNodeByGroupAddr()
 Purpose: select a dev node by group address
 Parameters: szGroupAddr -- [IN], group address
             p_node -- [OUT], result
 return: 0 -- success
         -1 -- failed
-------------------------------------------------------------------*/
int rdb_selectDevNodeByGroupAddr(const char* szGroupAddr, void* p_node);


/////////////////////////////////////////////////////////////////////
//interface of table operation
class CTblOperator
{
public:
    //0 -- success, -1 -- failed
    virtual int insert(void* p_node) = 0;

    //0 -- success, -1 -- failed
    virtual int remove(int key) = 0;

    //0 -- success, -1 -- failed
    virtual int update(void* p_node, int key) = 0;

    //0 -- success, -1 -- failed
    virtual int find(void* p_node, int key) = 0;

    //0 -- success, -1 -- failed
    virtual int findByIndex(void* p_node, int index) = 0;

    virtual int getRecordCount(void) = 0;

    virtual void removeAll(void) = 0;

    virtual void printAllNode(void) = 0;
};

typedef struct {
    char* szTblName;
    CTblOperator* p_op;
} TBLOBJ, *PTBLOBJ;

/////////////////////////////////////////////////////////////////////
//dev table operator
class CDevTblOperator : public CTblOperator
{
private:
    list <PTB001_DEVOBJ> m_listDev;
    EasyMutex m_DevTblLock;    //lock

public:
    CDevTblOperator(void);
    ~CDevTblOperator(void);

    virtual int insert(void* p_node);
    virtual int remove(int key);
    virtual int update(void* p_node, int key);
    virtual int find(void* p_node, int key);
    virtual int findByIndex(void* p_node, int index);
    virtual void removeAll(void);
    virtual int getRecordCount(void);

    int updateByGroupAddr(const char* szGroupAddr, const char* szVal);
    int selectByGroupAddr(const char* szGroupAddr, void* p_node);

    void printAllNode(void);
};


/////////////////////////////////////////////////////////////////////
class CRDBManager
{
private:
    map<string, CTblOperator*> m_mapTbl;

    EasyMutex m_MutexLock;    //lock

public:
    CRDBManager(void);
    ~CRDBManager(void);

    void clear(void);

    //initial device
    int initDev(void);

    //register table to map
    int registerTbl(const char* szTblName, CTblOperator* p_tblop);

    //record count of a table
    int getRecordCount(const char* szTblName);

    //insert point data to table, 0 -- success, -1 -- failed
    int insert(const char* szTblName, void* p_node);

    //0 -- success, -1 -- failed
    int remove(const char* szTblName, int key);

    //0 -- success, -1 -- failed
    int update(const char* szTblName, void* p_node, int key);

    //0 -- success, -1 -- failed
    int find(const char* szTblName, void* p_node, int key);

    //0 -- success, -1 -- failed
    int findByIndex(const char* szTblName, void* p_node, int index);

    void printTbl(const char* szTblName);

    void printTblInfo(void);
};


#endif  //__RDBMANAGER_H__