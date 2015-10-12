#ifndef __TESTEASYTCPCLT_H__
#define __TESTEASYTCPCLT_H__

#include "platcomm.h"

class CTestEasyTcpClt
{
private:
    CEasyTcpClt m_easyTcpClt;

    void testInit(void);
    void testWrite(void);
    void testRead(void);


public:
    void test(void);
};

#endif  //__TESTEASYTCPCLT_H__