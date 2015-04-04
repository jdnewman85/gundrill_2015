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
	pifacedigital_write_bit(state, output, OUTPUT, piFaceHW_Addr);
}

int readInput(int input) {
	int result, i;
	result = 1;
	for(i = 0; i < DEBOUNCE_LOOPS; i++) {
		result &= pifacedigital_read_bit(input, INPUT, piFaceHW_Addr); 
	}
	return result;
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

int readInput(int input) {
	printf("readInput(%d)\n", input);
	return 1;
}
#endif
