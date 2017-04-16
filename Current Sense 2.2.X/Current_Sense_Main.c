/*
 * File:   newmain.c
 * Author: gt1165
 *
 * Created on November 6, 2015, 3:33 PM
 */
#include "pragma.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "config.h"
#include "Communications.h"

//#include <p18cxxx.h>
#include <p18f25k22.h>


// PIC18F25K22 Configuration Bit Settings

// 'C' source line config statements

#include <xc.h>



#define LED LATBbits.LATB4
#define LEDDIR TRISBbits.RB4

#define pulseOut LATAbits.LATA0
#define pulseIn LATAbits.LATA1

/*
 *
 */
//void delayMS(unsigned int a);
/*
 *
 */

/*
 * This is the init funciton for the current sense code
 */
void init(void);
void delayMS10( int count );


void main(void) {
    unsigned int togglePulse = 0;
    init();

    SPISlaveInit( );

    for( int inx = 0; inx < 10; inx++)
    {
	LED = 1;
	delayMS10( 10);
	LED = 0;
	delayMS10( 10);
    }
    
    
    /*
     * I'm just trying to simulate the current sense board since mine is
     * blown right now. This will be a loop that just outputs a "pulse" that
     * hopefully will be somewhat around the same as a 1kW load. It will be
     * logic 1 (5v) and drop down to zero quickly, and then stay up at logic
     * 1 again.
    while(1) {
        delayMS(250);
        for(;;) {
            pulseOut = 1;
            LED = 1;
            delayMS(10000);

            pulseOut = 0;
            LED = 0;
            delayMS(5);
        }
    }
    */

    // Take the pulse and send it out to the command board

    communications( true ); // to init everything
    
    
    while(1) {
	communications( false );
	
//        if(PORTAbits.RA1 == 1) {
//            pulseOut = 0b1;
//            //LATBbits.LATB4 = 0;
//        }
//        else {
//            pulseOut = 0b0;
//
//            // for testing purposes only
//            if (togglePulse == 0) {
//                //LED = 1; // turn on the LED
//                togglePulse = 1;
//                delayMS(100);
//            } else {
//                //LED = 0; // turn off the LED
//                togglePulse = 0;
//                delayMS(100);
//            }
//        }
    }
    return;// (EXIT_SUCCESS);
}

void delayMS10( int count )
{
    for( int inx = 0; inx< count; inx++)
    {
	__delay_ms(10);
    }
}

void init() {

    OSCCONbits.IDLEN = 0;
    OSCCONbits.IRCF = 0b111;
    OSCCONbits.SCS = 0b11;
    
    OSCCON2bits.MFIOSEL = 0;
    OSCCON2bits.SOSCGO = 0;
    OSCCON2bits.PRISD = 0;
    
    OSCTUNEbits.INTSRC = 1;
    OSCTUNEbits.PLLEN = 0;
    
    
    TRISAbits.TRISA0 = 0; // pin 2 connected as an output for pulse
    TRISAbits.TRISA1 = 1; // pin 3 connected as an input for pulse
    LEDDIR = 0; // pin 25 connected as an output for LED
//    TRISCbits.TRISC3 = 0; // pin 14 connected as an output for pulse freq.
//    TRISCbits.TRISC5 = 0; // pin 16 connected as an output for pulse freq.
    TRISCbits.TRISC6 = 0; // set pin 17 as an output for MCLR
//    TRISCbits.TRISC7 = 0; // set pin 18 as an output for pulse freq.
    ANSELAbits.ANSA1 = 0b0; // turn off analog to digital conversion

    LATCbits.LATC6 = 1; // set the MCLR of the MCP high
//    LATCbits.LATC3 = 1; // set pin 14 to a 1 to set freq. control F2 for pulse
//    LATCbits.LATC5 = 1; // set pin 16 to a 1 to set freq. control F1 for pulse
//    LATCbits.LATC7 = 1; // set pin 18 to a 1 to set freq. control F0 for pulse
    

            // flashing loop 15 times to make sure it works
//        unsigned int i;
//        for (i = 0; i < 5; i++)  {
//            LED = 1; // turn on the LED
//            delayMS(25);
//            LED = 0; // turn off the LED
//            delayMS(25);
//        }
    
}

//void delayMS(unsigned int a) {
//    unsigned int i, j;
//
//    for (i = 0; i < a; i++)
//        for (j = 0; j < 100; j++); // delay for 1ms
//}
