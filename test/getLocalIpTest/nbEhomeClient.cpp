/********************************************************************
 File: nbEhomeClient.cpp
 Function: provided interaface to communicate with ehome server. 
 Author: Luobihe
 Date: 2015-8-13
*********************************************************************/

#include <stdio.h>
#include "nbEhomeClient.h"
//#include "logsys.h"


CNbEhomeTcpClient::CNbEhomeTcpClient(void)
{
    m_bIsClosed = 1;
}

/*
void CNbEhomeTcpClient::setWinHandle(HWND hWnd)
{
    m_hWnd = hWnd;
}
*/

//初始化：创建socket, 连接设备
int CNbEhomeTcpClient::init(const char* szDevAddr, int iPort)
{
    if (m_tcpClt.open())
        return -1;

    TCPCLTAPPPARAS paras = {0};
    strcpy(paras.szAddr, szDevAddr);
    paras.iPort = iPort; 
    paras.p_app = this;

    return m_tcpClt.init(&paras);    
}

int CNbEhomeTcpClient::disConnect(void)
{
    m_tcpClt.clear();

    return 0;
}

int CNbEhomeTcpClient::isClosed(void)
{
    return m_tcpClt.isClosed();
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
int CNbEhomeTcpClient::writeDev(int iDevNo, int value)
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

    m_tcpClt.write(szWBuf, 12);
    
    return 0;
}

int CNbEhomeTcpClient::readDev(int iDevNo)
{
    return sendReadDevCmd(iDevNo);
}

//send cmd to read device
int CNbEhomeTcpClient::sendReadDevCmd(int iDevNo)
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

    m_tcpClt.write(szWBuf, 11);

    return 0;
}

//读处理回调函数，当sock监测到有可读的数据时，将回调该函数
int CNbEhomeTcpClient::readHandle(void)
{
    char szRBuf[1024] = {0};
    int iLen = 0;
    CMD_BODY cmdbody = {0};

    iLen = m_tcpClt.read((char*)szRBuf, 1024);

#ifdef _WIN32

    if (iLen == 0 || (iLen == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET)) { // Client socket closed   
#else

    if (iLen <= 0) {
        if ((iLen < 0) && (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))
            continue;
#endif                         

        return -1;
    }

    char szLogout[1024] = {0};
    for (int i = 0; i < iLen; i++)
        sprintf(szLogout, "%s 0x%X", szLogout, szRBuf[i]);

    printf("%s\n", szLogout);
    
    //check cmd and get device infomation
    if (getCmdBodyFromInput(szRBuf, iLen, &cmdbody)) 
        return -1;

    int iVal = 0;
    int iDataStartPos = cmdbody.iDataStartPos;
    for (int i = 0; i < cmdbody.iDataLen; i++)
        iVal += (unsigned char)szRBuf[i + iDataStartPos] << (8 * i);

    switch (cmdbody.iCmdId) {
        case NEWBEST_CMD_ADDR_READ_ACK:
            printf("addr = %d, val = %d\n", cmdbody.iAddrId, iVal);
            break;

        case NEWBEST_CMD_ADDR_WRITE_ACK:
        case NEWBEST_CMD_GETJSONFILE_ACK:
            break;

        default:
            break;
    }

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
int CNbEhomeTcpClient::getCmdBodyFromInput(const char* szIn, 
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
int CNbEhomeTcpClient::checkInput(const char* szIn, int iInLen)
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

