/****************
 INCLUDES
 only include the header files that are required
 ****************/
#include "common.h"
#include "LEDControl.h"
#include "Timer.h"
#include "MCP3909_Interface.h"
#include "SlaveComm.h"

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
    // TODO Testing
    //    ANSELA = 0;
    //    ANSELB = 0;
    //    ANSELC = 0;
    //
    //
    //
    //#define LED1_DIR    TRISBbits.TRISB4
    //#define LED1_SET    LATBbits.LATB4
    //#define LED1_READ   PORTBbits.RB4
    //
    //#define LED2_DIR    TRISCbits.TRISC0
    //#define LED2_SET    LATCbits.LATC0
    //#define LED2_READ   PORTCbits.RC0
    //
    //#define LED3_DIR    TRISCbits.TRISC1
    //#define LED3_SET    LATCbits.LATC1
    //#define LED3_READ   PORTCbits.RC1
    //
    //    LED1_DIR = 0;
    //    LED2_DIR = 0;
    //    LED3_DIR = 0;
    //
    //    LED1_SET = 0;
    //    LED2_SET = 1;
    //    LED3_SET = 0;
    //
    //
    //    while( 1 );


    // TODO end testing

    initOSC( );
    initIO( );
    ledInit( );

    ledSetAllOff( );

    initInterruptsClear( );
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

	// TODO testing heartbeat
	{
	    static bool oneShot = false;
	    unsigned long timerHeartbeat;
	    timerHeartbeat = timerGetCount( 3 );
	    if( (timerHeartbeat % 500) == 0 )
	    {
		if( oneShot == false )
		{
		    oneShot = true;
		    ledTestToggle( 1 );
		}

	    }
	    else
	    {
		oneShot = false;
	    }
	}
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



//// HFout pulse input pin 13
//#define MCP_HFOUT_DIR TRISCbits.TRISC2
//#define MCP_HFOUT_READ PORTCbits.RC2
//
//// Fout pulse input pin 3
//#define MCP_LFOUT_DIR TRISAbits.TRISA1
//#define MCP_LFOUT_READ PORTAbits.RA1
//
//// Fout pulse output (pass thru) pin 2
//#define MCP_LFOUT_PASS_DIR TRISAbits.TRISA0
//#define MCP_LFOUT_PASS_SET LATAbits.LATA0
//
//// MCP MCLR
//// set pin 17 as an output for MCLR
//#define MCP_MCLR_DIR TRISCbits.TRISC6
//#define MCP_MCLR_SET LATCbits.LATC6
//
//
//// MCP F0 Frequency Config
//#define MCP_FREQ_F0_DIR TRISCbits.TRISC7
//#define MCP_FREQ_F0_SET LATCbits.LATC7
//
//// MCP F1 Frequency Config
//#define MCP_FREQ_F1_DIR TRISCbits.TRISC5
//#define MCP_FREQ_F1_SET LATCbits.LATC5
//
//// MCP F2 Frequency Config
//#define MCP_FREQ_F2_DIR TRISCbits.TRISC3
//#define MCP_FREQ_F2_SET LATCbits.LATC3

//
//void init( void );
//void initOSC( void );
//void initIO( void );
//void initInterruptsClear( void );
//void initMCPFout( void );
//void timerInit( void );
//void pulseFoutPassThru( void );
//void powerPulseCheck( void );

//void mcpSPIInit( void );
//void mcpSPIStart( void );

//void delayMS10( int count );

//
// watts variables were here

//volatile unsigned long timerCountHF = 0;
//volatile unsigned long timerCountLF = 0;

