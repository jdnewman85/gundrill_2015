//VSD drive internal SPI commands & parameters
//Copyright (c) Granite Devices Oy


#ifndef VSD_CMD_H
#define VSD_CMD_H

/*
SPI COMMAND FORMAT

32 bit command packet from PC:
( 8bit command id | 16bit parameter data | 8bit checksum )

32 bit reply packet from drive:
( 8bit drive status | 16bit reply data | 8bit checksum )

Total delay of reply is 64 bits

*/

/////////////////////////////////////////////////////////////////////
//SPECIAL RETURN VALUES
#define RET_COMM_ERROR    0x0000FFFF
#define RET_INVALID_PARAM 0x000FFFFF
#define RET_INVALID_CMD   0x00FFFFFF

//CRC start value. see communications.c
#define CRC_INIT 51


//REMOTE COMMANDS
//if data is not specified, data must be 0

//BASIC CMD's
//data=0
///#define CMD_DRIVE_DISABLE 0
//#define CMD_DRIVE_ENABLE 1

//param=0 illegal but will disable, 1=disable, 2=enable
#define CMD_SET_ENABLE_STATUS 0
//#define CMD_DRIVE_ENABLE 1

//BL_NOP provided for mode autodetection. Returns always 0xD123 in non-BL mode
#define BL_NOP 1

#define CMD_NOP 2
#define CMD_SAVE_CONFIG 3
#define CMD_LOAD_CONFIG 4
//reserved for flash mass erase in BL!
#define CMD_RESERVED 9

//CAPTURE CMD's
#define CMD_CAPTURE_ON 11
//data=buffer position, return capture buffer value
#define CMD_GET_CAPTURE_SAMPLE 12
//data=buffer position, return two adjacent capture buffer values without status bits and CRC
//lower word is asked sample and upper word is the next sample
#define CMD_GET_CAPTURE_SAMPLE_HS 13

//clear only selected bits, 0xffff clears all
#define CMD_CLR_FAULTBITS 20

//CONTROL CMD's
//data=absolute input value
#define CMD_ABS_TARGET_VALUE 30
//data=incremental input value
#define CMD_INC_TARGET_VALUE 31
//data=0, user should wait at least 2 pid cycles after cmd
#define CMD_SET_PULSE_INPUT_SOURCE 35

//data=upper word of 32 bit absolute value, lower word must be uploaded before this with CMD_UPLOAD_PARAM
#define CMD_ABS32_TARGET_VALUE 33


//NEW configuration API
//first upload 16 bit value
#define CMD_UPLOAD_PARAM 50
//then tell where to store it
#define CMD_SET_PARAM 51
//this returns parameter. use PARAM_nnn masks in paramenter number to get min/max allowed values
#define CMD_GET_PARAM 52
//stores uploaded value (CMD_UPLOAD_PARAM) to user buffer at given address (param)
#define CMD_STORE_TO_BUFFER 55
//get 16 bit word from buffer. param=address
//same as GET_CAPTURE_SAMPLE #define CMD_READ_FROM_BUFFER 56

//HOMING CMDs
//param 0=abort, 1=start
#define CMD_HOMING 60


//for factory testing
#define CMD_GET_TEST_INPUTS 110

//for special functions
//param
// 0=reset cpu
// 10=get 32 bit drive-to-host stream checksum (32bit data, no crc & stat)
#define CMD_MISC 120


//extended command for future use, ie 16 bit cmd's
//OR select command page?
#define CMD_EXTENDED 127


//parameters for CMD_MISC
#define MISC_RESET 0
#define MISC_GET_CRC32 10
#define MISC_GET_ABS32_TARGET_VALUE 15
#define MISC_START_FACTORY_BOUNDARY_SCAN 1000
//starts runnign motion sequece at address that is uploaded before with CMD_UPLOAD_PARAM
//see documentation.txt
#define MISC_START_SEQUENCE 100

////////////////////////////////////////////////////////////////////////////////CONFIG PARAMS 0-4999

//basic settings 0-199
#define CFG_MOTOR_MODE 10
//position follow error trip point
#define CFG_FERROR_TRIP 20
#define CFG_RECOVERY_SPEED 21
//velocity follow error trip point. units: velocity command (counts per PIDcycle * divider)
#define CFG_VELOCITY_FERROR_TRIP 24

#define CFG_INPUT_FILTER_LEN 30
#define CFG_DEFAULT_CONTROL_MODE 40
#define CFG_MULTIPLIER 61
#define CFG_DIVIDER 62
//pulse count mode. PULSE_QUAD PULSE_STEPDIR
#define CFG_PULSE_MODE 70
#define CFG_ABS_IN_OFFSET 75
#define CFG_ABS_IN_SCALER 76

//flag bits & general
#define CFG_DRIVE_FLAGS 80
#define CFG_MOTION_FAULT_THRESHOLD 90
#define CFG_HV_VOLTAGE_LIMIT 100
#define CFG_HV_VOLTAGE_LOW_LIMIT 101
#define CFG_STARTUP_DELAY 105

//bitIO
#define CFG_OPTO_OUT1_SOURCE 150
#define CFG_OPTO_OUT2_SOURCE 151
#define CFG_DISABLE_IN_SOURCE 152
#define CFG_CLEAR_FAULTS_EDGESOURCE 153
#define CFG_START_HOMING_EDGESOURCE 154
#define CFG_STAT_USER_BIT_SOURCE 155
//indexer, 0 for disabled indexer
#define CFG_INDEXER_DATA_ADDR 180


