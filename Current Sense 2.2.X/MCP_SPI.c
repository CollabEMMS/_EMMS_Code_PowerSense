/************************
 MCP-3909 Communication Module
 Read values from MCP using SPI
  */
#include <stdio.h>
#include <stdbool.h>

#include <xc.h>
#include <p18cxxx.h>


voi mcp_spi_init( void)
{
    / use SPI 1

	    
    
        TRISAbits.TRISA0 = 0; // pin 2 connected as an output for pulse
    TRISAbits.TRISA1 = 1; // pin 3 connected as an input for pulse
    LEDDIR = 0; // pin 25 connected as an output for LED
    TRISCbits.TRISC3 = 0; // pin 14 connected as an output for pulse freq.
    TRISCbits.TRISC5 = 0; // pin 16 connected as an output for pulse freq.
    TRISCbits.TRISC6 = 0; // set pin 17 as an output for MCLR
    TRISCbits.TRISC7 = 0; // set pin 18 as an output for pulse freq.
    ANSELAbits.ANSA1 = 0b0; // turn off analog to digital conversion

    LATCbits.LATC6 = 1; // set the MCLR of the MCP high
    LATCbits.LATC3 = 1; // set pin 14 to a 1 to set freq. control F2 for pulse
    LATCbits.LATC5 = 1; // set pin 16 to a 1 to set freq. control F1 for pulse
    LATCbits.LATC7 = 1; // set pin 18 to a 1 to set freq. control F0 for pulse


    SSP2CON1bits.SSPEN = 0; //Synchronous Serial Port Enable bit


    TRISBbits.RB0 = 0b1;
    TRISBbits.RB1 = 0b1;
    TRISBbits.RB2 = 0b1;
    TRISBbits.RB3 = 0b0;

    SSP2STATbits.SMP = 0;
    SSP2STATbits.CKE = 1;

    SSP2CON1bits.WCOL = 0; //Write Collision Detect bit
    SSP2CON1bits.SSPOV = 0; //Receive Overflow Indicator bit
    SSP2CON1bits.SSPEN = 0; //Synchronous Serial Port Enable bit
    SSP2CON1bits.CKP = 1; //Clock Polarity Select bit
    SSP2CON1bits.SSPM = 0b0100; //Synchronous Serial Port Mode Select bits


    SSP2CON3 = 0x00;
    SSP2CON3bits.BOEN = 0b0; //Buffer Overwrite Enable bit

    SSP2CON1bits.SSPEN = 1; //Synchronous Serial Port Enable bit

    //    SPIWatchdogTimerInit(); 

    return;

    
    
    
    
    
}

