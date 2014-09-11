#ifndef IO_H
#define IO_H

void openIO();
void setDriveOnOff(int state);
void setOutput(int output, int state);
int readInput(int input);

#endif //IO_H
