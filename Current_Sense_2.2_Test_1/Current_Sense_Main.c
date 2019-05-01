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
//#include <p18f25k22.h>
#include <p18cxxx.h>
#include <xc.h>
#include "config.h" 
#include "Communications.h"

// HFout pulse input pin 5 - RA3
#define MCP_HFOUT_DIR TRISAbits.TRISA3           // Current HFout. new prototype
#define MCP_HFOUT_READ PORTAbits.RA3              // Current HFout. new prototype   

// Fout1 pulse input pin 6 - RA4
#define MCP_LFOUT_DIR_one TRISAbits.TRISA4                  //  Current Fout1, new prototype
#define MCP_LFOUT_READ_one PORTAbits.RA4                    //  Current Fout1. new prototype

// Fout0 pulse input pin 4 RA2
#define MCP_LFOUT_DIR_zero TRISAbits.TRISA2         //  Current Fout0, new prototype
#define MCP_LFOUT_READ_zero PORTAbits.RA2           //  Current Fout0, new prototype

// Fout pulse output (pass thru) pin 2
#define MCP_LFOUT_PASS_DIR TRISAbits.TRISA0     // not connected to anything
#define MCP_LFOUT_PASS_SET LATAbits.LATA0       // not connected to anything    

// MCP MCLR
// set pin 17 as an output for MCLR // good
#define MCP_MCLR_DIR TRISCbits.TRISC6
#define MCP_MCLR_SET LATCbits.LATC6
#define MCP_MCLR_READ PORTCbits.RC6


// MCP F0 Frequency Config // pin 18
#define MCP_FREQ_F0_DIR TRISCbits.TRISC7
#define MCP_FREQ_F0_SET LATCbits.LATC7

// MCP F1 Frequency Config // pin 15
#define MCP_FREQ_F1_DIR TRISCbits.TRISC4    // used to be C5
#define MCP_FREQ_F1_SET LATCbits.LATC4      // used to be C5

// MCP F2 Frequency Config // pin 14
#define MCP_FREQ_F2_DIR TRISCbits.TRISC3
#define MCP_FREQ_F2_SET LATCbits.LATC3


void init(void);
void initOSC(void);
void initIO(void);
void initInterruptsClear(void);
void initMCPFout(void);
void initTimer(void);
void pulseFoutPassThru(void);
void powerPulseCheck(void);
void toggleBlue(void);

//void mcpSPIInit( void );
//void mcpSPIStart( void );

void delayMS10(int count);

unsigned long meterWatts = 0;
unsigned long meterEnergyUsed = 0;

volatile unsigned long timerCountHF = 0;
volatile unsigned long timerCountLF = 0;


#define LOW_BYTE(x)     ((unsigned char)((x)&0xFF))
#define HIGH_BYTE(x)    ((unsigned char)(((x)>>8)&0xFF))

#define TIMER_COUNTDOWN 1000   // if timer is running at 1Mhz with prescaler, then 1000 will cause interrupt every 1ms, 500 will cause 2 interrupts every 1ms
// need to calc the timer preset because it only counts up and interrupts on rollover from 65535 to 0
// also need high and low bytes since we need to set timer preset one byte at a time
#define TIMER_PRESET ( 65536 - TIMER_COUNTDOWN )
#define TIMER_PRESET_LOW LOW_BYTE( TIMER_PRESET )
#define TIMER_PRESET_HIGH HIGH_BYTE( TIMER_PRESET)


// global
bool isLow = false; 

