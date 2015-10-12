/********************************************************************
 File: ip_knxPtl.cpp
 Function: define the protocol to communicate with ip_knx device
 Author: Luobihe
 Date: 2015-8-4
********************************************************************/

#include <stdio.h>
#include "ip_knxPtl.h"
#include "rdbManager.h"

/*-------------------------------------------------------------------
 Function: init()
 Purpose: initial, create socket, connect device
 Parameters: 
 return: 0 --  success
         -1 -- failed
-------------------------------------------------------------------*/
int CIpKnxPtl::init(void)
{
    FILE *pFile = NULL;
    char szFilePath[256] = {0};
    char szFileName[256] = {0};
    char szCfgFilePath[256] = {0};
    char szIp[64] = {0};
    int  iPort = 0;

    if (getExePath(szFilePath, szFileName, 256))
        return -1;

    sprintf(szCfgFilePath, "%s/%s", szFilePath, IPKNXFILE);

    pFile = fopen(szCfgFilePath, "rb");
    if (pFile) {
        fscanf(pFile, "%[^:]%*c%d", szIp, &iPort);
        fclose(pFile);
        pFile = NULL;
    }

    return m_ipKnxApp.init(szIp, iPort);
}

int CIpKnxPtl::readDev(const char* szAddr, void* p_value, int iDataType)
{
    int iLen = dtToIpKnxLen(iDataType);
    if (iLen < 1)
        return -1;

    return m_ipKnxApp.readGroupAddr((char*)szAddr, (int*)p_value, iLen);
}

int CIpKnxPtl::writeDev(const char* szAddr, void* p_value, int iDataType)
{
    int iLen = dtToIpKnxLen(iDataType);
    if (iLen < 1)
        return -1;

    return m_ipKnxApp.writeGroupAddr((char*)szAddr, *(int*)p_value, iLen);
}

int CIpKnxPtl::monitorDev(const char* szAddr, int iDataType)
{
    int iLen = dtToIpKnxLen(iDataType);
    if (iLen < 1)
        return -1;

    return m_ipKnxApp.monitorGroupAddr((char*)szAddr, iLen);
}

/*-------------------------------------------------------------------
 Function: dtToIpKnxLen(iDataType)
 Purpose: get data len used in ip_knx protocol from datatype
 Parameters: 
 return: 0 --  success
         -1 -- failed
-------------------------------------------------------------------*/
int CIpKnxPtl::dtToIpKnxLen(int iDataType)
{
    int iRet = 0;

    switch (iDataType) {
        case DEV_TYPE_BOOL:
            iRet = 1;
            break;

        case DEV_TYPE_BYTE:
            iRet = 2;
            break;

        case DEV_TYPE_SHORT:
            iRet = 3;
            break;

        case DEV_TYPE_INT:
            iRet = 5;
            break;

        case DEV_TYPE_LONG:
            iRet = 9;
            break;

        case DEV_TYPE_FLOAT:
            iRet = 5;
            break;

        case DEV_TYPE_DOUBLE:
            iRet = 9;

        default:
            break;
    }

    return iRet;
}
