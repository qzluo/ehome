#ifndef __EASYTCPCLTTESTER_H__
#define __EASYTCPCLTTESTER_H__

#include "platcomm.h"

class CEasyTcpCltTester : public CTcpCltAppInterface
{
private:
    CEasyTcpClt m_tcpclt;


public:
    void test(void);

    void testInit(void);

    void testWrite(void);
    void testRead(void);
    void testClear(void);

public:
    int readHandle(void);
};

#endif  //__EASYTCPCLTTESTER_H__