//primary feedback loop 200-299
//primary feedback loop 200-299
#define CFG_VEL_I 200
#define CFG_POS_P 201
#define CFG_VEL_P 202
#define CFG_AD_VEL_I 210
#define CFG_AD_POS_P 211
#define CFG_AD_VEL_P 212
#define CFG_VEL_FF 220
#define CFG_ACC_FF 221
//anti dither limits
#define CFG_AD_LIM 230
//D filtering
//#define CFG_D_AVG_LEN 240

//secondary feedback loop 300-399

//current loop related 400-499
#define CFG_TORQ_P 401
#define CFG_TORQ_I 402
//new in stepperdev
#define CFG_MOTOR_RES 405
#define CFG_MOTOR_IND 406

#define CFG_TORQUELIMIT_CONT 410
#define CFG_TORQUELIMIT_PEAK 411
#define CFG_TORQUELIMIT_HOMING 415
#define CFG_TORQUELIMIT_FAULT 420
#define CFG_THERMAL_TIMECONSTANT 430
//#define CFG_COUNTS_PER_REV 460
#define CFG_ENCODER_PPR 460
//#define CFG_EREV_SCALER 461
#define CFG_MOTOR_POLEPAIRS 462
#define CFG_PHASESEARCH_VOLTAGE_SLOPE 480
#define CFG_PHASESEARCH_CURRENT 481
//not implemented:
#define CFG_HARMONIC3 450
#define CFG_TORQUE_LPF_BANDWIDTH 490
//#define CFG_TORQUE_LPF_BANDWIDTH 240


//trajectory planner 800-899
#define CFG_TRAJ_PLANNER_ACCEL 800
#define CFG_TRAJ_PLANNER_VEL 802
//accel limit for homing & ferror recovery
//if set to 0, use CFG_TRAJ_PLANNER_ACCEL instead (for backwards compatibility)
#define CFG_TRAJ_PLANNER_HOMING_ACCEL 803
#define CFG_TRAJ_PLANNER_HOMING_VEL 804
#define CFG_TRAJ_PLANNER_HOMING_BITS 806
//hard stop triggered by follow error in encoder counts
#define CFG_TRAJ_PLANNER_HOMING_HARD_STOP_THRESHOLD 808
//32bit position offset in encoder counts, value must not overflow on divider*offset
#define CFG_TRAJ_PLANNER_HOMING_OFFSET_LO 820
#define CFG_TRAJ_PLANNER_HOMING_OFFSET_HI 821
//signed 32bit absolute position limits, active after homing. disabled when both=0
#define CFG_ABSPOSITION_HI_LIMIT_HI 830
#define CFG_ABSPOSITION_HI_LIMIT_LO 831
#define CFG_ABSPOSITION_LO_LIMIT_HI 833
#define CFG_ABSPOSITION_LO_LIMIT_LO 834

//////////////////////////////////////////////////////////////////////////////////RUNTIME PARAMS 5000-9999

#define RUNTIME_CAPTURE_SOURCE 5020
#define RUNTIME_CAPTURE_SOURCE_2 5021
#define RUNTIME_CAPTURE_TRIGGER 5011
#define RUNTIME_CAPTURE_SAMPLERATE 5012
//rdonly
#define RUNTIME_CAPTURE_BUF_LENGHT 5013
//rd only
#define RUNTIME_FAULTBITS  5100
//rd only
#define RUNTIME_STATUSBITS 5102
#define RUNTIME_CONTROL_MODE 5200


//0=disabled, other=number PID cycles to wait for serial command
#define RUNTIME_INPUT_WATCHDOG 5300

#define RUNTIME_BUS_VOLTAGE_SCALE_100V 5500
#define RUNTIME_CURRENT_SCALE_1A 5510
#define RUNTIME_RETURN_PARAMETER_TYPE 5600

#define RUNTIME_FEATURES1 6000
#define RUNTIME_FIRMWARE_VERSION 6010
#define READONLY_BACKWARDS_COMP_VERSION 6012
#define RUNTIME_DEVICE_TYPE 6020
#define READONLY_PWM_FREQUENCY 6050
#define READONLY_PID_FREQUENCY 6055

//affects only in MC_VECTOR & MC_BLDC mode
#define RUNTIME_OVERRIDE_COMMUTATION_ANGLE 8000
//#define RUNTIME_FLAGS 8050


#define RUNTIME_DEBUGPARAM1 15436
#define RUNTIME_DEBUGPARAM2 15437
#define RUNTIME_DEBUGPARAM3 15438
#define RUNTIME_FAULT_LOCATION  15440



/*
PARAMS 16384-65535 are not avaiable
Two upper bits decide whether to get value, min or max

00=value
01=min
10=max
11=reserved

*/

#define PARAM_VALUE_MASK  0x0000
#define PARAM_MIN_VALUE_MASK  0x4000
#define PARAM_MAX_VALUE_MASK  0x8000
#define PARAM_MASK_BITS  0xC000




//user buffer base addressses
#define BASE_ADDR_SEQUENCE_BUF 0
#define BASE_ADDR_SEQUENCE_BUFbits 0
#define BASE_ADDR_USER_FLASH 4096
#define BASE_ADDR_USER_FLASHbits 12
#define BASE_ADDR_ENCODER_CORRECTION 8192
#define BASE_ADDR_ENCODER_CORRECTIONbits 13
#define ENCODER_CORRECTION_LEN 64





#endif // VSD_CMD_H
