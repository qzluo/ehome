/********************************************************************
 File: ip_knxPtl.h
 Function: define the protocol to communicate with ip_knx device
 Author: Luobihe
 Date: 2015-8-4
********************************************************************/

#ifndef __IP_KNXPTL_H__
#define __IP_KNXPTL_H__

#include "ptlInterface.h"
#include "ip_knxapp.h"

#define IPKNXFILE "ipknxsrv.ini"

class CIpKnxPtl : public CPtlInterface
{
private:
    CIpKnxApp m_ipKnxApp;

private:
    int dtToIpKnxLen(int iDataType);

public:
    int init(void);
    virtual int readDev(const char* szAddr, void* p_value, int iDataType);
    virtual int writeDev(const char* szAddr, void* p_value, int iDataType);
    virtual int monitorDev(const char* szAddr, int iDataType);
};

#endif  //__IP_KNXPTL_H__