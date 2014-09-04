#ifndef DRIVEBITS
#define DRIVEBITS

#ifndef BV
//bit select
#define BV(bit) (1<<(bit))
#define BVL(bit) (1L<<(bit))
#endif





///////////////////////////////////////////////////////////////////////////////////////
// FLAGS //////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//for CFG_DRIVE_FLAGS
#define FLAG_DISABLED_AT_STARTUP BV(0)
#define FLAG_NO_DCBUS_FAULT BV(1)
//for following error & PID, not commutation
//#define FLAG_CLOSED_LOOP_STEPPER BV(2)
#define FLAG_INVERT_ENCODER BV(3)
#define FLAG_DISABLE_DEADTIMECORR_HI_SPEED BV(4)
#define FLAG_DISABLE_DEADTIMECORR_LO_SPEED BV(5)
#define FLAG_USE_POSITIONLIMIT_RANGE_FAULT BV(6)
#define FLAG_USE_PULSE_IN_ACCEL_LIMIT BV(7)
//#define FLAG_ALLOW_FAULT_OUT_ON_EXT_DISABLE BV(8)
#define FLAG_2PHASE_AC_MOTOR BV(9)

//#define FLAG_USE_FIELD_WEAKENING BV(10)
#define FLAG_ALLOW_VOLTAGE_CLIPPING BV(10)

//the new FIR filter, fixed gain 1x, not in use in torque mode
#define FLAG_USE_INPUT_LP_FILTER BV(11)
//PIV is default
#define FLAG_USE_PID_CONTROLLER BV(12)
#define FLAG_INVERTED_HALLS BV(13)
#define FLAG_USE_HALLS BV(14)
//#define ILLEGAL_FLAGS 0xff00

//changing these flags will require reset
//#define FLAG_CHANGE_REQUIRES_RESET (FLAG_CLOSED_LOOP_STEPPER|FLAG_INVERT_ENCODER|FLAG_2PHASE_AC_MOTOR|FLAG_INVERTED_HALLS|FLAG_USE_HALLS)
#define FLAG_CHANGE_REQUIRES_RESET (FLAG_INVERT_ENCODER|FLAG_2PHASE_AC_MOTOR|FLAG_INVERTED_HALLS|FLAG_USE_HALLS)

//for RUNTIME_DRIVE_FLAGS
//sets accel and velocity limits to max
/*#define RUNTIMEFLAG_DISABLE_TRAJ_PLANNER BV(0)
//does nothing now
#define RUNTIMEFLAG_DISABLE_SCALING BV(1)
//does nothing now
#define RUNTIMEFLAG_DISABLE_INPUT_FILTER BV(2)
//disables scaling & vel/accel limits. Filterig works without additional gain
#define RUNTIMEFLAG_TORQUE_MODE BV(3)*/
//injects torque distbturbance in sequencer for stiffness testing purpose
//if 1, sequence will be added to raw torque command instead of target value
//#define RUNTIMEFLAG_INJECT_TEST_TORQUE BV(0)

///////////////////////////////////////////////////////////////////////////////////////
// FAULT BITS /////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
#define FLT_INVALIDCMD	BV(0)
#define FLT_FOLLOWERROR	BV(1)
#define FLT_OVERCURRENT BV(2)
#define FLT_COMMUNICATION BV(3)
#define FLT_ENCODER	BV(4)
#define FLT_OVERTEMP BV(5)
#define FLT_UNDERVOLTAGE BV(6)
#define FLT_OVERVOLTAGE BV(7)
#define FLT_PROGRAM BV(8)
#define FLT_HARDWARE BV(9)
#define FLT_MEM BV(10)
#define FLT_INIT BV(11)
#define FLT_MOTION BV(12)
//#define FLT_PHASESEARCH_OVERCURRENT BV(12)
#define FLT_RANGE BV(13)
//to make it necessary to clear faults to activate again
#define FLT_PSTAGE_FORCED_OFF BV(14)



///////////////////////////////////////////////////////////////////////////////////////
// STATUS BITS/////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
#define STAT_POWER_ON BV(0)
//this is 1 when trajectory planner target reached
#define STAT_TARGET_REACHED BV(1)
#define STAT_FERROR_RECOVERY BV(2)
//run is true only if motor is being actually driven. run=0 clears integrators etc
#define STAT_RUN BV(3)
#define STAT_ENABLED BV(4)
#define STAT_FAULTSTOP BV(5)
//follow error warning, recovering or disabled
#define STAT_FERROR_WARNING BV(6)
//get bit using CFG_STAT_USER_BIT_SOURCE
#define STAT_USER_BIT BV(7)
//ready for user command: initialized, running (no fault), not recovering, not homing & no homing aborted, not running sequence
#define STAT_SERVO_READY BV(8)

