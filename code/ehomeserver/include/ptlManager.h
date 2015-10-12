/********************************************************************
 File: ptlManager.h
 Function: define the interface to call different protocol
 Author: Luobihe
 Date: 2015-8-4
********************************************************************/

#ifndef __PTLMANAGER_H__
#define __PTLMANAGER_H__

#include "ptlInterface.h"

typedef union {
    int     iVal;
    long    lVal;
    float   fVal;
    double  dVal;
} PtllDataType, *PPtlDataType;

typedef struct {
    int iPtlId;
    CPtlInterface* p_ptl;
} TagPtl, *PTagPtl;

//initial device, 0 -- sucess, -1 failed
int ptl_init(void);

//0 -- sucess, -1 failed
int readDev(int iPtlId, const char* szAddr, void* p_value, int iDataType);

//0 -- sucess, -1 failed
int writeDev(int iPtlId, const char* szAddr, void* p_value, int iDataType);

//0 -- sucess, -1 failed
int monitorDev(int iPtlId, const char* szAddr, int iDataType);


#endif  //__PTLMANAGER_H__