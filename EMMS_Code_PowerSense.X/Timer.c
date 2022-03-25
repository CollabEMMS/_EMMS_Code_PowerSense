/****************
 INCLUDES
 only include the header files that are required
 ****************/
#include  "common.h"

/****************
 MACROS
 ****************/
#define LOW_BYTE(x)     ((unsigned char)((x)&0xFF))
#define HIGH_BYTE(x)    ((unsigned char)(((x)>>8)&0xFF))

#define TIMER_COUNTDOWN 1000   // if timer is running at 1Mhz with prescaler, then 1000 will cause interrupt every 1ms, 500 will cause 2 interrupts every 1ms
// need to calc the timer preset because it only counts up and interrupts on rollover from 65535 to 0
// also need high and low bytes since we need to set timer preset one byte at a time
#define TIMER_PRESET ( 65536 - TIMER_COUNTDOWN )
#define TIMER_PRESET_LOW LOW_BYTE( TIMER_PRESET )
#define TIMER_PRESET_HIGH HIGH_BYTE( TIMER_PRESET)



/****************
 VARIABLES
 these are the globals required by only this c file
 there should be as few of these as possible to help keep things clean
 variables required by other c functions should be here and also in the header .h file
 as external
 ****************/

// external

// internal only
volatile unsigned long timerCount_ms[4]; // set the number of timers here
// timer 0 - HFCount_ms
// timer 1 - LFCount_ms
// timer 2 - Power Reduce Timer
// timer 3 - General purpose

/****************
 FUNCTION PROTOTYPES
 only include functions called from within this code
 external functions should be in the header
 ideally these are in the same order as in the code listing
 any functions used internally and externally (prototype here and in the .h file)
	 should be marked
 *****************/

/****************
 CODE
 ****************/

void timerInit( void )
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

	timerCount_ms[0] = 0;
	timerCount_ms[1] = 0;
	timerCount_ms[2] = 0;
	timerCount_ms[3] = 0;

	INTCONbits.TMR0IF = 0;

	TMR0H = TIMER_PRESET_HIGH;
	TMR0L = TIMER_PRESET_LOW;

}

void interrupt Timer0_ISR( void )
{
	INTCONbits.TMR0IF = 0;

	TMR0H = TIMER_PRESET_HIGH;
	TMR0L = TIMER_PRESET_LOW;

	// these timers are reset by code when used
	timerCount_ms[0]++;
	timerCount_ms[1]++;
	timerCount_ms[2]++;

	// timer 3 needs to be rolled over manually
	timerCount_ms[3]++;
	if( timerCount_ms[3] > TIMER_ROLLOVER )
	{
		 timerCount_ms[3] = 0;
	}

	return;
}

unsigned long timerGetCount( int timerIndex )
{
	return timerCount_ms[timerIndex];
}

void timerResetCount( int timerIndex )
{
	timerCount_ms[timerIndex] = 0;

	return;
}


// this function is because the __delay_ms macro will have an overflow with even a slightly large delay
// because we are operating at 16MHz

void delayMS10( int count )
{
	for( int inx = 0; inx < count; inx++ )
	{
		__delay_ms( 10 );
	}
}
