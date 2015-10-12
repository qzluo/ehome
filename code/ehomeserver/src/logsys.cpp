/********************************************************************
 File: logsys.cpp
 Function: 提供写日志的接口.     
 Author: Luobihe
 Date: 2015-7-3
*********************************************************************/

#define  _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <stdio.h>
#include <time.h>
#include "logsys.h"

static int s_iLogLevel = LOG_ERR;
static char s_szLogFileName[256] = {0};

const char* szLogLevel[] = {
    "Emergency",
    "Alarm",
    "Critical",
    "Error",
    "Warning",
    "Notice",
    "Infomation",
    "Debug"
};

/*-------------------------------------------------------------------
 Function: loginit(filename, outlevel)
 Purpose: 初始化日志系统
 Parameters: filename -- [IN], 日志保存的路径
             loglevel -- [IN], 日志输出的级别
 return: 0 --  成功
         -1 -- 失败
-------------------------------------------------------------------*/
int loginit(const char* filename, int loglevel)
{
    if (filename == NULL)
        return -1;

    strcpy(s_szLogFileName, filename);
    s_iLogLevel = loglevel;
    
    return 0;
}

/*-------------------------------------------------------------------
 Function: logout(...)
 Purpose: 写日志信息到文件或服务器
 Parameters: level -- [IN], 打印日志的级别
             info -- [IN], 保存的信息
             filename -- [IN], 产生日志的文件名
             line -- [IN], 产生日志的行号          
 return: 0  --  成功
         -1 --  失败
-------------------------------------------------------------------*/
int logout(int level, const char* info, 
           const char* filename, int line)
{
    FILE* pFile = NULL;
    char wBuf[10240] = {0};
    time_t t = 0;
    struct tm* ltm = NULL;
    char szFmtTime[64] = {0};

    if (level < 0)
        return -1;

    if (level > s_iLogLevel)
        return 0;
        
    pFile = fopen(s_szLogFileName, "a+");
    if (pFile) {
        //get time
        t = time(NULL);
        ltm = localtime(&t);
        sprintf(szFmtTime, "%4d-%02d-%02d %02d:%02d:%02d",
            ltm->tm_year + 1900, ltm->tm_mon + 1,
            ltm->tm_mday, ltm->tm_hour,
            ltm->tm_min, ltm->tm_sec);

        sprintf(wBuf, "[%s] [%s] [%s-%d: %s]\r\n",
            szFmtTime, szLogLevel[level], 
            filename, line, info);
        fwrite(wBuf, 1, strlen(wBuf), pFile);

        fclose(pFile);
        pFile = NULL;
    }

    return 0;
}