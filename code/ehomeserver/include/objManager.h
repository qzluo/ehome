/********************************************************************
 File: objManager.h
 Function: 对象管理
 Author: Luobihe
 Date: 2015-7-22
********************************************************************/

#ifndef __OBJMANAGER_H__
#define __OBJMANAGER_H__

/*-------------------------------------------------------------------
 Function: init_obj(void)
 Purpose: initial wort, including socket, data, device
 Parameters:
 return: 0 --  success
         -1 -- failed
-------------------------------------------------------------------*/
int init_obj(void);

/*-------------------------------------------------------------------
 Function: deinit_obj(void)
 Purpose: free
 Parameters:
 return: 0 --  success
         -1 -- failed
-------------------------------------------------------------------*/
int deinit_obj(void);

#endif  //__OBJMANAGER_H__