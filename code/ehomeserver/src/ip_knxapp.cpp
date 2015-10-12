/********************************************************************
 File: ip_knxapp.cpp
 Function: communicate with ip_knx device
 Author: Luobihe
 Date: 2015-7-10
********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ip_knxapp.h"
#include "knxcomm.h"

#include "rdbManager.h"
#include "logsys.h"


CIpKnxApp::CIpKnxApp(void)
{
    m_hIpKnxLock.init();
    m_hIpKnxCltCmdLock.init();
    m_hIpKnxMonCmdLock.init();
    m_iBlockMode = CMD_RESPONSE_MODE_NONBLOCK;
}

CIpKnxApp::~CIpKnxApp(void)
{
    m_hIpKnxLock.destroy();
    m_hIpKnxCltCmdLock.destroy();
    m_hIpKnxMonCmdLock.destroy();
}

/*-------------------------------------------------------------------
 Function: init(szDevAddr, iPort)
 Purpose: initial, create socket, connect device
 Parameters: szDevAddr -- [IN], ip
             iPort -- [IN], port
             iBlockMode -- [IN], block mode
                  CMD_RESPONSE_MODE_NONBLOCK
                  CMD_RESPONSE_MODE_BLOCK

 return: 0 --  success
         -1 -- failed
-------------------------------------------------------------------*/
int CIpKnxApp::init(const char* szDevAddr, int iPort, int iBlockMode)
{
    if (m_tcpClt.open())
        return -1;

    TCPCLTAPPPARAS paras = {0};
    strcpy(paras.szAddr, szDevAddr);
    paras.iPort = iPort; 
    paras.p_app = this;

    m_iBlockMode = iBlockMode;

    m_iCmdState = CMD_STATE_SENDTO_DEV;

    if (m_tcpClt.init(&paras))
        return -1;

    if (easyCreateThread(writeIpKnxFun, this) == -1)
        return -1;

    return 0;
}

/*-------------------------------------------------------------------
 Function: writeIpKnxFun(arg)
 Purpose: thread to write cmd to ipknx device
 Parameters: arg -- [IN], args from caller
 return:
-------------------------------------------------------------------*/
void* CIpKnxApp::writeIpKnxFun(void* arg)
{
    CIpKnxApp* p_temp = (CIpKnxApp*)arg;
    KNXCMD cmd = {0};

    while (1) {
        if (p_temp->getListCltCmd(&cmd) == 0) {
            p_temp->sendCmd(cmd);
            //platMSleep(100);
        }
        else if (p_temp->getListMonCmd(&cmd) == 0)
            p_temp->sendCmd(cmd);
        else
            platSleep(1);
    }

    return 0;
}

/*-------------------------------------------------------------------
 Function: getListCltCmd(p_cmd)
 Purpose: get first knx cmd from list
 Parameters: p_cmd -- [OUT], to preserve the cmd from list
 return: 0 --  success
         -1 -- failed
-------------------------------------------------------------------*/
int CIpKnxApp::getListCltCmd(KNXCMD* p_cmd)
{
    int ret = 0;

    m_hIpKnxCltCmdLock.lock();
    if (m_listClientCmd.size() > 0) {
        *p_cmd = m_listClientCmd.front();
        m_listClientCmd.pop_front();
    }
    else
        ret = -1;
    m_hIpKnxCltCmdLock.unlock();

    return ret;
}

/*-------------------------------------------------------------------
 Function: getListMonCmd(p_cmd)
 Purpose: get first knx cmd from list
 Parameters: p_cmd -- [OUT], to preserve the cmd from list
 return: 0 --  success
         -1 -- failed
-------------------------------------------------------------------*/
int CIpKnxApp::getListMonCmd(KNXCMD* p_cmd)
{
    int ret = 0;

    m_hIpKnxMonCmdLock.lock();
    if (m_listMonitorCmd.size() > 0) {
        *p_cmd = m_listMonitorCmd.front();
        m_listMonitorCmd.pop_front();
    }
    else
        ret = -1;
    m_hIpKnxMonCmdLock.unlock();

    return ret;
}

