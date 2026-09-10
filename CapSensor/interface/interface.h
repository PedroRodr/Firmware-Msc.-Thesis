#ifndef INTERFACE_H_
#define INTERFACE_H_

//**********************************************************************************************************************************
//                                                      Includes Section
//**********************************************************************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "acquisition/acquisition.h"
//#include "demodulation/demodulation.h"
//#include "generation/generation.h"
//#include <driverlib.h>
#include <msp430.h>



//**********************************************************************************************************************************
//                                                      Define Section
//**********************************************************************************************************************************
#define     COMMAND_HEADER_SIZE     6
#define     COMMAND_CTRL_SIZE       2
#define     NCHANNELS               8
#define     DATA_SIZE               24

typedef struct command_def{
    uint8_t function;
    uint32_t value;
    uint8_t size;
    uint8_t data[DATA_SIZE];
    uint8_t crc;
    uint8_t control;
}command_t;

#define     ANSWER_OK                       0x01
#define     ANSWER_NOK                      0x00

/*
//#define     COMMAND_TEST                    0x00
#define     COMMAND_CAP_DATA_H              0x01
#define     COMMAND_VT_DATA_H               0x04
#define     COMMAND_CAP_SETUP               0x07
#define     COMMAND_VT_SETUP                0x08
#define     COMMAND_EXC_SETUP               0x09
#define     COMMAND_STOP_ACQUISITION        0x22
#define     COMMAND_OUTPUT_FREQUENCY        0x30
#define     COMMAND_OUTPUT_AMPLITUDE        0x31
#define     COMMAND_INPUT_FREQUENCY         0x32
#define     COMMAND_INPUT_HFGAIN            0x33
#define     COMMAND_INPUT_IQGAIN            0x34
#define     COMMAND_INPUT_IQBANDWIDTH       0x35
#define     COMMAND_INPUT_IOFFSET           0x36
#define     COMMAND_INPUT_QOFFSET           0x37
#define     COMMAND_INPUT_AUTO_IQOFFSET     0x38
#define     COMMAND_PHASE_SHIFT             0x39
#define     COMMAND_CALIBRATION             0x3A
*/

// state machine states
#define IDLE_ST      0         // process incomming cmds if any available,
#define BEGIN_ST     1         //first state of the state machine, the module should send the begin of a new data packet
#define SEND_DATA_1  2         //second state of the state machine, the module should send 6 consecutive data points
#define SEND_DATA_16 17        //16st state of the state machine, the module should send 16 consecutive data points
#define ENDING_ST    18        //last state of the state machine, where the termination packet should be sent containing the CRC and stuff termination char

//**********************************************************************************************************************************
//                                                     Templates Section
//**********************************************************************************************************************************
class Interface{
    public:
        static void sendCommand(command_t* command);
        static uint8_t receiveCommand(command_t* command);
        static void cleanCommand(command_t* command);
        static void processCommand(command_t* command, command_t* answer);
        static uint8_t dataCommand(uint8_t * dataCmd);
        static void sendDataCommand(uint8_t * serializedDataCmd);
        static uint8_t acquisition;
        static uint8_t machineState;
        static void runStateMachine();
        static bool incommingCmd();

};



#endif
