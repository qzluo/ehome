/********************************************************************
 File: EHSCmdParser.cpp
 Function: provide operation by ehome server to parse command
 Author: Luobihe
 Date: 2015-7-30
*********************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "EHSCmdParser.h"
#include "ehservercomm.h"
#include "rdbManager.h"
#include "ptlManager.h"
#include "logsys.h"

static unsigned char s_gCmdId[] = {
    NEWBEST_CMD_GETJSONFILE,
    NEWBEST_CMD_ADDR_READ,
    NEWBEST_CMD_ADDR_WRITE,
    NEWBEST_CMD_GETJSONFILE_ACK,
    NEWBEST_CMD_ADDR_READ_ACK,
    NEWBEST_CMD_ADDR_WRITE_ACK,
    NEWBEST_CMD_ADDR_PUSHOUT,
};

//-1 -- error, >= 0: handled length in message 
int CEHSCmdParser::parseCmd(const char* szIn, 
                            int iInLen, 
                            char* szOut, 
                            int* p_iRetlen)
{
    int iRet = 0;
    CMD_BODY cmdbody = {0};
    PtllDataType value = {0};

    //check cmd and get device infomation
    if (getCmdBodyFromInput(szIn, iInLen, &cmdbody)) {
        packPutOut_errorCmd(szOut, p_iRetlen);
        return -1;
    }

    //get json file cmd
    if (cmdbody.iCmdId == NEWBEST_CMD_GETJSONFILE) {
        int iFileSize = getJsonFileSize();
        if (iFileSize > LARGE_BUFFER_SIZE)
            szOut = (char*)realloc(szOut, iFileSize);

        readJsonFile(szOut, p_iRetlen);
        return 0;
    }

    char szLogout[1024] = {0};
    sprintf(szLogout, "parse cmd: addrId = %d, cmdId = %d", 
        cmdbody.iAddrId, cmdbody.iCmdId);
    logout(LOG_DEBUG, szLogout, __FILE__, __LINE__);

    //read or write cmd
    //communicate with device and get result
    TB001_DEVOBJ devobj = {0};    
    if (rdb_find(TBL_DEV, &devobj, cmdbody.iAddrId)) {
        packPutOut_errorCmd(szOut, p_iRetlen);
        return -1;
    }

    sprintf(szLogout, "after find: groupAddr = %s", 
        devobj.szGroupAddr);
    logout(LOG_DEBUG, szLogout, __FILE__, __LINE__);

    //carry cmd and pack the output
    switch (cmdbody.iCmdId) {
        case NEWBEST_CMD_ADDR_READ:
            iRet = readDev(devobj.iProtocolID, 
                devobj.szGroupAddr, 
                &value, 
                devobj.iDataType);
            if (iRet == -1)
                packPutOut_errorDevAck(NEWBEST_CMD_ADDR_READ,
                      cmdbody.iAddrId, szOut, p_iRetlen);
            else
                packPutOut_readOk(cmdbody.iAddrId, &value, 
                                  devobj.iDataType, 
                                  szOut, p_iRetlen);
            break;

        case NEWBEST_CMD_ADDR_WRITE:
            //get data from input
            iRet = getDataFromInput(szIn + cmdbody.iDataStartPos,
                cmdbody.iDataLen, 0, &value);
            if (iRet == -1) {
                packPutOut_errorCmd(szOut, p_iRetlen);
                return -1;
            }

            iRet = writeDev(devobj.iProtocolID, 
                devobj.szGroupAddr, 
                &value, 
                devobj.iDataType);
            if (iRet == -1) {
                logout(LOG_ALARM, "write error", __FILE__, __LINE__);
                packPutOut_errorDevAck(NEWBEST_CMD_ADDR_WRITE,
                      cmdbody.iAddrId, szOut, p_iRetlen);
            }
            else
                packPutOut_writeOk(cmdbody.iAddrId, szOut, p_iRetlen);

            break;

        default:
            packPutOut_errorCmd(szOut, p_iRetlen);
            break;
    }

    return 0;
}

/*-------------------------------------------------------------------
 Function: getFirstCmd(szIn, iInLen, p_iStartPos, p_iCmdLen)
 Purpose: split to get first cmd from message read
 Parameters: szIn -- [IN], command form client
             iInLen -- [IN], command length
             p_iStartPos -- [OUT], first command start pos in message
             p_iCmdLen -- [OUT], first command length in message
 return: 0 --  get correct command
         -1 -- not correct
-------------------------------------------------------------------*/
int CEHSCmdParser::getFirstCmd(const char* szIn, int iInLen, 
                               int* p_iStartPos, int* p_iCmdLen)
{
    int i = 0;
    for (i = 0; i < iInLen - 1; i++) {
        if ((unsigned char)szIn[i] == 0xF0 &&
            (unsigned char)szIn[i + 1] == 0x0F)
            break;
    }

    if (i + MIN_CLIENT_CMD_LEN > iInLen)
        return -1;

    //get start position
    *p_iStartPos = i;

    //get data length
    int iDataLen = 0;
    iDataLen += (unsigned char)szIn[i + 5];
    iDataLen += (unsigned char)szIn[i + 6] << 8;
    iDataLen += (unsigned char)szIn[i + 7] << (2 * 8);
    iDataLen += (unsigned char)szIn[i + 8] << (3 * 8);

    if (i + MIN_CLIENT_CMD_LEN + iDataLen > iInLen)
        return -1;

    *p_iCmdLen = MIN_CLIENT_CMD_LEN + iDataLen;

    return 0;
}

