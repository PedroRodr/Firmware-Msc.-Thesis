//**********************************************************************************************************************************
//                                                      Includes Section
//**********************************************************************************************************************************
#include "i2c.h"
#include "serial.h"


//**********************************************************************************************************************************
//                                                     External Functions
//**********************************************************************************************************************************

//**********************************************************************************************************************************
//                                                      Global Variables
//**********************************************************************************************************************************
uint8_t TXByteCtr = 2;
uint8_t TransmitRegAddr = 0;
uint8_t TransmitInfo = 0;
uint8_t Readmode = 0;
uint8_t RXByteCtr = 6;
uint8_t Bufval = 0;
uint8_t ReceiveBuffer[10] = {0};
uint8_t EnterRCV = 0;
uint8_t ReceiveIndex = 0;
uint8_t counter = 0;
int axis = 0;
int flag_inv = 0;
int I2C::flag_inv = 0;
int I2C::axis = 0;
//volatile uint8_t I2C::ReceiveBuffer_I2C_X[10] = {0,0,0,0,0,0,0,0,0,0};
volatile uint8_t I2C::ReceiveBuffer_I2C[10] = {0,0,0,0,0,0,0,0,0,0};

//**********************************************************************************************************************************
//                                                        Code Section
//**********************************************************************************************************************************

//**********************************************************************************************************************************
// Header: I2C::init
// Function: Initializes the I2C port
//**********************************************************************************************************************************
void I2C::init(void){

    P1SEL |= BIT6 + BIT7;                               // Set I2C pins
    P1SEL2|= BIT6 + BIT7;                               // Set I2C pins

    UCB0CTL1 |= UCSWRST;                                // Enable SW reset
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;               // I2C Master, synchronous mode
    UCB0CTL1 = UCSSEL_2 + UCSWRST;                      // Use SMCLK, keep SW reset
    UCB0BR0 = 160;                                      // fSCL = SMCLK/160 = ~100kHz
    UCB0BR1 = 0;
    UCB0I2CSA = SLAVE_ADDR;                             // Slave Address
    UCB0CTL1 &= ~UCSWRST;                               // Clear SW reset, resume operation
    UCB0I2CIE |= UCNACKIE;

    //I2C::write(0x07,0xA0);
    I2C::write(0x07,0xE0);
    I2C::write(0x09,0x2B);                                //Configuracao basica cdc liga ambas as excitações
}

//**********************************************************************************************************************************
// Header: SPI::read
// Function: Reads a maximum of "size" bytes to the memory pointed by "buffer". Timeout is expreesed in ms
//**********************************************************************************************************************************
unsigned int I2C::read(uint8_t reg_addr, uint8_t read_bytes){
    Readmode = 1;
    TXByteCtr = 1;
    TransmitInfo = reg_addr;

    RXByteCtr = read_bytes;

    UCB0I2CSA = SLAVE_ADDR;
    IFG2 &= ~(UCB0TXIFG + UCB0RXIFG);                   // Clear any pending interrupts
    IE2 &= ~UCB0RXIE;                                   // Disable RX interrupt
    IE2 |= UCB0TXIE;                                    // Enable TX interrupt

    UCB0CTL1 |= UCTR + UCTXSTT;                         // I2C TX, start condition

    __bis_SR_register(CPUOFF + GIE);                    // Enter LPM0 w/ interrupts

    return 0;
}


//**********************************************************************************************************************************
// Header: SPI::write
// Function: Writes "size" bytes from the memory pointed by "buffer". Timeout is expreesed in ms
//**********************************************************************************************************************************
unsigned int I2C::write(uint8_t reg_addr, uint8_t reg_data){

    TransmitRegAddr = reg_addr;
    TransmitInfo = reg_data;
    TXByteCtr = 2;
    Readmode = 0;

    UCB0I2CSA = SLAVE_ADDR;
    IFG2 &= ~(UCB0TXIFG + UCB0RXIFG);                   // Clear any pending interrupts
    IE2 &= ~UCB0RXIE;                                   // Disable RX interrupt
    IE2 |= UCB0TXIE;                                    // Enable TX interrupt

    UCB0CTL1 |= UCTR + UCTXSTT;                         // I2C TX, start condition
    __bis_SR_register(CPUOFF + GIE);                    // Enter LPM0 w/ interrupts
    return 0;
}

//******************************************************************************
// I2C Interrupt For Received and Transmitted Data******************************
//******************************************************************************

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCIAB0TX_ISR (void)
#else
#error Compiler not supported!
#endif
{

    if (IFG2 & UCB0RXIFG)                               // Receive Data Interrupt
    {
        //Must read from UCB0RXBUF
        uint8_t rx_val = UCB0RXBUF;

        RXByteCtr--;
        I2C::ReceiveBuffer_I2C[RXByteCtr] = rx_val;

        if (RXByteCtr == 1)
        {

            UCB0CTL1 |= UCTXSTP;
        }

        else if (RXByteCtr == 0)
        {
            IE2 &= ~UCB0RXIE;
            __bic_SR_register_on_exit(CPUOFF);          // Exit LPM0
        }
    }
    else if (IFG2 & UCB0TXIFG){                         // Transmit Data Interrupt

       if (TXByteCtr == 2){
            UCB0TXBUF = TransmitRegAddr;
            TXByteCtr --;
       }
       else if (TXByteCtr == 1){
            UCB0TXBUF = TransmitInfo;
            TXByteCtr --;
       }
       else if (TXByteCtr == 0){

           if (Readmode == 0){
               UCB0CTL1 |= UCTXSTP;                     // Send stop condition
               IE2 &= ~UCB0TXIE;                        // disable TX interrupt
               TXByteCtr = 2;
               __bic_SR_register_on_exit(CPUOFF);       // Exit LPM0
           }
           else{

               IE2 |= UCB0RXIE;                         // Enable RX interrupt
               IE2 &= ~UCB0TXIE;                        // Disable TX interrupt
               UCB0CTL1 &= ~UCTR;                       // Switch to receiver
               UCB0CTL1 |= UCTXSTT;                     // Send repeated start
           }
       }
   }

}