void main(void)
{
    init();
    
   // Set output for all LEDs 
   LED_DIR = 0;
   LED_DIR_OR = 0;
   LED_DIR_PR = 0;

   
   // Light all LEDs
   LED_SET = 1;             // blue
   LED_SET_PR = 1;          // purple
   LED_SET_OR = 1;          // orange
   
   // Set up directions for F0, F1, and F2
   MCP_FREQ_F0_DIR = 0;
   MCP_FREQ_F1_DIR = 0;
   MCP_FREQ_F2_DIR = 0;
   MCP_MCLR_DIR = 0;            // as output
   ANSELA = 0b00000000;         // Digital input buffer enabled for PORT A
   ANSELB = 0b00000000;         // Digital input buffer enabled for PORT A
   ANSELC = 0b00000000;         // Digital input buffer enabled for PORT A
   
   

   
   // Set up F0, F1, and F2
   MCP_MCLR_SET = 1;            // needs 5 V
   MCP_FREQ_F0_SET = 1;
   MCP_FREQ_F1_SET = 1;
   MCP_FREQ_F2_SET = 0;
   
//   MCP_LFOUT_SET_zero = 0;
   delayMS10(5);
    
   // To catch pulse, set it as input:
        MCP_LFOUT_DIR_zero = 1;
   
    LED_SET = 0;    
    LED_SET_PR = 0;          
    LED_SET_OR = 0; 
    
    bool isLow = false; 
    communications(true);
    

    
   while (1)
    {
     
        
//        // blinking orange
//        for(int i = 0; i <5; i++) {
//        LED_SET_OR = 1;
//        delayMS10(2);
//        LED_SET_OR = 0;
//        delayMS10(2); }
        
        
        toggleBlue();
        // toggle blue led as the speed of Fout 1 (same as yellow LEDs)
//       if(MCP_LFOUT_READ_zero == 0) {
//           if(isLow == false) {
//            LED_DIR_PR = 1;
//            if (LED_READ_PR == 0) 
//            {
//                LED_DIR_PR = 0;
//                LED_SET_PR = 1;}
//
//            else {
//                LED_DIR_PR = 0;
//                LED_SET_PR = 0;
//            }
//            isLow = true;
//            
//           }
//        }  
//        else{
//           isLow = false;
//        }
//       
//    if(isLow) {
//           LED_SET_PR = 0;
//       } else {
//           LED_SET_PR = 1;
//       }
//    }
    
//    for (int inx = 0; inx < 10; inx++)
//    {
//        LED_SET = 1;
//        delayMS10(10);
//        LED_SET = 0;
//        delayMS10(10);
//    }

    initTimer(); //

    //SPISlaveInit( );    // this is now done in the first communications call



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
//    communications(false);
    
//    for(int i = 0; i < 2; i++) {
//        LED_SET = 1;
//        LED_SET_OR = 1;
//        LED_SET_PR = 1;
//        delayMS10(100);
//        LED_SET = 0;
//        LED_SET_OR = 0;
//        LED_SET_PR = 0;
//        delayMS10(100);
//    }
    
//    meterWatts = 50;
//    meterEnergyUsed = 200;
    
    communications(false);
//    communications(true); // to init SPI communications everything
    
    // need to set up a counter to time the LED    
    // counting at 16Mhz is crazy
    // figure out good prescaler
    // determine if this needs to be cascading    

//    bool initDone = false;
//
//    while (1)
//    {
//        communications(false);
//        pulseFoutPassThru();
       powerPulseCheck();
   }
//        // reset MCP after 1 second
//        if (initDone == false)
//        {
//            if (timerCountLF > 1000)
//            {
//                initMCPFout();
//                initDone = true;
//
//                for (int inx = 0; inx < 10; inx++)
//                {
//                    LED_SET = 1;
//                    delayMS10(3);
//                    LED_SET = 0;
//                    delayMS10(5);
//                }
//
//
//            }
//        }

        //	if ( timerHFcount >= 1000 )
        //	{
        //	    count = 0;
        //	    if ( LED_READ == 1 )
        //	    {
        //		LED_SET = 0;
        //	    }
        //	    else
        //	    {
        //		LED_SET = 1;
        //
        //	    }
        //	}

        //        if(PORTAbits.RA1 == 1) {
        //            pulseOut = 0b1;
        //            //LATBbits.LATB4 = 0;
        //        }
        //        else {
        //            pulseOut = 0b0;
        //
        //            // for testing purposes only
        //            if (togglePulse == 0) {
                        //LED = 1; // turn on the LED
        //                togglePulse = 1;
        //                delayMS(100);
        //            } else {
        //                //LED = 0; // turn off the LED
        //                togglePulse = 0;
        //                delayMS(100);
        //            }
        //        }
//    }
    return;
}

void toggleBlue(void) 
{
//    bool isLow = false;
      if(MCP_LFOUT_READ_zero == 0) { // low frequency
           if(isLow == false) {
            LED_DIR = 1;
            if (LED_READ == 0) 
            {
                LED_DIR = 0;
                LED_SET = 1;
            }
            else {
                LED_DIR = 0;
                LED_SET = 0;
            }
            isLow = true;
            
           }
        }  
        else{
           isLow = false;
        }
     
          if(isLow) {
           LED_SET_PR = 0;
       } else {
           LED_SET_PR = 1;
       }
    
      
    // toggle blue led as the speed of Fout 1 (same as yellow LEDs)
//       if(MCP_LFOUT_READ_zero == 0) {
//           if(isLow == false) {
//            LED_DIR_PR = 1;
//            if (LED_READ_PR == 0) 
//            {
//                LED_DIR_PR = 0;
//                LED_SET_PR = 1;}
//
//            else {
//                LED_DIR_PR = 0;
//                LED_SET_PR = 0;
//            }
//            isLow = true;
//            
//           }
//        }  
//        else{
//           isLow = false;
//        }
//       
//    if(isLow) {
//           LED_SET_PR = 0;
//       } else {
//           LED_SET_PR = 1;
//       }
//    }
}






