
#include <stdio.h>
#include "testEHSCmdParser.h"
#include "rdbManager.h"
#include "ptlManager.h"


void CTestEHSCmdParser::testCheckCmdID(void)
{
    int iRet = 0;

    iRet = m_ehsparser.checkCmdID(NEWBEST_CMD_GETJSONFILE);
    printf("cmd = 0X%x, ret = %d\n", NEWBEST_CMD_GETJSONFILE, iRet);

    iRet = m_ehsparser.checkCmdID(NEWBEST_CMD_ADDR_READ);
    printf("cmd = 0X%x, ret = %d\n", NEWBEST_CMD_ADDR_READ, iRet);

    iRet = m_ehsparser.checkCmdID(NEWBEST_CMD_ADDR_WRITE);
    printf("cmd = 0X%x, ret = %d\n", NEWBEST_CMD_ADDR_WRITE, iRet);

    iRet = m_ehsparser.checkCmdID(NEWBEST_CMD_GETJSONFILE_ACK);
    printf("cmd = 0X%x, ret = %d\n", NEWBEST_CMD_GETJSONFILE_ACK, iRet);

    iRet = m_ehsparser.checkCmdID(NEWBEST_CMD_ADDR_READ_ACK);
    printf("cmd = 0X%x, ret = %d\n", NEWBEST_CMD_ADDR_READ_ACK, iRet);

    iRet = m_ehsparser.checkCmdID(NEWBEST_CMD_ADDR_WRITE_ACK);
    printf("cmd = 0X%x, ret = %d\n", NEWBEST_CMD_ADDR_WRITE_ACK, iRet);

    iRet = m_ehsparser.checkCmdID(0X93);
    printf("cmd = 0X%x, ret = %d\n", 0X93, iRet);
}

