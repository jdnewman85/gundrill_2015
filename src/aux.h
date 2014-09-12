#ifndef AUX_H
#define AUX_H

extern smint16 previousDrivePosition;
extern smint16 diff;

void sigIntHandler();
void updatePosition();
float withOverride(float value, float override, float min, float max);

#endif //AUX_H
