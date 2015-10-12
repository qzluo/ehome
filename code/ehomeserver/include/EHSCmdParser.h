/********************************************************************
 File: EHSCmdParser.h
 Function: provide operation by ehome server to parse command
 Author: Luobihe
 Date: 2015-7-30
*********************************************************************/

/*
2015-7-30
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
бн	         0X--               data, may be empty
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
бн	         0X--               data, may be empty
last-2       0X--               data, may be empty
last-1       0X--               check sum
last	     0X--               check sum

*/

#ifndef __EHSCMDPARSER_H__
#define __EHSCMDPARSER_H__

#include "cmdParser.h"

#define FILENAME_JSON       "knx_json.dat"

#define MIN_CLIENT_CMD_LEN   11    //min client command length

//command id
#define NEWBEST_CMD_GETJSONFILE      0X01
#define NEWBEST_CMD_ADDR_READ        0X02
#define NEWBEST_CMD_ADDR_WRITE       0X03
#define NEWBEST_CMD_GETJSONFILE_ACK  0X81
#define NEWBEST_CMD_ADDR_READ_ACK    0X82
#define NEWBEST_CMD_ADDR_WRITE_ACK   0X83
#define NEWBEST_CMD_ADDR_PUSHOUT     0X84

//cmd state
#define CLIENT_CMD_ERROR             0
#define CLIENT_CMD_RIGHT             1

//device communicate state
#define DEVICE_COMM_ERROR            0
#define DEVICE_COMM_RIGHT            1


//effective command data
typedef struct {
    int iCmdId;             //command id, read or write
    int iAddrId;            //address by a number
    int iDataLen;           //data len
    int iDataStartPos;      //data start position, -1 represents uneffective
} CMD_BODY, *PCMD_BODY;


class CEHSCmdParser : public CCmdParser
{
private:
public:

    //get the command infomation from input
    //0 -- correct, -1 -- not correct
    int getCmdBodyFromInput(const char* szIn, int iInLen, PCMD_BODY p_result);

    //check whether the cmd from client is correct or not
    //0 -- correct, -1 -- not correct
    int checkInput(const char* szIn, int iInLen);

    //check whether the cmdID is correct or not
    //0 -- correct, -1 -- not correct
    int checkCmdID(unsigned char ucCmdId);

    //get sum of each byte in input
    unsigned short getByteSum(const char* szIn, int iInLen);

    //get data value from input
    int getDataFromInput(const char* p_start, int iDataLen, 
        int iDataType, void* p_result);

    //pack the output according to input parameters
    void packPutOut(int iParseRight, int iDevCommRight, 
        int iCmdId, int iAddrId, int iDataLen,
        void* data, char* szOut, int* p_iRetlen); 

    void packPutOut_errorCmd(char* szOut, int* p_iRetlen); 

    void packPutOut_errorDevAck(int iCmdId, int iAddrId, char* szOut, int* p_iRetlen); 

    void packPutOut_writeOk(int iAddrId, char* szOut, int* p_iRetlen); 

    void packPutOut_readOk(int iAddrId, void* p_data, int iValType, 
        char* szOut, int* p_iRetlen); 

    //json response
    int getJsonFileSize(void);    
    int readJsonFile(char* szOut, int* p_iRetLen);
    int getJsonFilePath(char* p_filePath);

public:
    //-1 -- error, >= 0: handled length in message 
    virtual int parseCmd(const char* szIn, int iInLen, char* szOut, int* p_iRetlen);

    //-1 -- error, 0 -- correct. split to get first cmd from message read
    virtual int getFirstCmd(const char* szIn, int iInLen, 
        int* p_iStartPos, int* p_iCmdLen);

    int packPushOut(int iAddrId, void* p_data, int iValType, 
        char* szOut, int* p_iRetLen);
};

#endif  //__EHSCMDPARSER_H__