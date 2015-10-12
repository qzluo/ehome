/********************************************************************
 File: testEHSCmdParser.h
 Function: provide operation by ehome server to parse command
 Author: Luobihe
 Date: 2015-7-30
*********************************************************************/

#ifndef __TESTEHSCMDPARSER_H__
#define __TESTEHSCMDPARSER_H__


#include "EHSCmdParser.h"

class CTestEHSCmdParser
{
private:
    CEHSCmdParser m_ehsparser;

    void testCheckCmdID(void);

    void testGetByteSum(void);

    void testCheckInput(void);

    void testGetCmdBodyFromInput(void);

    void testGetDataFromInput(void);

    void testPackPutOut(void);

    void testPackPutOut_ErrorCmd(void);

    void testPackPutOut_ErrorDevAck(void);

    void testPackPutOut_WriteOk(void);

    void testPackPutOut_readOk(void);

    void testPrintOut(const char* szOut, int iLen);

    void testParseCmd(void);

    void testPackPushOut(void);

    void testReadJsonFile(void);

public:
    void test(void);
};

#endif  //__TESTEHSCMDPARSER_H__