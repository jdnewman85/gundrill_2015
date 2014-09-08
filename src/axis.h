#ifndef AXIS_H
#define AXIS_H

#include "SimpleMotion/simplemotion.h"
#include "SimpleMotion/vsd_cmd.h"

//Globals
extern char* AxisName;
extern SM_STATUS AxisStatus;
extern smint32 returnValue;

int waitForMoveDone(char *axis);
const char* statusString(SM_STATUS status);

#endif //AXIS_H
