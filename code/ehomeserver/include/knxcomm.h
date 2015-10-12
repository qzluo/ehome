/********************************************************************
 File: knxcomm.h
 Function: 处理 knx 网络通信的接口 
 Author: Luobihe
 Date: 2015-7-15
*********************************************************************/

/*-------------------------------------------------------------------
 Function: parseGroupAddr(szAddr, p_chFirst, p_chSecond)
 Purpose: 解析 knx 组地址，将形如 10/1/31 的组地址解析成2字节的数据，
          第一字节保存 10/1 信息，
             保存方式为： 0-2位保存1， 3-6位保存10；
          第二字节保存 31 信息。
 Parameters: szAddr -- [IN], 字符形式的组地址
             p_chFirst -- [OUT], 转化后的第一字节
             p_chSecond -- [OUT], 转化后的第二字节
 return: 0 --  成功
         -1 -- 失败
-------------------------------------------------------------------*/
int parseGroupAddr(const char* szAddr, char* p_chFirst, char* p_chSecond);

/*-------------------------------------------------------------------
 Function: generateGroupAddr(chFirst, chSecond, szAddr)
 Purpose: 由knx字节地址生成 knx 组地址，将2字节的数据
          生成形如 10/1/31 的组地址
          第一字节: 3-6位用于生成 10，0-2位用于生成1
          第二字节: 用于生成 31
 Parameters: chFirst -- [IN], 第一字节
             chSecond -- [IN], 第二字节
             szAddr -- [OUT], 转化后的组地址
 return: 0 --  成功
         -1 -- 失败
-------------------------------------------------------------------*/
int generateGroupAddr(unsigned char chFirst, unsigned char chSecond, char* szAddr);