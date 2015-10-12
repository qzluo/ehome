/********************************************************************
 File: platcomm.cpp
 Function: 提供经常使用的函数的接口. 
 Author: Luobihe
 Date: 2015-7-9
*********************************************************************/

#include "platcomm.h"
#include <string.h>
#include <stdio.h>

#ifdef _WIN32

#include <winsock.h>
#pragma comment(lib, "ws2_32.lib")

#include <process.h>

#else

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#include <pthread.h>

#endif

void platSleep(int iSeconds)
{
#ifdef _WIN32
    Sleep(iSeconds * 1000);
#else
    sleep(iSeconds);
#endif
}

void platMSleep(int iMSecons)
{
#ifdef _WIN32
    Sleep(iMSecons);
#else
    usleep(iMSecons * 1000);
#endif
}

/*-------------------------------------------------------------------
 Function: getExePath(processDir, name, len)
 Purpose: get current process path
 Parameters: processDir -- [OUT], dir of current process
             processName -- [OUT], current process name
             len -- [IN], size of path
 return: 0 --  success
         -1 -- failed
-------------------------------------------------------------------*/
int getExePath(char* processDir, char* processName, int len)
{
    char* path_end = NULL;

#ifdef _WIN32

    if (GetModuleFileName(NULL, processDir, len) <= 0)
        return -1;

    path_end = strrchr(processDir, '\\'); 

#else    

    if (readlink("/proc/self/exe", processDir, len) <= 0)
        return -1;

    path_end = strrchr(processDir, '/');    
    
#endif    

    if (path_end == NULL)
        return -1;

    *path_end = '\0';
    path_end++;

    strcpy(processName, path_end);

    return 0;
}

/*-------------------------------------------------------------------
 Function: initSocket(void)
 Purpose: 初始化socket,windows平台下，使用socket前需调用
 Parameters:
 return: 0 --  成功
         -1 -- 失败
-------------------------------------------------------------------*/
int initSocket(void)
{
#ifdef _WIN32

    WORD wVersionRequested = 0;
    WSADATA wsaData = {0};    
    wVersionRequested = MAKEWORD(1, 1);
    if (WSAStartup(wVersionRequested, &wsaData))
        return -1;

    if ( LOBYTE( wsaData.wVersion ) != 1 ||
        HIBYTE( wsaData.wVersion ) != 1 ) {
            WSACleanup();
            return -1;
    }

#endif 

    return 0;
}

/*-------------------------------------------------------------------
 Function: deinitSocket(void)
 Purpose: 不再使用socket时，清理工作
 Parameters:
 return:
-------------------------------------------------------------------*/
void deinitSocket(void)
{
#ifdef _WIN32
    WSACleanup();
#endif
}

/*-------------------------------------------------------------------
 Function: sendUdpData(peerIPAddr, port, p_data, datalen)
 Purpose: send data to peer ip by udp mode
 Parameters: peerIPAddr -- [IN], ip addr to send
             port -- [IN], port 
             p_data -- [IN], data to send
             dataLen -- [IN], data length
 return: 0 -- success
         -1 -- failed
-------------------------------------------------------------------*/
int sendUdpData(const char* peerIPAddr, int port, 
                const char* p_data, int dataLen)
{
    int ret = 0;
        
    //1 socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd == -1) 
        return -1;

    int opt = 1;
    if (-1 == setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt)))
        return -1;

    //2 通信地址
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;//与socket一致
    addr.sin_port = htons(port);//改端口
    addr.sin_addr.s_addr = inet_addr(peerIPAddr);//改IP

    sendto(sockfd, p_data, dataLen, 0,
        (struct sockaddr*)&addr, sizeof(addr));   
    
#ifdef _WIN32
    closesocket(sockfd);
#else
    close(sockfd);
#endif
             
    return 0;
}

/*-------------------------------------------------------------------
 Function: broadCastUdpData(port, p_data, datalen)
 Purpose: broadcast udp data
 Parameters: port -- [IN], port 
             p_data -- [IN], data to send
             dataLen -- [IN], data length
 return: 0 -- success
         -1 -- failed
-------------------------------------------------------------------*/
int broadCastUdpData(int port, const char* p_data, int dataLen)
{
    int ret = 0;
        
    //1 socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd == -1) 
        return -1;

    //设置该套接字为广播类型
    int opt = 1;
    if (-1 == setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt)))
        return -1;

    //2 通信地址
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;//与socket一致
    addr.sin_port = htons(port);//改端口
    addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    sendto(sockfd, p_data, dataLen, 0,
        (struct sockaddr*)&addr, sizeof(addr));   
    
