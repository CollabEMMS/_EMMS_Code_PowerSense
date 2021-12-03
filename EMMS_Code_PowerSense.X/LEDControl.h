/* 
 * File:   LEDControl.h
 * Author: Austin
 *
 * Created on January 31, 2020, 5:43 PM
 */

#ifndef LEDCONTROL_H
#    define	LEDCONTROL_H

/****************
 MACROS
 these are the macros that are required by external c files
 do not include macros that are only used internally within this module
 ****************/

/****************
 VARIABLES
 these are the globals required by external c files
 there should be as few of these as possible to help keep things clean
 these are all 'extern' and require that the variable is declared in the c file
 ****************/


/****************
 FUNCTION PROTOTYPES
 only include functions required by external c files
 ideally these are in the same order as in the code listing
 any functions used internally and externally must have the prototype in both the c and h files and should be marked
 
 *****************/


// external and internal
void ledSetAll( int led1Value, int led2Value, int led3Value, int led4Value );
void ledSetAllOn( void );
void ledSetAllOff( void );


// letTest functions work only in led debug mode (see common.h)

// external only
void ledInit( void );

// the "Test" functions work only if dbug is true
void ledTestSetOn( int ledNum );
void ledTestSetOff( int ledNum );
void ledTestSetAll( int led1Value, int led2Value, int led3Value, int led4Value );
void ledTestSetAllOn( void );
void ledTestSetAllOff( void );
void ledTestToggle( int ledNum );

// the "Go" functions only work if debug is false
void ledGoSetOn( int ledNum );
void ledGoSetOff( int ledNum );
void ledGoSetAll( int led1Value, int led2Value, int led3Value, int led4Value );
void ledGoSetAllOn( void );
void ledGoSetAllOff( void );
void ledGoToggle( int ledNum );

void ledRunUp( int ledRunDelay );
void ledRunDown( int ledRunDelay );
void ledRun( int ledRunDelay );



#endif	/* LEDCONTROL_H */
