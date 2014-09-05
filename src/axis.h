#ifndef AXIS_H
#define AXIS_H

#include "SimpleMotion/simplemotion.h"
#include "SimpleMotion/vsd_cmd.h"

//Globals
extern char* axis;
extern SM_STATUS status;
extern smint32 returnValue;

int waitForMoveDone(char *axis);
const char* statusString(SM_STATUS status);

#endif //AXIS_H
