
/****************
 INCLUDES
 only include the header files that are required
 ****************/

#include "common.h"
#include "Main_PowerSense.h"
#include "Timer.h"
#include "LEDControl.h"

/****************
 MACROS
 ****************/

// pin 1    MCLR Programming
// pin 2    NC
// pin 3    NC
// pin 4    MCP_Fout0
// pin 5    MCP HFout
// pin 6    MCP Fout1
// pin 7    NC
// pin 8    GND
// pin 9    NC
// pin 10   NC
// pin 11   LED
// pin 12   LED
// pin 13   NC
// pin 14   MCP F2
// pin 15   MCP F1
// pin 16   MCP ??????????
// pin 17   MCP MCLR
// pin 18   MCP F0
// pin 19   GND
// pin 20   +5V
// pin 21   CS
// pin 22   SPI_CLK
// pin 23   SPI_MISO
// pin 24   SPI_MOSI
// pin 25   LED
// pin 26   NC
// pin 27   PGM
// pin 28   PGM




// Fout0	pin 4	RA2
// Fout1	pin 6	RA4
// HFout	pin 5	RA3
// F0		pin 18	RC7
// F1		pin 15	RC4
// F2		pin 14	RC3
// MCLR		pin 17	RC6


// MCP LFout0
// pin 2    RA2
#define MCP_LFOUT0_DIR TRISAbits.TRISA2
#define MCP_LFOUT0_READ PORTAbits.RA2

// MCP LFout1
// pin 6    RA4
#define MCP_LFOUT1_DIR TRISAbits.TRISA4
#define MCP_LFOUT1_READ PORTAbits.RA4

// MCP HFout
// pin 5    RA3
#define MCP_HFOUT_DIR TRISAbits.TRISA3
#define MCP_HFOUT_READ PORTAbits.RA3

// MCP F0 Frequency Config
// pin 18   RC7
#define MCP_F0_DIR TRISCbits.TRISC7
#define MCP_F0_SET LATCbits.LC7
#define MCP_F0_READ PORTCbits.RC7

// MCP F1 Frequency Config
// pin 15   RC4
#define MCP_F1_DIR TRISCbits.TRISC4
#define MCP_F1_SET LATCbits.LC4
#define MCP_F1_READ PORTCbits.RC4

// MCP F2 Frequency Config
// pin 14   RC3
#define MCP_F2_DIR TRISCbits.TRISC3
#define MCP_F2_SET LATCbits.LC4
#define MCP_F2_READ PORTCbits.RC4


// MCP MCLR
// pin 17   RC6
#define MCP_MCLR_DIR  TRISCbits.TRISC6
#define MCP_MCLR_SET LATCbits.LATC6
#define MCP_MCLR_READ PORTCbits.RC6







// the below needs to be verified before it is used
//// MCP SPI MSDO
//// set pin 16 as an output for SPI
//#define MCP_SPI_MSDO_DIR TRISCbits.TRISC5
//
//// MCP SPI MSDI
//// set pin 15 as an input for SPI
//#define MCP_SPI_MSDI_DIR TRISCbits.TRISC4
//
//// MCP SPI MSLK
//// set pin 14 as the SPI clock SCK
//#define MCP_SPI_MSCK_DIR TRISCbits.TRISC3
//
//// MCP SPI CS
//// set pin 18 as the SPI CS for MCP
//#define MCP_SPI_CS_DIR TRISCbits.TRISC7
//#define MCP_SPI_CS_SET LATCbits.LATC7





/****************
 VARIABLES
 these are the globals required by only this c file
 there should be as few of these as possible to help keep things clean
 variables required by other c functions should be here and also in the header .h file
 as external
 ****************/

// external

// internal only
unsigned long meterEnergyUsedPart = 0; // this does not need to be global


/****************
 FUNCTION PROTOTYPES
 only include functions called from within this code
 external functions should be in the header
 ideally these are in the same order as in the code listing
 any functions used internally and externally (prototype here and in the .h file)
     should be marked
 *****************/

void mcpInitF( void );
unsigned long powerCalculateWatts( unsigned long timer_ms, bool outHF );
void powerReduction( unsigned long timerLast_ms );

/****************
 CODE
 ****************/
void mcpInit( void )
{
    MCP_HFOUT_DIR = 1;
    MCP_LFOUT0_DIR = 1;
    MCP_LFOUT1_DIR = 1;

    meterEnergyUsedPart = 0;

    delayMS10( 100 );

    mcpInitF( );

    return;
}

