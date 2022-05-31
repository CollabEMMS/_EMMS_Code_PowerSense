
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


#define FACTOR_HOURS_TO_MS	3600000UL // (3600) * (1000) unsigned long


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
unsigned long meterPowerFactor_module = 0;

/****************
 FUNCTION PROTOTYPES
 only include functions called from within this code
 external functions should be in the header
 ideally these are in the same order as in the code listing
 any functions used internally and externally (prototype here and in the .h file)
	 should be marked
 *****************/

void mcpInitF( void );
unsigned long powerCalculateWatts( unsigned long factorPower, unsigned long timer_ms, bool outHF );

/****************
 CODE
 ****************/
void mcpInit( void )
{
	MCP_HFOUT_DIR = 1;
	MCP_LFOUT0_DIR = 1;
	MCP_LFOUT1_DIR = 1;

	meterEnergyUsedPart_module = 0;
	meterPowerFactor_module = 0;

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

void mcpUpdateCalibrationFactors( void )
{
	if( (energyCalibration1_global > 0) && ( energyCalibration2_global > 0 ) )
	{
		unsigned long timeBaseFactor;
		unsigned long unitFactor;
		
		timeBaseFactor = FACTOR_HOURS_TO_MS;		// to convert hours to milliseconds
		unitFactor = energyCalibration2_global;
		
		while( ( ( timeBaseFactor % 10 ) == 0 ) && ( unitFactor >= 10 ) )
		{
			timeBaseFactor /= 10;
			unitFactor /= 10;
		}
		
		meterPowerFactor_module = (energyCalibration1_global * timeBaseFactor ) / unitFactor;
	}
	
	return;
}


void mcpUpdatePower( void )
{

	// use the HF pulse to calculate power (watts)
	// use the LF pulse to calculate energy (watt-hours)
	
		
	unsigned long factorEnergy;
	unsigned long factorEnergyUnits;
	unsigned long factorPower;
	
	// bring calibration values into local variables
	// this way if the meaning changes this code is easier to change
	factorEnergy = energyCalibration1_global;
	factorEnergyUnits = energyCalibration2_global;
	factorPower = meterPowerFactor_module;
	
#define HF_TO_LF_TIME_MS_THRESHOLD		1000		// at what level do we switch from using HF to LF to show and calc watts
#define POWER_REDUCTION_TIMEOUT			1000		// allow a bit of time beyond the last HF time before power reducing
#define POWER_REDUCTION_RATE			250			// if no pulse, update the power based on timer every X ms
#define POWER_REDUCTION_MAX_TIME		90000		// just zero the power after this much time without a pulse - no calcs needed since it is very close to 0 anyway
#define PULSE_WATCHDOG_TIMEOUT_RESET_MS	60000		// if we don't get pulses for this many ms, then reset the PIC because something might be wrong
	
	// check each of the pulse outputs from the MCP
	// 1 = inactive
	// 0 is active
	// time between transitions to 0

	unsigned long timerHFout_ms_static;
	unsigned long timerLFout_ms_static;
	static int meterWattsHF_static = 0;
	static unsigned long timerPowerReductionNextTime_static = 0;
	static bool oneShotHFout_static = false;

	static int meterWattsLF_static = 0;
	static bool oneShotLFout_static = false;

	unsigned long timerReduce_ms;

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
	//	if( testThisTime > 10000 )
	//	{
	//		testPulseTriggerHF = false;
	//		testPulseTriggerLF = false;
	//	}
	//

	//	if( ( MCP_HFOUT_READ == 0 ) || ( testPulseTrigger == true ) )
	//	if( ( testPulseTriggerHF == true ) )
	if( MCP_HFOUT_READ == 0 )	// 0 is for a pulse
	{
		if( oneShotHFout_static == false )
		{
			ledGoToggle( 1 );

			oneShotHFout_static = true;

			timerHFout_ms_static = timerGetCount( 0 );
			timerResetCount( 0 );
			timerResetCount( 2 ); // the power reduce counter

			meterWattsHF_static = powerCalculateWatts( factorPower, timerHFout_ms_static, true );

			timerPowerReductionNextTime_static = timerHFout_ms_static + POWER_REDUCTION_RATE;
		}
	}
	else
	{
		oneShotHFout_static = false;
	}

	// get a power from the LF Output
	// read both outputs

	//	if( ( MCP_LFOUT0_READ == 0 ) || ( MCP_LFOUT1_READ == 0 ) || ( testPulseTriggerLF == true ) )
	//if( ( testPulseTriggerLF == true ) )
	if( ( MCP_LFOUT0_READ == 0 ) || ( MCP_LFOUT1_READ == 0 ) )	// 0 is for a pulse - there are two outputs which alternate equally
	{
		if( oneShotLFout_static == false )
		{
			ledGoToggle( 2 );

			oneShotLFout_static = true;

			timerLFout_ms_static = timerGetCount( 1 );
			timerResetCount( 1 );
			timerResetCount( 2 ); // the power reduce counter

			meterWattsLF_static = powerCalculateWatts( factorPower, timerLFout_ms_static, false );

			// with every pulse we add to energy used accumulator then 
			meterEnergyUsedPart_module += factorEnergy;
			while( meterEnergyUsedPart_module > factorEnergyUnits )
			{
				meterEnergyUsed_global++;
				meterEnergyUsedPart_module -= factorEnergyUnits;
			}
		}
	}
	else
	{
		oneShotLFout_static = false;
	}


	// if no power is used, there are no pulses and the wattage will never be updated
	// if the current time is longer than the last pulse, then calculate a new power based on this time
	// we cannot just drop to zero because there may be some power being used - very low wattage. 4 watts can take 10-15 seconds between pulses
	// the power displayed, while not calculated on pulse time,is the most accurate depiction  of the power that could be used at this time
	//	for example
	//		if we are using 100 watts and then drop to 4 watts then a pulse will not come in until only 4 watts is used, which can take a long time
	//		if we calculate based on the no pulse time then the power will slowly drop down to 4 watts
	//	this also applies for 0 watts being used - slowly drops to 0
	//	on a maximum timeout we stop calculating and just use 0

	timerReduce_ms = timerGetCount( 2 );

	if( timerReduce_ms > ( timerHFout_ms_static + POWER_REDUCTION_TIMEOUT ) )	// we add a timeout to prevent nuisance 'tripping' of the power reduction
	{
		if( ( timerReduce_ms > POWER_REDUCTION_MAX_TIME ) || ( timerHFout_ms_static == 0 ) )
		{
			meterWatts_global = 0;
		}
		else if( timerReduce_ms > timerPowerReductionNextTime_static ) // we need to wait until the time is longer than the last pulse
		{
			meterWatts_global = powerCalculateWatts( factorPower, timerReduce_ms, true );
			timerPowerReductionNextTime_static += POWER_REDUCTION_RATE;
			// flash the led to indicate power reduction by time mode
			ledGoSetOn( 3 );
			__delay_us( 250 );
			ledGoSetOff( 3 );
		}
	}
	else
	{
		// to prevent too much lag in showing power (watts) use the high frequency pulse when the
		// pulse rate is slow
		// switch to the low frequency pulse once within reasonable update time since it is more accurate
		// the meterWattsLF and meterWattsHF are static variables, so they always have values, even if not triggered on this loop
		if( timerLFout_ms_static < HF_TO_LF_TIME_MS_THRESHOLD )
		{
			meterWatts_global = meterWattsLF_static;
			ledGoSetOn( 3 );
		}
		else
		{
			meterWatts_global = meterWattsHF_static;
			ledGoSetOff( 3 );
		}
	}

	// sometimes the Power Sense is not working right and we get no pulses when we expect them
	// if this happens, reset the PIC (kludge fix for now)
	// just use the    timerReduce_ms   since this contains the time since the last pulse
	
	if( timerReduce_ms > PULSE_WATCHDOG_TIMEOUT_RESET_MS )
	{
//		RESET();
		mcpInitF();
	}
	
	return;
}

unsigned long powerCalculateWatts( unsigned long factorPower, unsigned long timer_ms, bool outHF )
{

	// calc the meter watts here
	// need to figure out the difference between HFout and LFout calculations
	// ideally it is a simple multiplier, but maybe not
	unsigned long calcWatts;

	calcWatts = factorPower / timer_ms;
	
	if( outHF == true )
	{
		calcWatts = calcWatts / 8;
	}

	//	
//	if( outHF == true )
//	{
//		// timer is from HFout
//		calcWatts = ( ( ( ( energyCalibration2_global * (unsigned long) 3600UL ) / ( (unsigned long) ENERGY_PER_PULSE_UNIT / (unsigned long) 1000UL ) ) ) * (unsigned long) 1UL ) / (unsigned long) timer_ms;
//	}
//	else
//	{
//		// timer is from LFout
//		calcWatts = ( ( ( ( energyCalibration1_global * (unsigned long) 3600UL ) / ( (unsigned long) ENERGY_PER_PULSE_UNIT / (unsigned long) 1000UL ) ) ) * (unsigned long) 1UL ) / (unsigned long) timer_ms;
//	}

	return calcWatts;
}
