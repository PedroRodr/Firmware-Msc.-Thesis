//**********************************************************************************************************************************
//                                                      Includes Section
//**********************************************************************************************************************************
#include <byteStuffing/stuffing.h>
#include "interface.h"
#include "communication/serial.h"
#include "communication/i2c.h"

#define MIN_CMD_SIZE 8

//**********************************************************************************************************************************
//                                                     External Functions
//**********************************************************************************************************************************

//**********************************************************************************************************************************
//                                                      Global Variables
//**********************************************************************************************************************************
int8_t index = 0;
uint8_t Interface::machineState = IDLE_ST;
uint8_t packetBegin[6] = {COMMAND_STATUS,0,0,0,0,64};

command_t receivedCommand, answerCommand;

uint8_t rcv_info_i2c[6];
uint8_t read_bytes = 3;

uint32_t buffer_aux = 0;
uint32_t sum_buffer_aux = 0;

uint8_t nrAcq, i,j;
uint8_t k=0;

volatile uint8_t rcv_int = 0;
uint8_t ReceiveBuffer_SPI2[16][NCHANNELS] = {};
uint16_t ReceiveBuffer_Acc[16]= {};
uint8_t ReceiveBuffer_SPI[16] = {};
uint8_t flag_read;
//**********************************************************************************************************************************
//                                                        Code Section
//**********************************************************************************************************************************
//**********************************************************************************************************************************
// Header: Interface::sendCommand
// Function: Sends a complete command
//**********************************************************************************************************************************
void Interface::sendDataCommand(uint8_t * serializedDataCmd){


    uint8_t serializedCmd[128];

    /*Perform Byte Stuffing*/
    uint16_t serializedCmdSize = Stuffing::performStuffing(serializedDataCmd, 71, serializedCmd);

    serializedCmd[serializedCmdSize-1]=0x7E;
    Serial::write(serializedCmd,serializedCmdSize);
}


//**********************************************************************************************************************************
// Header: Interface::sendCommand
// Function: Sends a complete command
//**********************************************************************************************************************************
void Interface::sendCommand(command_t* command){


    int serializedCmdSize = (COMMAND_HEADER_SIZE+(command->size)+1);
    //int serializedCmdSize = (COMMAND_HEADER_SIZE+COMMAND_CTRL_SIZE+(command->size));
    uint8_t serializedCmd[128];
    //uint8_t serializedCmd[128];

    serializedCmd[0]=command->function;
    memcpy(&serializedCmd[1], &command->value,4);
    serializedCmd[5]=command->size;


    if(command->size != 0){
        memcpy(&serializedCmd[COMMAND_HEADER_SIZE], command->data, command->size);

    }

    /*Perform Byte Stuffing and CRC at the same time*/
    serializedCmdSize = Stuffing::performStuffing(serializedCmd, serializedCmdSize, serializedCmd);

    serializedCmd[serializedCmdSize]=command->control;
    serializedCmdSize++;
    //Serial::write(stuffedCmd,stuffedCmdSize); tentativa de retirar o byte stuffing
    Serial::write(serializedCmd,serializedCmdSize);

}


//**********************************************************************************************************************************
// Header: Interface::receiveCommand
// Function: Receives a complete command
//**********************************************************************************************************************************
uint8_t Interface::receiveCommand(command_t* command){

    uint8_t serializedCmd[8];
    int serializedCmdSize = Serial::readUntil(serializedCmd, 0x7e, 0);

    //discard any command that don't fill the packet size
    if(serializedCmdSize>= MIN_CMD_SIZE){
        memcpy(&command->function,&serializedCmd[0],1);
        memcpy(&command->value,&serializedCmd[1],1);
        return 1;
    }

    return 0;
}


//**********************************************************************************************************************************
// Header: Interface::cleanCommand
// Function: Liberates the data field memory from a command, reinitializes all the remaining fields
//**********************************************************************************************************************************
void Interface::cleanCommand(command_t* command){

    if(command->data != NULL)
        free(command->data);
    command->function = 0;
    command->value = 0;
    command->size = 0;
}

