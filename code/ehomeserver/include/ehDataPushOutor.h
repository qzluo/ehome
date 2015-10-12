/********************************************************************
 File: ehDataPushOutor.h
 Function: push out data to client connected 
 Author: Luobihe
 Date: 2015-8-26
*********************************************************************/

#ifndef __EHDATAPUSHOUTOR_H__
#define __EHDATAPUSHOUTOR_H__


#define FILENAME_UDPLISTENER  "udplistener.ini"

#define UDP_LISTENER_PORT 5678

int pushOut_init(void);

/*-------------------------------------------------------------------
 Function: pushOutDevData(iAddrId, p_data, iValType)
 Purpose: pack the output for getting right read data
 Parameters: iAddrId -- [IN], address id
             p_data -- [IN], data from device
             iValType -- [IN], date type
 return: 0 --  success
         -1 -- failed
-------------------------------------------------------------------*/
int pushOutDevData(int iAddrId, void* p_data, int iValType);

#endif  //__EHDATAPUSHOUTOR_H__