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
#include <p18f25k22.h>
#include <xc.h>
#include "config.h"
#include "Communications.h"


// HFout pulse input pin 13
#define MCP_HFOUT_DIR TRISCbits.TRISC2
#define MCP_HFOUT_READ PORTCbits.RC2

// Fout pulse input pin 3
#define MCP_LFOUT_DIR TRISAbits.TRISA1
#define MCP_LFOUT_READ PORTAbits.RA1

// Fout pulse output (pass thru) pin 2
#define MCP_LFOUT_PASS_DIR TRISAbits.TRISA0
#define MCP_LFOUT_PASS_SET LATAbits.LATA0

// MCP MCLR
// set pin 17 as an output for MCLR
#define MCP_MCLR_DIR TRISCbits.TRISC6
#define MCP_MCLR_SET LATCbits.LATC6


// MCP F0 Frequency Config
#define MCP_FREQ_F0_DIR TRISCbits.TRISC7
#define MCP_FREQ_F0_SET LATCbits.LATC7

// MCP F1 Frequency Config
#define MCP_FREQ_F1_DIR TRISCbits.TRISC5
#define MCP_FREQ_F1_SET LATCbits.LATC5

// MCP F2 Frequency Config
#define MCP_FREQ_F2_DIR TRISCbits.TRISC3
#define MCP_FREQ_F2_SET LATCbits.LATC3


void init(void);
void initOSC(void);
void initIO(void);
void initInterruptsClear(void);
void initMCP(void);
void initTimer(void);
void pulseFoutPassThru(void);
void powerPulseCheck(void);
short getSerialData(void);

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

