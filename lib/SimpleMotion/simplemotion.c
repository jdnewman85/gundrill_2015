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

#include <stdio.h>
#include <string.h>
#include "vsd_cmd.h"
#include "vsd_drive_bits.h"


#include "simplemotion_private.h"

#define HANDLE_STAT(stat) if(stat!=SM_OK)return (stat);
#define HANDLE_STAT_AND_RET(stat,returndata) { if(returndata==RET_INVALID_CMD||returndata==RET_INVALID_PARAM) return SM_ERR_PARAMETER; if(stat!=SM_OK) return (stat); }

int smNumDevices=0;//number of physical devices connected to PC (TTL232R adapters or USB7AXes etc)
smbool quiet=smtrue, ignorecrc=smfalse;
SM_DEVICE smDevice[SM_MAX_DEVICES];





unsigned long smGetVersion()
{
    return SM_VERSION;
}


//find device handle and place pointer to dev, if not opened, try to open
//dev=NULL if not found
SM_STATUS smGetDevice( SM_DEVICE **dev, const char *axisname )
{
    int i;
    char devname[128];

    //find if open
    for(i=0;i<smNumDevices;i++)
    {
	if(!strncmp(axisname, smDevice[i].axisName, SM_AXISNAMELEN ))
	{
	    *dev=&smDevice[i];
	    return SM_OK;
	}
    }

    if(smNumDevices>=SM_MAX_DEVICES)
    {
	*dev=NULL;
	return SM_ERR_NODEVICE;
    }

    i=smNumDevices;

    //try to open usb device
    strncpy(devname,axisname,SM_AXISNAMELEN-1);
    devname[SM_AXISNAMELEN-1]=0;//force terminate because strncpy does not in all cases


    smDevice[i].ftStatus = FT_OpenEx( devname, FT_OPEN_BY_DESCRIPTION, &smDevice[i].ftHandle );
    if ( smDevice[i].ftStatus == FT_OK )
    {
    }
    else
    {
	dev=NULL;
	return SM_ERR_NODEVICE;
    }

    FT_ResetDevice(smDevice[i].ftHandle);
    FT_SetBitMode( smDevice[i].ftHandle, BV( USB_DOUT_PIN ) | BV( USB_CLK_PIN ), USB_MODE );
    FT_SetBreakOff(smDevice[i].ftHandle);
    FT_SetRts(smDevice[i].ftHandle);
    FT_SetDtr(smDevice[i].ftHandle);
    FT_SetDataCharacteristics(smDevice[i].ftHandle,FT_BITS_8,FT_STOP_BITS_1, FT_PARITY_NONE);

    FT_SetFlowControl(smDevice[i].ftHandle,FT_FLOW_NONE,0 ,0);
    FT_SetTimeouts( smDevice[i].ftHandle, 1000, 1000 );	// 1 second read timeout
    FT_SetLatencyTimer( smDevice[i].ftHandle, 4 );
    smDevice[i].ftStatus = FT_SetBaudRate( smDevice[i].ftHandle, 10000 );
    if ( smDevice[i].ftStatus != FT_OK )
    {
	//emit statusFromDevice(tr("Unable to set USB interface. Retrying..."),0);
	FT_Close(smDevice[i].ftHandle);
	dev=NULL;
	return SM_ERR_BUS;
    }

    //init SPI VSD comm
    DWORD bytesSend;
    unsigned char data[1] = {0};
    //if (configMode == smfalse)
    if(0)
    {
	// To enter BL mode set STEP active (CLK pin)
	data[0] =  BV( USB_DOUT_PIN );
    }else
    {
	data[0] = BV( USB_DOUT_PIN )|BV( USB_CLK_PIN );//spi mode
    }

    FT_Write( smDevice[i].ftHandle, data, 1, &bytesSend );
    FT_Read( smDevice[i].ftHandle, data, 1, &bytesSend );

    //update table
    smNumDevices++;
    strcpy(smDevice[i].axisName,devname);
    smDevice[i].lastError=5550;
    smDevice[i].currentReturnDataType=0;//none
    *dev=&smDevice[i];

    //send NOP commands to flush incorrect return data out (2 first SPI commands)
    SM_STATUS stat;
    smuint32 retdata;
    ignorecrc=smtrue;
    smRawCmd( axisname, CMD_NOP, 0, &retdata);//no guarantee on return data correctness
    smRawCmd( axisname, CMD_NOP, 0, &retdata);//no guarantee on return data correctness
    ignorecrc=smfalse;
    stat=smRawCmd( axisname, CMD_NOP, 0, &retdata);//should have valid return data now and after this

    return SM_OK;
}


