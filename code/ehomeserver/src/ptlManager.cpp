/********************************************************************
 File: ptlManager.cpp
 Function: define the interface to call different protocol
 Author: Luobihe
 Date: 2015-8-4
********************************************************************/

#include <stdlib.h>

#include "ptlManager.h"
#include "ip_knxPtl.h"

static CIpKnxPtl s_ipKnxPtl;

static TagPtl s_arrPtl[] = {
    {1, &s_ipKnxPtl},
};

TagPtl* getTagPtl(int iPtlId)
{
    for (int i = 0; i < sizeof(s_arrPtl) / sizeof(TagPtl); i++) {
        if (s_arrPtl[i].iPtlId == iPtlId)
            return &s_arrPtl[i];
    }

    return NULL;
}

int ptl_init(void)
{
    for (int i = 0; i < sizeof(s_arrPtl) / sizeof(TagPtl); i++) {
        if (s_arrPtl[i].p_ptl->init())
            return -1;
    }

    return 0;
}

/*-------------------------------------------------------------------
 Function: readDev(iPtlId, szAddr, p_value, iDataType)
 Purpose: read the special data differed by id
 Parameters: iPtlId -- [IN], protocol id
             szAddr -- [IN], device address used by protocol
             p_value -- [OUT], result
             iDataType -- [IN], data type
 return: 0 -- success, -1 -- failed
-------------------------------------------------------------------*/
int readDev(int iPtlId, const char* szAddr, void* p_value, int iDataType)
{
    PTagPtl p_tagptl = getTagPtl(iPtlId);
    if (!p_tagptl)
        return -1;

    return p_tagptl->p_ptl->readDev(szAddr, p_value, iDataType);
}


/*-------------------------------------------------------------------
 Function: writeDev(iPtlId, szAddr, p_value, iDataType)
 Purpose: write the special data differed by id
 Parameters: iPtlId -- [IN], protocol id
             szAddr -- [IN], device address used by protocol
             p_value -- [IN], data to write
             iDataType -- [IN], data type
 return: 0 -- success, -1 -- failed
-------------------------------------------------------------------*/
int writeDev(int iPtlId, const char* szAddr, void* p_value, int iDataType)
{
    PTagPtl p_tagptl = getTagPtl(iPtlId);
    if (!p_tagptl)
        return -1;

    return p_tagptl->p_ptl->writeDev(szAddr, p_value, iDataType);
}

/*-------------------------------------------------------------------
 Function: monitorDev(iPtlId, szAddr, iDataType)
 Purpose: monitor device
 Parameters: iPtlId -- [IN], protocol id
             szAddr -- [IN], device address used by protocol
             iDataType -- [IN], data type
 return: 0 -- success, -1 -- failed
-------------------------------------------------------------------*/
int monitorDev(int iPtlId, const char* szAddr, int iDataType)
{
    PTagPtl p_tagptl = getTagPtl(iPtlId);
    if (!p_tagptl)
        return -1;

    return p_tagptl->p_ptl->monitorDev(szAddr, iDataType);
}