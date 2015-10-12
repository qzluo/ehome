#ifndef __NBEHOMEPTLHANDLER_H__
#define __NBEHOMEPTLHANDLER_H__

//command id
#define NEWBEST_CMD_GETJSONFILE      0X01
#define NEWBEST_CMD_ADDR_READ        0X02
#define NEWBEST_CMD_ADDR_WRITE       0X03
#define NEWBEST_CMD_GETJSONFILE_ACK  0X81
#define NEWBEST_CMD_ADDR_READ_ACK    0X82
#define NEWBEST_CMD_ADDR_WRITE_ACK   0X83
#define NEWBEST_CMD_ADDR_PUSHOUT     0X84

#define MIN_SERVER_RES_LEN   13  

//effective command data
typedef struct {
    int iCmdId;             //command id, read or write
    int iAddrId;            //address by a number
    int iDataLen;           //data len
    int iDataStartPos;      //data start position, -1 represents uneffective
} CMD_BODY, *PCMD_BODY;

class CNbEhomePtlHandler
{
private:
    int checkInput(const char* szBuf, int iLen);

public:
    int getCmdBodyFromInput(const char* szIn, int iInLen, PCMD_BODY p_result);
};

#endif  //__NBEHOMEPTLHANDLER_H__