SM_STATUS smSPIword( SM_DEVICE *dev, smuint16 cmd, smuint16 *retdata )
{
	int i;

	unsigned char sendBuf[ USB_PACKET_LEN ];
	unsigned char recvBuf[ USB_PACKET_LEN ];
	unsigned int in32 = 0;

	unsigned int cmd32 = cmd;
	DWORD bytesSend = 0;
	DWORD bytesRead = 0;

	for ( i = 0; i < USB_PACKET_LEN; ++i )
	{
		sendBuf[ i ] = 0;
		recvBuf[ i ] = 0;
	}

	//form send packet
	for( i = 0; i < 16; ++i )
	{
		//datat asettuu nousevalla reunalla
		//molemmat samplettaa laskevalla reunalla
		if ( ( cmd32 >> i )& 0x1 )
		{
			sendBuf[ 2*i ] = BV( USB_DOUT_PIN ) | BV( USB_CLK_PIN ); // dout 1, clk 1
			sendBuf[ 2*i + 1 ] = BV( USB_DOUT_PIN ); // dout 1, clk 0
		}
		else
		{

			sendBuf[ 2*i ] = BV( USB_CLK_PIN ); // dout 0, clk 1
			sendBuf[ 2*i + 1 ] = 0; // dout 0, clk 0
		}
	}

	//transmit && recv
	dev->ftStatus= FT_Write( dev->ftHandle, sendBuf, USB_PACKET_LEN, &bytesSend );
	if ( dev->ftStatus != FT_OK )
	{
	    return SM_ERR_BUS;
	}
	dev->ftStatus = FT_Read( dev->ftHandle, recvBuf, USB_PACKET_LEN, &bytesRead );
	if ( dev->ftStatus != FT_OK )
	{
	    return SM_ERR_BUS;
	}

	//parse results
	for ( i = 0; i < 16; ++i )
	{
		in32 >>= 1;
		if( !( recvBuf[ 2*i + 2 ] & BV( USB_DIN_PIN ) ) )
		{
			in32 |= 0x80000000;
		}
	}
	*retdata=(in32>>16);

	return SM_OK;
}

//Transmit and receive 16bits trhu SPI
//
//Example pseudo code of smSPIword for porting SimpleMotion to other hardware (such as microcontroller).
//Also smGetDevice should be ported (remove USB code).
//
//The following macros interface digital I/O of programmable device (parallel port, MCU, PLC etc):
//SCLK0 and SCLK1 set output pin status (0V and 3.3-5V). This output is connected to CMD connector SCLK pin.
//MOSI0 and MOSI1 set output pin status (0V and 3.3-5V). This output is connected to CMD connector MOSI pin.
//MISO reads input pin status (returns 0 for 0V and 1 for 3.3-5V). This input is connected to CMD connector MISO pin.
//Additionally CMD connector IO_COM must be connected to SPI master ground and IO_VCC to SPI master 3.3-5V.
//Total wires 5
/*SM_STATUS smSPIword( smuint16 d, smuint16 *retdata )
{
        int i;
        smuint16 in=0;

        for( i=0; i<16; i++ )
        {
                SCLK1; //set clock to 1
                if( (d>>i)&1 )
                        MOSI1; //set data output to 1
                else
                        MOSI0; //set data output to 0

                delay(6);//6 microseconds minimum

                SCLK0; //set clock to 0
                in>>=1;
                if( !MISO() ) //sample data input
                {
                        in|=0x8000;
                }
                delay(6);//6 microseconds minimum
        }

        *retdata = in;

        return SM_OK;
}*/



//calculate CRC8
#define CRC_INIT 51
smuint8 calcCrc8( smuint8 cmd, smuint16 data )
{
	smuint8 crc;
	smuint8 testbyte;

	testbyte=(byte)cmd;
	crc=CRC8table[CRC_INIT^testbyte];

	testbyte=(byte)data;
	crc=CRC8table[(crc^testbyte)];

	testbyte=(byte)(data>>8);
	crc=CRC8table[(crc^testbyte)];

	return crc;
}