/*-------------------------------------------------------------------
 Function: getCmdBodyFromInput(szIn, iInLen, p_result)
 Purpose: get the command infomation from input
 Parameters: szIn -- [IN], command form client
             iInLen -- [IN], command length
             p_result -- [OUT], command information gotten
 return: 0 --  correct command
         -1 -- not correct
-------------------------------------------------------------------*/
int CEHSCmdParser::getCmdBodyFromInput(const char* szIn, 
                                       int iInLen, 
                                       PCMD_BODY p_result)
{
    if (checkInput(szIn, iInLen))
        return -1;

    p_result->iCmdId = (unsigned char)szIn[2];
    p_result->iAddrId = (unsigned char)szIn[3] + 
        ((unsigned char)szIn[4] << 8);

    p_result->iDataLen = (unsigned char)szIn[5];
    p_result->iDataLen += (unsigned char)szIn[6] << 8;
    p_result->iDataLen += (unsigned char)szIn[7] << (2 * 8);
    p_result->iDataLen += (unsigned char)szIn[8] << (3 * 8);

    p_result->iDataStartPos = p_result->iDataLen ? 9 : -1;

    return 0;
}

/*-------------------------------------------------------------------
 Function: checkInput(szIn, iInLen)
 Purpose: check whether the command from client is correct or not
 Parameters: szIn -- [IN], command form client
             iInLen -- [IN], command length
 return: 0 --  correct command
         -1 -- not correct
-------------------------------------------------------------------*/
int CEHSCmdParser::checkInput(const char* szIn, int iInLen)
{
    //check length
    if (iInLen < MIN_CLIENT_CMD_LEN)
        return -1;

    if ((unsigned char)szIn[0] != 0xF0 ||
        (unsigned char)szIn[1] != 0x0F)
        return -1;
    
    unsigned char uchCmdId = (unsigned char)szIn[2];
    if (checkCmdID(uchCmdId))
        return -1;

    //get data length
    int iDataLen = 0;
    iDataLen += (unsigned char)szIn[5];
    iDataLen += (unsigned char)szIn[6] << 8;
    iDataLen += (unsigned char)szIn[7] << (2 * 8);
    iDataLen += (unsigned char)szIn[8] << (3 * 8);

    if (iDataLen + MIN_CLIENT_CMD_LEN != iInLen)
        return -1;

    //check sum;
    unsigned short usCheckSum = getByteSum(szIn, iInLen - 2);
    if ( ((unsigned char)szIn[iInLen - 2] != (unsigned char)(usCheckSum & 0xff)) ||
         ((unsigned char)szIn[iInLen - 1] != (unsigned char)(usCheckSum >> 8)) )
         return -1;

    return 0;
}

