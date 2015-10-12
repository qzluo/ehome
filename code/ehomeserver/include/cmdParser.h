/********************************************************************
 File: cmdParser.h
 Function: command parser to generate output from input
 Author: Luobihe
 Date: 2015-7-30
*********************************************************************/

#ifndef __CMDPARSER_H__
#define __CMDPARSER_H__

//interface to parse command from client
class CCmdParser
{
public:
    virtual int parseCmd(const char* szIn, int iInLen, char* szOut, int* p_iRetlen) = 0;

    //split to get first cmd from message read
    virtual int getFirstCmd(const char* szIn, int iInLen, 
        int* p_iStartPos, int* p_iCmdLen) = 0;
};

#endif  //__CMDPARSER_H__