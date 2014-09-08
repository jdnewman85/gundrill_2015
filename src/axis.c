//Axis Routines

#include "SimpleMotion/simplemotion.h"
#include "SimpleMotion/simplemotion_private.h"
#include "SimpleMotion/vsd_cmd.h"
#include "axis.h"

char* AxisName = "TTL232R";
SM_STATUS AxisStatus;

int waitForMoveDone(char *axis) {
	smint32 status;
	SM_STATUS cmdstat;
	do {
		cmdstat = smGetParam(axis, "SimpleStatus", &status);
		//printf("status=%d, cmdstat=%d\n", status, cmdstat);
	} while(status==0 && cmdstat==SM_OK);

	if(status == 2) {
		return smtrue;
	} else {
		return smfalse;
	}

}

const char* statusString(SM_STATUS status) {
	switch(status) { 
		case SM_OK:
			return "OK";
		case SM_ERR_NODEVICE:
			return "ERR_NODEVICE";
		case SM_ERR_BUS:
			return "ERR_BUS";
		case SM_ERR_COMMUNICATION:
			return "ERR_COMMUNICATION";
		case SM_ERR_PARAMETER:
			return "ERR_PARAMETER";
	}
	
	//default
	return "UNKNOWN STATUS";
}

