//Internal functions & definitions, not for library user
//Copyright (c) Granite Devices Oy

/*
     This program is free software; you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation; version 2 of the License.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.
*/

#ifndef SIMPLEMOTION_PRIVATE_H
#define SIMPLEMOTION_PRIVATE_H

#include "simplemotion.h"
#include <ftd2xx.h>
//#include "ftdilib/ftd2xx.h"

#define SM_VERSION 0x000201
#define SM_MAX_DEVICES 100
#define SM_AXISNAMELEN 128
#define SM_PARAMNAMELEN 128

typedef struct SM_DEVICE_
{
    FT_HANDLE ftHandle;
    FT_STATUS ftStatus;

    smuint32 lastError;
    smuint16 statusBits, faultBits;
    smint32 currentPosition;
    char axisName[SM_AXISNAMELEN];
    smuint16 currentReturnDataType;//one of CAPTURE_ numbers in vsd_drive_bits.h
} SM_DEVICE;

typedef struct SM_PARAM_
{
    char paramName[SM_PARAMNAMELEN];
    smuint16 paramNum;
    smbool specialHandling;
    smint32 scalerMultiplier, scalerDivider;//parameter scaling. 1/1 if not scaled

} SM_PARAM;

//placed in sm_consts.cpp
extern const SM_PARAM smParams[];
extern const byte CRC8table[];

#define USB_MODE 0x4 //syncronous bit bang mode
#define USB_DOUT_PIN 0
#define USB_DIN_PIN 1
#define USB_CLK_PIN 3
#define USB_PACKET_LEN 33
#ifndef BV
#define BV( bit ) ( 1<<bit )
#endif

//find device handle and return it, if not opened, try to open
//return NULL if not found
SM_STATUS smGetDevice( SM_DEVICE **dev, const char *axisname );
SM_STATUS smRawCmd( const char *axisname, smuint8 cmd, smuint16 val, smuint32 *retdata );


#endif // SIMPLEMOTION_PRIVATE_H
