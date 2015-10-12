#ifndef __TESTNBEHOMECLIENT_H__
#define __TESTNBEHOMECLIENT_H__

#include "nbEhomeClient.h"

class CNbEhomeClientTester
{
private:
    CNbEhomeTcpClient m_client;

public:
    void test(void);
    void testInit(void);
    void testWriteDev(void);

    void testReadDev(void);
};

#endif  //__TESTNBEHOMECLIENT_H__