/*-------------------------------------------------------------------
 Function: addListCltCmd(cmd)
 Purpose: add cmd to list
 Parameters: cmd -- [IN], cmd to add
 return: 
-------------------------------------------------------------------*/
void CIpKnxApp::addListCltCmd(KNXCMD cmd)
{
    char szLogOut[1024] = {0};
    sprintf(szLogOut, "add list cmd, mode = %d, ch1 = %d, "
        "ch2 = %d, value = %d, len = %d", cmd.mode, cmd.chAddrFirst,
        cmd.chAddrSecond, cmd.value, cmd.len);
    //logout(LOG_ALARM, szLogOut, __FILE__, __LINE__);

    m_hIpKnxCltCmdLock.lock();
    m_listClientCmd.push_back(cmd);
    m_hIpKnxCltCmdLock.unlock();
}

/*-------------------------------------------------------------------
 Function: addListMonCmd(cmd)
 Purpose: add cmd to list
 Parameters: cmd -- [IN], cmd to add
 return: 
-------------------------------------------------------------------*/
void CIpKnxApp::addListMonCmd(KNXCMD cmd)
{
    char szLogOut[1024] = {0};
    sprintf(szLogOut, "add list cmd, mode = %d, ch1 = %d, "
        "ch2 = %d, value = %d, len = %d", cmd.mode, cmd.chAddrFirst,
        cmd.chAddrSecond, cmd.value, cmd.len);
    //logout(LOG_ALARM, szLogOut, __FILE__, __LINE__);

    KNXCMD tempCmd = {0};

    m_hIpKnxMonCmdLock.lock();

    list <KNXCMD>::iterator it = m_listMonitorCmd.begin();
    for (it = m_listMonitorCmd.begin(); it != m_listMonitorCmd.end(); it++) {
        tempCmd = *it;
        if (tempCmd.mode == cmd.mode &&
            tempCmd.chAddrFirst == cmd.chAddrFirst &&
            tempCmd.chAddrSecond == cmd.chAddrSecond)
            break;
    }

    if (it == m_listMonitorCmd.end())
        m_listMonitorCmd.push_back(cmd);

    m_hIpKnxMonCmdLock.unlock();
}

/*-------------------------------------------------------------------
 Function: writeGroupAddr(szAddr, value, len)
 Purpose: write group address, check the response
 Parameters: szAddr -- [IN], group address
             value -- [IN], value to write
             len -- [IN], data length to write
 return: 0 --  success
         -1 -- failed
-------------------------------------------------------------------*/
int CIpKnxApp::writeGroupAddr(char* szAddr, int value, int len)
{    
    int iRet = 0;
    int iCount = 0;
    int iState = 0;

    //2015-9-17 for write cmd
    KNXCMD cmd = {0};

    iRet = parseGroupAddr(szAddr, &cmd.chAddrFirst, &cmd.chAddrSecond);
    if (iRet == -1)
        return -1;

    cmd.mode = CMD_MODE_WRITE;
    cmd.value = value;
    cmd.len = len;

    addListCltCmd(cmd);
    return 0;

    setCmdState(CMD_STATE_SENDTO_DEV);

    //write cmd    
    iRet = sendWriteCmd(szAddr, value, len);
    if (iRet < 0)
        return -1;

    if (m_iBlockMode == CMD_RESPONSE_MODE_NONBLOCK)
        return 0;

    //check response
    while (iCount++ < 10) {
        iState = getCmdState();
        if (iState != CMD_STATE_SENDTO_DEV)
            break;

        platSleep(1);
    }

    return (iState == CMD_STATE_RES_CORRECT) ? 0 : -1;
}

