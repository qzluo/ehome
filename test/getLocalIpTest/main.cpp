/*
 * UdpBroadMain.c
 * UDP广播程序
*/

#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <winsock.h>
#include <CONIO.H>

#pragma comment(lib, "Ws2_32.lib")

#ifdef SERVER

int main( int argc, char *argv[] )
{
    WSADATA             wsaData;
    SOCKET              s;
    struct sockaddr_in  sin, sto;
    char				pBuf[1024];
	bool				fBroadcase = true;
	int					ncount = 0;

	// 检查WinSock版本，不能低于1.1
    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0)
    {
        printf("\r\nUnable to initialize WinSock for host info");
        exit(0);
    }

	// 创建客户端套接字
    s = socket( AF_INET, SOCK_DGRAM, 0 );		// UDP
    if( s < 0 )
    {
        printf("failed socket (%d)\n",WSAGetLastError());
        goto leave;
    }
	// 设置套接字为广播形式
	setsockopt( s, SOL_SOCKET, SO_BROADCAST, (char*)&fBroadcase, sizeof(bool) );


    sin.sin_family      = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);	// 本机IP地址
    sin.sin_port        = htons(0);				// 随机分配端口

	// 绑定套接字，不绑定，系统会自动为套接字指定本机IP地址和随机分配端口号
    if ( bind( s, (struct sockaddr*)&sin, sizeof(sin) ) != 0 )	
    {
        printf("failed bind socket (%d)\n",WSAGetLastError());
        goto leave;
    }	

	sto.sin_family      = AF_INET;
    sto.sin_addr.s_addr = htonl(INADDR_BROADCAST);	// 指定广播地址255.255.255.255
	//sto.sin_addr.s_addr = inet_addr("192.168.1.255");	// 局域网192.168.0.xxx内的广播
    sto.sin_port        = htons(7001);					// 发送到端口								

	while( !_kbhit() )		// 检查是否有键盘输入，无输入则继续执行，有输入则结束循环
	{
		sprintf( pBuf, "Message %d", ncount++);

		sendto( s, pBuf, lstrlen(pBuf), 0, (struct sockaddr*)&sto, sizeof(sockaddr_in) );					// 接收数据

		printf("Send To %s:%d : %s\n", inet_ntoa(sto.sin_addr), ntohs(sto.sin_port), pBuf );

		Sleep(1000);		// 等待1s，降低发送速度
	}

leave:
    if( s >= 0 )
        closesocket( s );

    WSACleanup();

	return 0;
}

#else

int main( int argc, char *argv[] )
{
    WSADATA             wsaData;
    SOCKET              s;
    struct sockaddr_in  sin, sfrom;
    char				pBuf[1024];
	bool				fBroadcase = true;
	int					nSize = 0, nbSize;

	// 检查WinSock版本，不能低于1.1
    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0)
    {
        printf("\r\nUnable to initialize WinSock for host info");
        exit(0);
    }

	// 创建客户端套接字
    s = socket( AF_INET, SOCK_DGRAM, 0 );		// UDP
    if( s < 0 )
    {
        printf("failed socket (%d)\n",WSAGetLastError());
        goto leave;
    }

    sin.sin_family      = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);	
   // sin.sin_addr.s_addr = inet_addr("192.168.157.1");
    sin.sin_port        = htons(1234);				

    if ( bind( s, (struct sockaddr*)&sin, sizeof(sin) ) != 0 )	// 绑定套接字
    {
        printf("failed bind socket (%d)\n",WSAGetLastError());
        goto leave;
    }	

	while( !_kbhit() )		// 检查是否有键盘输入，无输入则继续执行，有输入则结束循环
	{
		nSize = sizeof(sockaddr_in);
		nbSize = recvfrom( s, pBuf, sizeof(pBuf), 0, (struct sockaddr*)&sfrom, &nSize );
		if ( nbSize == SOCKET_ERROR )					// 接收数据
		{
			printf("Receive Error\n");
			break;
		}

		pBuf[nbSize] = '\0';
		printf("recv from %s:%d : %s\n", inet_ntoa( sfrom.sin_addr ), ntohs(sfrom.sin_port), pBuf);
	}

leave:
    if( s >= 0 )
        closesocket( s );

    WSACleanup();

	return 0;
}

#endif