//transmit and receive CMD
SM_STATUS smDriveCmd( SM_DEVICE *dev, smuint16 cmd, smuint16 data, smuint32 *retdata )
{
	smuint8 cksum;
	SM_STATUS stat;

	//calculate CRC8
	cksum=calcCrc8(cmd,data);

	smuint16 in1,in2;

	stat=smSPIword(dev, cksum+((data&0xff)<<8), &in1);
	if(stat!=SM_OK) return stat;
	stat=smSPIword(dev, (data>>8)+(cmd<<8), &in2);
	if(stat!=SM_OK) return stat;

	if( !quiet)
	{
	    printf("%02x,%04x,%02x ", cmd, data,cksum );
	    printf("%03d,%05d ", cmd, data );
	}
	*retdata=in1+(in2<<16);

	return SM_OK;
}


SM_STATUS smRawCmd( const char *axisname, smuint8 cmd, smuint16 val, smuint32 *retdata )
{
	smuint32 ret;
	SM_STATUS stat;
	SM_DEVICE *dev;

	stat=smGetDevice(&dev,axisname);
	HANDLE_STAT(stat);
	stat=smDriveCmd(dev, cmd, val, &ret);
	HANDLE_STAT(stat);

	unsigned char rstat,rcrc, cksum;
	unsigned short rdata;

	//parse
	rstat=ret>>24;
	rdata=((ret>>8)&0x0000ffff);
	rcrc=ret&0xff;

	if( !quiet)
	{
	    printf(" %08X\n",(int)ret);fflush(stdout);
	}

	if(retdata!=NULL)
		*retdata=ret;

	//check
	cksum=calcCrc8(rstat,rdata);
	if(ret==RET_INVALID_PARAM)
	{
	    if( !quiet) printf("Invalid param reported by drive");
	    return SM_ERR_PARAMETER;
	}
        else if(cksum!=rcrc && ignorecrc==smfalse)
	{
	    if( !quiet) printf("CRC error (correct %02x) ",cksum);
	    return SM_ERR_COMMUNICATION;
	}


	return SM_OK;
}


//send two NOPs to get possible error
SM_STATUS smCheckDriveError(const char *axisname)
{
    SM_STATUS stat;
    smuint32 retdata;

    stat=smRawCmd( axisname, CMD_NOP, 0, &retdata);
    HANDLE_STAT_AND_RET(stat,retdata);
    stat=smRawCmd( axisname, CMD_NOP, 0, &retdata);
    HANDLE_STAT_AND_RET(stat,retdata);
    return SM_OK;
}

SM_STATUS smRawCommand( const char *axisname, smuint8 command_id, smuint16 param, smuint32 *returndata )
{
    return smRawCmd(axisname,command_id,param,returndata);
}

SM_STATUS smCommand( const char *axisname, const char *commandname, smint32 param )
{
    SM_STATUS stat;
    smuint32 retdata;

    if(!stricmp(commandname,"ABSTARGET")) //param=target value signed 32 bits
    {
	stat=smRawCmd( axisname, CMD_UPLOAD_PARAM, param&0xffff, &retdata);
	HANDLE_STAT_AND_RET(stat,retdata);
	stat=smRawCmd( axisname, CMD_ABS32_TARGET_VALUE, param>>16, &retdata);
	HANDLE_STAT_AND_RET(stat,retdata);
	return smCheckDriveError(axisname);
    }
    if(!stricmp(commandname,"INCTARGET")) //param=target value signed 16 bits
    {
	//accepts 16 bit signed value only
	if(param<-32768) return SM_ERR_PARAMETER;
	if(param>32767) return SM_ERR_PARAMETER;

	stat=smRawCmd( axisname, CMD_INC_TARGET_VALUE, param, &retdata);
	HANDLE_STAT_AND_RET(stat,retdata);
	return smCheckDriveError(axisname);
    }
    else if(!stricmp(commandname,"HOMING"))//param=1=start homing, 0=stop homing
    {
	stat=smRawCmd( axisname, CMD_HOMING, param, &retdata);
	HANDLE_STAT_AND_RET(stat,retdata);
	return smCheckDriveError(axisname);
    }
    else if(!stricmp(commandname,"CLEARFAULTS"))//param=0
    {
	stat=smRawCmd( axisname, CMD_CLR_FAULTBITS, 0xffff, &retdata);
	HANDLE_STAT_AND_RET(stat,retdata);
	return smCheckDriveError(axisname);
    }
    else if(!stricmp(commandname,"ENABLE"))//params: disable=0, enable=1
    {
        //accepts 0 and 1  value only
        if(param<0) return SM_ERR_PARAMETER;
	if(param>1) return SM_ERR_PARAMETER;

	stat=smRawCmd( axisname, CMD_SET_ENABLE_STATUS, param+1, &retdata);
        HANDLE_STAT_AND_RET(stat,retdata);
        return smCheckDriveError(axisname);
    }
    else if(!stricmp(commandname,"TESTCOMMUNICATION"))//param=0
    {
	return smCheckDriveError(axisname);
    }
    else
	return SM_ERR_PARAMETER; //no such command

    return SM_OK;
}