//从 knx 设备地址读值
/*-------------------------------------------------------------------
 Function: readGroupAddr(szAddr, p_value, len)
 Purpose: read from knx device
 Parameters: szAddr -- [IN], group address
             p_value -- [IN], result
             len -- [IN], data length, 
                     1 - bool, 2 - byte, 3 - short, 5 - int
 return: 0 --  success
         -1 -- failed
-------------------------------------------------------------------*/
int CIpKnxApp::readGroupAddr(char* szAddr, int* p_value, int len)
{
    int iRet = 0;
    int iCount = 0;
    int iState = 0;

    if (m_iBlockMode == CMD_RESPONSE_MODE_NONBLOCK)
        return readGroupAddrNoBlock(szAddr, p_value, len);

    setCmdState(CMD_STATE_SENDTO_DEV);

    //write cmd    
    iRet = sendReadCmd(szAddr, len);
    if (iRet < 0)
        return -1;

    //check response
    while (iCount++ < 10) {
        iState = getCmdState();
        if (iState != CMD_STATE_SENDTO_DEV)
            break;

        platSleep(1);
    }

    //check the return
    TB001_DEVOBJ temp;
    if (rdb_selectDevNodeByGroupAddr(szAddr, &temp))
        return -1;
    else
        *p_value = atoi(temp.szValue);

    return (iState == CMD_STATE_RES_CORRECT) ? 0 : -1;
}

//从 knx 设备地址读值,不等待设备响应
/*-------------------------------------------------------------------
 Function: readGroupAddrNoBlock(szAddr, p_value, len)
 Purpose: read from knx device, not waiting the response
 Parameters: szAddr -- [IN], group address
             p_value -- [IN], result
             len -- [IN], data length, 
                     1 - bool, 2 - byte, 3 - short, 5 - int
 return: 0 --  success
         -1 -- failed
-------------------------------------------------------------------*/
int CIpKnxApp::readGroupAddrNoBlock(char* szAddr, int* p_value, int len)
{
    int iRet = 0;
    int iCount = 0;
    int iState = 0;

    //2015-9-17 for write cmd
    KNXCMD cmd = {0};

    iRet = parseGroupAddr(szAddr, &cmd.chAddrFirst, &cmd.chAddrSecond);
    if (iRet == -1)
        return -1;

    cmd.mode = CMD_MODE_READ;
    cmd.value = 0;
    cmd.len = len;

    addListCltCmd(cmd);

    /*
    //write cmd    
    iRet = sendReadCmd(szAddr, len);
    if (iRet < 0)
        return -1;
        */

    //check the return
    TB001_DEVOBJ temp;
    if (rdb_selectDevNodeByGroupAddr(szAddr, &temp))
        return -1;
    else
        *p_value = atoi(temp.szValue);

    return 0;
}

//monitor group addr by sending read addr cmd
/*-------------------------------------------------------------------
 Function: monitorGroupAddr(szAddr, len)
 Purpose: monitor group addr by sending read addr cmd
 Parameters: szAddr -- [IN], group address
             len -- [IN], data length, 
                     1 - bool, 2 - byte, 3 - short, 5 - int
 return: 0 --  success
         -1 -- failed
-------------------------------------------------------------------*/
int CIpKnxApp::monitorGroupAddr(char* szAddr, int len)
{
    int iRet = 0;

    //2015-9-17 for write cmd
    KNXCMD cmd = {0};

    iRet = parseGroupAddr(szAddr, &cmd.chAddrFirst, &cmd.chAddrSecond);
    if (iRet == -1)
        return -1;

    cmd.mode = CMD_MODE_READ;
    cmd.value = 0;
    cmd.len = len;

    addListMonCmd(cmd);

    return 0;
    
    //return sendReadCmd(szAddr, len);
}

/*-------------------------------------------------------------------
 Function: sendWriteCmd(szAddr, value, len)
 Purpose: send write group address to knx device
 Parameters: szAddr -- [IN], group address
             value -- [IN], value to write
             len -- [IN], length of data to send 
 return: >=0 -- send success, the sent data length is returned
         <0 -- failed
-------------------------------------------------------------------*/
int CIpKnxApp::sendWriteCmd(char* szAddr, int value, int len)
{
    return sendCmd(szAddr, value, len, CMD_MODE_WRITE);
}