void main(void)
{
    init();

    for (int inx = 0; inx < 10; inx++)
    {
        LED_SET = 1;
        delayMS10(10);
        LED_SET = 0;
        delayMS10(10);
    }

    initTimer();

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

    communications(true); // to init SPI communications everything

    // need to set up a counter to time the LED    
    // counting at 16Mhz is crazy
    // figure out good prescaler
    // determine if this needs to be cascading    

    bool initDone = false;

    while (1)
    {
        communications(false);
        pulseFoutPassThru();
        powerPulseCheck();

        // reset MCP after 1 second
        if (initDone == false)
        {
            if (timerCountLF > 1000)
            {
                initMCP();
                initDone = true;

                for (int inx = 0; inx < 10; inx++)
                {
                    LED_SET = 1;
                    delayMS10(3);
                    LED_SET = 0;
                    delayMS10(5);
                }


            }
        }

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
    return;
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



#define ENERGY_PER_PULSE 27000 //(221.24 mWh per pulse)
#define ENERGY_PER_PULSE_UNIT 100000 // energy per pulse is divided by this to get Wh    

    static unsigned long meterEnergyUsedPart = 0;
    static unsigned long timerCountHFLast = 2147483647;
    static unsigned int timerCountHFCheck = 1;
    static bool firstPulse = true;


    static bool mcpHFoutLast = false; // this is so we run a calc only once each time the pulse comes
    static bool mcpLFoutLast = false; // this is so we run a calc only once each time the pulse comes

    
    // Checking SDO data line
    
    unsigned short voltageData = 0;
    unsigned short currentData = 0;
    
    if (MCP_SPI_SDI_READ == 1){         // This is the data-ready output flag. If 1, data is ready to read
        voltageData = getSerialData();
        currentData = getSerialData();
        // this is just to test that we are seeing data come in
        meterWatts = voltageData;
        
        // calculation code needs to be added
    }

    
    // LF out is for calculating Watt-Hour (not watts)
    if (MCP_LFOUT_READ == 0)
    {
        if (mcpLFoutLast == false)
        {
            mcpLFoutLast = true;

            meterEnergyUsedPart += ENERGY_PER_PULSE * (unsigned long) 16;
            while (meterEnergyUsedPart > ENERGY_PER_PULSE_UNIT)
            {
                meterEnergyUsed++;
                meterEnergyUsedPart -= ENERGY_PER_PULSE_UNIT;
            }

            timerCountLF = 0;
        }
    }
    else
    {
        mcpLFoutLast = false;
    }

    return;

}
    
short getSerialData() {
    char dataIn[16];
    bool negNumber = false;
    __delay_us(1); 
    for (int i = 0; i < 16; i++) {
        MCP_FREQ_F2_SET = 1;
        // read the serial data line
        if (MCP_SPI_SDI_READ == 1) {
            dataIn[i] = 1;
        }
        else {
            dataIn[i] = 0;
        }
        // if code is negative, invert the code to make it positive. should 
        // never run for MSB of code
        if (negNumber) {
            dataIn[i] = !dataIn[i];
        }
        MCP_FREQ_F2_SET = 0;
        // check to see if the code is a negative number in 2's compliment
        if ((i == 0) && (dataIn[i] == 1)) {
            negNumber = true;
        }
    }
    // dataIn[0] is sign bit. Since we have inverted the code if it is negative,
    // we can ignore the sign bit and add 1 if the code was negative, giving us 
    // the absolute value of the code
    short data = dataIn[1] * 16384
            + dataIn[2] * 8192
            + dataIn[3] * 4096
            + dataIn[4] * 2048
            + dataIn[5] * 1024
            + dataIn[6] * 512
            + dataIn[7] * 256
            + dataIn[8] * 128
            + dataIn[9] * 64
            + dataIn[10] * 32
            + dataIn[11] * 16
            + dataIn[12] * 8
            + dataIn[13] * 4
            + dataIn[14] * 2
            + dataIn[15] * 1;
    if (negNumber) {
        data = data + 1;
    }        
    return data;
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
    initOSC();
    initIO();
    initInterruptsClear();
    initMCP();

    return;
}

void initOSC(void)
{
    // 16 Mhz internal
    OSCCONbits.IDLEN = 0;
    OSCCONbits.IRCF = 0b111;
    OSCCONbits.SCS = 0b11;

    OSCCON2bits.MFIOSEL = 0;
    OSCCON2bits.SOSCGO = 0;
    OSCCON2bits.PRISD = 0;

    OSCTUNEbits.INTSRC = 1;
    OSCTUNEbits.PLLEN = 0;

    return;
}

void initIO(void)
{
    ADCON0bits.ADON = 0;
    ANSELA = 0b00000000;
    ANSELB = 0b00000000;
    ANSELC = 0b00000000;

    LED_DIR = 0;
    LED_SET = 0;

    MCP_HFOUT_DIR = 1;
    MCP_LFOUT_DIR = 1;
    MCP_LFOUT_PASS_DIR = 0;
    MCP_LFOUT_PASS_SET = 0;

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

void initMCP(void)
{
    // reset the MCP
    // wait until SPI timeout is reached before continuing
    
    // set MCLR as output
    MCP_MCLR_DIR = 0;
    
    // set frequency control pins as outputs
    MCP_FREQ_F0_DIR = 0;
    MCP_FREQ_F1_DIR = 0;
    MCP_FREQ_F2_DIR = 0;
    
    // set directions of SPI pins
    MCP_SPI_SDO_DIR = 0; // Set the direction of PIC pin as output for MCP
    MCP_SPI_CS_DIR = 0;  // Set the direction of PIC pin as output for MCP
    MCP_SPI_CLK_DIR = 0; // Set the direction of PIC pin as output for MCP
    MCP_SPI_SDI_DIR = 1; // Set the direction of PIC pin as input for MCP
    
    // Init SPI with command 0xac, for dual-channel output post-HPF
    int initSPICommand[8] = {1, 0, 1, 0, 1, 1, 0, 0};
    MCP_MCLR_SET = 0;   // Set the Master clear pin low
    __delay_us(1);      // delay for 1us to ensure MCP resets
    MCP_MCLR_SET = 1;   // Set the Master clear pin high
    // Select MCP       
    MCP_SPI_CS_SET = 0; // Set the Chip select/F0 pin low to clock data in
    // Send SPI code
    for (int i = 0; i < 8; i++) {
        MCP_SPI_CLK_SET = 0;                 // start the clock at 0, after 1st bit clock data into MCP
        MCP_SPI_SDO_SET = initSPICommand[i]; // get data ready 
                                             // MCP_SPI_SDO_SET = MCP_FREQ_F1_SET = MCP's SPI pin
                                             // Setting the SDI pin of MCP by setting MCP_SPI_SDO_SET on the PIC 
        MCP_SPI_CLK_SET = 1;                 // pass data into SDI of MCP when clock goes high
    }
    MCP_SPI_CLK_SET = 0;
    
    return;
}

void initSPI(void)
{
  // This function initializes all necessary registers to 
  // set up data direction bit, SPI mode, SPI clock
  // SPI Communication between the PIC and the MCP (on the Power sense board)
    
  // Set up SPI Master (PIC): mode and clock rate 
  // Using Module 1 -> SSP1, 
    
    // page 260 of data sheet
    // Set SSP1CON1 register, from bit 5 to 0
    SSP1CON1.SSP1EN = 1;    // Enable serial port in SPI mode
    SSP1CON1.CKP = 1;       // Clock parity select bit: idle high level
    SSP1CON1.M = 0000;      // SPI Master Mode, clock = FOSC/4
    
    // page 259 of data sheet
    // Set SSP1STAT register, only bit 7 and 6
    SSP1STAT.SMP = 1;       // Input data sampled at end of data output time?
    SSP1STAT.CKE = 0;      // Transmit occurs on transition from Idle to active clock state? 
    
   // Set up data direction register, using SPI Port 1 for communication between MCP and PIC18
    // SCK1 connects to pin 14: RC3
    TRISC.TRISC3 = 0;        // Output, master clock
    
    // SDI1 connects to pin 15: RC4
    TRISC.TRISC4 = 1;        // MSSP1 SPI data input. 
    
    // SDO1 connects to pin 16: RC5
    TRISC.TRISC5 = 0;        // MSSP1 SPI data output.
    
    // SS1 connects to pin 18:  RC7
    TRISC.TRISC7 = 1;        //  Slave Select set page 215
    
    
    
    
    
}