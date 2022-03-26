/****************
 INCLUDES
 only include the header files that are required
 ****************/
#include "common.h"
#include "LEDControl.h"
#include "Timer.h"
#include "MCP3909_Interface.h"
#include "SlaveComm.h"
#include "eeprom.h"

/****************
 MACROS
 ****************/

/****************
 VARIABLES
 these are the globals required by only this c file
 there should be as few of these as possible to help keep things clean
 variables required by other c functions should be here and also in the header .h file
 as external
 ****************/

// external
unsigned long meterWatts_global = 0;
unsigned long meterEnergyUsed_global = 0;

unsigned long energyCalibration1_global = 0;
unsigned long energyCalibration2_global = 0;

// internal only


/****************
 FUNCTION PROTOTYPES
 only include functions called from within this code
 external functions should be in the header
 ideally these are in the same order as in the code listing
 any functions used internally and externally (prototype here and in the .h file)
	 should be marked
 *****************/

void initOSC( void );
void initIO( void );
void initInterruptsClear( void );

/****************
 CODE
 ****************/

void main( void )
{
//	unsigned char eepromAddress;
//	volatile unsigned char x;
//	volatile unsigned char y;
//
//	eepromAddress = 0xE5;
//
//	x = eepromReadMy( eepromAddress );
//
//
//
//	y = 19;
//	eepromWriteMy( eepromAddress, y );
//
//	int a;
//	a = 0;









	initOSC( );
	initIO( );
	ledInit( );

	ledSetAllOff( );

	initInterruptsClear( );
	
	energyCalibration1_global = eepromCalibrate1Read();
	energyCalibration2_global = eepromCalibrate2Read();
	
	timerInit( );

	mcpInit( );


	for( int inx = 0; inx < 5; inx++ )
	{
		ledSetAllOn( );
		__delay_ms( 10 );
		ledSetAllOff( );
		__delay_ms( 10 );
	}


	commInit( );

	ledRun( 100 );

	while( 1 )
	{
		mcpUpdatePower( );
		commRun( );

		// in the following, code blocks {} are used
		// to encapsulate the nextRunTime variable
		// why
		// - otherwise we would need to declare separately named
		// nextRunTime variables outside the while(true) loop
		// the nextRunTime variables are local to each block
		// and self contained which makes the code leaner
		//
		// in the test to see if the timer is triggered we need to check for timer rollover
		// if the difference is really large then we rolled over and need to wait until the internal timer rolls over as well


		// isolate nextRunTime block
		{
			static unsigned long nextRunTime = 0;
			static unsigned long thisTimer;

			thisTimer = timerGetCount( 3 );

			// need to also check for timer rollover - that's the 10000000
			if( ( thisTimer > nextRunTime ) && ( ( thisTimer - nextRunTime ) < TIMER_ROLLOVER_CHECK ) )
			{
				nextRunTime = thisTimer + 500;
				if( nextRunTime > TIMER_ROLLOVER )
				{
					nextRunTime -= TIMER_ROLLOVER;
				}

				ledTestToggle( 1 );

			}

		} // isolate nextRunTime block

	}

}

void initOSC( void )
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

void initIO( void )
{
	ADCON0bits.ADON = 0;
	ANSELA = 0b00000000;
	ANSELB = 0b00000000;
	ANSELC = 0b00000000;

	return;

	// TODO verify this is taken care of elsewhere
	//    MCP_HFOUT_DIR = 1;
	//    MCP_LFOUT_DIR = 1;
	//    MCP_LFOUT_PASS_DIR = 0;
	//    MCP_LFOUT_PASS_SET = 0;
	//


}

void initInterruptsClear( void )
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

	return;
}