/*-------------------------------------------------------------------
 Function: sendReadCmd(szAddr, value, len)
 Purpose: send read group address to knx device
 Parameters: szAddr -- [IN], group address
             len -- [IN], length of data to send 
 return: >=0 -- send success, the sent data length is returned
         <0 -- failed
-------------------------------------------------------------------*/
int CIpKnxApp::sendReadCmd(char* szAddr, int len)
{
    return sendCmd(szAddr, 0, len, CMD_MODE_READ);
} 

/*-------------------------------------------------------------------
 Function: sendCmd(szAddr, value, len, mode)
 Purpose: send command to knx bus
 Parameters: szAddr -- [IN], group address
             value -- [IN], value to write
             len -- [IN], length to write
             mode -- [IN], 0: write, 1：read
 return: 0 --  success
         -1 -- failed
-------------------------------------------------------------------*/
int CIpKnxApp::sendCmd(char* szAddr, int value, int len, int mode)
{
    unsigned char data[16] = {0x55,   //0.head
                              0xaa,   //1.head
                              0x00,   //2.reserve
                              0x00,   //3.reserve
                              0x00,   //4.reserve
                              0x00,   //5.reserve
                              0x05,   //6.cmd id
                              0x01,   //7.cmd id
                              0x03,   //8.priority
                              0x01,   //9.length
                              0x00,   //10.desc addr
                              0x00,   //11.desc addr
                              0x00,   //12.value
                              0x00,   //13.value
                              0x00,   //14.check sum
                              0x00};  //15.check sum
    int iRet = 0;
    char chAddrFirst = 0;
    char chAddrSecond = 0;
    unsigned short sum = 0;

    iRet = parseGroupAddr(szAddr, &chAddrFirst, &chAddrSecond);
    if (iRet == -1)
        return -1;

    data[6] = (mode == CMD_MODE_WRITE) ? 0x05 : 0x04;

    data[9] = len;
    data[10] = chAddrFirst;
    data[11] = chAddrSecond;
    data[12] = (char)value;
    data[13] = (char)(value>>8);  
        
    for (int i = 0; i < 14; i++) 
        sum += data[i];
    
    data[14] = (unsigned char)sum;
    data[15] = (unsigned char)(sum>>8); 

    m_hIpKnxLock.lock();
    iRet = m_tcpClt.write((char*)data, 16);
    platMSleep(IPKNX_CMD_DELAY);
    m_hIpKnxLock.unlock();

    return iRet;
}

/*-------------------------------------------------------------------
 Function: sendCmd(cmd)
 Purpose: send command to knx bus
 Parameters: cmd -- [IN], command parameter
 return: 0 --  success
         -1 -- failed
-------------------------------------------------------------------*/
int CIpKnxApp::sendCmd(KNXCMD cmd)
{
    unsigned char data[16] = {0x55,   //0.head
                              0xaa,   //1.head
                              0x00,   //2.reserve
                              0x00,   //3.reserve
                              0x00,   //4.reserve
                              0x00,   //5.reserve
                              0x05,   //6.cmd id
                              0x01,   //7.cmd id
                              0x03,   //8.priority
                              0x01,   //9.length
                              0x00,   //10.desc addr
                              0x00,   //11.desc addr
                              0x00,   //12.value
                              0x00,   //13.value
                              0x00,   //14.check sum
                              0x00};  //15.check sum
    int iRet = 0;
    unsigned short sum = 0;    

    data[6] = (cmd.mode == CMD_MODE_WRITE) ? 0x05 : 0x04;

    data[9] = cmd.len;
    data[10] = cmd.chAddrFirst;
    data[11] = cmd.chAddrSecond;
    data[12] = (char)cmd.value;
    data[13] = (char)(cmd.value>>8);  
        
    for (int i = 0; i < 14; i++) 
        sum += data[i];
    
    data[14] = (unsigned char)sum;
    data[15] = (unsigned char)(sum>>8); 

    //m_hIpKnxLock.lock();
    iRet = m_tcpClt.write((char*)data, 16);
    platMSleep(IPKNX_CMD_DELAY);
    //m_hIpKnxLock.unlock();

    return iRet;
}