//return -1 if not found
int smSearchParameter( const char *paramname )
{
    smbool done=smfalse;
    int i=0;
    while(done==smfalse)
    {
	if(!stricmp(paramname,smParams[i].paramName))
	{
	    return i;
	}

	i++;
	if(smParams[i].paramNum==0) done=smtrue; //last in table
    }
    return -1;
}

SM_STATUS smSetParam( const char *axisname, const char *paramname, smint32 value )
{
    SM_STATUS stat;
    smuint32 retdata;
    smuint16 paramnum=0;
    smbool specialhandling;
    int i=0;
    smint32 scaledvalue;

    //look in param table
    i=smSearchParameter(paramname);
    if(i<0) return SM_ERR_PARAMETER;//not found

    paramnum=smParams[i].paramNum;
    specialhandling=smParams[i].specialHandling;
    scaledvalue=value*smParams[i].scalerMultiplier/smParams[i].scalerDivider;//scale

    if(specialhandling==smtrue)
    {
	if(!stricmp(paramname,"SimpleStatus"))
	{
	    return SM_ERR_PARAMETER;//cannot set this param, read only
	}
	else
	    return SM_ERR_PARAMETER;//cannot set other special handling params, read onlys
    }
    else //std handling
    {
	//TODO: paramter range checking by downloading limit values from drive
	//if(scaledvalue < -32768 || scaledvalue > 32767 ) return SM_ERR_PARAM;//value out of 16 bit bounds

	stat=smRawCmd( axisname, CMD_UPLOAD_PARAM, scaledvalue, &retdata);
	HANDLE_STAT_AND_RET(stat,retdata);
	stat=smRawCmd( axisname, CMD_SET_PARAM, paramnum, &retdata);
	HANDLE_STAT_AND_RET(stat,retdata);
    }

    return smCheckDriveError(axisname);
}


