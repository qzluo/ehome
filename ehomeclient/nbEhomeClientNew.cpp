/********************************************************************
 File: nbEhomeClient.cpp
 Function: provided interaface to communicate with ehome server. 
 Author: Luobihe
 Date: 2015-8-13
*********************************************************************/

#include <stdio.h>
#include "nbEhomeClientNew.h"
#include "logsys.h"


CNbEhomeTcpClientNew::CNbEhomeTcpClientNew(void)
{
    m_bIsClosed = 1;
    m_sfd = -1;
}

void CNbEhomeTcpClientNew::setWinHandle(HWND hWnd)
{
    m_hWnd = hWnd;
}

//初始化：创建socket, 连接设备
int CNbEhomeTcpClientNew::init(const char* szDevAddr, int iPort)
{
    m_sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sfd < 0) 
        return -1;    

    struct sockaddr_in srvaddr = {0};
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_addr.s_addr = inet_addr(szDevAddr);
    srvaddr.sin_port = htons(iPort);

    if (connect(m_sfd, (struct sockaddr*)&srvaddr, sizeof(srvaddr)))
        return -1;

    m_bIsClosed = 0;

    return 0;
}

int CNbEhomeTcpClientNew::disConnect(void)
{
    if (m_sfd != -1) {
        closesocket(m_sfd);
        m_sfd = -1;
    }

    m_bIsClosed = 1;

    return 0;
}

int CNbEhomeTcpClientNew::isClosed(void)
{
    return m_bIsClosed;
}

/*
position	contend             sense
1	         0XF0               protocol header
2	         0X0F               protocol header
3  	         0X02               command id
4-5          0X01,00            address id
6-9          0X00,00,00,00      data length
10	         0X--               data, may be empty
…	         0X--               data, may be empty
last-2       0X--               data, may be empty
last-1       0X--               check sum
last	     0X--               check sum
*/

//send cmd to ehome server
int CNbEhomeTcpClientNew::writeDev(int iDevNo, int value)
{
    char szWBuf[12] = {0};
    //head
    szWBuf[0] = 0xF0;
    szWBuf[1] = 0x0F;

    //cmd
    szWBuf[2] = NEWBEST_CMD_ADDR_WRITE;

    //address
    szWBuf[3] = iDevNo & 0xFF;
    szWBuf[4] = iDevNo >> 8;

    //data len
    szWBuf[5] = 0x01;
    szWBuf[6] = 0x00;
    szWBuf[7] = 0x00;
    szWBuf[8] = 0x00;

    //data
    szWBuf[9] = value;

    unsigned short checksum = calcByteSum(szWBuf, 10);
    szWBuf[10] = checksum & 0xFF;
    szWBuf[11] = checksum >> 8;

    send(m_sfd, szWBuf, 12, 0);

    char szRBuf[1024] = {0};

    recv(m_sfd, szRBuf, 1024, 0);

    return 0;
}

int CNbEhomeTcpClientNew::getJsonFile(char* p_rBuf, int iBufSize)
{
    char szWBuf[11] = {0};
    //head
    szWBuf[0] = 0xF0;
    szWBuf[1] = 0x0F;

    //cmd
    szWBuf[2] = NEWBEST_CMD_GETJSONFILE;

    //address
    szWBuf[3] = 0;
    szWBuf[4] = 0;

    //data len
    szWBuf[5] = 0x00;
    szWBuf[6] = 0x00;
    szWBuf[7] = 0x00;
    szWBuf[8] = 0x00;

    //data
    szWBuf[9] = 0x00;
    szWBuf[10] = 0x01;

    send(m_sfd, szWBuf, 11, 0);

    recv(m_sfd, p_rBuf, iBufSize, 0);

    return 0;
}

int CNbEhomeTcpClientNew::readDev(int iDevNo)
{
    sendReadDevCmd(iDevNo);

    char szRBuf[1024] = {0};
    CMD_BODY cmdbody = {0};

    int iLen = recv(m_sfd, szRBuf, 1024, 0);
    //check cmd and get device infomation
    if (getCmdBodyFromInput(szRBuf, iLen, &cmdbody)) 
        return -1;

    int iVal = 0;
    int iDataStartPos = cmdbody.iDataStartPos;
    for (int i = 0; i < cmdbody.iDataLen; i++)
        iVal += (unsigned char)szRBuf[i + iDataStartPos] << (8 * i);

    switch (cmdbody.iCmdId) {
        case NEWBEST_CMD_ADDR_READ_ACK:
            SendMessage(m_hWnd, WM_CLIETN_READBACK, cmdbody.iAddrId, iVal);
            break;

        case NEWBEST_CMD_ADDR_WRITE_ACK:
        case NEWBEST_CMD_GETJSONFILE_ACK:
            break;

        default:
            break;
    }

    return 0;
}

//send cmd to read device
int CNbEhomeTcpClientNew::sendReadDevCmd(int iDevNo)
{
    char szWBuf[11] = {0};
    //head
    szWBuf[0] = 0xF0;
    szWBuf[1] = 0x0F;

    //cmd
    szWBuf[2] = NEWBEST_CMD_ADDR_READ;

    //address
    szWBuf[3] = iDevNo & 0xFF;
    szWBuf[4] = iDevNo >> 8;

    //data len
    szWBuf[5] = 0x00;
    szWBuf[6] = 0x00;
    szWBuf[7] = 0x00;
    szWBuf[8] = 0x00;

    unsigned short checksum = calcByteSum(szWBuf, 9);
    szWBuf[9] = checksum & 0xFF;
    szWBuf[10] = checksum >> 8;

    send(m_sfd, szWBuf, 11, 0);

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
int CNbEhomeTcpClientNew::getCmdBodyFromInput(const char* szIn, 
                                           int iInLen, 
                                           PCMD_BODY p_result)
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
int CNbEhomeTcpClientNew::checkInput(const char* szIn, int iInLen)
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
        uchCmdId != NEWBEST_CMD_ADDR_WRITE_ACK)
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

    if (iDataLen + MIN_SERVER_RES_LEN != iInLen)
        return -1;

    //check sum;
    unsigned short usCheckSum = calcByteSum(szIn, iInLen - 2);
    if ( ((unsigned char)szIn[iInLen - 2] != (unsigned char)(usCheckSum & 0xff)) ||
         ((unsigned char)szIn[iInLen - 1] != (unsigned char)(usCheckSum >> 8)) )
         return -1;

    return 0;
}