//void main( void )
//{
//    //    init( );
//    //
//    //    for( int inx = 0; inx < 10; inx++ )
//    //    {
//    //	LED1_SET = 1;
//    //	delayMS10( 10 );
//    //	LED1_SET = 0;
//    //	delayMS10( 10 );
//    //    }
//    //
//    //    timerInit( );
//
//    //SPISlaveInit( );    // this is now done in the first communications call
//
//
//
//    /*
//     * I'm just trying to simulate the current sense board since mine is
//     * blown right now. This will be a loop that just outputs a "pulse" that
//     * hopefully will be somewhat around the same as a 1kW load. It will be
//     * logic 1 (5v) and drop down to zero quickly, and then stay up at logic
//     * 1 again.
//    while(1) {
//    delayMS(250);
//    for(;;) {
//	pulseOut = 1;
//	LED = 1;
//	delayMS(10000);
//
//	pulseOut = 0;
//	LED = 0;
//	delayMS(5);
//    }
//    }
//     */
//
//    // Take the pulse and send it out to the command board
//
//    communications( true ); // to init SPI communications everything
//
//    bool initDone = false;
//
//    while( 1 )
//    {
//	communications( false );
//
//	//	pulseFoutPassThru( );
//	powerPulseCheck( );
//
//	// reset MCP after 1 second
//	if( initDone == false )
//	{
//	    if( timerCountLF > 1000 )
//	    {
//		initMCPFout( );
//		initDone = true;
//
//		for( int inx = 0; inx < 10; inx++ )
//		{
//		    LED_SET = 1;
//		    delayMS10( 3 );
//		    LED_SET = 0;
//		    delayMS10( 5 );
//		}
//
//
//	    }
//	}
//
//	//	if ( timerHFcount >= 1000 )
//	//	{
//	//	    count = 0;
//	//	    if ( LED_READ == 1 )
//	//	    {
//	//		LED_SET = 0;
//	//	    }
//	//	    else
//	//	    {
//	//		LED_SET = 1;
//	//
//	//	    }
//	//	}
//
//	//        if(PORTAbits.RA1 == 1) {
//	//            pulseOut = 0b1;
//	//            //LATBbits.LATB4 = 0;
//	//        }
//	//        else {
//	//            pulseOut = 0b0;
//	//
//	//            // for testing purposes only
//	//            if (togglePulse == 0) {
//	//                //LED = 1; // turn on the LED
//	//                togglePulse = 1;
//	//                delayMS(100);
//	//            } else {
//	//                //LED = 0; // turn off the LED
//	//                togglePulse = 0;
//	//                delayMS(100);
//	//            }
//	//        }
//    }
//    return;
//}
//
////void pulseFoutPassThru( void )
////{
////    // mimic the pulse from the MCP Fout pins
////    static bool runonce = false;
////
////    if( MCP_HFOUT_READ == 0 )
////    {
////	MCP_LFOUT_PASS_SET = 1;
////	if( runonce == false )
////	{
////	    runonce = true;
////	    if( LED_READ == 1 )
////	    {
////		LED_SET = 0;
////	    }
////	    else
////	    {
////		LED_SET = 1;
////	    }
////	}
////    }
////    else
////    {
////	MCP_LFOUT_PASS_SET = 0;
////	runonce = false;
////    }
////
////    return;
////}
//
//
////void powerPulseCheck( void )
////{
////
////    // here we check if a pulse has some in from both the HF and the LF pulses
////    // the timerCounters are in milli-seconds
////    // if the timer prescaler or countdown is changed this will change the meaning of the timerCounters
////
////
////
////#define ENERGY_PER_PULSE 27000 //(221.24 mWh per pulse)
////#define ENERGY_PER_PULSE_UNIT 100000 // energy per pulse is divided by this to get Wh
////
////    static unsigned long meterEnergyUsedPart = 0;
////    static unsigned long timerCountHFLast = 2147483647;
////    static unsigned int timerCountHFCheck = 1;
////    static bool firstPulse = true;
////
////
////    static bool mcpHFoutLast = false; // this is so we run a calc only once each time the pulse comes
////    static bool mcpLFoutLast = false; // this is so we run a calc only once each time the pulse comes
////
////
////    // HF output is for calculating watts
////    if( MCP_HFOUT_READ == 0 )
////    {
////	if( mcpHFoutLast == false )
////	{
////	    mcpHFoutLast = true;
////	    firstPulse = false;
////
////	    timerCountHFLast = timerCountHF;
////	    timerCountHF = 0;
////	    meterWatts = (((((unsigned long) ENERGY_PER_PULSE * (unsigned long) 3600) / ((unsigned long) ENERGY_PER_PULSE_UNIT / (unsigned long) 1000))) * (unsigned long) 1) / (unsigned long) timerCountHFLast;
////	    //            meterWatts = timerCountHFLast;
////
////
////	    timerCountHFCheck = 1; //reset the periodic power reduction
////	}
////    }
////    else
////    {
////	mcpHFoutLast = false;
////    }
////
////
////    // if there is no power then no pulses
////    // if our pulse time is greater than the last measurement we know we are at a lower power.
////    // go ahead and calculate
////#define POWER_REDUCTION_INTERVAL 1000  //ms (1000 = 1 second ))
////
////    if( (firstPulse == false) && (timerCountHF > timerCountHFLast) )
////    {
////	if( timerCountHF > ((unsigned long) POWER_REDUCTION_INTERVAL * (unsigned long) timerCountHFCheck) )
////	{
////	    if( timerCountHFCheck < 90 )
////	    {
////		timerCountHFCheck++;
////		meterWatts = (((((unsigned long) ENERGY_PER_PULSE * (unsigned long) 3600) / ((unsigned long) ENERGY_PER_PULSE_UNIT / (unsigned long) 1000))) * (unsigned long) 1) / (unsigned long) timerCountHF;
////	    }
////	    else
////	    {
////		meterWatts = 0;
////	    }
////	    //          checkWattsHFvsLF = true;
////	}
////    }
////
////    if( firstPulse == true )
////    {
////	meterWatts = 0;
////    }
////
////
////    // LF out is for calculating Watt-Hour (not watts)
////    if( MCP_LFOUT_READ == 0 )
////    {
////	if( mcpLFoutLast == false )
////	{
////	    mcpLFoutLast = true;
////
////	    meterEnergyUsedPart += ENERGY_PER_PULSE * (unsigned long) 16;
////	    while( meterEnergyUsedPart > ENERGY_PER_PULSE_UNIT )
////	    {
////		meterEnergyUsed++;
////		meterEnergyUsedPart -= ENERGY_PER_PULSE_UNIT;
////	    }
////
////	    timerCountLF = 0;
////	}
////    }
////    else
////    {
////	mcpLFoutLast = false;
////    }
////
////    meterWatts = 777;
////
////
////
////    LED_SET = 1;
////
////
////
////    return;
////
////}
//
//void init( )
//{
//    initOSC( );
//    initIO( );
//
//    ledInit( );
//    initInterruptsClear( );
//    initMCPFout( );
//
//
//
//    return;
//}
//
//void initMCPFout( void )
//{
//    // reset the MCP
//    // wait until SPI timeout is reached before continuing
//
//    MCP_MCLR_DIR = 0;
//    MCP_FREQ_F0_DIR = 0;
//    MCP_FREQ_F1_DIR = 0;
//    MCP_FREQ_F2_DIR = 0;
//
//
//    MCP_FREQ_F0_SET = 0;
//    MCP_FREQ_F1_SET = 0;
//    MCP_FREQ_F2_SET = 0;
//
//    __delay_ms( 5 );
//    MCP_MCLR_SET = 0;
//    __delay_ms( 5 );
//
//    MCP_FREQ_F0_SET = 0;
//    MCP_FREQ_F1_SET = 0;
//    MCP_FREQ_F2_SET = 0;
//
//    __delay_ms( 5 );
//
//    MCP_FREQ_F0_SET = 1;
//    MCP_FREQ_F1_SET = 1;
//    MCP_FREQ_F2_SET = 1;
//
//    __delay_ms( 5 );
//    MCP_MCLR_SET = 1;
//    delayMS10( 10 );
//
//    return;
//}
//