void mcpInitF( void )
{
    // reset the MCP
    // wait until SPI timeout is reached before continuing

    MCP_MCLR_DIR = 0;
    MCP_F0_DIR = 0;
    MCP_F1_DIR = 0;
    MCP_F2_DIR = 0;


    MCP_F0_SET = 0;
    MCP_F1_SET = 0;
    MCP_F2_SET = 0;

    __delay_ms( 5 );
    MCP_MCLR_SET = 0;
    __delay_ms( 5 );

    MCP_F0_SET = 0;
    MCP_F1_SET = 0;
    MCP_F2_SET = 0;

    __delay_ms( 5 );

    MCP_F0_SET = 1;
    MCP_F1_SET = 1;
    MCP_F2_SET = 1;

    __delay_ms( 5 );
    MCP_MCLR_SET = 1;
    delayMS10( 10 );

    return;
}

void mcpUpdatePower( void )
{

#define ENERGY_PER_PULSE 27000 //(221.24 mWh per pulse)
#define ENERGY_PER_PULSE_UNIT 100000 // energy per pulse is divided by this to get Wh
#define HF_TO_LF_WATTS_THRESHOLD 500 // at what level do we switch from using HF to LF to show and calc watts

    // check each of the pulse outputs from the MCP
    // 1 = inactive
    // 0 is active
    // time between transitions to 0


    // get a power from the HF Output
    static int meterWattsHF = 0;
    static unsigned long timerHFoutLast_ms = 0;

    static bool oneShotHFout = false;

    if( MCP_HFOUT_READ == 0 )
    {
	if( oneShotHFout == false )
	{
	    // TODO testing
	    ledGoToggle( 0 );

	    oneShotHFout = true;

	    unsigned long timerHFout_ms;
	    timerHFout_ms = timerGetCount( 0 );
	    timerResetCount( 0 );
	    timerResetCount( 2 ); // the power reduce counter

	    meterWattsHF = powerCalculateWatts( timerHFout_ms, true );

	    timerHFoutLast_ms = timerHFout_ms; // store the last time for a pulse - if we go longer than this without a pulse, calc a new power value
	}
    }
    else
    {
	oneShotHFout = false;
    }

    // get a power from the LF Output
    // read both outputs
    static int meterWattsLF = 0;

    static bool oneShotLFout = false;

    if( (MCP_LFOUT0_READ == 0) || (MCP_LFOUT1_READ == 0) )
    {
	if( oneShotLFout == false )
	{
	    // TODO testing
	    ledGoToggle( 1 );

	    oneShotLFout = true;

	    unsigned long timerLFout_ms;
	    timerLFout_ms = timerGetCount( 1 );
	    timerResetCount( 1 );
	    timerResetCount( 2 ); // the power reduce counter

	    // TODO verify calculation
	    meterWattsLF = powerCalculateWatts( timerLFout_ms, false );

	    // with every pulse we add to energy used
	    meterEnergyUsedPart += ENERGY_PER_PULSE * (unsigned long) 16;
	    while( meterEnergyUsedPart > ENERGY_PER_PULSE_UNIT )
	    {
		meterEnergyUsed_global++;
		meterEnergyUsedPart -= ENERGY_PER_PULSE_UNIT;
	    }
	}
    }
    else
    {
	oneShotLFout = false;
    }

    // TODO choose which to store as power - LF or HF
    // be careful that we do this only if we have a new power calculated

    // the meterWatts variables are static in this function
    // so there will always be values in them
    // if the HFwatts is > set number then switch to LFwatts

    // TODO will the LFwatts keep up with HFwatts - when we switch will the new number be ready?
    if( meterWattsLF > HF_TO_LF_WATTS_THRESHOLD )
    {
	meterWatts_global = meterWattsLF;
    }
    else
    {
	meterWatts_global = meterWattsHF;
    }

    // we need to reduce the power if there are no pulses
    powerReduction( timerHFoutLast_ms );

    return;
}

unsigned long powerCalculateWatts( unsigned long timer_ms, bool outHF )
{

    // calc the meter watts here
    // need to figure out the difference between HFout and LFout calculations
    // ideally it is a simple multiplier, but maybe not
    int calcWatts;

    if( outHF == true )
    {
	// timer is from HFout
	calcWatts = (((((unsigned long) ENERGY_PER_PULSE * (unsigned long) 3600) / ((unsigned long) ENERGY_PER_PULSE_UNIT / (unsigned long) 1000))) * (unsigned long) 1) / (unsigned long) timer_ms;
    }
    else
    {
	// timer is from LFout
	calcWatts = (((((unsigned long) ENERGY_PER_PULSE * (unsigned long) 3600) / ((unsigned long) ENERGY_PER_PULSE_UNIT / (unsigned long) 1000))) * (unsigned long) 1) / (unsigned long) timer_ms;
    }

    return calcWatts;
}