SM_STATUS smGetParam( const char *axisname, const char *paramname, smint32 *value )
{
    SM_STATUS stat;
    SM_DEVICE *dev;
    smuint32 retdata;
    smuint16 paramnum=0;
    smbool specialhandling;
    int i=0;
    smint32 scaledvalue;

    *value=0;//returning 0 if param not found

    //get device
    stat=smGetDevice(&dev,axisname);
    if(stat!=SM_OK) return stat;

    //look in param table
    i=smSearchParameter(paramname);
    if(i<0)
    {
	if(!quiet) printf("Parameter '%s' doesn't exist\n", paramname);
	return SM_ERR_PARAMETER;//not found
    }

    paramnum=smParams[i].paramNum;
    specialhandling=smParams[i].specialHandling;

    if(specialhandling==smtrue)
    {
	smint32 par;

	if(!stricmp(paramname,"SimpleStatus"))
	{
	    //check if drive is stopped on fault and if idling
	    stat=smGetParam(axisname,"StatusBits",&par);
	    if(stat!=SM_OK)
	    {
		*value=1;//some fault
		return stat;
	    }
	    //handle result
	    if(par&STAT_FAULTSTOP) *value=1; //faulted
	    else if ( !(par&STAT_SERVO_READY) || !(par&STAT_TARGET_REACHED)) *value=0; //busy
	    else *value=2; //ready & operational
	}
	else if(!stricmp(paramname,"FollowingError"))
	{
	    if(dev->currentReturnDataType != CAPTURE_FOLLOW_ERROR )  //if not currently selected, set it
	    {
		smSetParam(axisname,"ReturnDataPayloadType",CAPTURE_FOLLOW_ERROR);
		dev->currentReturnDataType=CAPTURE_FOLLOW_ERROR;
	    }
	    //send nops to get return data with wanted payload
	    stat=smRawCmd( axisname, CMD_NOP, 0, &retdata);
	    HANDLE_STAT_AND_RET(stat,retdata);
	    stat=smRawCmd( axisname, CMD_NOP, 0, &retdata); //this returns actual return value due to SPI delay
	    HANDLE_STAT_AND_RET(stat,retdata);
	    //scale back
	    scaledvalue= (smint32) ((smint16)((retdata>>8)&0xffff));
	    scaledvalue=scaledvalue*smParams[i].scalerDivider/smParams[i].scalerMultiplier;//scale
	    *value= scaledvalue;
	}
	else if(!stricmp(paramname,"ActualTorque"))
	{
	    if(dev->currentReturnDataType != CAPTURE_TORQUE_ACTUAL )  //if not currently selected, set it
	    {
		smSetParam(axisname,"ReturnDataPayloadType",CAPTURE_TORQUE_ACTUAL);
		dev->currentReturnDataType=CAPTURE_TORQUE_ACTUAL;
	    }
	    //send nops to get return data with wanted payload
	    stat=smRawCmd( axisname, CMD_NOP, 0, &retdata);
	    HANDLE_STAT_AND_RET(stat,retdata);
	    stat=smRawCmd( axisname, CMD_NOP, 0, &retdata); //this returns actual return value due to SPI delay
	    HANDLE_STAT_AND_RET(stat,retdata);
	    //scale back
	    scaledvalue= (smint32) ((smint16)((retdata>>8)&0xffff));
	    scaledvalue=scaledvalue*smParams[i].scalerDivider/smParams[i].scalerMultiplier;//scale
	    *value= scaledvalue;
	}
	else if(!stricmp(paramname,"ReturnDataPayload"))
	{
	    //send nops to get return data with wanted payload
	    stat=smRawCmd( axisname, CMD_NOP, 0, &retdata); //value is delayed by 2 commands, not corrected here (by reading sample 3 times) to cause less overhead
	    HANDLE_STAT_AND_RET(stat,retdata);
	    //scale back
	    scaledvalue= (smint32) ((smint16)((retdata>>8)&0xffff));
	    scaledvalue=scaledvalue*smParams[i].scalerDivider/smParams[i].scalerMultiplier;//scale
	    *value= scaledvalue;
	}


	else
	    return SM_ERR_PARAMETER;//should not ever happen
    }
    else
    {
	//get param from device
	stat=smRawCmd( axisname, CMD_GET_PARAM, paramnum, &retdata);
	HANDLE_STAT_AND_RET(stat,retdata);
	stat=smRawCmd( axisname, CMD_NOP, 0, &retdata);
	HANDLE_STAT_AND_RET(stat,retdata);
	stat=smRawCmd( axisname, CMD_NOP, 0, &retdata); //this returns actual return value due to SPI delay
	HANDLE_STAT_AND_RET(stat,retdata);

	//scale back
	scaledvalue= (smint32) ((smint16)((retdata>>8)&0xffff));
	scaledvalue=scaledvalue*smParams[i].scalerDivider/smParams[i].scalerMultiplier;//scale
	*value= scaledvalue;
    }

    return SM_OK;
    //not needed because nops commanded above
    //return smCheckDriveError(axisname);
}

SM_STATUS smCloseDevices()
{
    int i;
    for(i=smNumDevices-1;i>=0;i--)//start from last one
    {
	if(!quiet) printf("closing dev=%d\n",i);
	FT_STATUS stat;
	stat=FT_Close(smDevice[i].ftHandle);
	if(stat!=FT_OK) return SM_ERR_BUS;
	smNumDevices--;
    }
    return SM_OK;
}

