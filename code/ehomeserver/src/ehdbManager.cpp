/********************************************************************
 File: ehdbManager.cpp
 Function: define the interface to read ehome data file
 Author: Luobihe
 Date: 2015-9-7
********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ehdbManager.h"
#include "rdbManager.h"

static DataTypeMap s_dtMap[] = {
    {JSON_ARRD_DATATYPE_BOOL, DEV_TYPE_BOOL},
    {JSON_ARRD_DATATYPE_BYTE, DEV_TYPE_BYTE},
    {JSON_ARRD_DATATYPE_SHROT, DEV_TYPE_SHORT},
    {JSON_ARRD_DATATYPE_INT, DEV_TYPE_INT},
    {JSON_ARRD_DATATYPE_LONG, DEV_TYPE_LONG},
    {JSON_ARRD_DATATYPE_FLOAT, DEV_TYPE_FLOAT},
    {JSON_ARRD_DATATYPE_DOUBLE, DEV_TYPE_DOUBLE},
};


CEhJsonReader::CEhJsonReader(void)
{
    m_jsonTransData = NULL;
}

CEhJsonReader::~CEhJsonReader(void)
{
    free();
}

void CEhJsonReader::free(void)
{
    if (m_jsonTransData) {
        cJSON_Delete(m_jsonTransData);
        m_jsonTransData = NULL;
    }
}

/*-------------------------------------------------------------------
 Function: setTbl(filename)
 Purpose: generate json to get dev data
 Parameters: filename -- [IN], json filename
 return: 0 -- success
         -1 -- failed
-------------------------------------------------------------------*/
int CEhJsonReader::setTbl(const char* filename)
{
    FILE* p_file = NULL;
    long len = 0;
    char *data = NULL;
    int ret = 0;
	
	p_file = fopen(filename,"rb");
    if (!p_file) 
        return -1;

    fseek(p_file, 0, SEEK_END);
    len = ftell(p_file);
    fseek(p_file, 0, SEEK_SET);

    data = (char*)malloc(len + 1);
    fread(data, 1, len, p_file);
    fclose(p_file);
    p_file = NULL;

    //transfer text to json struct
    ret = genTransJsonFromText(data);

    ::free(data);

    return ret;
}

/*-------------------------------------------------------------------
 Function: getDataCount()
 Purpose: get data count
 Parameters: 
 return: >=0 -- data count
         -1 -- failed
-------------------------------------------------------------------*/
int CEhJsonReader::getDataCount(void)
{
    if (!m_jsonTransData)
        return -1;

    return cJSON_GetArraySize(m_jsonTransData);
}

/*-------------------------------------------------------------------
 Function: genTransJsonFromText(text)
 Purpose: generate json to get dev data
 Parameters: text -- [IN], text to generate json
 return: 0 -- success
         -1 -- failed
-------------------------------------------------------------------*/
int CEhJsonReader::genTransJsonFromText(const char* text)
{
    cJSON* json_root     = NULL;
    cJSON* json_region   = NULL;
    cJSON* json_devArray = NULL;
    int regionCount = 0;
    int ret = 0;

    //initial
    free();

    json_root = cJSON_Parse(text);
	if (!json_root) 
        return -1;

    regionCount = cJSON_GetArraySize(json_root);
    for (int i = 0; i < regionCount; i++) {
        json_region = cJSON_GetArrayItem(json_root, i);
        if (!json_region) {
            ret = -1;
            break;
        }

        json_devArray = cJSON_GetObjectItem(json_region, JSONDEVICE);
        if (!json_devArray) {
            ret = -1;
            break;
        }

        ret = genFromDevArrayJson(json_devArray);
        if (ret)
            break;
    }

    cJSON_Delete(json_root);

    return ret;
}

/*-------------------------------------------------------------------
 Function: genFromDevArrayJson(json_devArray)
 Purpose: generate json data from dev list json
 Parameters: json_devArray -- [IN], dev list json in json file
 return: 0 -- success
         -1 -- failed
-------------------------------------------------------------------*/
int CEhJsonReader::genFromDevArrayJson(cJSON* json_devArray)
{
    cJSON* json_dev = NULL;
    cJSON* json_addrArray = NULL;
    cJSON* json_addr = NULL;
    int devCount = 0;
    int addrCount = 0;
    int devPos = 0;
    int addrPos = 0;

    if (!json_devArray)
        return -1;

    if (!m_jsonTransData)
        m_jsonTransData = cJSON_CreateArray();

    devCount = cJSON_GetArraySize(json_devArray);
    for (devPos = 0; devPos < devCount; devPos++) {
        json_dev = cJSON_GetArrayItem(json_devArray, devPos);
        if (!json_dev)
            return -1;

        json_addrArray = cJSON_GetObjectItem(json_dev, JSONADDRESS);
        if (!json_addrArray)
            return -1;

        addrCount = cJSON_GetArraySize(json_addrArray);
        for (addrPos = 0; addrPos < addrCount; addrPos++) {
            json_addr = cJSON_GetArrayItem(json_addrArray, addrPos);

            //copy
            cJSON_AddItemToArray(m_jsonTransData, 
                cJSON_Duplicate(json_addr, 1));
        }
    }

    return 0;
}

/*-------------------------------------------------------------------
 Function: fetchData(pos, p_data)
 Purpose: fetch data
 Parameters: pos -- [IN], position in json array
             p_data -- [OUT], result
 return: 0 -- success
         -1 -- failed
-------------------------------------------------------------------*/
int CEhJsonReader::fetchData(int pos, void* p_data)
{
    cJSON* json_item = NULL;
    cJSON* json_temp = NULL;
    TB001_DEVOBJ* p_devObj = (TB001_DEVOBJ*)p_data;

    if (pos < 0)
        return -1;

    if (pos >= getDataCount())
        return -1;

    json_item = cJSON_GetArrayItem(m_jsonTransData, pos);
    if (!json_item)
        return -1;

    //id
    json_temp = cJSON_GetObjectItem(json_item, JSON_ADDR_ID);
    if (!json_temp)
        return -1;

    p_devObj->iID = json_temp->valueint;

    //description
    json_temp = cJSON_GetObjectItem(json_item, JSON_ADDR_DESC);
    if (!json_temp)
        return -1;
    
    strcpy(p_devObj->szDesc, json_temp->valuestring);

    //data type
    json_temp = cJSON_GetObjectItem(json_item, JSON_ADDR_DATATYPE);
    if (!json_temp)
        return -1;
    
    p_devObj->iDataType = getDataTypeFromStr(json_temp->valuestring);

    //protocol
    p_devObj->iProtocolID = 1;

    //group addr
    json_temp = cJSON_GetObjectItem(json_item, JSON_ADDR_GROUPADDR);
    if (!json_temp)
        return -1;
    
    strcpy(p_devObj->szGroupAddr, json_temp->valuestring);

    //status group addr
    json_temp = cJSON_GetObjectItem(json_item, JSON_ADDR_STATUS_GROUPADDR);
    if (!json_temp)
        return -1;
    
    strcpy(p_devObj->szGroupAddr2, json_temp->valuestring);

    //value
    strcpy(p_devObj->szValue, "0");

    return 0;
}

int CEhJsonReader::getDataTypeFromStr(const char* p_str)
{
    for (int i = 0; i < sizeof(s_dtMap) / sizeof(DataTypeMap); i++) {
        if (strcmp(p_str, s_dtMap[i].dtdesc) == 0)
            return s_dtMap[i].dataType;
    }

    return -1;
}