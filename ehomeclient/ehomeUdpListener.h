#ifndef __EHOMEUDPLISTENER_H__
#define __EHOMEUDPLISTENER_H__

#include "platcomm.h"

#define UDPLISTENER_FILENAME  "cltlistener.ini"

class CEhomeUdpListener
{
private:
    HWND m_hWnd;

private:
    static void* udpListenFun(void* arg);

    int handleCmd(const char* p_data, int iLen);

public:
    void setWinHandle(HWND hWnd);

    void startListenThread(void);

};

#endif  //__EHOMEUDPLISTENER_H__