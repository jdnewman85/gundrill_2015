#include "io.h"
#include "constant.h"

#ifdef __arm__

#include "pifacedigital.h"
int piFaceHW_Addr = 0;

void openIO() {
	pifacedigital_open(piFaceHW_Addr);
}

void setDriveOnOff(int state) {
	pifacedigital_write_bit(1, 0, OUTPUT, piFaceHW_Addr);
}

void setOutput(int output, int state) {
	pifacedigital_write_bit(state, output, OUTPUT, piFaceHW_addr);
}

#else //__arm__
#include <stdio.h>

void openIO() {
	printf("openIO()\n");
}

void setDriveOnOff(int state) {
	printf("setDriveOnOff(%d)\n", state);
}

void setOutput(int output, int state) {
	printf("setOutput(%d, %d)\n", output, state);
}
#endif
