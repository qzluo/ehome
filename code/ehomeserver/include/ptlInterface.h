/********************************************************************
 File: ptlInterface.h
 Function: define the protocol interfac used in ehome
 Author: Luobihe
 Date: 2015-8-4
********************************************************************/

#ifndef __PTLINTERFACE_H__
#define __PTLINTERFACE_H__

class CPtlInterface
{
public:
    virtual int init(void) = 0;
    virtual int readDev(const char* szAddr, void* p_value, int iDataType) = 0;
    virtual int writeDev(const char* szAddr, void* p_value, int iDataType) = 0;
    virtual int monitorDev(const char* szAddr, int iDataType) = 0;
};

#endif  //__PTLINTERFACE_H__