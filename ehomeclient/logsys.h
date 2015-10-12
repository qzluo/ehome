/********************************************************************
 File: logsys.h
 Function: 提供写日志的接口.   
    日志格式如下：
    [yyyy-mm-dd HH:MM:SS] [loglevel] [info]
 Author: Luobihe
 Date: 2015-7-3
*********************************************************************/

#ifndef __LOGSYS_H__
#define __LOGSYS_H__

#ifdef __cplusplus
extern "C"
{
#endif

//打印输出级别
#define LOG_EMERGENCY        0
#define LOG_ALARM            1
#define LOG_CRIT             2
#define LOG_ERR              3
#define LOG_WARNING          4
#define LOG_NOTICE           5
#define LOG_INFO             6
#define LOG_DEBUG            7

/*-------------------------------------------------------------------
 Function: loginit(filename, outlevel)
 Purpose: 初始化日志系统
 Parameters:
 return: 0 --  成功
         -1 -- 失败
-------------------------------------------------------------------*/
int loginit(const char* filename, int outlevel);


/*-------------------------------------------------------------------
 Function: logout(...)
 Purpose: 写日志信息到文件或服务器
 Parameters: level -- [IN], 打印日志的级别
             info -- [IN], 保存的信息
             filename -- [IN], 产生日志的文件名
             line -- [IN], 产生日志的行号          
 return: 0 --  成功
         -1 -- 失败
-------------------------------------------------------------------*/
int logout(int level, const char* info, 
           const char* filename, int line);


#ifdef __cplusplus
}
#endif

#endif  //__LOGSYS_H__