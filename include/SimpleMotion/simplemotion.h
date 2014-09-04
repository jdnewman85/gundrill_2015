//Global SimpleMotion functions & definitions
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

#ifndef SIMPLEMOTION_H
#define SIMPLEMOTION_H

#ifdef WIN32
//dll specs
#ifdef BUILD_DLL
    #define LIB __declspec(dllexport)
#else
//    #define LIB __declspec(dllimport)
#define LIB
#endif
#else
#define LIB
#endif

#ifdef linux
#define stricmp strcasecmp
#define strnicmp strncasecmp
#endif

#define byte char

#ifdef __cplusplus
extern "C"{
#endif

///////////////////////////////////////////////////////////////////////////////////////
//TYPES & VALUES //////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//declare SM lib types
typedef unsigned long smuint32;
typedef unsigned short smuint16;
typedef unsigned char smuint8;
typedef long smint32;
typedef short smint16;
typedef char smint8;
typedef char smbool;
#define smtrue 1
#define smfalse 0
typedef int SM_STATUS;
//return values (SM_STATUS type)
#define SM_OK 0
#define SM_ERR_NODEVICE 1
#define SM_ERR_BUS 2
#define SM_ERR_COMMUNICATION 4
#define SM_ERR_PARAMETER 8

///////////////////////////////////////////////////////////////////////////////////////
//FUNCTIONS////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

/** Close connection to all connected devices. This frees communication links therefore makes them available for other apps for opening.
  -return value: a SM_STATUS value, i.e. SM_OK if command succeed
*/
LIB SM_STATUS smCloseDevices();

/** Run command on axis. Parameters:
  -axisname: name of connected device, i.e. "AXIS_1" or "TTL232R"
  -commandname: name of command to be executed, i.e. "ABSTARGET", "CLEARFAULTS" or "HOMING"
  -param: parameter for given gommand, i.e. target position
  -return value: a SM_STATUS value, i.e. SM_OK if command succeed

  Possible commands and their parameters:
    command: "ABSTARGET" sets drive target reference value which represents either position, velocity or torque depending on drive control mode, parameter: interpreted as signed 32 bit integer representing absolute target reference value
    command: "INCTARGET" increments/decrements drive target reference value which represents either position, velocity or torque depending on drive control mode, parameter: interpreted as signed 16 bit integer representing target value icrement
    command: "CLEARFAULTS" clears all clearable drive faults and resumes drive operation, parameter: 0 (no meaning)
    command: "HOMING" starts homing sequence defined by GDtool parameters, parameter: 0=abort homing, 1=start homing
    command: "TESTCOMMUNICATION" tests communication link with no-operation (NOP) command, parameter: 0 (no meaning)
  */
LIB SM_STATUS smCommand( const char *axisname, const char *commandname, smint32 param );

/** Set axis parameter. Parameters:
  -axisname: name of connected device, i.e. "AXIS_1" or "TTL232R"
  -paramname: name of parameter to be modified, i.e. "VELOCITY_LIMIT", "CURRENTLIMIT_PEAK" or "WATCHDOG_TIMEOUT"
  -value: new value for given parameter
  -return value: a SM_STATUS value, i.e. SM_OK if command succeed
  */
LIB SM_STATUS smSetParam( const char *axisname, const char *paramname, smint32 value );

/** Get axis parameter or variable. Parameters:
  -axisname: name of connected device, i.e. "AXIS_1" or "TTL232R"
  -paramname: name of parameter to be read, i.e. "STATUS_BITS", "FAULT_BITS" or "CURRENTLIMIT_PEAK"
  -value: pointer to smint32 where read value is stored
  -return value: a SM_STATUS value, i.e. SM_OK if command succeed
  */
LIB SM_STATUS smGetParam( const char *axisname, const char *paramname, smint32 *value );

/** Send raw SPI command to device. Don't use unless you know what you're doing. */
LIB SM_STATUS smRawCommand( const char *axisname, smuint8 command_id, smuint16 param, smuint32 *returndata );

/** Return SM lib version number in hexadecimal format.
Ie V 0.5.1 would be 0x000501 and 1.2.33 0x010233 */
LIB smuint32 smGetVersion();


#ifdef __cplusplus
}
#endif
#endif // SIMPLEMOTION_H
