/******
 Common Header File
 Only put something in here if:
	it is required for all C files
	and
	it is not best included in any other header file
 */


/**********************
 READ THIS
 * PROBLEM
 * 
 * Sometimes a standard include file (included with the < include.h > symbology
 *    gets a red underline error, but the program compiles fine
 * The IDE cannot resolve correctly the include file so it notes an error
 * The compiler is separate from the IDE and works differently - it compiles just fine
 * The issue seems to stem from spaces in file paths - the default paths used by the IDE/compiler
 * This keeps the code completion from working properly and generally causes headaches as you try to code
 * Holding <CTRL> while mouse hovering over the include shows the paths searched
 *     you will notice that the path for the include file is there, it just isn't working right
 * 
 * The fix is to manually add the include paths to the compiler
 *     the paths must be in old DOS 8.3 notation
 * Open Project Properties -> Conf:[default] -> XC16
 *     in the "C include dirs" add the following entries
 * 
 * C:\PROGRA~2\Microchip\xc16\v1.36\support\PIC24F\h
 * C:\PROGRA~2\Microchip\xc16\v1.36\support\generic\h
 * C:\PROGRA~2\Microchip\xc16\v1.36\include
 *
 * ensure the above is adjusted for the specific processor
 * ensure the above is adjusted for the specific compiler version
 * 
 * now the code completion works and the red underlines go away
 
 ******************/



#ifndef COMMON_H
#    define	COMMON_H


/****************
 INCLUDES - COMMON
 these include files are external to this project and are used by all c modules
 this list here should be very limited
 things like stdlib should be included in the c module where it is needed
 not all c modules really need stdlib
 ****************/

// operating frequency
// the UL postfix stipulates that the number is unsigned long
#    define _XTAL_FREQ 16000000UL  //16 Mhz

//#    include <xc.h>
#    include <p18f25k22.h>
#    include <stdbool.h>


/****************
 MACROS
 these are the macros that are required by external c files
 do not include macros that are only used internally within this module
 ****************/

//							"          "  10 character spacing reference - do not exceed or the system will crash
#    define MODULE_INFO_THIS_0	"Power Sens"			// 10 chars max name
#    define MODULE_INFO_THIS_1	"v3.1.7"				// 10 chars max version
#    define MODULE_INFO_THIS_2	""					// 10 chars max info
#    define MODULE_INFO_THIS_3	""					// 10 chars max info
#    define MODULE_INFO_THIS_4	""					// 20 chars max info (only 18 will show on UI)



#    define MODULE_INFO_COUNT	5	// number of module info items

// if DEBUG == true, turn off "Go" turn on "Debug" LED functions
#    define LEDS_FOR_DEBUG false


#    define CHAR_NULL '\0'   //yes, used in many many places


#    define TIMER_ROLLOVER			4000000000	// maximum timer value before we force a rollover back to 0
#    define TIMER_ROLLOVER_CHECK	100000000	// if the difference between the timer and our check is greater than this then our check time has rolled over and we need to wait

/****************
 STRUCTS
 define the structures here
 ***************/


#endif	/* COMMON_H */






/***********************/
/***********************/
/***********************/
/***********************/
/***********************/
/***********************/



/******
 Below are the comments for the headers of C files
 This should be placed at the top of each c file to provide some sense and
   structure to make things easier to understand
 */

/****************
 INCLUDES
 only include the header files that are required
 ****************/

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

// internal only


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










/******
 Below are the comments for the H files
 This should be placed at the top of each h file to provide some sense and
   structure to make things easier to understand
 */

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

// external only

