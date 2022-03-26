
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
// these are for the SPI reading of the MCP3909 when we want to try to use it
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
unsigned long meterEnergyUsedPart_module = 0; // this does not need to be global


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

	meterEnergyUsedPart_module = 0;

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

	//#define ENERGY_PER_PULSE 22124 //(221.24 mWh per pulse) - NOW CALIBRATION FACTOR VARIABLE
#define ENERGY_PER_PULSE_UNIT 100000UL // energy per pulse is divided by this to get Wh
#define HF_TO_LF_TIME_MS_THRESHOLD 1000 // at what level do we switch from using HF to LF to show and calc watts

	// check each of the pulse outputs from the MCP
	// 1 = inactive
	// 0 is active
	// time between transitions to 0

	unsigned long timerHFout_ms;
	unsigned long timerLFout_ms;

	static int meterWattsHF = 0;
	static unsigned long timerHFoutLast_ms = 0;
	static bool oneShotHFout = false;

	static int meterWattsLF = 0;
	static bool oneShotLFout = false;


	//	//TODO testing
	//	// generate a pulse for testing
	//#define TEST_PULSE_TIME	3516	// LF pulse time -- HF is LF / 16
	//	bool testPulseTriggerHF = false;
	//	bool testPulseTriggerLF = false;
	//	static unsigned long testNextPulseTimeHF = 0;
	//	static unsigned long testNextPulseTimeLF = 0;
	//	unsigned long testThisTime;
	//
	//	testThisTime = timerGetCount( 3 );
	//
	//	if( testThisTime >= testNextPulseTimeHF )
	//	{
	//		testPulseTriggerHF = true;
	//		testNextPulseTimeHF = testThisTime + ( TEST_PULSE_TIME / 16UL );
	//	}
	//	else
	//	{
	//		testPulseTriggerHF = false;
	//	}
	//
	//	if( testThisTime >= testNextPulseTimeLF )
	//	{
	//		testPulseTriggerLF = true;
	//		testNextPulseTimeLF = testThisTime + TEST_PULSE_TIME;
	//	}
	//	else
	//	{
	//		testPulseTriggerLF = false;
	//	}
	//



	//	if( ( MCP_HFOUT_READ == 0 ) || ( testPulseTrigger == true ) )
	//if( ( testPulseTriggerHF == true ) )
	if( MCP_HFOUT_READ == 0 )
	{

		if( oneShotHFout == false )
		{
			// TODO testing
			ledGoToggle( 1 );

			oneShotHFout = true;

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

	//if( ( testPulseTriggerLF == true ) )
	//	if( ( MCP_LFOUT0_READ == 0 ) || ( MCP_LFOUT1_READ == 0 ) || ( testPulseTriggerLF == true ) )
	if( ( MCP_LFOUT0_READ == 0 ) || ( MCP_LFOUT1_READ == 0 ) )
	{
		if( oneShotLFout == false )
		{
			// TODO testing
			ledGoToggle( 2 );

			oneShotLFout = true;

			//		unsigned long timerLFout_ms;
			timerLFout_ms = timerGetCount( 1 );
			timerResetCount( 1 );
			timerResetCount( 2 ); // the power reduce counter

			// TODO verify calculation
			meterWattsLF = powerCalculateWatts( timerLFout_ms, false );

			// with every pulse we add to energy used
			meterEnergyUsedPart_module += energyCalibration1_global * (unsigned long) 16;
			while( meterEnergyUsedPart_module > ENERGY_PER_PULSE_UNIT )
			{
				meterEnergyUsed_global++;
				meterEnergyUsedPart_module -= ENERGY_PER_PULSE_UNIT;
			}
		}
	}
	else
	{
		oneShotLFout = false;
	}

	// to prevent too much lag in showing power (watts) use the high frequency pulse when the
	// pulse rate is slow
	// switch to the low frequency pulse once within reasonable update time since it is more accurate
	// the meterWattsLF and meterWattsHF are static variables, so they always have values, even if not triggered on this loop
	if( timerLFout_ms < HF_TO_LF_TIME_MS_THRESHOLD )
	{
		meterWatts_global = meterWattsLF;
		// TODO testing
		ledGoSetOn( 3 );
	}
	else
	{
		meterWatts_global = meterWattsHF;
		// TODO testing
		ledGoSetOff( 3 );
	}

	// we need to reduce the power if there are no pulses
	// this will update the meterWatts_global variable in the function if needed
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
		calcWatts = ( ( ( ( energyCalibration2_global * (unsigned long) 3600UL ) / ( (unsigned long) ENERGY_PER_PULSE_UNIT / (unsigned long) 1000UL ) ) ) * (unsigned long) 1UL ) / (unsigned long) timer_ms;
	}
	else
	{
		// timer is from LFout
		calcWatts = ( ( ( ( energyCalibration1_global * (unsigned long) 3600UL ) / ( (unsigned long) ENERGY_PER_PULSE_UNIT / (unsigned long) 1000UL ) ) ) * (unsigned long) 1UL ) / (unsigned long) timer_ms;
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

	if( ( timerReduce_ms > POWER_REDUCTION_MAX_TIME ) || ( timerLast_ms == 0 ) )
	{
		meterWatts_global = 0;
	}
	else if( timerReduce_ms > timerLast_ms ) // we need to wait until the time is longer than the last pulse
	{
		meterWatts_global = powerCalculateWatts( timerReduce_ms, true );
	}

	return;
}

