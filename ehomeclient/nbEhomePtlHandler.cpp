#include "nbEhomePtlHandler.h"
#include "platcomm.h"

int CNbEhomePtlHandler::getCmdBodyFromInput(const char* szIn, int iInLen, PCMD_BODY p_result)
{
    if (checkInput(szIn, iInLen))
        return -1;

    p_result->iCmdId = (unsigned char)szIn[2];
    p_result->iAddrId = (unsigned char)szIn[5] + 
        ((unsigned char)szIn[6] << 8);

    p_result->iDataLen = (unsigned char)szIn[7];
    p_result->iDataLen += (unsigned char)szIn[8] << 8;
    p_result->iDataLen += (unsigned char)szIn[9] << (2 * 8);
    p_result->iDataLen += (unsigned char)szIn[10] << (3 * 8);

    p_result->iDataStartPos = p_result->iDataLen ? 11 : -1;

    return 0;
}

/*-------------------------------------------------------------------
 Function: checkInput(szIn, iInLen)
 Purpose: check whether the response from server is correct or not
 Parameters: szIn -- [IN], command form client
             iInLen -- [IN], command length
 return: 0 --  correct command
         -1 -- not correct
-------------------------------------------------------------------*/
int CNbEhomePtlHandler::checkInput(const char* szIn, int iInLen)
{
    //check length
    if (iInLen < MIN_SERVER_RES_LEN)
        return -1;

    if ((unsigned char)szIn[0] != 0x0F ||
        (unsigned char)szIn[1] != 0xF0)
        return -1;
    
    unsigned char uchCmdId = (unsigned char)szIn[2];
    if (uchCmdId != NEWBEST_CMD_GETJSONFILE_ACK &&
        uchCmdId != NEWBEST_CMD_ADDR_READ_ACK &&
        uchCmdId != NEWBEST_CMD_ADDR_WRITE_ACK &&
        uchCmdId != NEWBEST_CMD_ADDR_PUSHOUT)        
        return -1;

    //check response bits
    if (szIn[3] == 0 || szIn[4] == 0)
        return -1;
    
    //get data length
    int iDataLen = 0;
    iDataLen += (unsigned char)szIn[7];
    iDataLen += (unsigned char)szIn[8] << 8;
    iDataLen += (unsigned char)szIn[9] << (2 * 8);
    iDataLen += (unsigned char)szIn[10] << (3 * 8);

    if (iDataLen + MIN_SERVER_RES_LEN > iInLen)
        return -1;

    //check sum;
    unsigned short usCheckSum = calcByteSum(szIn, iDataLen + 11);
    if ( ((unsigned char)szIn[iDataLen + 11] != (unsigned char)(usCheckSum & 0xff)) ||
         ((unsigned char)szIn[iDataLen + 12] != (unsigned char)(usCheckSum >> 8)) )
         return -1;

    return 0;
}