//writing flash or init for example
//#define STAT_BUSY BV(6)
//for example limit switch status or analog level triggered
//#define STAT_EXT_INPUT BV(7)
//writing 1 to this initiates config write
//////////////////#define STAT_WRITEFLASH BV(8)
//phasesearch complete
////////////////#define STAT_ROTORALIGNED BV(9)
//power stage disable. this must be cleared only by enablePowerStage
//#define STAT_POWERSTAGE_DISABLED BV(10)
//external disable interrupt occurred, cleared when PID loop sees ext disable release

//running sequence
#define STAT_RUN_SEQUENCE BV(9)

//for information only
//#define STAT_EXT_DISABLE_EVENT BV(10)
#define STAT_BRAKING BV(10)
//writing 1 to this initiates homing
#define STAT_HOMING BV(11)
#define STAT_INITIALIZED BV(12)
#define STAT_VOLTAGES_OK BV(13)
//this is 1 when opto out should indicate error, same as STAT_FAULTSTOP
//#define STAT_FAULT_INDICATOR_OUT BV(14)
//outputs disabled until reset
#define STAT_PERMANENT_STOP BV(15)



///////////////////////////////////////////////////////////////////////////////////////
// MOTOR TYPES ////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
#define MOTOR_NONE 0
//ac servo
#define MOTOR_AC_VECTOR 1
//trapezoidal ac
#define MOTOR_BLDC 2
//with regen resistor
#define MOTOR_DC 3
//without regen, parallel out
#define MOTOR_PARALLEL_DC 4
//open loop AC=stepper, no feedback
#define MOTOR_STEPPER 5
#define MOTOR_STEPPER_W_ENCODER 6
#define MOTOR_STEPPER_SERVO 7
#define _MOTOR_LAST 7




///////////////////////////////////////////////////////////////////////////////////////
// HOMING CONF BITS ///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//homing config bits. illegal to set Not to use INDEX or HOME
#define HOMING_POS_INDEX_POLARITY BV(0)
#define HOMING_POS_INDEX_SEARCH_DIRECTION BV(1)
#define HOMING_POS_HOME_SWITCH_POLARITY BV(2)
#define HOMING_USE_HOME_SWITCH BV(3)
#define HOMING_USE_INDEX_PULSE BV(4)
#define HOMING_HOME_AT_POWER_ON BV(5)
#define HOMING_FULL_SPEED_OFFSET_MOVE BV(6)
#define _HOMING_CFG_MAX_VALUE 0x007f

///////////////////////////////////////////////////////////////////////////////////////
// PULSE INPUT MODES //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
#define PULSEMODE_STEPDIR 0
#define PULSEMODE_QUADRATURE 1
#define PULSEMODE_PWM 2
#define PULSEMODE_ANALOG_PM10V 3
//none is for indexer, all opto inputs are usable
#define PULSEMODE_NONE 4
//this always forces SPI mode without caring CLK & MOSI opto status during reset
#define PULSEMODE_FORCE_SPI 5
#define _PULSEMODES_LAST 5

///////////////////////////////////////////////////////////////////////////////////////
// SEQUENCER FORMATS //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
#define SEQ_FORMAT_INC32_DLY16 2
#define SEQ_FORMAT_ABS32_DLY16 4
//torque injection test (stiffness test)
#define SEQ_FORMAT_TORQUE_INJECT 4096

///////////////////////////////////////////////////////////////////////////////////////
// CAPTURE SIGNALS ////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
/*#define CAPTURE_CURRENT BV(0)
//old QDIN
#define CAPTURE_ACTUAL_TORQUE BV(1)
//old QDOUT
#define CAPTURE_OUTPUT_VOLTAGE BV(2)
#define CAPTURE_POS BV(3)
#define CAPTURE_FERROR BV(4)
#define CAPTURE_FEEDBACK BV(6)
#define CAPTURE_PWM BV(7)
#define CAPTURE_VOLTAGE BV(8)
#define CAPTURE_TORQUECMD BV(9)
#define CAPTURE_DEBUG BV(10)
#define CAPTURE_TARGET BV(11)
#define CAPTURE_STATUSBITS BV(12)
#define CAPTURE_FAULTBITS BV(13)
*/

//use BV() with these numbers when forming capture set!
//OLD way: setparam( RUNTIME_CAPTURE_SOURCE, CAPTURE_QDIN|CAPTURE_QDOUT );
//NEW way: setparam( RUNTIME_CAPTURE_SOURCE, BV(CAPTURE_QDIN)|BV(CAPTURE_QDOUT) );


