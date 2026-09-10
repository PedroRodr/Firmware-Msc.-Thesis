#ifndef SERIAL_H_
#define SERIAL_H_

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
#define SERIAL_RX_BUFFER_SIZE 64


//**********************************************************************************************************************************
//                                                     Templates Section
//**********************************************************************************************************************************
class Serial{
    public:
    static void init(void);
    static uint16_t write(uint8_t* buffer, uint16_t size);
    static uint16_t read(uint8_t* buffer, uint16_t size, int16_t timeout);
    static uint16_t readUntil(uint8_t* buffer, uint8_t termChar, int16_t timeout);
    static uint8_t dataAvailable;
    private:
    static void isr(void);
    static uint16_t detectChar(uint8_t targetChar);
};




#endif