/*-------------------------------------------------------------------
 Function: checkCmdID(ucCmdId)
 Purpose: check whether the cmdID is correct or not
 Parameters: ucCmdId -- [IN], command id
 return: 0 --  correct command
         -1 -- not correct
-------------------------------------------------------------------*/
int CEHSCmdParser::checkCmdID(unsigned char ucCmdId)
{
    for (int i = 0; i < sizeof(s_gCmdId) / sizeof(unsigned char); i++) {
        if (ucCmdId == s_gCmdId[i]) 
            return 0;
    }

    return -1;
}

/*-------------------------------------------------------------------
 Function: getByteSum(szIn, iInLen)
 Purpose: get sum of each byte in input
 Parameters: szIn -- [IN], input bytes
             iInLen -- [IN], byte count
 return: sum of byte
-------------------------------------------------------------------*/
unsigned short CEHSCmdParser::getByteSum(const char* szIn, int iInLen)
{
    unsigned short res = 0;
    for (int i = 0; i < iInLen; i++) 
        res += (unsigned char)szIn[i];

    return res;
}

/*-------------------------------------------------------------------
 Function: getDataFromInput(p_start, iDataLen, iDataType, p_result)
 Purpose: get data value from input
 Parameters: p_start -- [IN], data position
             iDataLen -- [IN], data length
             iDataType -- [IN], target data type
             p_result -- [OUT], result
 return: 0 -- success
         -1 -- failed
-------------------------------------------------------------------*/
int CEHSCmdParser::getDataFromInput(const char* p_start, 
                                    int iDataLen, 
                                    int iDataType, 
                                    void* p_result)
{
    if (iDataLen <= 0 || iDataLen > 4)
        return -1;

    int* p_num = (int*)p_result;
    *p_num = 0;

    for (int i = 0; i < iDataLen; i++)
        *p_num += (unsigned char)p_start[i] << (8 * i);

    return 0;
}

/*-------------------------------------------------------------------
 Function: getJsonFileSize()
 Purpose: get file size of json file
 Parameters: 
 return: -1 -- file not exist
         >=0 -- file size
-------------------------------------------------------------------*/
int CEHSCmdParser::getJsonFileSize(void)
{
    char szCfgFilePath[256] = {0};

    if (getJsonFilePath(szCfgFilePath))
        return -1;

    return getFileSize(szCfgFilePath);
}

/*-------------------------------------------------------------------
 Function: readJsonFile(szOut, p_iRetlen)
 Purpose: read json file to buffer
 Parameters: szOut -- [OUT], to save file content
             p_iRetlen -- [OUT], length read
 return: -1 -- failed
          0 -- success
-------------------------------------------------------------------*/
int CEHSCmdParser::readJsonFile(char* szOut, int* p_iRetlen)
{
    FILE *pFile = NULL;
    char szCfgFilePath[256] = {0};

    if (getJsonFilePath(szCfgFilePath))
        return -1;

    pFile = fopen(szCfgFilePath, "rb");
    if (pFile) {
        fseek(pFile, 0, SEEK_END);
        *p_iRetlen = ftell(pFile);

        fseek(pFile, 0, SEEK_SET);
        fread(szOut, 1, *p_iRetlen, pFile);

        fclose(pFile); 
    }
    else 
        packPutOut_errorCmd(szOut, p_iRetlen);

    return 0;
}

/*-------------------------------------------------------------------
 Function: getJsonFilePath(p_filePath)
 Purpose: get file path of json file
 Parameters: p_filePath -- [OUT], to save file path
 return: -1 -- failed to get
          0 -- success
-------------------------------------------------------------------*/
int CEHSCmdParser::getJsonFilePath(char* p_filePath)
{
    char szFilePath[256] = {0};
    char szFileName[256] = {0};
    char szCfgFilePath[256] = {0};

    if (getExePath(szFilePath, szFileName, 256))
        return -1;

    sprintf(p_filePath, "%s/%s", szFilePath, FILENAME_JSON);

    return 0;
}

/*-------------------------------------------------------------------
 Function: packPutOut_errorCmd(szOut, p_iRetlen)
 Purpose: pack the output for error command
 Parameters: szOut -- [OUT], packet output
             p_iRetlen -- [OUT], packet length
 return: 
-------------------------------------------------------------------*/
void CEHSCmdParser::packPutOut_errorCmd(char* szOut, int* p_iRetlen)
{
    packPutOut(CLIENT_CMD_ERROR, 0, 0, 0, 0, NULL, szOut, p_iRetlen);
}