//**********************************************************************************************************************************
// Header: Control::processCommand
// Function: Parses and process a command
//**********************************************************************************************************************************
void Interface::processCommand(command_t* command, command_t* answer){
    answer->function = 0;
    answer->value = 0;
    answer->size = 0;
    answer->crc = 0x01;
    answer->control=0x7E;

    switch(command->function){
    case COMMAND_STATUS:
        //Status register read only
        read_bytes = 2;

        I2C::read(COMMAND_STATUS, read_bytes);
        //memcpy(&answer->value, &rcv_info_i2c[1],1);
        answer->value =  ((uint32_t) I2C::ReceiveBuffer_I2C[1] & 0x000000FF);
        case CMD_GET_ID:
            answer->value = 43;
            break;
        case COMMAND_CAP_DATA:

            answer->value = 28;

            break;
        case COMMAND_VT_DATA:

            break;
        case COMMAND_CAP_SETUP:

            break;
        case COMMAND_VT_SETUP:

            break;
        case COMMAND_EXC_SETUP:

            break;
        case COMMAND_CONFIGURATION:


            break;
        case COMMAND_CAP_DAC_A:
            read_bytes = 2;

            I2C::read(COMMAND_CAP_DAC_A, read_bytes);
            //memcpy(&answer->value, &rcv_info_i2c[1],1);
            answer->value =  ((uint32_t) I2C::ReceiveBuffer_I2C[1] & 0x000000FF);
            break;
        case COMMAND_CAP_DAC_B:
            I2C::write(COMMAND_CAP_DAC_B, command->value);

            read_bytes = 2;

            I2C::read(COMMAND_CAP_DAC_B, read_bytes);
            answer->value =  ((uint32_t) I2C::ReceiveBuffer_I2C[1] & 0x000000FF);
            break;
        case COMMAND_CAP_OFFSET:

            break;
        case COMMAND_CAP_GAIN:

                        break;

        case COMMAND_VOLTAGE_GAIN:
            I2C::read(0x09,2);
            //memcpy(&answer->value, &rcv_info_i2c[1],1);
            answer->value =  ((uint32_t) I2C::ReceiveBuffer_I2C[1] & 0x000000FF);
            break;

        case COMMAND_READ_SINGLE_SEQ:

            //Only one Acq
/*
            I2C::write(COMMAND_CONFIGURATION, 0x3A);

            while(rcv_int != 1){}


            rcv_int = 0;

            read_bytes = 4;

            I2C::read(COMMAND_CAP_DATA, read_bytes);
            //memcpy(&answer->value, &rcv_info_i2c[1],3);
            answer->value =  ( (uint32_t) I2C::ReceiveBuffer_I2C[3] << 16 & 0x00FF0000) |
                             ( (uint32_t) I2C::ReceiveBuffer_I2C[2] << 8 & 0x0000FF00) |
                             ( (uint32_t) I2C::ReceiveBuffer_I2C[1] & 0x000000FF);
*/

            read_bytes = 4;
            nrAcq = 1;
            sum_buffer_aux = 0;

            for (i=0; i < nrAcq; i++){
                //I2C::write(COMMAND_CONFIGURATION, 0x3A); //Single conversion and 109.6 conv time
                //I2C::write(COMMAND_CONFIGURATION, 0x21); //Single conversion and 62 ms conv time
                I2C::write(COMMAND_CONFIGURATION, 0x39); //Continuo conversion and 109.6 ms conv time

                while(rcv_int != 1){}

                rcv_int = 0;
                I2C::read(COMMAND_CAP_DATA, read_bytes);

                buffer_aux =  ( (uint32_t) I2C::ReceiveBuffer_I2C[3] << 16 & 0x00FF0000) |
                              ( (uint32_t) I2C::ReceiveBuffer_I2C[2] << 8 & 0x0000FF00) |
                              ( (uint32_t) I2C::ReceiveBuffer_I2C[1] & 0x000000FF);

                sum_buffer_aux = sum_buffer_aux + buffer_aux;

                // onde estava o Switch anteriormente
                //ID 0, apenas horizontal CIN1
                answer->data[0] = I2C::axis;
                I2C::write(0x07,0xE1);      //Muda o MUX para medir o CIN2
                I2C::axis = 1;
                break;

                //__delay_cycles(1000);
            }

            answer->value = sum_buffer_aux;
            answer->size = 1;
            //answer->value = buffer_aux;

            break;
        default:
            answer->function = 0;
            answer->value = 0;
            answer->size = 0;
            answer->crc = 0x00;
            answer->control=0x7E;
            break;
    }
}


//**********************************************************************************************************************************
// Header: Control::readChannel
// Function: Reads a given channel
//**********************************************************************************************************************************
uint8_t Interface::dataCommand(uint8_t*  dataCmd){
    return 0;
}

//**********************************************************************************************************************************
// Header: Interface::runAcquisitionStateMachine
// Function: gathers data from the acquisition and send's it over the serial port.
//**********************************************************************************************************************************
void Interface::runStateMachine() {

        if(Interface::receiveCommand(&receivedCommand)==1) {
            Interface::processCommand(&receivedCommand, &answerCommand);
            Interface::sendCommand(&answerCommand);
            Interface::cleanCommand(&receivedCommand);
            Interface::cleanCommand(&answerCommand);}
        }


// Port 1 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT1_VECTOR))) Port_1 (void)
#else
#error Compiler not supported!
#endif
{

  P1IFG &= ~0x20;                           // P1.5 IFG cleared
  rcv_int = 1;
}