void CTestEHSCmdParser::testGetByteSum(void)
{
    printf("==========test getByteSum===========\n");

    char data[16] = {0x55,   //0.head
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

    unsigned short res = m_ehsparser.getByteSum(data, 16);

    printf("first byte = 0x%02x, second byte = 0x%02x\n",
        res & 0xff, res >> 8);
}

void CTestEHSCmdParser::testCheckInput(void)
{
    printf("==========test checkInput===========\n");

    char data[12] = {0xF0,   //0.head
                     0x0F,   //1.head
                     0x02,   //2.reserve
                     0x01,   //3.reserve
                     0x00,   //4.reserve
                     0x00,   //5.reserve
                     0x00,   //6.cmd id
                     0x00,   //7.cmd id
                     0x00,   //8.priority
                     0x02,   //9.length
                     0x01,   //10.desc addr
                     0x00 };   //11.desc addr

    //F00F03010001000000000401

    //F00F020100000000000201

    int iRet = m_ehsparser.checkInput(data, 11);
    printf("right result, ret = %d\n", iRet);

    iRet = m_ehsparser.checkInput(data, 13);
    printf("bad result, ret = %d\n", iRet);

    iRet = m_ehsparser.checkInput(data, 10);
    printf("bad result, ret = %d\n", iRet);

    data[0] = 0xFF;
    data[1] = 0x00;
    iRet = m_ehsparser.checkInput(data, 11);
    printf("bad result, ret = %d\n", iRet);

    data[0] = 0xF0;
    data[1] = 0x0F;
    data[2] = 0x04;
    data[9] = 0x04;

    iRet = m_ehsparser.checkInput(data, 11);
    printf("bad result, ret = %d\n", iRet);


    data[10] = 0x00;

    iRet = m_ehsparser.checkInput(data, 11);
    printf("bad result, ret = %d\n", iRet);

    data[2] = 0x03;
    data[3] = 0x01;
    data[4] = 0x00;
    data[5] = 0x01;
    data[6] = 0x00;
    data[7] = 0x00;
    data[8] = 0x00;
    data[9] = 0x00;
    data[10] = 0x04;
    data[11] = 0x01;

    iRet = m_ehsparser.checkInput(data, 12);
    printf("right result, ret = %d\n", iRet);

    data[10] = 0x00;

    iRet = m_ehsparser.checkInput(data, 12);
    printf("bad result, ret = %d\n", iRet);

}

void CTestEHSCmdParser::testGetCmdBodyFromInput(void)
{
    printf("==========test getCmdBodyFromInput===========\n");

    char data[12] = {0xF0,   //0.head
                     0x0F,   //1.head
                     0x02,   //2.reserve
                     0x01,   //3.reserve
                     0x00,   //4.reserve
                     0x00,   //5.reserve
                     0x00,   //6.cmd id
                     0x00,   //7.cmd id
                     0x00,   //8.priority
                     0x02,   //9.length
                     0x01,   //10.desc addr
                     0x00 };   //11.desc addr

    //F00F03010001000000000401

    //F00F020100000000000201

    CMD_BODY cmdbody = {0};

    int iRet = m_ehsparser.getCmdBodyFromInput(data, 11, &cmdbody);
    if (iRet == 0) {
        printf("effective data\n");
        printf("cmdid = %d\n", cmdbody.iCmdId);
        printf("address id = %d\n", cmdbody.iAddrId);
        printf("datalen = %d\n", cmdbody.iDataLen);
        printf("data start position = %d\n\n", cmdbody.iDataStartPos);
    }
}

void CTestEHSCmdParser::testGetDataFromInput(void)
{
    printf("==========test GetDataFromInput===========\n");
    int iValue = 0;

    char data[4] = {0x01,   
                     0x01,   
                     0x00,   
                     0x00 };  

    int iRet = m_ehsparser.getDataFromInput(data, 4, 1, &iValue);

    if (iRet)
        printf("get failed\n");
    else
        printf("get data success, value = %d\n", iValue);

}

void CTestEHSCmdParser::testPackPutOut(void)
{
     printf("==========test PackPutOut===========\n");

     char szOut[1024] = {0};
     int iLen = 0;

     printf("\n -- error cmd -- \n");
     m_ehsparser.packPutOut(CLIENT_CMD_ERROR, 0, 0, 0, 0, NULL, szOut, &iLen);

     testPrintOut(szOut, iLen);

     printf("\n -- error dev ack -- \n");
     m_ehsparser.packPutOut(CLIENT_CMD_RIGHT, DEVICE_COMM_ERROR,
         NEWBEST_CMD_ADDR_READ_ACK, 1, 0, NULL, szOut, &iLen);
     testPrintOut(szOut, iLen);

     printf("\n -- right write dev ack -- \n");
     m_ehsparser.packPutOut(CLIENT_CMD_RIGHT, DEVICE_COMM_RIGHT,
         NEWBEST_CMD_ADDR_WRITE_ACK, 1, 0, NULL, szOut, &iLen);
     testPrintOut(szOut, iLen);

     printf("\n -- right read dev ack -- \n");
     char data[4] = {0};
     data[0] = 1;
     int i = 1;
     m_ehsparser.packPutOut(CLIENT_CMD_RIGHT, DEVICE_COMM_RIGHT,
         NEWBEST_CMD_ADDR_READ_ACK, 1, 2, &i, szOut, &iLen);
     testPrintOut(szOut, iLen);    
}

void CTestEHSCmdParser::testPackPutOut_ErrorCmd(void)
{
    printf("==========test PackPutOut_ErrorCmd===========\n");

    char szOut[1024] = {0};
    int iLen = 0;

    printf("\n -- error cmd -- \n");
    m_ehsparser.packPutOut_errorCmd(szOut, &iLen);
    testPrintOut(szOut, iLen);  
}

void CTestEHSCmdParser::testPackPutOut_ErrorDevAck(void)
{
    printf("==========test PackPutOut_ErrorDevAck===========\n");

    char szOut[1024] = {0};
    int iLen = 0;

    printf("\n -- error dev ack -- \n");
    m_ehsparser.packPutOut_errorDevAck(NEWBEST_CMD_ADDR_READ_ACK, 1, szOut, &iLen);
    testPrintOut(szOut, iLen);  
}

void CTestEHSCmdParser::testPackPutOut_WriteOk(void)
{
    printf("==========test PackPutOut_WriteOk===========\n");

    char szOut[1024] = {0};
    int iLen = 0;

    printf("\n -- write ok -- \n");
    m_ehsparser.packPutOut_writeOk(1, szOut, &iLen);
    testPrintOut(szOut, iLen);
}

void CTestEHSCmdParser::testPackPutOut_readOk(void)
{
    printf("==========test PackPutOut_ReadOk===========\n");

    char szOut[1024] = {0};
    int iLen = 0;

    printf("\n -- read ok -- \n");

    PtllDataType data = {0};

    /*packPutOut_readOk(int iAddrId, 
                                      void* p_data, 
                                      int iValType, 
                                      char* szOut, 
                                      int* p_iRetlen)
*/

    printf("\n -- read bool ok -- \n");
    data.iVal = 1;
    m_ehsparser.packPutOut_readOk(1, &data, DEV_TYPE_BOOL, szOut, &iLen);
    testPrintOut(szOut, iLen);

    printf("\n -- read byte ok -- \n");
    data.iVal = 7;
    m_ehsparser.packPutOut_readOk(1, &data, DEV_TYPE_BYTE, szOut, &iLen);
    testPrintOut(szOut, iLen);

    printf("\n -- read short ok -- \n");
    data.iVal = 0x1234;
    m_ehsparser.packPutOut_readOk(1, &data, DEV_TYPE_SHORT, szOut, &iLen);
    testPrintOut(szOut, iLen);

    printf("\n -- read int ok -- \n");
    data.iVal = 0x12345678;
    m_ehsparser.packPutOut_readOk(1, &data, DEV_TYPE_INT, szOut, &iLen);
    testPrintOut(szOut, iLen);

    printf("\n -- read long ok -- \n");
    data.lVal = 0x12345678AB345678;

    printf("sizeof(long) = %d\n", sizeof(long));
    m_ehsparser.packPutOut_readOk(1, &data, DEV_TYPE_LONG, szOut, &iLen);
    testPrintOut(szOut, iLen);
}

void CTestEHSCmdParser::testParseCmd(void)
{
    printf("==========test parseCmd===========\n");

    char data[12] = {0xF0,   //0.head
                     0x0F,   //1.head
                     0x02,   //2.reserve
                     0x01,   //3.reserve
                     0x00,   //4.reserve
                     0x00,   //5.reserve
                     0x00,   //6.cmd id
                     0x00,   //7.cmd id
                     0x00,   //8.priority
                     0x02,   //9.length
                     0x01,   //10.desc addr
                     0x00 };   //11.desc addr

    char data2[12] = {0xF0,   //0.head
                     0x0F,   //1.head
                     0x03,   //2.reserve
                     0x01,   //3.reserve
                     0x00,   //4.reserve
                     0x01,   //5.reserve
                     0x00,   //6.cmd id
                     0x00,   //7.cmd id
                     0x00,   //8.priority
                     0x01,   //9.length
                     0x05,   //10.desc addr
                     0x01 };   //11.desc addr
    

    char szOut[1024] = {0};
    int iOutLen = 0;
    int iRet = 0;

    printf("\n---test read---\n");

    iRet = m_ehsparser.parseCmd(data, 11, szOut, &iOutLen);
    printf("ret = %d\n", iRet);
    testPrintOut(szOut, iOutLen);

    printf("\n---test write---\n");


    iRet = m_ehsparser.parseCmd(data2, 12, szOut, &iOutLen);
    printf("ret = %d\n", iRet);
    testPrintOut(szOut, iOutLen);
}

void CTestEHSCmdParser::testPackPushOut(void)
{
    printf("==========test pack push out===========\n");  

    char szOut[1024] = {0};
    int iOutLen = 0;
    int iRet = 0;

    int data = 1;

    iRet = m_ehsparser.packPushOut(1, &data, DEV_TYPE_BYTE, szOut, &iOutLen);
    printf("ret = %d\n", iRet);
    testPrintOut(szOut, iOutLen);

    data = 0;
    iRet = m_ehsparser.packPushOut(258, &data, DEV_TYPE_BYTE, szOut, &iOutLen);
    printf("ret = %d\n", iRet);
    testPrintOut(szOut, iOutLen);
}

void CTestEHSCmdParser::testReadJsonFile(void)
{
    printf("==========test readJosnFile===========\n"); 

    char szOut[1024] = {0};
    int iOutLen = 0;

    int iSize = m_ehsparser.getJsonFileSize();
    printf("json file size is %d\n", iSize);

    m_ehsparser.readJsonFile(szOut, &iOutLen);

    printf("read %d counts, result is \n", iOutLen);
    printf("%s\n", szOut);
}

void CTestEHSCmdParser::testPrintOut(const char* szOut, int iLen)
{
    for (int i = 0; i < iLen; i++)
        printf("out[%d] = 0x%02x\n", i, szOut[i]);
}

/*
void packPutOut(int iParseRight, int iDevCommRight, 
                int iCmdId, int iAddrId, int iDataLen,
                void* data, char* szOut, int* p_iRetlen); 
                */

void CTestEHSCmdParser::test(void)
{
   // testCheckCmdID();

    //testGetByteSum();

    //testCheckInput();

    //testGetCmdBodyFromInput();

    //testGetDataFromInput();

    //testPackPutOut();

    //testPackPutOut_ErrorCmd();

    //testPackPutOut_ErrorDevAck();

    //testPackPutOut_WriteOk();

    //testPackPutOut_readOk();

    //testParseCmd();

    //testPackPushOut();

    testReadJsonFile();
}