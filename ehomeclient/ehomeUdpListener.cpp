#include <stdio.h>
#include "ehomeUdpListener.h"
#include "nbEhomePtlHandler.h"
#include "logsys.h"

#define WM_CLIETN_READBACK  WM_USER + 100

void CEhomeUdpListener::setWinHandle(HWND hWnd)
{
    m_hWnd = hWnd;
}

void CEhomeUdpListener::startListenThread(void)
{
    easyCreateThread(udpListenFun, this);
}

void* CEhomeUdpListener::udpListenFun(void* arg)
{
    int sockfd = 0;
    int ret = 0;
    char szLogout[1024] = {0};
    CEhomeUdpListener* pTemp = (CEhomeUdpListener*)arg;

#ifdef _WIN32

    WORD wVersionRequested = 0;
    WSADATA wsaData = {0};    
    wVersionRequested = MAKEWORD(1, 1);
    ret = WSAStartup(wVersionRequested, &wsaData);
    if ( ret != 0 )
        return (void*)(-1);

    // check winsock version
    if ( LOBYTE( wsaData.wVersion ) != 1 ||
        HIBYTE( wsaData.wVersion ) != 1 ) {
            WSACleanup( );
            return (void*)(-1);
    }

#endif

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
        return (void*)(-1);

    //prepare address
    //get ip address and port
    
    FILE *pFile = NULL;
    char szIp[64] = {0};
    int  iPort = 0;

    pFile = fopen(UDPLISTENER_FILENAME, "rb");
    if (pFile) {
        fscanf(pFile, "%[^:]%*c%d", szIp, &iPort);
        fclose(pFile);
        pFile = NULL;
    }

    struct hostent *phostinfo = gethostbyname("");
    struct sockaddr_in srvaddr = {0};
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_addr.s_addr = inet_addr(szIp);
    //srvaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //srvaddr.sin_addr = *(struct in_addr *)phostinfo->h_addr_list[0];
    srvaddr.sin_port = htons(iPort);

    ret = bind(sockfd, (struct sockaddr*)&srvaddr, sizeof(srvaddr));
    if (ret == -1) 
        return (void*)(-1);

    char buf[8192] = {0};

    while(1) {
        struct sockaddr_in from;
        int len = sizeof(from);
        ret = recvfrom(sockfd, buf, sizeof(buf), 0,
            (struct sockaddr*)&from, &len);

        //send message
        if (ret > 0) {
            sprintf(szLogout, "recv data, len = %d, data: ", ret);
            for (int i = 0; i < ret; i++)
                sprintf(szLogout, "%s0x%02x ", szLogout, buf[i]);
            
            logout(LOG_ALARM, szLogout, __FILE__, __LINE__);
            pTemp->handleCmd(buf, ret);
        }
    }

#ifdef _WIN32
    closesocket(sockfd);
    WSACleanup();
#else
    close(sockfd);
#endif

    return NULL;
}

int CEhomeUdpListener::handleCmd(const char* p_data, int iLen)
{
    CNbEhomePtlHandler ptlHandler;
    CMD_BODY cmdbody = {0};
    int iHandleDataLen = 0;

    while (1) {
        if (ptlHandler.getCmdBodyFromInput(p_data, iLen, &cmdbody))
            return -1;

        int iVal = 0;
        int iDataStartPos = cmdbody.iDataStartPos;
        for (int i = 0; i < cmdbody.iDataLen; i++)
            iVal += (unsigned char)p_data[i + iDataStartPos] << (8 * i);

        if (NEWBEST_CMD_ADDR_PUSHOUT == cmdbody.iCmdId)
            SendMessage(m_hWnd, WM_CLIETN_READBACK, cmdbody.iAddrId, iVal);  

        iHandleDataLen = cmdbody.iDataStartPos + cmdbody.iDataLen + 2;
        p_data += iHandleDataLen;
        iLen -= iHandleDataLen;
    }

    return 0;
}