//call back function, when tcp client can be read, 
//this function will be called
int CIpKnxApp::readHandle(void)
{
    int iLen = 0;
    int iResult = 0;
    char szRBuf[1024] = {0};
    char szLogout[1024] = {0};
    char* p_buf = NULL;
    int iHandleLen = 0;

    iLen = m_tcpClt.read((char*)szRBuf, 1024);
    p_buf = szRBuf;
    while (iLen > 0) {
        memset(szLogout, 0, 1024);
        for (int i = 0; i < iLen; i++) 
            sprintf(szLogout, "%s0x%02x ", szLogout, p_buf[i]);

        //logout(LOG_EMERGENCY, szLogout, __FILE__, __LINE__);

        //check whether the data is the response data
        //0 -- communicate correctly， 
        //-1 -- not the data required，
        //-2 -- error communication
        /*
        //old code
        iResult = checkCmdRes(szRBuf, iLen);    
        if (iResult == 0)
            setCmdState(CMD_STATE_RES_CORRECT);
        else if (iResult == -2)
            setCmdState(CMD_STATE_RES_ERROR);

        //检查是否读数据
        char szAddr[16] = {0};
        char szVal[8] = {0};
        iResult = checkDataRes(szRBuf, iLen, szAddr, szVal);
        if (iResult == 0) {
            //save to rdb  
            sprintf(szLogout, "read addr: %s, value = %s", szAddr, szVal);
            logout(LOG_DEBUG, szLogout, __FILE__, __LINE__);
   
            rdb_updateDevValByGroupAddr(szAddr, szVal);
        }

        */

        iResult = checkCmdRes(p_buf, iLen);    
        if (iResult == 0)
            setCmdState(CMD_STATE_RES_CORRECT);
        else if (iResult == -2)
            setCmdState(CMD_STATE_RES_ERROR);

        //检查是否读数据
        char szAddr[16] = {0};
        char szVal[8] = {0};
        iResult = checkDataResNew(p_buf, iLen, szAddr, szVal, &iHandleLen);
        if (iResult != 0)
            break;

        sprintf(szLogout, "addr: %s, value = %s", szAddr, szVal);
        //logout(LOG_ALARM, szLogout, __FILE__, __LINE__);

        rdb_updateDevValByGroupAddr(szAddr, szVal);

        iLen -= iHandleLen;
        p_buf += iHandleLen;
    }

    return 0;
}

/*-------------------------------------------------------------------
 Function: checkCmdRes(const char* szBuf, int iLen)
 Purpose: check the response from knx bus
          correct format {5A A5 00 00 01 00 00 01}
                         {AA 55 00 00 00 00 01 00 00 00 00 00 00 01}
 Parameters: szBuf -- [IN], data readed
             iLen -- [IN], data length
 return: 0  -- success
         -1 -- not the data required
         -2 -- error (including cmd error, sumcheck error)
-------------------------------------------------------------------*/
int CIpKnxApp::checkCmdRes(const char* szBuf, int iLen)
{
    int i = 0;
    if (iLen < CMD_ACK_LEN + CMD_RES_LEN)       //读取的字节不够存命令回应及命令响应
        return -1;

    //check cmd ack
    //check 0X5A
    for (i = 0; i < iLen - 1; i++) {
        if (((unsigned char)szBuf[i] == 0x5A) &&
            ((unsigned char)szBuf[i + 1] == 0xA5))
            break;
    }

    if (i + CMD_ACK_LEN + CMD_RES_LEN > iLen)     //有效的字节不够存命令回应及命令响应
        return -1;

    if (szBuf[i + 4] != 1) 
        return -2;

    //check cmd res
    i += CMD_ACK_LEN;
    if ((unsigned char)szBuf[i] != 0xAA || 
        (unsigned char)szBuf[i + 1] != 0x55)
        return -1;

    return (szBuf[i + 6] == 1 ? 0 : -2);
}

