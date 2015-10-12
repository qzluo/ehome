/********************************************************************
 File: tcplistener.h
 Function: tcp 监听app消息
 Author: Luobihe
 Date: 2015-7-8
********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tcplistener.h"
#include "ehservercomm.h"
#include "EHSCmdParser.h"
#include "logsys.h"

#ifdef _WIN32

#include <Windows.h>
#include <winsock.h>

#else

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>

#endif


//监听线程函数  
static void* TcpListenThreadFun(void* pM)  
{  
    printf("Tcp server starting ...\n");

    //init tcp server socket
    FILE *pFile = NULL;
    char szFilePath[256] = {0};
    char szFileName[256] = {0};
    char szCfgFilePath[256] = {0};
    char szIp[64] = {0};
    int  iPort = 0;
    
    if (getExePath(szFilePath, szFileName, 256))
        return 0;

    sprintf(szCfgFilePath, "%s/%s", szFilePath, INIFILENAME);

    pFile = fopen(szCfgFilePath, "rb");
    if (pFile) {
        fscanf(pFile, "%[^:]%*c%d", szIp, &iPort);
        fclose(pFile);
        pFile = NULL;
    }

#ifndef _WIN32
    getLocalIp(szIp);
#endif

    CTcpSocket tcpServer;

    if (tcpServer.startListen(szIp, iPort) == -1) {
        logout(LOG_EMERGENCY, "Failed to start lisener thread", 
           __FILE__, __LINE__);
        
        return NULL;
    }

    tcpServer.clear();

    return 0;
}


/*-------------------------------------------------------------------
 Function: startTcpListen(void);
           启动tcp监听线程
 Parameters:
 Return: 0 -- success
         -1 -- failed
-------------------------------------------------------------------*/
int startTcpListen(void)
{
    return easyCreateThread(TcpListenThreadFun, NULL);
}

/////////////////////////////////////////////////////////////////////
//CTcpSocket

CTcpSocket::CTcpSocket(void)
{
    m_sfd = -1;
    m_pParser = NULL;

    m_MutexLock.init();;
}

CTcpSocket::~CTcpSocket(void)
{
    clear();

    m_MutexLock.destroy();
}

void CTcpSocket::clear(void)
{
    if (m_sfd) {
        removeAllClient();

#ifdef _WIN32
        closesocket(m_sfd);
#else
        close(m_sfd);
#endif

        m_sfd = -1;
    }

    if (m_pParser) {
        delete m_pParser;
        m_pParser = NULL;
    }
}

//启动监听
int CTcpSocket::startListen(const char* szIpAddr, int iPort)
{
    if (initTcpServer(szIpAddr, iPort, &m_sfd) == -1) {
        char szMessage[256] = {0};
        sprintf(szMessage, "Failed to initialize tcp server. ip = %s, port = %d\n",
            szIpAddr, iPort);

        logout(LOG_EMERGENCY, szMessage, __FILE__, __LINE__);
        return -1;
    }

    logout(LOG_INFO, "Start tcp listen success", __FILE__, __LINE__);

    if (m_pParser) {
        delete m_pParser;
        m_pParser = NULL;
    }

    m_pParser = new CEHSCmdParser();
    if (!m_pParser) {
        logout(LOG_EMERGENCY, "Initial parse failed", __FILE__, __LINE__);
        return -1;
    }

    printf("Start server success\n");

    if (easyCreateThread(CTcpSocket::clientHandle, this) == -1)
        return NULL;

    while(1) {
        int cfd = -1;
        struct sockaddr_in peeraddr = {0};
        int addrlen = sizeof(peeraddr);
#ifdef _WIN32
        cfd = accept(m_sfd, (struct sockaddr*)&peeraddr, &addrlen);
#else
        cfd = accept(m_sfd, (struct sockaddr*)&peeraddr, (socklen_t*)&addrlen);
#endif
        printf("Accepted client:%s:%d\n", 
            inet_ntoa(peeraddr.sin_addr), 
            ntohs(peeraddr.sin_port));

        addClient(cfd);
    }    

    return 0;
}

