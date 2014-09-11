#include <gtk/gtk.h>
#include "SimpleMotion/simplemotion.h"
#include "SimpleMotion/simplemotion_private.h"
#include "SimpleMotion/vsd_cmd.h"
#include "aux.h"
#include "axis.h"
#include "main.h"

smint16 previousDrivePosition;
smint16 diff;

void sigIntHandler() {
	gtk_main_quit();
}

void updatePosition() {
	smint32 returnValue;

	//TODO Status update here?
	AxisStatus = smGetParam(AxisName, "ReturnDataPayload", &returnValue);
	diff = returnValue-previousDrivePosition;
	Position += diff;
	previousDrivePosition = returnValue;
}

float withOverride(float value, float override, float min, float max) {
	float temp;
	temp = value*(override/100.0);
	if(temp > max) {
		temp = max;
	}
	if(temp < min) {
		temp = min;
	}
	return temp;
}