/*-------------------------------------------------------------------
 Function: checkDataRes(const char* szBuf, int iLen)
 Purpose: 检查knx总线返回的数据
          正常格式{A5 5A 00 00 srcaddr srcaddr descaddr descaddr length 40 00 data ... checksum checksum}
          数据长度 >2,未考虑
 Parameters: szBuf -- [IN], 读到的数据
             iLen -- [IN], 数据长度
             szAddr -- [OUT], 读到的组地址
             szVal -- [OUT], 读到的数据
 return: 0  -- 成功
         -1 -- 失败
         -2 -- 回应错误（包括命令错误，和校验失败等）
-------------------------------------------------------------------*/
int CIpKnxApp::checkDataRes(const char* szBuf, int iLen, char* szAddr, char* szVal)
{
    int i = 0;
    if (iLen < DATA_RES_LEN)       //读取的字节不够存数据响应
        return -1;

    //check cmd ack
    //check 0XA5 0X5A 
    for (i = 0; i < iLen - 1; i++) {
        if (((unsigned char)szBuf[i] == 0xA5) &&
            ((unsigned char)szBuf[i + 1] == 0x5A))
            break;
    }

    if (i + DATA_RES_LEN > iLen)     //有效的字节不够存数据响应
        return -1;

    //get addr
    unsigned char chFirst = (unsigned char)szBuf[i + 7];
    unsigned char chSecond = (unsigned char)szBuf[i + 6];
    if (generateGroupAddr(chFirst, chSecond, szAddr))
        return -1;

    //get value
    int iLength = szBuf[i + 8];
    int iVal = 0;
    if (iLength <= 2) {
        iVal = (unsigned char)szBuf[i + 11];
        sprintf(szVal, "%u", iVal);
    }

    return 0;
}

/*-------------------------------------------------------------------
 Function: checkDataResNew(szBuf, iLen, szAddr, szVal, p_iHandleSize)
 Purpose: 检查knx总线返回的数据
          正常格式{A5 5A 00 00 srcaddr srcaddr descaddr descaddr length 40 00 data ... checksum checksum}
          数据长度 >2,未考虑
 Parameters: szBuf -- [IN], 读到的数据
             iLen -- [IN], 数据长度
             szAddr -- [OUT], 读到的组地址
             szVal -- [OUT], 读到的数据
             p_iHandleSize -- [OUT], 处理的数据
 return: 0  -- 成功
         -1 -- 失败
         -2 -- 回应错误（包括命令错误，和校验失败等）
-------------------------------------------------------------------*/
int CIpKnxApp::checkDataResNew(const char* szBuf, int iLen,
                               char* szAddr, char* szVal, 
                               int* p_iHandleSize)
{
    int i = 0;
    if (iLen < DATA_RES_LEN)       //读取的字节不够存数据响应
        return -1;

    //check cmd ack
    //check 0XA5 0X5A 
    for (i = 0; i < iLen - 1; i++) {
        if (((unsigned char)szBuf[i] == 0xA5) &&
            ((unsigned char)szBuf[i + 1] == 0x5A))
            break;
    }

    if (i + DATA_RES_LEN > iLen)     //有效的字节不够存数据响应
        return -1;

    //get addr
    unsigned char chFirst = (unsigned char)szBuf[i + 7];
    unsigned char chSecond = (unsigned char)szBuf[i + 6];
    if (generateGroupAddr(chFirst, chSecond, szAddr))
        return -1;

    //get value
    int iLength = szBuf[i + 8];
    int iVal = 0;
    if (iLength <= 2) {
        iVal = (unsigned char)szBuf[i + 11];
        sprintf(szVal, "%u", iVal);
    }

    *p_iHandleSize = iLength <= 2 ? 
        (i + 14) : (i + 14 + iLength - 2);

    return 0;
}

//set current app state
void CIpKnxApp::setCmdState(int iState)
{
    if (iState < CMD_STATE_SENDTO_DEV ||
        iState > CMD_STATE_RES_ERROR)
        return ;

    m_hIpKnxLock.lock();
    m_iCmdState = iState;
    m_hIpKnxLock.unlock();
}

int CIpKnxApp::getCmdState(void)
{
    int iState = 0;

    m_hIpKnxLock.lock();
    iState = m_iCmdState;
    m_hIpKnxLock.unlock();

    return iState;
}