//客户端通信线程
void* CTcpSocket::clientHandle(void* arg)
{
    printf("Client handle thread\n");

    fd_set fdread;        
    CTcpSocket* p_tcpSock = (CTcpSocket*)arg;
    struct timeval tv = {1, 0};               
    int ret = 0;   
    char szMessage[BUFFER_SIZE] = {0}; 
    
    list <int> listTemp;
    list <int>::iterator pos;    

    while (1) {                 
        //initial read set
        p_tcpSock->copyClientList(&listTemp);

        FD_ZERO(&fdread);

        FD_SET(p_tcpSock->m_sfd, &fdread);
        int nfds = p_tcpSock->m_sfd;        
        for (pos = listTemp.begin(); pos != listTemp.end(); pos++) {
            FD_SET(*pos, &fdread); 
            if (*pos > nfds)
                nfds = *pos;
        }    

        tv.tv_sec = 1;
        tv.tv_usec = 0;
        ret = select(nfds + 1, &fdread, NULL, NULL, &tv); 

        if (ret == 0)  // Time expired
            continue;

        for (pos = listTemp.begin(); pos != listTemp.end(); pos++) {    
            int cfd = *pos;
            if (FD_ISSET(cfd, &fdread)) {
                ret = recv(cfd, szMessage, BUFFER_SIZE, 0);

#ifdef _WIN32

                if (ret == 0 || (ret == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET)) { // Client socket closed   
#else

                if (ret <= 0) {
                    if ((ret < 0) && (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))
                        continue;
#endif                         

                    printf("Client socket %d closed.\n", cfd);                
                    p_tcpSock->removeClient(cfd);
                }                       
                else {
                    //解析收到的命令并处理
                    char* p_wBuf = (char*)malloc(LARGE_BUFFER_SIZE);
                    if (!p_wBuf)
                        continue;

                    memset(p_wBuf, 0, LARGE_BUFFER_SIZE);
                    int iWLen = 0;

                    char szLogout[1024] = {0};
                    sprintf(szLogout, "recv cmd: ");
                    for (int i = 0; i < ret; i++)
                        sprintf(szLogout, "%s0x%x ", szLogout, szMessage[i]);

                    //logout(LOG_ALARM, szLogout, __FILE__, __LINE__);

                    p_tcpSock->m_pParser->parseCmd(szMessage, ret, p_wBuf, &iWLen);
                    
                    send(cfd, p_wBuf, iWLen, 0); 

                    /*
                    int iCmdLen = 0;
                    int iStartPos = 0;
                    char* p_in = szMessage;
                    //split message to a series of cmds
                    while (p_tcpSock->m_pParser->getFirstCmd(p_in, ret, &iStartPos, &iCmdLen) == 0) {
                        p_tcpSock->m_pParser->parseCmd(p_in + iStartPos, iCmdLen, p_wBuf, &iWLen);

                        send(cfd, p_wBuf, iWLen, 0);   

                        p_in += iStartPos + iCmdLen;
                        ret -= iStartPos + iCmdLen;
                    }
                    */
                    
                    free(p_wBuf);                    
                }
            }
        }
    }

    return NULL;
}

void CTcpSocket::addClient(int cfd)
{
    m_MutexLock.lock();
    m_listClient.push_back(cfd);
    m_MutexLock.unlock();
}

void CTcpSocket::copyClientList(list<int> * p_listDesc)
{
    p_listDesc->clear();

    m_MutexLock.lock();
    p_listDesc->assign(m_listClient.begin(), m_listClient.end());
    m_MutexLock.unlock();
}

void CTcpSocket::removeClient(int cfd)
{
    m_MutexLock.lock();

    list <int>::iterator pos = m_listClient.begin();
    list <int>::iterator last = m_listClient.end();

    while (pos != last) {
        if (*pos == cfd) {
#ifdef _WIN32
            closesocket(cfd); 
#else
            close(cfd);
#endif
            pos = m_listClient.erase(pos);
        }
        else
            pos++;        
    }

    m_MutexLock.unlock();
}

void CTcpSocket::removeAllClient(void)
{
    m_MutexLock.lock();

    list <int>::iterator pos = m_listClient.begin();
    list <int>::iterator last = m_listClient.end();

    while (pos != last) {
#ifdef _WIN32
        closesocket(*pos); 
#else
        close(*pos);
#endif
        pos = m_listClient.erase(pos);              
    }

    m_MutexLock.unlock();
}