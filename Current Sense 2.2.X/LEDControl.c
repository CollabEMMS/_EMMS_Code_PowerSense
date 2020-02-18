/****************
 INCLUDES
 only include the header files that are required
 ****************/

#include "common.h"
#include "Timer.h"

/****************
 MACROS
 ****************/
//#define LEDS_FOR_DEBUG false   // if true will not allow energy remain to update LEDS so leds can be used for testing and debugging


//  1	pin 25	RB4
//  2	pin 12	RC1
//  3	pin 11	RC0


#define LED1_DIR    TRISBbits.TRISB4
#define LED1_SET    LATBbits.LATB4
#define LED1_READ   PORTBbits.RB4

#define LED2_DIR    TRISCbits.TRISC1
#define LED2_SET    LATCbits.LATC1
#define LED2_READ   PORTCbits.RC1

#define LED3_DIR    TRISCbits.TRISC0
#define LED3_SET    LATCbits.LATC0
#define LED3_READ   PORTCbits.RC0


/****************
 VARIABLES
 these are the globals required by only this c file
 there should be as few of these as possible to help keep things clean
 variables required by other c functions should be here and also in the header .h file
 as external
 ****************/

// external

// internal only


/****************
 FUNCTION PROTOTYPES
 only include functions called from within this code
 external functions should be in the header
 ideally these are in the same order as in the code listing
 any functions used internally and externally (prototype here and in the .h file)
     should be marked
 *****************/

// internal only
void ledSet( int ledNum, int setValue );
void ledSetOn( int ledNum );
void ledSetOff( int ledNum );
void ledToggle( int ledNum );


// internal and external
void ledSetAll( int led1Value, int led2Value, int led3Value );
void ledSetAllOn( void );
void ledSetAllOff( void );

/****************
 CODE
 ****************/

void ledInit( void )
{
    LED1_DIR = 0;
    LED2_DIR = 0;
    LED3_DIR = 0;

    ledSetAllOff( );

    return;
}

void ledSet( int ledNum, int setValue )
{
    switch( ledNum )
    {
	case 1:
	    LED1_SET = setValue;
	    break;
	case 2:
	    LED2_SET = setValue;
	    break;
	case 3:
	    LED3_SET = setValue;
	    break;
    }

    return;
}

void ledSetOn( int ledNum )
{
    int setValue = 1;

    ledSet( ledNum, setValue );

    return;
}

void ledSetOff( int ledNum )
{
    int setValue = 0;

    ledSet( ledNum, setValue );

    return;
}

void ledTestSetOn( int ledNum )
{
    if( LEDS_FOR_DEBUG == true )
    {
	ledSetOn( ledNum );
    }

    return;
}

void ledGoSetOn( int ledNum )
{
    if( LEDS_FOR_DEBUG != true )
    {
	ledSetOn( ledNum );
    }

    return;
}

void ledTestSetOff( int ledNum )
{
    if( LEDS_FOR_DEBUG == true )
    {
	ledSetOff( ledNum );
    }

    return;
}

void ledGoSetOff( int ledNum )
{
    if( LEDS_FOR_DEBUG != true )
    {
	ledSetOff( ledNum );
    }

    return;
}

void ledSetAll( int led1Value, int led2Value, int led3Value )
{
    ledSet( 1, led1Value );
    ledSet( 2, led2Value );
    ledSet( 3, led3Value );

    return;
}

void ledTestSetAll( int led1Value, int led2Value, int led3Value )
{
    if( LEDS_FOR_DEBUG == true )
    {
	ledSetAll( led1Value, led2Value, led3Value );
    }

    return;
}

void ledGoSetAll( int led1Value, int led2Value, int led3Value )
{
    if( LEDS_FOR_DEBUG != true )
    {
	ledSetAll( led1Value, led2Value, led3Value );
    }

    return;
}

void ledSetAllOn( void )
{
    ledSetAll( 1, 1, 1 );

    return;
}

void ledSetAllOff( void )
{
    ledSetAll( 0, 0, 0 );

    return;
}

void ledTestSetAllOn( void )
{
    if( LEDS_FOR_DEBUG == true )
    {
	ledSetAllOn( );
    }

    return;
}

void ledGoSetAllOn( void )
{
    if( LEDS_FOR_DEBUG != true )
    {
	ledSetAllOn( );
    }

    return;
}

void ledTestSetAllOff( void )
{
    if( LEDS_FOR_DEBUG == true )
    {
	ledSetAllOff( );

    }

    return;
}

void ledGoSetAllOff( void )
{
    if( LEDS_FOR_DEBUG != true )
    {
	ledSetAllOff( );

    }

    return;
}

void ledToggle( int ledNum )
{

    int getValue;
    int setValue;

    switch( ledNum )
    {
	case 1:
	    getValue = LED1_READ;
	    break;
	case 2:
	    getValue = LED2_READ;
	    break;
	case 3:
	    getValue = LED3_READ;
	    break;
    }

    if( getValue == 0 )
    {
	setValue = 1;
    }
    else
    {
	setValue = 0;
    }

    ledSet( ledNum, setValue );

    return;
}

void ledTestToggle( int ledNum )
{
    if( LEDS_FOR_DEBUG == true )
    {
	ledToggle( ledNum );
    }

    return;
}

void ledGoToggle( int ledNum )
{
    if( LEDS_FOR_DEBUG != true )
    {
	ledToggle( ledNum );
    }

    return;
}

void ledRunUp( int ledRunDelay )
{

    int delay10ms;
    delay10ms = ledRunDelay / 10;

    ledSetAll( 0, 0, 0 );
    delayMS10( delay10ms );
    ledSetAll( 1, 0, 0 );
    delayMS10( delay10ms );
    ledSetAll( 0, 1, 0 );
    delayMS10( delay10ms );
    ledSetAll( 0, 0, 1 );
    delayMS10( delay10ms );

    return;
}

void ledRunDown( int ledRunDelay )
{

    int delay10ms;
    delay10ms = ledRunDelay / 10;

    ledSetAll( 0, 0, 1 );
    delayMS10( delay10ms );
    ledSetAll( 0, 1, 0 );
    delayMS10( delay10ms );
    ledSetAll( 1, 0, 0 );
    delayMS10( delay10ms );
    ledSetAll( 0, 0, 0 );
    delayMS10( delay10ms );

    return;
}

void ledRun( int ledRunDelay )
{
    ledRunUp( ledRunDelay );
    ledRunDown( ledRunDelay );

    return;
}

//void ledShowChar( char showMe )
//{
//    bool b[8];
//
//    for( int j = 0; j < 8; ++j )
//    {
//	b [j] = 0 != (showMe & (1 << j));
//    }
//
//    for( int inx = 0; inx < 3; inx++ )
//    {
//	ledTestSetAllOn( );
//	__delay_ms( 250 );
//	ledTestSetAllOff( );
//	__delay_ms( 250 );
//    }
//
//    for( int inx = 7; inx > 0; inx -= 2 )
//    {
//	ledTestSetAll( b[inx], b[inx - 1], 0 );
//	delayMS10( 50 );
//	ledTestSetOn( 3 );
//	delayMS10( 200 );
//	ledTestSetOff( 3 );
//
//    }
//
//    ledTestSetAllOff( );
//
//    return;
//}