void pulseFoutPassThru(void)
{
    // mimic the pulse from the MCP Fout pins
    static bool runonce = false;

    if (MCP_HFOUT_READ == 0)
    {
        MCP_LFOUT_PASS_SET = 1;
        if (runonce == false)
        {
            runonce = true;
            if (LED_READ == 1)
            {
                LED_SET = 0;
            }
            else
            {
                LED_SET = 1;
            }
        }
    }
    else
    {
        MCP_LFOUT_PASS_SET = 0;
        runonce = false;
    }

    return;
}

void interrupt Timer0_ISR(void)
{

    INTCONbits.TMR0IF = 0;

    TMR0H = TIMER_PRESET_HIGH;
    TMR0L = TIMER_PRESET_LOW;

    timerCountHF++;
    timerCountLF++;

    return;
}



void powerPulseCheck(void)
{

    // here we check if a pulse has some in from both the HF and the LF pulses
    // the timerCounters are in milli-seconds
    // if the timer prescaler or countdown is changed this will change the meaning of the timerCounters



//#define ENERGY_PER_PULSE 27000 //(221.24 mWh per pulse)
#define ENERGY_PER_PULSE 22124 //(22.124 mWh per pulse)
//    #define ENERGY_PER_PULSE 2700     //(22.124 mWh per pulse)
#define ENERGY_PER_PULSE_UNIT 1000000 // energy per pulse is divided by this to get Wh    

    static unsigned long meterEnergyUsedPart = 0;
    static unsigned long timerCountHFLast = 2147483647;
    static unsigned int timerCountHFCheck = 1;
    static bool firstPulse = true;


    static bool mcpHFoutLast = false; // this is so we run a calc only once each time the pulse comes
    static bool mcpLFoutLast = false; // this is so we run a calc only once each time the pulse comes

    
    // HF output is for calculating watts
    if (MCP_HFOUT_READ == 0)
    {
        if (mcpHFoutLast == false)
        {
            mcpHFoutLast = true;
            firstPulse = false;

            timerCountHFLast = timerCountHF;
            timerCountHF = 0;
            meterWatts = (((((unsigned long) ENERGY_PER_PULSE * (unsigned long) 3600) / ((unsigned long) ENERGY_PER_PULSE_UNIT / (unsigned long) 1000))) * (unsigned long) 1) / (unsigned long) timerCountHFLast;
//            meterWatts = timerCountHFLast;
            

            timerCountHFCheck = 1; //reset the periodic power reduction
        }
    }
    else
    {
        mcpHFoutLast = false;
    }

    
    // if there is no power then no pulses
    // if our pulse time is greater than the last measurement we know we are at a lower power.
    // go ahead and calculate 
#define POWER_REDUCTION_INTERVAL 1000  //ms (1000 = 1 second ))

    if ((firstPulse == false) && (timerCountHF > timerCountHFLast))
    {
        if (timerCountHF > ((unsigned long) POWER_REDUCTION_INTERVAL * (unsigned long) timerCountHFCheck))
        {
            if (timerCountHFCheck < 90)
            {
                timerCountHFCheck++;
                meterWatts = (((((unsigned long) ENERGY_PER_PULSE * (unsigned long) 3600) / ((unsigned long) ENERGY_PER_PULSE_UNIT / (unsigned long) 1000))) * (unsigned long) 1) / (unsigned long) timerCountHF;
            }
            else
            {
                meterWatts = 0;
            }
            //          checkWattsHFvsLF = true;
        }
    }

    if (firstPulse == true)
    {
        meterWatts = 0;
    }

    
    // LF out is for calculating Watt-Hour (not watts)
    if (MCP_LFOUT_READ_zero == 0)
    {
        if (mcpLFoutLast == false)
        {
            mcpLFoutLast = true;
            timerCountLF = 0;
            
            meterEnergyUsedPart += ENERGY_PER_PULSE * (unsigned long) 16;
            while (meterEnergyUsedPart > ENERGY_PER_PULSE_UNIT)
            {
                meterEnergyUsed++;
                meterEnergyUsedPart -= ENERGY_PER_PULSE_UNIT;
            }

//            timerCountLF = 0;
        }
    }
    /*
     * Potential functionality for the meter to "sleep" if it hasn't
     * received a pulse in more than XXX number of seconds
     */
    else if(timerCountLF >= 200000) {
        
        LED_SET_PR = 1;
        
    }
    
    else if(timerCountLF >= 50000) {
        
        LED_SET_OR = 1;
        
    }
    
    else
    {
        mcpLFoutLast = false;
    }

    return;

}

