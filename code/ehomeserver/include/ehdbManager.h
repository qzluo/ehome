/********************************************************************
 File: ehdbManager.h
 Function: define the interface to read ehome data file
 Author: Luobihe
 Date: 2015-9-7
********************************************************************/

#ifndef __EHDBMANAGER_H__
#define __EHDBMANAGER_H__

#include "cJSON.h"

#define JSONDEVICE      "devices"
#define JSONADDRESS     "address"

#define JSON_ADDR_ID                "addr id"
#define JSON_ADDR_DESC              "description"
#define JSON_ADDR_DATATYPE          "data type"
#define JSON_ADDR_GROUPADDR         "address"
#define JSON_ADDR_STATUS_GROUPADDR  "status address"


#define JSON_ARRD_DATATYPE_BOOL     "bool"
#define JSON_ARRD_DATATYPE_BYTE     "byte"
#define JSON_ARRD_DATATYPE_SHROT    "short"
#define JSON_ARRD_DATATYPE_INT      "integer"
#define JSON_ARRD_DATATYPE_LONG     "long"
#define JSON_ARRD_DATATYPE_FLOAT    "float"
#define JSON_ARRD_DATATYPE_DOUBLE   "double"


//数据类型
typedef struct {
    char* dtdesc;
    int dataType;
} DataTypeMap;

class CEhDbInterface
{
public:
    virtual int setTbl(const char* filename) = 0;
    virtual int getDataCount(void) = 0;
    virtual int fetchData(int pos, void* p_data) = 0;
};

class CEhJsonReader : public CEhDbInterface
{
public:
    cJSON* m_jsonTransData;

private:
    void free(void);

public:
    //0 -- success, -1 -- failed
    int genTransJsonFromText(const char* text);

    int genFromDevArrayJson(cJSON* json_devArray);

    int getDataTypeFromStr(const char* p_str);

    cJSON* getJson(void) { return m_jsonTransData; }

public:
    CEhJsonReader(void);
    ~CEhJsonReader(void);

    int setTbl(const char* filename);
    int getDataCount(void);
    int fetchData(int pos, void* p_data);
};

#endif  //__EHDBMANAGER_H__