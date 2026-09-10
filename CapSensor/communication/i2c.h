#ifndef I2C_H_
#define I2C_H_

//**********************************************************************************************************************************
//                                                      Includes Section
//**********************************************************************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <msp430.h>


//**********************************************************************************************************************************
//                                                      Define Section
//**********************************************************************************************************************************
#define SLAVE_ADDR  0x48

#define COMMAND_STATUS              0x00
#define COMMAND_CAP_DATA            0x01
#define COMMAND_VT_DATA             0x04
#define COMMAND_CAP_SETUP           0x07
#define COMMAND_VT_SETUP            0x08
#define COMMAND_EXC_SETUP           0x09
#define COMMAND_CONFIGURATION       0x0A
#define COMMAND_CAP_DAC_A           0x0B
#define COMMAND_CAP_DAC_B           0x0C
#define COMMAND_CAP_OFFSET          0x0D
#define COMMAND_CAP_GAIN            0x0F
#define COMMAND_VOLTAGE_GAIN        0x11
#define COMMAND_READ_SINGLE_SEQ     0x13
#define CMD_GET_ID                  0x38

#define RESET_ADDRESS               0xBF
//**********************************************************************************************************************************
//                                                     Templates Section
//**********************************************************************************************************************************
class I2C{
    public:
        static void init(void);
        static unsigned int read(uint8_t reg_addr, uint8_t read_bytes);
        static unsigned int write(uint8_t reg_addr, uint8_t reg_data);
        static int axis;
        static int flag_inv;
        static volatile uint8_t ReceiveBuffer_I2C[10];
    private:
};


#endif