void delayMS10(int count)
{
    for (int inx = 0; inx < count; inx++)
    {

        __delay_ms(10);
    }
}

void init()
{
    initOSC();  //
//    initIO();
//    initInterruptsClear();
//    initMCPFout();

    return;
}

void initOSC(void)
{
    // 16 Mhz internal
    OSCCONbits.IDLEN = 0;       // Device enters Sleep mode on SLEEP instruction
    OSCCONbits.IRCF = 0b111;    // 16 MHz (HFINTOSC drives clock directly)
    OSCCONbits.SCS = 0b11;      // Internal oscillator block

    OSCCON2bits.MFIOSEL = 0;
    OSCCON2bits.SOSCGO = 0;
    OSCCON2bits.PRISD = 0;

    OSCTUNEbits.INTSRC = 1;
    OSCTUNEbits.PLLEN = 0;

    return;
}

void initIO(void)
{
    ADCON0bits.ADON = 0; // ADC is disabled and consumes no operating current
    ANSELA = 0b00000000;
    ANSELB = 0b00000000;
    ANSELC = 0b00000000;

    LED_DIR = 0;
    LED_SET = 0;

    MCP_HFOUT_DIR = 1;
    MCP_LFOUT_DIR_zero = 1;
//    MCP_LFOUT_PASS_DIR = 0;
//    MCP_LFOUT_PASS_SET = 0;

    return;
}

void initInterruptsClear(void)
{

    INTCON = 0b00000000;
    INTCON2 = 0b00000000;
    INTCON3 = 0b00000000;

    RCONbits.IPEN = 0;

    PIR1 = 0b00000000;
    PIR2 = 0b00000000;
    PIR3 = 0b00000000;
    PIR4 = 0b00000000;
    PIR5 = 0b00000000;

    PIE1 = 0b00000000;
    PIE2 = 0b00000000;
    PIE3 = 0b00000000;
    PIE4 = 0b00000000;
    PIE5 = 0b00000000;

    IPR1 = 0b00000000;
    IPR2 = 0b00000000;
    IPR3 = 0b00000000;
    IPR4 = 0b00000000;
    IPR5 = 0b00000000;

}

void initTimer(void)
{

    T0CONbits.TMR0ON = 0; // turn off while configuring
    T0CONbits.T08BIT = 0; // 16 bit
    T0CONbits.T0CS = 0; // clock source internal
    T0CONbits.PSA = 0; // 0= use prescaler
    T0CONbits.T0PS = 0b001; // prescaler = 4 - gives us about 1Mhz since timer is already clocked at fosc/4

    INTCONbits.TMR0IE = 1; // enable timer 0 overflow interrupt
    INTCONbits.GIE = 1; // enable interrupts
    T0CONbits.TMR0ON = 1;

    // each interrupt should be ~1ms

}

void initMCPFout(void)
{
    // reset the MCP
    // wait until SPI timeout is reached before continuing

    MCP_MCLR_DIR = 0;
    MCP_FREQ_F0_DIR = 0;
    MCP_FREQ_F1_DIR = 0;
    MCP_FREQ_F2_DIR = 0;


//    MCP_FREQ_F0_SET = 0;
//    MCP_FREQ_F1_SET = 0;
//    MCP_FREQ_F2_SET = 0;
//
//    __delay_ms(5);
//    MCP_MCLR_SET = 0;
//    __delay_ms(5);
//
//    MCP_FREQ_F0_SET = 0;
//    MCP_FREQ_F1_SET = 0;
//    MCP_FREQ_F2_SET = 0;
//
//    __delay_ms(5);

    MCP_FREQ_F0_SET = 1;
    MCP_FREQ_F1_SET = 1;
    MCP_FREQ_F2_SET = 1;

//    __delay_ms(5);
//    MCP_MCLR_SET = 1; // 0 resets all registers
//    delayMS10(10);

    return;
}

