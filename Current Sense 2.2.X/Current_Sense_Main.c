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
void initSPI(void);
void initMCP(void);
void initTimer(void);
void pulseFoutPassThru(void);
void energyPulseCheck(void);
void readSerialData(void);
void powerCalculation(void);

void delayMS10(int count);

signed short voltageData = 0;
signed short currentData = 0;
unsigned long meterWatts = 0;
unsigned long meterEnergyUsed = 0;

volatile unsigned long timerCountHF = 0;
volatile unsigned long timerCountLF = 0;

bool dataRead = false;  // tells code if a complete set of data has been read from the MCP
bool dataAvailable = false;     // tells code if there is data available to read

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
        energyPulseCheck();     // get energy (Whr) data from MCP (pulses)
        readSerialData();       // get voltage and power data from MCP (SPI)
        powerCalculation();         // calculate power (W) data

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
    // Interrupt every 1 ms
    INTCONbits.TMR0IF = 0;

    TMR0H = TIMER_PRESET_HIGH;
    TMR0L = TIMER_PRESET_LOW;

    timerCountHF++;
    timerCountLF++;
    dataAvailable = true;

    return;
}

void energyPulseCheck(void)
{
    // here we check if a pulse has some in from the LF pulse
    // the timerCounters are in milli-seconds
    // if the timer prescaler or countdown is changed this will change the meaning of the timerCounters

#define ENERGY_PER_PULSE 27000 //(221.24 mWh per pulse)
#define ENERGY_PER_PULSE_UNIT 100000 // energy per pulse is divided by this to get Wh    

    static unsigned long meterEnergyUsedPart = 0;
    static bool mcpLFoutLast = false; // this is so we run a calc only once each time the pulse comes
    
    // LF out is for calculating energy in Watt-Hours (not watts)
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

/**
 * This code reads the data from the MCP when it is available. The code structure
 * is non-blocking, so it doesn't wait for data to be available, but instead
 * continues with the code and retrieves the data the next time this function is 
 * called.
 */
void readSerialData() {
    // booleans to create non-blocking SPI comm code
    static bool readingData = false;
    static bool byte1Read = false;
    static bool byte2Read = false;
    static bool byte3Read = false;
    // only look for data if there is data available OR if we are already reading data
    if (dataAvailable || readingData) {
        // Data is available but we're not reading it, so start communication
        if (!readingData) {
            SSP1BUF = 0xac;     // send dummy data to initiate communication
            readingData = true;
            dataRead = false;
            dataAvailable = false;
        }
        // Data has been read and the buffer is full, so we save the new data
        if (readingData && SSP1STATbits.BF) {
            char data = SSP1BUF;    // char is 8-bit data type, read data
            PIR1bits.SSP1IF = 0;    // Clear interrupt flag
            SSP1CON1bits.WCOL = 0;  // Clear write collision bit
            readingData = false;    // no longer reading data
            // Check which data has just been read
            if (!byte1Read) {   // MS byte of channel 1 data
                voltageData = (data << 8) & 0xFF;
                byte1Read = true;
            }
            else if (!byte2Read) {  // LS byte of channel 1 data
                voltageData = voltageData | data;
                byte2Read = true;
            }
            else if (!byte3Read) {  // MS byte of channel 0 data
                currentData = (data << 8) & 0xFF;
                byte3Read = true;
            }
            else if (!dataRead) {
                currentData = currentData | data;   // LS byte of channel 0 data
                // Last byte of data, reset all bools for next set of data
                byte1Read = false;
                byte2Read = false;
                byte3Read = false;
                dataRead = true;    // we have read a complete set of data, so 
            }                       // let calculation code know it's ready
        }
    } 
    return;
}

/**
 * This function contains all the calculation code for determining power in Watts
 */
void powerCalculation() {
    static int size = 14;   // number of samples we take per line cycle, max 233
    static int instantPower[14];    // this number must match size
    static int counter = 0;
    // These scalars convert the stepped-down voltage and current data read from the MCP into
    // the full values actually in use. Values determined by physical circuit.
    // ei: MCP sees 200mV, 120V actually present (numbers don't reflect reality)
    static int voltageScalar = 1;   // need to find these values
    static int currentScalar = 1;
    
    // Calculation of real power (average of instantaneous power over 1 cycle)
    if (dataRead) {
        instantPower[counter] = (voltageData * voltageScalar) * (currentData * currentScalar);
        counter++;
    }
    if (counter == size) {
        int realPower = 0;
        for (int i = 0; i < size; i++) {
            realPower =+ instantPower[i];
        }
        counter = 0;
        meterWatts = realPower / size;
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
    initOSC();
    initIO();
    initInterruptsClear();
    initSPI();
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

void initSPI(void) {
   
  // This function initializes all necessary registers to 
  // set up data direction bit, SPI mode, SPI clock
  // SPI Communication between the PIC and the MCP (on the Power sense board)
    
  // Set up SPI Master (PIC): mode and clock rate 
  // Using Module 1 -> SSP1, 
    
    // page 260 of data sheet
    // Set SSP1CON1 register, from bit 5 to 0
    SSP1CON1bits.SSPEN = 1;    // Enable serial port in SPI mode
    SSP1CON1bits.CKP = 1;       // Clock parity select bit: idle high level
    SSP1CON1bits.SSPM = 0000;      // SPI Master Mode, clock = FOSC/4
    
    // page 259 of data sheet
    // Set SSP1STAT register, only bit 7 and 6
    SSP1STATbits.SMP = 1;       // Input data sampled at end of data output time?
    SSP1STATbits.CKE = 0;      // Transmit occurs on transition from Idle to active clock state? 
 
    // set directions of SPI pins
    MCP_SPI_SDO_DIR = 0; // Set the direction of PIC pin as output for MCP
    MCP_SPI_CS_DIR = 0;  // Set the direction of PIC pin as output for MCP
    MCP_SPI_CLK_DIR = 0; // Set the direction of PIC pin as output for MCP
    MCP_SPI_SDI_DIR = 1; // Set the direction of PIC pin as input for MCP
}

void initMCP(void)
{
    // set MCLR as output
    MCP_MCLR_DIR = 0;  

    // reset MCP
    MCP_MCLR_SET = 0;   // Set the Master clear pin low
    __delay_us(1);
    MCP_MCLR_SET = 1;   // Set the Master clear pin high   
    MCP_SPI_CS_SET = 0; // Set the Chip select/F0 pin low to clock data in
    
    // Init SPI with command 0xac, for dual-channel output post-HPF
    SSP1BUF = 0b10101100;
    
    // NOTE: CS is set low the entire time. this slows the pulse speed but makes
    // SPI waveform data easier to read.
    // IF YOU WANT TO CHANGE THIS: make sure CS goes low when attempting to read
    // SPI data. Then go back to high state once data has been fully read.
    return;
}

