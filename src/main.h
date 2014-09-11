#ifndef MAIN_H
#define MAIN_H


extern int Position;
extern int Target;
extern int Feedrate;
extern int FeedrateOverride;
extern int Spindle;
extern int SpindleOverride;
extern char Status[];

extern int InputType;

extern float CntPerInch;
extern float VelPerRPM;
extern float VelPerIPM;
 
extern int State;
 
extern int JogFeedrate[];
 
extern int JogMode;
extern int JogDirection;

#endif //MAIN_H
