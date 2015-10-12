#include <stdlib.h>
#include <stdio.h>
#include "testEhdbManager.h"
#include "rdbManager.h"

void CTestEhdbManager::testGenFromDevArrayJson(void)
{
    printf("=============test genFromDevArrayJson======================\n");

    FILE* p_file = NULL;
    long len = 0;
    char *data = NULL;
	
	p_file = fopen("test1.dat","rb");
    if (!p_file) 
        return ;

    fseek(p_file, 0, SEEK_END);
    len = ftell(p_file);
    fseek(p_file, 0, SEEK_SET);

    data = (char*)malloc(len + 1);
    fread(data, 1, len, p_file);
    fclose(p_file);
    p_file = NULL;

    //transfer text to json struct

    char *out = NULL;
    cJSON *json = NULL;
    json = cJSON_Parse(data);
	if (!json) {printf("Error before: [%s]\n",cJSON_GetErrorPtr());}
	else
	{
        if (m_jsonReader.genFromDevArrayJson(json)) {
            printf("failed to generate\n");
            return ;
        }

		out = cJSON_Print(m_jsonReader.getJson());
		cJSON_Delete(json);
		printf("%s\n",out);
		free(out);
	}

    ::free(data);
}

void CTestEhdbManager::testGenTransJsonFromText(void)
{
    printf("=============test genTransJsonFromText======================\n");

    FILE* p_file = NULL;
    long len = 0;
    char *data = NULL;
	
	p_file = fopen("test2.dat","rb");
    if (!p_file) 
        return ;

    fseek(p_file, 0, SEEK_END);
    len = ftell(p_file);
    fseek(p_file, 0, SEEK_SET);

    data = (char*)malloc(len + 1);
    fread(data, 1, len, p_file);
    fclose(p_file);
    p_file = NULL;

    //transfer text to json struct

    if (m_jsonReader.genTransJsonFromText(data))
        printf("failed to generate\n");
    else {
        char* out = NULL;
        out = cJSON_Print(m_jsonReader.getJson());
		printf("%s\n",out);
		free(out);
    }

    ::free(data);
}

void CTestEhdbManager::testSetTbl(void)
{
    printf("=============test selectTbl======================\n");

    if (m_jsonReader.setTbl("test2.dat"))
        printf("failed to select tbl\n");
    else {
        char* out = NULL;
        out = cJSON_Print(m_jsonReader.getJson());
		printf("%s\n",out);
		free(out);
    }
}

void CTestEhdbManager::testGetDataCount(void)
{
    printf("================test GetDataCount====================\n");
    printf("count = %d\n", m_jsonReader.getDataCount());
}

void CTestEhdbManager::testGetDataTypeFromStr(void)
{
    printf("================test GetDataTypeFromStr====================\n");

    printf("%s, type = %d\n", JSON_ARRD_DATATYPE_BOOL,
        m_jsonReader.getDataTypeFromStr(JSON_ARRD_DATATYPE_BOOL));

    printf("%s, type = %d\n", JSON_ARRD_DATATYPE_BYTE,
        m_jsonReader.getDataTypeFromStr(JSON_ARRD_DATATYPE_BYTE));

    printf("%s, type = %d\n", JSON_ARRD_DATATYPE_SHROT,
        m_jsonReader.getDataTypeFromStr(JSON_ARRD_DATATYPE_SHROT));

    printf("%s, type = %d\n", JSON_ARRD_DATATYPE_INT,
        m_jsonReader.getDataTypeFromStr(JSON_ARRD_DATATYPE_INT));

    printf("%s, type = %d\n", JSON_ARRD_DATATYPE_LONG,
        m_jsonReader.getDataTypeFromStr(JSON_ARRD_DATATYPE_LONG));

    printf("%s, type = %d\n", JSON_ARRD_DATATYPE_FLOAT,
        m_jsonReader.getDataTypeFromStr(JSON_ARRD_DATATYPE_FLOAT));

    printf("%s, type = %d\n", JSON_ARRD_DATATYPE_DOUBLE,
        m_jsonReader.getDataTypeFromStr(JSON_ARRD_DATATYPE_DOUBLE));

    printf("%s, type = %d\n", "test",
        m_jsonReader.getDataTypeFromStr("test"));
}

void CTestEhdbManager::testFetchData(void)
{
    printf("================test fetchData====================\n");
    TB001_DEVOBJ devObj = {0};

    for (int i = 0 ; i < m_jsonReader.getDataCount() + 1; i++) {
        if (m_jsonReader.fetchData(i, &devObj))
            printf("fetch failed\n");
        else 
            printf("id = %d, desc = %s, dataype = %d, protocolID = %d,"
            " groupAddr = %s, status groupAddr = %s, value = %s\n",
            devObj.iID, devObj.szDesc, devObj.iDataType,
            devObj.iProtocolID, devObj.szGroupAddr, devObj.szGroupAddr2,
            devObj.szValue);
    }
}

void CTestEhdbManager::test(void)
{
    //testGenFromDevArrayJson();
    //testGenTransJsonFromText();
    //testGetDataCount();
    testSetTbl();
    //testGetDataCount();
    //testGetDataTypeFromStr();

    testFetchData();
}