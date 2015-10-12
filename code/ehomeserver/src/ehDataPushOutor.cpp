/********************************************************************
 File: ehDataPushOutor.cpp
 Function: push out data to client connected 
 Author: Luobihe
 Date: 2015-8-26
*********************************************************************/

#include <stdio.h>
#include <string.h>

#include "ehDataPushOutor.h"
#include "EHSCmdParser.h"
#include "platcomm.h"
#include "logsys.h"

static char s_szPeerIn[16];
static int s_peerPort;

int pushOut_init(void)
{
    FILE *pFile = NULL;
    char szFilePath[256] = {0};
    char szFileName[256] = {0};
    char szCfgFilePath[256] = {0};
    char szIp[64] = {0};
    int  iPort = 0;
    
    if (getExePath(szFilePath, szFileName, 256))
        return -1;

    sprintf(szCfgFilePath, "%s/%s", szFilePath, FILENAME_UDPLISTENER);

    pFile = fopen(szCfgFilePath, "rb");
    if (pFile) {
        fscanf(pFile, "%[^:]%*c%d", s_szPeerIn, &s_peerPort);
        fclose(pFile);
        pFile = NULL;
    }
    else {
        strcpy(s_szPeerIn, "192.168.1.255");
        s_peerPort = UDP_LISTENER_PORT;
    }

    return 0;
}

/*-------------------------------------------------------------------
 Function: pushOutDevData(iAddrId, p_data, iValType)
 Purpose: pack the output for getting right read data
 Parameters: iAddrId -- [IN], address id
             p_data -- [IN], data from device
             iValType -- [IN], date type
 return: 0 --  success
         -1 -- failed
-------------------------------------------------------------------*/
int pushOutDevData(int iAddrId, void* p_data, int iValType)
{
    CEHSCmdParser parser;
    char szOut[1024] = {0};
    int iOutLen = 0;
    char szLogout[1024] = {0};
    
    //generate packet to push out
    if (parser.packPushOut(iAddrId, p_data, iValType, szOut, &iOutLen)) {
        sprintf(szLogout, "failed to pack pushout, id = %d", iAddrId);
        logout(LOG_ALARM, szLogout, __FILE__, __LINE__);
        return -1;
    }

    sprintf(szLogout, "send udp data ip = %s, port = %d, data: ", s_szPeerIn, s_peerPort);
    for (int i = 0; i < iOutLen; i++)
        sprintf(szLogout, "%s0x%x ", szLogout, szOut[i]);

    //logout(LOG_ALARM, szLogout, __FILE__, __LINE__);

    //send to udp peer
    return sendUdpData(s_szPeerIn, s_peerPort, szOut, iOutLen);

    /*
#ifdef _WIN32
    return sendUdpData(s_szPeerIn, s_peerPort, szOut, iOutLen);
#else
    return broadCastUdpData(s_peerPort, szOut, iOutLen);
#endif
    */
}