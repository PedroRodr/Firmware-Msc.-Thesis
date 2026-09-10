//**********************************************************************************************************************************
//                                                      Includes Section
//**********************************************************************************************************************************
#include "serial.h"


//**********************************************************************************************************************************
//                                                     External Functions
//**********************************************************************************************************************************

//**********************************************************************************************************************************
//                                                      Global Variables
//**********************************************************************************************************************************
uint8_t rxBuffer[SERIAL_RX_BUFFER_SIZE];
uint8_t aux_buffer[8];
uint16_t rxBufferReadPtr = 0;
uint16_t rxBufferWritePtr = 0;
uint16_t oi = 0;


//**********************************************************************************************************************************
//                                                        Code Section
//**********************************************************************************************************************************

//**********************************************************************************************************************************
// Header: Serial::init
// Function: Initializes the serial port
//**********************************************************************************************************************************
void Serial::init(void){

    UCA0CTL1 |= UCSSEL_2;                               // SMCLK
    UCA0BR0 = 8;                                      // 16MHz 115200
    UCA0BR1 = 0;                                        // 16MHz 115200
    UCA0MCTL = UCBRS2 + UCBRS0;                                   // Modulation UCBRSx = 7
    UCA0CTL1 &= ~UCSWRST;                               // **Initialize USCI state machine**
    IFG2 &= ~(UCA0RXIFG);
    IE2 |= UCA0RXIE;

}

//**********************************************************************************************************************************
// Header: Serial::isr
// Function: Serial port interrupt service routine
//**********************************************************************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
//    if ((IFG2&UCA0RXIFG)&&(IE2&UCA0RXIE))
    if (IFG2 & UCA0RXIFG)
    {
        rxBuffer[oi] = UCA0RXBUF;
        rxBufferWritePtr++;
        oi++;
    }
    else{
            if (UCB0STAT & UCNACKIFG)
            {
                UCB0STAT &= ~UCNACKIFG;                 // Clear NACK Flags
            }
            if (UCB0STAT & UCSTPIFG)                    //Stop or NACK Interrupt
            {
                UCB0STAT &=
                    ~(UCSTTIFG + UCSTPIFG + UCNACKIFG); //Clear START/STOP/NACK Flags
            }
            if (UCB0STAT & UCSTTIFG)
            {
                UCB0STAT &= ~(UCSTTIFG);                //Clear START Flags
            }
    }
}

//**********************************************************************************************************************************
// Header: Serial::read
// Function: Serial reads a buffer from the serial port
//**********************************************************************************************************************************
unsigned int Serial::write(uint8_t* buffer, unsigned int size){
    unsigned int i = 0;
    for(i = 0; i < size; i++){
        while (!(IFG2&UCA0TXIFG));                      // USCI_A0 TX buffer ready?
        UCA0TXBUF = buffer[i];
    }
    return size;
}
//**********************************************************************************************************************************
// Header: Serial::readUntil
// Function: Reads a maximum of "size" bytes until find the terminator char. Timeout is expressed in ms.
//**********************************************************************************************************************************
uint16_t Serial::readUntil(uint8_t * buffer, uint8_t termChar, int16_t timeout){
    int16_t size = detectChar(termChar);
    if(size>0){
        memcpy(buffer, aux_buffer, 8);
        return size;
    }else{
        return 0;
    }
}

//**********************************************************************************************************************************
// Header: Serial::detectChar
// Function: The the number of bytes available until reach the termination char
//**********************************************************************************************************************************
uint16_t Serial::detectChar(uint8_t targetChar){
    uint8_t size =oi;
    uint8_t i = 0;
    if (rxBuffer[size - 1] == targetChar){
        memcpy(aux_buffer, rxBuffer, size);

        for(i = 0; i < size; i++){
            rxBuffer[i] = 0;
        }

        oi = 0;
        return size;
    }

    return 0;
}
