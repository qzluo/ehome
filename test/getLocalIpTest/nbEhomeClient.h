/********************************************************************
 File: nbEhomeClient.h
 Function: provided interaface to communicate with ehome server. 
 Author: Luobihe
 Date: 2015-8-13
*********************************************************************/

/*
2015-8-13
///////////////////////////////////////////////////////////
ehome servert protocol 
///////////////////////////////////////////////////////////

commmand packet from client

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

///////////////////////////////////////////////////////////

commmand packet back to client

position	contend             sense
1	         0X0F               protocol header
2	         0XF0               protocol header
3  	         0X82               command id
4            0X01               command check result
5            0X01               result is gotten or not
6-7          0X01,00            address id
8-11         0X00,00,00,00      data length
12	         0X--               data, may be empty
…	         0X--               data, may be empty
last-2       0X--               data, may be empty
last-1       0X--               check sum
last	     0X--               check sum

*/

#ifndef __NBEHOMECLIENT_H__
#define __NBEHOMECLIENT_H__

#include "platcomm.h"

//command id
#define NEWBEST_CMD_GETJSONFILE      0X01
#define NEWBEST_CMD_ADDR_READ        0X02
#define NEWBEST_CMD_ADDR_WRITE       0X03
#define NEWBEST_CMD_GETJSONFILE_ACK  0X81
#define NEWBEST_CMD_ADDR_READ_ACK    0X82
#define NEWBEST_CMD_ADDR_WRITE_ACK   0X83

#define MIN_SERVER_RES_LEN   13  

#define WM_CLIETN_READBACK  WM_USER + 100

//effective command data
typedef struct {
    int iCmdId;             //command id, read or write
    int iAddrId;            //address by a number
    int iDataLen;           //data len
    int iDataStartPos;      //data start position, -1 represents uneffective
} CMD_BODY, *PCMD_BODY;

class CNbEhomeTcpClient : public CTcpCltAppInterface
{
private:
    CEasyTcpClt m_tcpClt;
    
    //HWND m_hWnd;

    int m_sfd;

    int m_bIsClosed;

private:
    //get the command infomation from input
    //0 -- correct, -1 -- not correct
    int getCmdBodyFromInput(const char* szIn, int iInLen, PCMD_BODY p_result);

    int checkInput(const char* szBuf, int iLen);

public:
    CNbEhomeTcpClient(void);

    //void setWinHandle(HWND hWnd);

    //初始化：创建socket, 连接设备
    int init(const char* szDevAddr, int iPort);

    int disConnect(void);

    int isClosed(void);

    //send cmd to ehome server
    int writeDev(int iDevNo, int value);

    int readDev(int iDevNo);

    //send cmd to read device
    int sendReadDevCmd(int iDevNo);

    //读处理回调函数，当sock监测到有可读的数据时，将回调该函数
    int readHandle(void);

};

#endif  //__NBEHOMECLIENT_H__