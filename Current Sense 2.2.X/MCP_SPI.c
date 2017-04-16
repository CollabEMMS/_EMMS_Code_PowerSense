/************************
 MCP-3909 Communication Module
 Read values from MCP using SPI
 */
#include <stdio.h>
#include <stdbool.h>
#include "config.h"

#include <xc.h>
//#include <p18cxxx.h>
#include <p18f25k22.h>

//extern void delayMS( unsigned int );


#define MCP_MCLR LATCbits.LATC6

void mcp_start( void )
{
    mcp_spi_init( );

    SSP1CON1bits.SSPEN = 1;

    MCP_MCLR = 0;
    __delay_us( 100 );
    MCP_MCLR = 1;
    __delay_us( 1 );

    SSP1BUF = 0b10100001;
    while ( SSP1STATbits.BF == 0 )
    {
	// wait until it is clocked out
    }

    SSP1CON1bits.SSPEN = 0;


}

void mcp_spi_init( void )
{
    // use SPI 1

    TRISCbits.TRISC5 = 0;
    TRISCbits.TRISC4 = 1;
    TRISCbits.TRISC3 = 0;
    TRISCbits.TRISC7 = 0;
    TRISCbits.TRISC6 = 0;

    ANSELCbits.ANSC3 = 0;
    ANSELCbits.ANSC4 = 0;
    ANSELCbits.ANSC5 = 0;
    ANSELCbits.ANSC6 = 0;
    ANSELCbits.ANSC7 = 0;

    SSP1CON1bits.SSPEN = 0;
    SSP1CON1bits.WCOL = 0;
    SSP1CON1bits.SSPOV = 0;

    SSP1CON1bits.CKP = 0;
    SSP1CON1bits.SSPM = 0b0001;

    SSP1CON2 = 0b00000000;

    SSP1STAT = 0b00000000;
    SSP1STATbits.SMP = 1;
    SSP1STATbits.CKE = 0;

    SSP1CON3 = 0b00000000;
    SSP1MSK = 0b00000000;
    SSP1ADD = 0b00000000;

    // set up interrupt

    INTCONbits.GIE = 0;
    INTCONbits.PEIE = 0;
    INTCONbits.TMR0IE = 0;
    INTCONbits.INT0IE = 0;
    INTCONbits.RBIE = 0;
    INTCONbits.TMR0IF = 0;
    INTCONbits.INT0IF = 0;
    INTCONbits.RBIF = 0;

    INTCON2bits.RBPU = 1;
    INTCON2bits.INTEDG0 = 0;
    INTCON2bits.INTEDG1 = 0;
    INTCON2bits.INTEDG2 = 0;
    INTCON2bits.TMR0IP = 0;
    INTCON2bits.RBIP = 1;

    INTCON3bits.INT2IP = 0;
    INTCON3bits.INT1IP = 0;
    INTCON3bits.INT2IE = 0;
    INTCON3bits.INT1IE = 0;
    INTCON3bits.INT2IF = 0;
    INTCON3bits.INT1IF = 0;

    PIR1bits.ADIF = 0;
    PIR1bits.SSP1IF = 0;
    PIR1bits.CCP1IF = 0;
    PIR1bits.TMR2IF = 0;
    PIR1bits.TMR1IF = 0;
    PIR2bits. OSCFIF = 0;
    PIR2bits.C1IF = 0;
    PIR2bits.C2IF = 0;
    PIR2bits.EEIF = 0;
    PIR2bits.BCL1IF = 0;
    PIR2bits.HLVDIF = 0;
    PIR2bits.TMR3IF = 0;
    PIR2bits.CCP2IF = 0;

    PIR3bits.SSP2IF = 0;
    PIR3bits.BCL2IF = 0;
    PIR3bits.RC2IF = 0;
    PIR3bits.TX2IF = 0;
    PIR3bits.CTMUIF = 0;
    PIR3bits.TMR5GIF = 0;
    PIR3bits.TMR3GIF = 0;
    PIR3bits.TMR1GIF = 0;

    PIR4bits.CCP3IF = 0;
    PIR4bits.CCP4IF = 0;
    PIR4bits.CCP5IF = 0;

    PIR5bits.TMR4IF = 0;
    PIR5bits.TMR5IF = 0;
    PIR5bits.TMR6IF = 0;

    PIE1bits.ADIE = 0;
    PIE1bits.RC1IE = 0;
    PIE1bits.TX1IE = 0;
    PIE1bits.SSP1IE = 0;
    PIE1bits.CCP1IE = 0;
    PIE1bits.TMR2IE = 0;
    PIE1bits.TMR1IE = 0;

    PIE2bits.OSCFIE = 0;
    PIE2bits.C1IE = 0;
    PIE2bits.C2IE = 0;
    PIE2bits.EEIE = 0;
    PIE2bits.BCL1IE = 0;
    PIE2bits.HLVDIE = 0;
    PIE2bits.TMR3IE = 0;
    PIE2bits.CCP2IE = 0;

    PIE3bits.SSP2IE = 0;
    PIE3bits.BCL2IE = 0;
    PIE3bits.RC2IE = 0;
    PIE3bits.TX2IE = 0;
    PIE3bits.CTMUIE = 0;
    PIE3bits.TMR5GIE = 0;
    PIE3bits.TMR3GIE = 0;
    PIE3bits.TMR1GIE = 0;

    PIE4bits.CCP3IE = 0;
    PIE4bits.CCP4IE = 0;
    PIE4bits.CCP5IE = 0;

    PIE5bits.TMR4IE = 0;
    PIE5bits.TMR5IE = 0;
    PIE5bits.TMR6IE = 0;

    IPR1bits.ADIP = 0;
    IPR1bits.RC1IP = 0;
    IPR1bits.TX1IP = 0;
    IPR1bits.SSP1IP = 0;
    IPR1bits.CCP1IP = 0;
    IPR1bits.TMR2IP = 0;
    IPR1bits.TMR1IP = 0;

    IPR2bits.OSCFIP = 0;
    IPR2bits.C1IP = 0;
    IPR2bits.C2IP = 0;
    IPR2bits.EEIP = 0;
    IPR2bits.BCL1IP = 0;
    IPR2bits.HLVDIP = 0;
    IPR2bits.TMR3IP = 0;
    IPR2bits.CCP2IP = 0;

    IPR3bits.SSP2IP = 0;
    IPR3bits.BCL2IP = 0;
    IPR3bits.RC2IP = 0;
    IPR3bits.TX2IP = 0;
    IPR3bits.CTMUIP = 0;
    IPR3bits.TMR5GIP = 0;
    IPR3bits.TMR3GIP = 0;
    IPR3bits.TMR1GIP = 0;

    IPR4bits.CC3IP = 0;
    IPR4bits.CCP4IP = 0;
    IPR4bits.CCP5IP = 0;

    IPR5bits.TMR4IP = 0;
    IPR5bits.TMR5IP = 0;
    IPR5bits.TMR6IP = 0;
    
	    




    return;

}

