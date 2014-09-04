#include "SimpleMotion_lib/simplemotion.h"
#include "SimpleMotion_lib/simplemotion_private.h"
#include "SimpleMotion_lib/vsd_cmd.h"
#include "time.h"
#include "stdio.h"

int waitForMoveDone(char *axis) {
	smint32 status;
	SM_STATUS cmdstat;
	do {
		//cmdstat = smGetParam(axis, "SimpleStatus", &status);
		//printf("status=%d, cmdstat=%d\n", status, cmdstat);
		cmdstat = smGetParam(axis, "ReturnDataPayload", &status);
		printf("RETURN PAY LOAD: %d\n", status);
	} while(status==0 && cmdstat==SM_OK);

	if(status == 2) {
		return smtrue;
	} else {
		return smfalse;
	}

}

main()
{
	char* device = "TTL232R";
	SM_STATUS ok;
	smint32 mahControlMode;

	//test communication
	printf("Test\n");

	 ok=smCommand(device, "TESTCOMMUNICATION", 0);

	 //print received status to screen
	 //
	 switch(ok) { 
		 case SM_OK:
			printf("Testcommunication SM_OK\n");
			break;
		 case SM_ERR_NODEVICE:
			printf("Testcommunication SM_ERR_NODEVICE\n");
			break;
		 case SM_ERR_BUS:
			printf("Testcommunication SM_ERR_BUS\n");
			break;
		 case SM_ERR_COMMUNICATION:
			printf("Testcommunication SM_ERR_COMMUNICATION\n");
			break;
		 case SM_ERR_PARAMETER:
			printf("Testcommunication SM_ERR_PARAMETER\n");
			break;
		 default:
			printf("Testcommunication Unknown result\n");
			break;
	  }

	smSetParam(device, "ReturnDataPayloadType", 6);

	ok = smGetParam(device, "CurrentLimitCont", &mahControlMode);
	printf("%d\n", mahControlMode);

	smSetParam(device, "VelocityLimit", 1366);

	printf("Move1\n");
	ok = smCommand(device, "ABSTARGET", 4096/5*500);
	sleep(1);
	smSetParam(device, "VelocityLimit", 100);
	sleep(1);
	smSetParam(device, "VelocityLimit", 200);
	sleep(1);
	smSetParam(device, "VelocityLimit", 300);
	sleep(1);
	smSetParam(device, "VelocityLimit", 200);
	sleep(1);
	smSetParam(device, "VelocityLimit", 100);
	sleep(1);
	smSetParam(device, "VelocityLimit", 500);
	waitForMoveDone(device);
		sleep(5000);
		mahControlMode = smGetParam(device, "ReturnDataPayload", &mahControlMode);
		printf("RETURN PAY LOAD: %d\n", mahControlMode);
	
	smSetParam(device, "VelocityLimit", 1000);
	ok = smCommand(device, "ABSTARGET", -4096/5);
	waitForMoveDone(device);
		sleep(5000);
		mahControlMode = smGetParam(device, "ReturnDataPayload", &mahControlMode);
		printf("RETURN PAY LOAD: %d\n", mahControlMode);

	printf("End\n");
	
	return 0;
}