void powerReduction( unsigned long timerLast_ms )
{
    // if there are no pulses we need a way to reflect that the power is lower
    // so, if the pulse time is > than the pulse time last used to calc the power
    // then recalc the power using the current elapsed time since the last pulse

#define POWER_REDUCTION_INTERVAL 1000  //ms (1000 = 1 second ))
#define POWER_REDUCTION_MAX_TIME 90000 //ms time before we zero the power

    unsigned long timerReduce_ms;
    static int countReduce = 0;

    timerReduce_ms = timerGetCount( 2 );

    if( (timerReduce_ms > POWER_REDUCTION_MAX_TIME) || (timerLast_ms == 0) )
    {
	meterWatts_global = 0;
    }
    else if( timerReduce_ms > timerLast_ms ) // we need to wait until the time is longer than the last pulse
    {
	meterWatts_global = (((((unsigned long) ENERGY_PER_PULSE * (unsigned long) 3600) / ((unsigned long) ENERGY_PER_PULSE_UNIT / (unsigned long) 1000))) * (unsigned long) 1) / (unsigned long) timerReduce_ms;
    }

    return;
}

//
//void powerPulseCheck( void )
//{
//
//    // here we check if a pulse has some in from both the HF and the LF pulses
//    // the timerCounters are in milli-seconds
//    // if the timer prescaler or countdown is changed this will change the meaning of the timerCounters
//
//
//
//#define ENERGY_PER_PULSE 27000 //(221.24 mWh per pulse)
//#define ENERGY_PER_PULSE_UNIT 100000 // energy per pulse is divided by this to get Wh
//
//    static unsigned long meterEnergyUsedPart = 0;
//    static unsigned long timerCountHFLast = 2147483647;
//    static unsigned int timerCountHFCheck = 1;
//    static bool firstPulse = true;
//
//
//    static bool mcpHFoutLast = false; // this is so we run a calc only once each time the pulse comes
//    static bool mcpLFoutLast = false; // this is so we run a calc only once each time the pulse comes
//
//
//    // HF output is for calculating watts
//    if( MCP_HFOUT_READ == 0 )
//    {
//	if( mcpHFoutLast == false )
//	{
//	    mcpHFoutLast = true;
//	    firstPulse = false;
//
//	    timerCountHFLast = timerCountHF;
//	    timerCountHF = 0;
//	    meterWatts = (((((unsigned long) ENERGY_PER_PULSE * (unsigned long) 3600) / ((unsigned long) ENERGY_PER_PULSE_UNIT / (unsigned long) 1000))) * (unsigned long) 1) / (unsigned long) timerCountHFLast;
//	    //            meterWatts = timerCountHFLast;
//
//
//	    timerCountHFCheck = 1; //reset the periodic power reduction
//	}
//    }
//    else
//    {
//	mcpHFoutLast = false;
//    }
//
//
//    // if there is no power then no pulses
//    // if our pulse time is greater than the last measurement we know we are at a lower power.
//    // go ahead and calculate
//#define POWER_REDUCTION_INTERVAL 1000  //ms (1000 = 1 second ))
//
//    if( (firstPulse == false) && (timerCountHF > timerCountHFLast) )
//    {
//	if( timerCountHF > ((unsigned long) POWER_REDUCTION_INTERVAL * (unsigned long) timerCountHFCheck) )
//	{
//	    if( timerCountHFCheck < 90 )
//	    {
//		timerCountHFCheck++;
//		meterWatts = (((((unsigned long) ENERGY_PER_PULSE * (unsigned long) 3600) / ((unsigned long) ENERGY_PER_PULSE_UNIT / (unsigned long) 1000))) * (unsigned long) 1) / (unsigned long) timerCountHF;
//	    }
//	    else
//	    {
//		meterWatts = 0;
//	    }
//	    //          checkWattsHFvsLF = true;
//	}
//    }
//
//    if( firstPulse == true )
//    {
//	meterWatts = 0;
//    }
//
//
//    // LF out is for calculating Watt-Hour (not watts)
//    if( MCP_LFOUT_READ == 0 )
//    {
//	if( mcpLFoutLast == false )
//	{
//	    mcpLFoutLast = true;
//
//	    meterEnergyUsedPart += ENERGY_PER_PULSE * (unsigned long) 16;
//	    while( meterEnergyUsedPart > ENERGY_PER_PULSE_UNIT )
//	    {
//		meterEnergyUsed++;
//		meterEnergyUsedPart -= ENERGY_PER_PULSE_UNIT;
//	    }
//
//	    timerCountLF = 0;
//	}
//    }
//    else
//    {
//	mcpLFoutLast = false;
//    }
//
//    meterWatts = 777;
//
//    return;
//}
//