/*-------------------------------------------------------------------
 Function: packPutOut_errorDevAck(iCmdId, iAddrId, 
                      szOut, p_iRetlen)
 Purpose: pack the output for error device return
 Parameters: iCmdId -- [IN], cmd id
             iAddrId -- [IN], address id
             szOut -- [OUT], packet output
             p_iRetlen -- [OUT], packet length
 return: 
-------------------------------------------------------------------*/
void CEHSCmdParser::packPutOut_errorDevAck(int iCmdId, 
                                           int iAddrId, 
                                           char* szOut, 
                                           int* p_iRetlen)
{
    packPutOut(CLIENT_CMD_RIGHT, DEVICE_COMM_ERROR,
         iCmdId, iAddrId, 0, NULL, szOut, p_iRetlen);
}

/*-------------------------------------------------------------------
 Function: packPutOut_writeOk(iAddrId, szOut, p_iRetlen)
 Purpose: pack the output for write device with right return
 Parameters: iAddrId -- [IN], address id
             szOut -- [OUT], packet output
             p_iRetlen -- [OUT], packet length
 return: 
-------------------------------------------------------------------*/
void CEHSCmdParser::packPutOut_writeOk(int iAddrId, 
                                       char* szOut, 
                                       int* p_iRetlen)
{
    packPutOut(CLIENT_CMD_RIGHT, DEVICE_COMM_RIGHT,
        NEWBEST_CMD_ADDR_WRITE_ACK, iAddrId, 
        0, NULL, 
        szOut, p_iRetlen);
}

/*-------------------------------------------------------------------
 Function: packPutOut_readOk(iAddrId, p_data, iValType, szOut, p_iRetlen)
 Purpose: pack the output for getting right read data
 Parameters: iAddrId -- [IN], address id
             p_data -- [IN], data from device
             iValType -- [IN], date type
             szOut -- [OUT], packet output
             p_iRetlen -- [OUT], packet length
 return: 
-------------------------------------------------------------------*/
void CEHSCmdParser::packPutOut_readOk(int iAddrId, 
                                      void* p_data, 
                                      int iValType, 
                                      char* szOut, 
                                      int* p_iRetlen)
{
    int iDataLen = getSizeofDataType(iValType);

    if (iDataLen <= 0)
        return packPutOut(CLIENT_CMD_RIGHT, DEVICE_COMM_RIGHT,
                          NEWBEST_CMD_ADDR_READ_ACK, iAddrId, 
                          0, NULL, 
                          szOut, p_iRetlen);

    PtllDataType* p_temp = (PtllDataType*)p_data;
    char szData[8] = {0};

    switch (iValType) {
        case DEV_TYPE_BOOL:
        case DEV_TYPE_BYTE:
        case DEV_TYPE_SHORT:
        case DEV_TYPE_INT:
            EasyMath::intToBytes(p_temp->iVal, szData);
            break;

        case DEV_TYPE_LONG:
            EasyMath::longToBytes(p_temp->lVal, szData);
            break;

        default:
            memcpy(szData, p_temp, iDataLen);
            break;
    }

    return packPutOut(CLIENT_CMD_RIGHT, DEVICE_COMM_RIGHT,
                      NEWBEST_CMD_ADDR_READ_ACK, iAddrId,
                      iDataLen, szData, 
                      szOut, p_iRetlen);
}