// 0 reserved for SPI return value
//old QDIN
#define CAPTURE_TORQUE_TARGET 1
#define CAPTURE_TORQUE_ACTUAL 2
#define CAPTURE_VELOCITY_TARGET 3
#define CAPTURE_VELOCITY_ACTUAL 4
#define CAPTURE_POSITION_TARGET 5
#define CAPTURE_POSITION_ACTUAL 6

#define CAPTURE_FOLLOW_ERROR 7
#define CAPTURE_OUTPUT_VOLTAGE 8
#define CAPTURE_BUS_VOLTAGE 9
#define CAPTURE_STATUSBITS 10
#define CAPTURE_FAULTBITS 11

#define CAPTURE_P_OUT 12
#define CAPTURE_I_OUT 13
#define CAPTURE_D_OUT 14
#define CAPTURE_FF_OUT 15

#define CAPTURE_RAW_POS 25

//rest are availalbe in debug/development firmware only:
#define CAPTURE_PWM1 16
#define CAPTURE_PWM2 17
#define CAPTURE_PWM3 18
#define CAPTURE_DEBUG1 19
#define CAPTURE_DEBUG2 20
#define CAPTURE_CURRENT1 21
#define CAPTURE_CURRENT2 22
#define CAPTURE_ACTUAL_FLUX 23
#define CAPTURE_OUTPUT_FLUX 24
#define CAPTURE_DEBUG3 26

/*OLD V111-114
#define CAPTURE_VELOCITY 1
#define CAPTURE_TORQUECMD 2
#define CAPTURE_ACTUAL_TORQUE 3
#define CAPTURE_ACTUAL_FLUX 4
#define CAPTURE_OUTPUT_VOLTAGE 5
#define CAPTURE_OUTPUT_FLUX 6
#define CAPTURE_POS 7
#define CAPTURE_FERROR 8
#define CAPTURE_FEEDBACK 9
#define CAPTURE_TARGET 10
#define CAPTURE_BUS_VOLTAGE 11
#define CAPTURE_STATUSBITS 12
#define CAPTURE_FAULTBITS 13
#define CAPTURE_PWM1 16
#define CAPTURE_PWM2 17
#define CAPTURE_PWM3 18
#define CAPTURE_DEBUG1 19
#define CAPTURE_DEBUG2 20
#define CAPTURE_CURRENT1 21
#define CAPTURE_CURRENT2 22
#define CAPTURE_P_OUT 23
#define CAPTURE_I_OUT 24
#define CAPTURE_D_OUT 25
//FF1 out is now (v111+) sum of both FF's
#define CAPTURE_FF1_OUT 26*/


///////////////////////////////////////////////////////////////////////////////////////
// CAPTURE TRIGGERS ///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
#define TRIG_NONE 0
#define TRIG_INSTANT 2
#define TRIG_SERIALCMD 4
#define TRIG_TARGETCHANGE 5
#define TRIG_FAULT 6

///////////////////////////////////////////////////////////////////////////////////////
// CONTROL MODES //////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
#define MODE_NONE 0
#define MODE_POS 1
#define MODE_VEL 2
#define MODE_TORQ 3
#define MODE_VOLT 4
//last mode number for mode setting
#define _MODES_LAST 4

///////////////////////////////////////////////////////////////////////////////////////
// SPI RETURN PARAM TYPES /////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
/*#define RETURN_PARAM_ZERO 0
#define RETURN_PARAM_POS 1
#define RETURN_PARAM_FEEDBACK 2
#define RETURN_PARAM_ACTUAL_TORQUE 3
#define RETURN_PARAM_FERROR 4
#define _RETURN_PARAM_LAST 4*/


///////////////////////////////////////////////////////////////////////////////////////
// FEATURE BITS ///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
#define FEAT_PMDC BV(0)
#define FEAT_PMAC BV(1)
#define FEAT_STEPPER BV(2)
#define FEAT_CURRENT_FB BV(3)
#define FEAT_POSITIONING BV(4)
#define FEAT_VELOCITY BV(5)
#define FEAT_TRAJ_PLANNER BV(6)
#define FEAT_HALLS BV(7)
#define FEAT_INDEX BV(8)
#define FEAT_PULSE_IN BV(9)
#define FEAT_HOMING BV(10)

//torque low pass filter frequencies, lookup:
//0=100 Hz
//1=150
//2=220
//3=330..
//... 470,680,1000,1500,2200,
//9=3300 Hz

#endif
