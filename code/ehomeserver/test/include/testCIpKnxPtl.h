#ifndef __TESTCIPKNXPTL_H__
#define __TESTCIPKNXPTL_H__

#include "ip_knxPtl.h"

class CTestIpxKnxPtl
{
private:
    CIpKnxPtl m_ptl;

    void testReadDev(void);
    void testReadDevWithAddr(const char* saAddr);

    void testWriteDev(void);
    void testWriteDevWithAddr(const char* saAddr, int iVal);

public:
    void test(void);
};

#endif  