/*-------------------------------------------------------------------
 Function: packPushOut(iAddrId, p_data, iValType, szOut, p_iRetlen)
 Purpose: pack the output to push out to client
 Parameters: iAddrId -- [IN], address id
             p_data -- [IN], data to pack
             iValType -- [IN], date type
             szOut -- [OUT], packet output
             p_iRetlen -- [OUT], packet length
 return: 0 -- success
         -1 -- failed
-------------------------------------------------------------------*/
int CEHSCmdParser::packPushOut(int iAddrId, void* p_data, int iValType, 
        char* szOut, int* p_iRetLen)
{
    int iDataLen = getSizeofDataType(iValType);

    if (iDataLen <= 0)
        return -1;

    PtllDataType* p_temp = (PtllDataType*)p_data;
    char szData[8] = {0};

    switch (iValType) {
        case DEV_TYPE_BOOL:
        case DEV_TYPE_BYTE:
        case DEV_TYPE_SHORT:
        case DEV_TYPE_INT:
            EasyMath::intToBytes(p_temp->iVal, szData);
            break;

        case DEV_TYPE_LONG:
            EasyMath::longToBytes(p_temp->lVal, szData);
            break;

        default:
            memcpy(szData, p_temp, iDataLen);
            break;
    }

    packPutOut(CLIENT_CMD_RIGHT, DEVICE_COMM_RIGHT,
        NEWBEST_CMD_ADDR_PUSHOUT, iAddrId,
        iDataLen, szData, 
        szOut, p_iRetLen);

    return 0;
}

/*-------------------------------------------------------------------
 Function: packPutOut(iParseRight, iDevCommRight, iCmdId, iAddrId, 
                      iDataLen, data, szOut, p_iRetlen)
 Purpose: pack the output according to parsing result
 Parameters: iParseRight -- [IN], the cmd is parsed not correct
             iDevCommRight -- [IN], communicate with device normally
             iCmdId -- [IN], cmd id
             iAddrId -- [IN], address id
             iDataLen -- [IN], data length
             data -- [IN], data
             szOut -- [OUT], packet output
             p_iRetlen -- [OUT], packet length
 return: 
-------------------------------------------------------------------*/
void CEHSCmdParser::packPutOut(int iParseRight, 
                               int iDevCommRight,
                               int iCmdId, 
                               int iAddrId, 
                               int iDataLen,
                               void* data, 
                               char* szOut, 
                               int* p_iRetlen)
{
    unsigned short usCheckSum = 0;    

    szOut[0] = (unsigned char)0x0F;                          //head
    szOut[1] = (unsigned char)0xF0;                          //head
    szOut[2] = (unsigned char)0x00;                          //cmd id
    szOut[3] = (unsigned char)0x00;                          //cmd check correctly
    szOut[4] = (unsigned char)0x00;                          //cmd ack correctly
    szOut[5] = (unsigned char)0x00;                          //addr
    szOut[6] = (unsigned char)0x00;                          //addr
    szOut[7] = (unsigned char)0x00;                          //data len
    szOut[8] = (unsigned char)0x00;                          //data len
    szOut[9] = (unsigned char)0x00;                          //data len
    szOut[10] = (unsigned char)0x00;                         //data len

    //1. error client cmd    
    if (iParseRight != CLIENT_CMD_RIGHT) { //bad imput client cmd
        usCheckSum = getByteSum(szOut, 11);
        szOut[11] = usCheckSum & 0xFF;
        szOut[12] = usCheckSum >> 8; 

        *p_iRetlen = 13;

        return ;
    }

    //2. correct client cmd    
    szOut[2] = iCmdId;
    szOut[3] = 0x01;

    szOut[5] = iAddrId & 0xFF;
    szOut[6] = iAddrId >> 8;  

    //2.1 correct cmd, not get return from device
    if (iDevCommRight != DEVICE_COMM_RIGHT) {
        usCheckSum = getByteSum(szOut, 11);
        szOut[11] = usCheckSum & 0xFF;
        szOut[12] = usCheckSum >> 8; 

        *p_iRetlen = 13;
        return;
    }

    //2.2 correct cmd, and get return from device
    //iDevCommRight == DEVICE_COMM_RIGHT
    szOut[4] = 0x01; 

    szOut[7] = iDataLen & 0xFF;
    szOut[8] = (iDataLen >> 8) & 0xFF;
    szOut[9] = (iDataLen >> 8 * 2) & 0xFF;
    szOut[10] = (iDataLen >> 8 * 3) & 0xFF;

    if (iDataLen > 0)
        memcpy(&szOut[11], data, iDataLen);

    usCheckSum = getByteSum(szOut, 11 + iDataLen);
    szOut[11 + iDataLen] = usCheckSum & 0xFF;
    szOut[12 + iDataLen] = usCheckSum >> 8; 

    *p_iRetlen = 13 + iDataLen;
}
