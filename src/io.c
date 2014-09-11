#include "io.h"

#ifdef __arm__

void openIO() {
	pifacedigital_open(piFaceHW_Addr);
}

void setDriveOnOff(int state) {
	pifacedigital_write_bit(1, 0, OUTPUT, piFaceHW_Addr);
}

#else //__arm__
#include <stdio.h>

void openIO() {
	printf("openIO()\n");
}

void setDriveOnOff(int state) {
	printf("setDriveOnOff(%d)\n", state);
}

#endif