#ifdef _WIN32
    closesocket(sockfd);
#else
    close(sockfd);
#endif
             
    return 0;
}

/*-------------------------------------------------------------------
 Function: getLocalIp(szIp)
 Purpose: get eth0 ip address
 Parameters: szIp -- [OUT], ip address
 return: 0 --  success
         -1 -- failed
-------------------------------------------------------------------*/
int getLocalIp(char* szIp)
{
#ifdef _WIN32

    char hname[256] = {0};
    struct hostent *hent = NULL;
   
    if (gethostname(hname, sizeof(hname)))
        return -1;

    hent = gethostbyname(hname);
    if (!hent)
        return -1;

    strcpy(szIp, inet_ntoa(*(struct in_addr*)(hent->h_addr_list[0])));

#else

    int sock_get_ip = 0;

    struct sockaddr_in *sin;
    struct ifreq ifr_ip = {0};

    if ((sock_get_ip = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        return -1;

    memset(&ifr_ip, 0, sizeof(ifr_ip));
    strncpy(ifr_ip.ifr_name, "eth0", sizeof(ifr_ip.ifr_name) - 1);

    if( ioctl( sock_get_ip, SIOCGIFADDR, &ifr_ip) < 0 ) 
        return -1; 

    sin = (struct sockaddr_in *)&ifr_ip.ifr_addr; 
    strcpy(szIp, inet_ntoa(sin->sin_addr));

    close( sock_get_ip );

#endif

    return 0;
}

/*-------------------------------------------------------------------
 Function: initTcpServer(int* p_sfd)
 Purpose: 初始化开始tcp监听
 Parameters: szIpAddr -- [IN], ip
             iPort -- [IN], port
             p_sfd -- [OUT], 返回的socket
 return: 0 --  成功
         -1 -- 失败
-------------------------------------------------------------------*/
int initTcpServer(const char* szIpAddr, int iPort, int* p_sfd)
{
    int ret = 0;
    struct sockaddr_in srvaddr = {0};

    *p_sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (*p_sfd < 0)
        return -1;

    //prepare address    
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_addr.s_addr = inet_addr(szIpAddr);
    srvaddr.sin_port = htons(iPort);

    //bind
    ret = bind(*p_sfd, (struct sockaddr*)&srvaddr, sizeof(srvaddr));
    if (ret == -1) {
#ifdef _WIN32
        closesocket(*p_sfd);
#else
        close(*p_sfd);
#endif
        *p_sfd = -1;

        return -1;
    }

    //listen
    ret = listen(*p_sfd, TCP_LISTEN_NUM);
    if (ret == -1) {
#ifdef _WIN32
        closesocket(*p_sfd);
#else
        close(*p_sfd);
#endif
        *p_sfd = -1;

        return -1;
    }

    return 0;
}

/*-------------------------------------------------------------------
 Function: checkStrIsInteger(szSrc)
 Purpose: 检查字符串是否全是数字
 Parameters: szSrc -- [IN], 待检查的字符串
 return: 0 --  不是
         1 -- 是
-------------------------------------------------------------------*/
int checkStrIsInteger(const char* szSrc)
{
    int len = 0;
    int i = 0;

    if (!szSrc)
        return 0;

    len = strlen(szSrc);
    for (i = 0; i < len; i++) {
        if (szSrc[i] < '0' || szSrc[i] > '9')
            return 0;
    }

    return 1;
}

/*-------------------------------------------------------------------
 Function: calcByteSum(szIn, iInLen)
 Purpose: calculate sum of bytes
 Parameters: szIn -- [IN], data bytes
             iInLen -- [IN], length of bytes to calculate
 return: sum result
-------------------------------------------------------------------*/
unsigned short calcByteSum(const char* szIn, int iInLen)
{
    unsigned short res = 0;
    for (int i = 0; i < iInLen; i++) 
        res += (unsigned char)szIn[i];

    return res;
}

/*-------------------------------------------------------------------
 Function: getFileSize(p_filename)
 Purpose: get file size
 Parameters: p_filename -- [IN], file name
 return: -1 -- file not exist
         >=0 -- file size
-------------------------------------------------------------------*/
int getFileSize(const char* p_filename)
{
    int iRet = -1;
    FILE *pFile = NULL;

    pFile = fopen(p_filename, "rb");
    if (pFile) {
        fseek(pFile, 0, SEEK_END);
        iRet = ftell(pFile);

        fclose(pFile); 
    }

    return iRet;
}

/*-------------------------------------------------------------------
 Function: easyCreateThread(threadId, start_routine, arg)
 Purpose: create a thread
 Parameters: start_routine -- [IN], thread function
             arg -- [IN], argument delivered to thread function
 return: 0 --  success
         -1 -- failed
-------------------------------------------------------------------*/
int easyCreateThread(void* (* start_routine)(void*), void* arg)
{
#ifdef _WIN32

    HANDLE hThread = NULL;
    hThread = (HANDLE)_beginthreadex(NULL, 0, (unsigned int(__stdcall* )(void*))start_routine, arg, 0, NULL); 
    if (hThread == NULL)
        return -1;

    CloseHandle(hThread);

#else

    pthread_t thread_id = 0;
    if (pthread_create(&thread_id, NULL, start_routine, arg))
        return -1;

    if (pthread_detach(thread_id))
        return -1;

#endif

    return 0;
}


/////////////////////////////////////////////////////////////////////
//EasyMutex

/*-------------------------------------------------------------------
 Function: init(void)
 Purpose: init lock
 Parameters: 
 return: 0 --  success
         other -- failed
-------------------------------------------------------------------*/
int EasyMutex::init(void)
{
#ifdef _WIN32

    m_hMutex = CreateMutex(NULL, false, NULL);
    if (m_hMutex == NULL)
        return -1;
    else 
        return 0;

#else

    return pthread_mutex_init(&m_lock, 0);

#endif
}

/*-------------------------------------------------------------------
 Function: lock(void)
 Purpose: lock
 Parameters: 
 return: 0 --  success
         other -- failed
-------------------------------------------------------------------*/
int EasyMutex::lock(void)
{
#ifdef _WIN32    
    return WaitForSingleObject(m_hMutex, INFINITE);
#else
    return pthread_mutex_lock(&m_lock);
#endif
}

/*-------------------------------------------------------------------
 Function: unlock(void)
 Purpose: unlock
 Parameters: 
 return: 0 --  success
         other -- failed
-------------------------------------------------------------------*/
int EasyMutex::unlock(void)
{
#ifdef _WIN32

    if (ReleaseMutex(m_hMutex))
        return 0;
    else
        return -1;
#else

    return pthread_mutex_unlock(&m_lock);

#endif
}

void EasyMutex::destroy(void)
{
#ifdef _WIN32

    if (m_hMutex) {
        CloseHandle(m_hMutex);
        m_hMutex = NULL;
    }

#else

    pthread_mutex_destroy(&m_lock);

#endif
}


/////////////////////////////////////////////////////////////////////
//tcp client
//CEasyTcpClt

CEasyTcpClt::CEasyTcpClt(void)
{
    m_sfd = -1;
    m_pApp = NULL;
    m_bIsClosed = 1;
    m_hLock.init();
}

CEasyTcpClt::~CEasyTcpClt(void)
{
    clear();
    m_hLock.destroy();
}

void CEasyTcpClt::clear(void)
{
    setClosedState(1);

    if (m_sfd != -1) {
#ifdef _WIN32
        closesocket(m_sfd);
#else
        close(m_sfd);
#endif

        m_sfd = -1;
    }             
}

int CEasyTcpClt::isClosed(void)
{
    return getClosedState();
}

/*-------------------------------------------------------------------
 Function: open(void)
 Purpose: open
 Parameters: 
 return: 0 --  success
         -1 -- failed
-------------------------------------------------------------------*/
int CEasyTcpClt::open(void)
{
    if (m_sfd != -1)
        clear();
        
    m_sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sfd < 0) 
        return -1;    

    return 0;
}

/*-------------------------------------------------------------------
 Function: init(pParas)
 Purpose: connect server and set timeout
 Parameters: pParas -- [IN], include server ip, port and app interface
 return: 0 --  success
         -1 -- failed
-------------------------------------------------------------------*/
int CEasyTcpClt::init(void* pParas)
{    
    int ret = 0;
    PTCPCLTAPPPARAS pTemp = PTCPCLTAPPPARAS(pParas);

    if (m_sfd == -1)
        return -1;

    if (pTemp == NULL || pTemp->szAddr == NULL)
        return -1;

    //setup timeout    
    struct timeval timeout = {3, 0}; 

#ifdef _WIN32

    int nNetTimeout = 3000;
    setsockopt(m_sfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&nNetTimeout, sizeof(int));
    setsockopt(m_sfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(int));
    
#else

    setsockopt(m_sfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));
    setsockopt(m_sfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

#endif
    
    //prepare address
    struct sockaddr_in srvaddr = {0};
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_addr.s_addr = inet_addr(pTemp->szAddr);
    srvaddr.sin_port = htons(pTemp->iPort);

    //set up unblock mode

#ifdef _WIN32

    unsigned long ul = 1;
    ret = ioctlsocket(m_sfd, FIONBIO, (unsigned long*)&ul);
    if (ret == SOCKET_ERROR)
        return -1;
#else
    int flag = fcntl(m_sfd, F_GETFL, 0); 
    if (fcntl(m_sfd, F_SETFL, flag | O_NONBLOCK) == -1)
        return -1;

#endif

    connect(m_sfd, (struct sockaddr*)&srvaddr, sizeof(srvaddr));

    //select
    fd_set writeSet;

    FD_ZERO(&writeSet);
    FD_SET(m_sfd, &writeSet);
    timeout.tv_sec = 3; //connect timeout
    timeout.tv_usec = 0;
    ret = select(m_sfd + 1, 0, &writeSet, 0, &timeout);
    if ( ret <= 0 )
        return -1;

    //set back to block mode
#ifdef _WIN32

    ul = 0 ;
    ret = ioctlsocket(m_sfd, FIONBIO, (unsigned long*)&ul);
    if(ret == SOCKET_ERROR)
        return -1;

#else

    if (fcntl(m_sfd, F_SETFL, flag) == -1)
        return -1;

#endif

    //set app interface
    m_pApp = pTemp->p_app;

    setClosedState(0);

    if (easyCreateThread(tcpReadThreadFun, this) == -1)
        return -1;

    return 0;
}

int CEasyTcpClt::write(const char* pBuf, int len)
{
    return send(m_sfd, pBuf, len, 0);
}

int CEasyTcpClt::read(char* pBuf, int len)
{
    return recv(m_sfd, pBuf, len, 0);
}

void* CEasyTcpClt::tcpReadThreadFun(void* arg)
{
    CEasyTcpClt* pTemp = (CEasyTcpClt*)arg;
    int sfd = pTemp->m_sfd;

    fd_set fdread;                
    int ret = 0;   

    FD_ZERO(&fdread);
    FD_SET(sfd, &fdread);

    while (1) {
        if (pTemp->getClosedState())
            break;

        ret = select(sfd + 1, &fdread, NULL, NULL, NULL);      
        if (ret <= 0)  // Time expired
            continue;

        if (FD_ISSET(sfd, &fdread))  //处理读 
            pTemp->readHandle();    
    }

    return 0;
}

int CEasyTcpClt::readHandle(void)
{
    if (!m_pApp)
        return -1;

    return m_pApp->readHandle();
}

void CEasyTcpClt::setClosedState(int iState)
{
    m_hLock.lock();
    m_bIsClosed = iState;
    m_hLock.unlock();
}

int CEasyTcpClt::getClosedState(void)
{
    int iRet = 0;

    m_hLock.lock();
    iRet = m_bIsClosed;
    m_hLock.unlock();

    return iRet;
}


/////////////////////////////////////////////////////////////////////
// EasyMath

void EasyMath::shortToBytes(short val, char* p_bytes)
{
    p_bytes[0] = val & 0xFF;
    p_bytes[1] = (val >> 8) & 0xFF;
}

void EasyMath::intToBytes(int val, char* p_bytes)
{
    p_bytes[0] = val & 0xFF;
    p_bytes[1] = (val >> 8) & 0xFF;
    p_bytes[2] = (val >> (8 * 2)) & 0xFF;
    p_bytes[3] = (val >> (8 * 3)) & 0xFF;
}

void EasyMath::longToBytes(long val, char* p_bytes)
{
    p_bytes[0] = val & 0xFF;
    p_bytes[1] = (val >> 8) & 0xFF;
    p_bytes[2] = (val >> (8 * 2)) & 0xFF;
    p_bytes[3] = (val >> (8 * 3)) & 0xFF;
}
